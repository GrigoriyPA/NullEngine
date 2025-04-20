#include "native_renderer.hpp"

#include <cassert>
#include <null_engine/util/geometry/helpers.hpp>

namespace null_engine {

using namespace detail;

namespace native {

Renderer::Renderer(const RendererSettings& settings)
    : Base(settings, std::bind(&Renderer::OnRenderEvent, this, std::placeholders::_1))
    , background_color_((settings.background_color * 255.0).cwiseMax(0.0).cwiseMin(255.0))
    , view_{.width = settings.view_width, .height = settings.view_height}
    , rasterizer_(view_) {
}

void Renderer::SubscribeToTextures(InPort<TextureData>* observer_port) const {
    out_texture_port_->Subscribe(observer_port, main_buffer_.colors);
}

void Renderer::OnRenderEvent(const RenderEvent& render_event) {
    const auto& scene = render_event.scene;

    FillLightsInfo(scene);
    RenderScene(scene, render_event.camera);

    out_texture_port_->Notify(main_buffer_.colors);
}

void Renderer::FillLightsInfo(const Scene& scene) {
    const auto& lights = scene.GetLights();
    assert(lights.size() <= MainFragmentShader::kMaxNumberLights && "Too many lights provided");

    lights_info_.clear();
    for (size_t i = 0; i < lights.size(); ++i) {
        const auto& light = lights[i];
        const auto& shadow = light.GetShadowInfo();
        if (!shadow) {
            lights_info_.push_back({.light = light});
            continue;
        }

        rasterizer_.UpdateView({.width = shadow->shadow_width, .height = shadow->shadow_height});
        depth_buffers_[i].depth.assign(shadow->shadow_width * shadow->shadow_height, 1.0);

        RenderingContext context = {
            .fragment_shader = depth_fragment_shader_,
            .buffer = depth_buffers_[i],
            .view_pos = shadow->light_pos,
            .camera_transform = shadow->transform,
        };
        for (const auto& [object, instances] : scene) {
            if (object.GetMaterial().shadow) {
                RenderObject(object, instances, context);
            }
        }

        lights_info_.push_back({.light = light, .depth = DepthBuffer(*shadow, depth_buffers_[i].depth.data())});
    }
}

void Renderer::RenderScene(const Scene& scene, AnyCameraRef camera) {
    ClearBuffer();
    rasterizer_.UpdateView(view_);

    RenderingContext context = {
        .fragment_shader = main_fragment_shader_,
        .buffer = main_buffer_,
        .view_pos = camera->GetViewPos(),
        .camera_transform = camera->GetNdcTransform(),
    };

    main_fragment_shader_.SetViewPos(context.view_pos);
    main_fragment_shader_.SetLights(lights_info_);
    for (const auto& [object, instances] : scene) {
        main_fragment_shader_.SetMaterial(object.GetMaterial());
        RenderObject(object, instances, context);
    }
}

void Renderer::RenderObject(
    const VerticesObject& object, const std::vector<Transform>& instances, RenderingContext& context
) {
    for (const auto& instance_transform : instances) {
        object_transform_ = instance_transform;

        if (object.IsPointsObject()) {
            RenderPointsObject(object, context);
        } else if (object.IsLinesObject()) {
            RenderLinesObject(object, context);
        } else if (object.IsTrianglesObject()) {
            RenderTrianglesObject(object, context);
        } else {
            assert(false && "Unsupported object type for rendering");
        }
    }
}

void Renderer::RenderPointsObject(const VerticesObject& object, RenderingContext& context) {
    assert(object.IsPointsObject() && "Unexpected object type");

    const auto& vertices = ConvertObjectVerices(context.camera_transform, object_transform_, object.GetVertices());
    for (uint64_t index : object.GetIndices()) {
        const auto& point = vertices[index];
        if (!Equal(point.position.w(), 0.0)) {
            rasterizer_.DrawPoint(point, context.buffer, context.fragment_shader);
        }
    }
}

void Renderer::RenderLinesObject(const VerticesObject& object, RenderingContext& context) {
    assert(object.IsLinesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipLines(
        ConvertObjectVerices(context.camera_transform, object_transform_, object.GetVertices()),
        object.GetLinesIndices()
    );

    for (auto [point_a, point_b] : clipped.indices) {
        rasterizer_.DrawLine(
            clipped.vertices[point_a], clipped.vertices[point_b], context.buffer, context.fragment_shader
        );
    }
}

void Renderer::RenderTrianglesObject(const VerticesObject& object, RenderingContext& context) {
    assert(object.IsTrianglesObject() && "Unexpected object type");

    const auto clipped = clipper_.ClipTriangles(
        context.view_pos, ConvertObjectVerices(context.camera_transform, object_transform_, object.GetVertices()),
        object.GetTriangleIndices()
    );

    for (auto [point_a, point_b, point_c] : clipped.indices) {
        rasterizer_.DrawTriangle(
            clipped.vertices[point_a], clipped.vertices[point_b], clipped.vertices[point_c], context.buffer,
            context.fragment_shader
        );
    }
}

void Renderer::ClearBuffer() {
    const auto view_size = view_width_ * view_height_;
    auto& colors = main_buffer_.colors;
    colors.resize(4 * view_size);
    for (size_t i = 0; i < 4 * view_size; i += 4) {
        colors[i] = static_cast<uint8_t>(background_color_.x());
        colors[i + 1] = static_cast<uint8_t>(background_color_.y());
        colors[i + 2] = static_cast<uint8_t>(background_color_.z());
        colors[i + 3] = 255;
    }

    main_buffer_.depth.assign(view_size, 1.0);
}

}  // namespace native

}  // namespace null_engine
