#pragma once

#include <null_engine/drawable_objects/vertices_object.hpp>

#include "light_interface.hpp"

namespace null_engine {

class AmbientLight {
public:
    explicit AmbientLight(FloatType strength);

    Vec3 CalculateLighting(const LightingMaterialSettings& settings) const;

private:
    FloatType strength_;
};

struct LightStrength {
    FloatType ambient;
    FloatType diffuse;
    FloatType specular;
};

class DirectLight {
public:
    DirectLight(const Vec3& direction, const LightStrength& strength);

    Vec3 CalculateLighting(const LightingMaterialSettings& settings) const;

    VerticesObject VisualizeLight(Vec3 position, Vec3 color = Vec3::Ident(1.0), FloatType scale = 1.0) const;

private:
    Vec3 inversed_direction_;
    LightStrength strength_;
};

struct AttenuationSettings {
    FloatType constant = 1.0;
    FloatType linear = 0.0;
    FloatType quadratic = 0.0;
};

class PointLight {
public:
    PointLight(const Vec3& position, const LightStrength& strength, const AttenuationSettings& attenuation = {});

    Vec3 CalculateLighting(const LightingMaterialSettings& settings) const;

    VerticesObject VisualizeLight(Vec3 color = Vec3::Ident(1.0), FloatType scale = 1.0) const;

private:
    Vec3 position_;
    LightStrength strength_;
    AttenuationSettings attenuation_;
};

}  // namespace null_engine
