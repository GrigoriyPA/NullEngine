#pragma once

#include <null_engine/renderer/shaders/vertex_shader.hpp>

namespace null_engine::detail {

class Interpolation {
public:
    Interpolation(FloatType z, FloatType h, const InterpolationParams& params);

    FloatType GetZ() const;
    InterpolationParams GetParams() const;

    Interpolation& operator+=(const Interpolation& other);
    friend Interpolation operator+(Interpolation left, const Interpolation& right);

    Interpolation& operator-=(const Interpolation& other);
    friend Interpolation operator-(Interpolation left, const Interpolation& right);

    Interpolation& operator*=(FloatType scale);
    friend Interpolation operator*(Interpolation left, FloatType scale);

    Interpolation& operator/=(FloatType scale);
    friend Interpolation operator/(Interpolation left, FloatType scale);

private:
    FloatType z_;
    FloatType h_;
    InterpolationParams params_;
};

template <typename Value>
class DirValue {
public:
    DirValue(const Value& start, const Value& delta)
        : current_(start)
        , delta_(delta) {
    }

    DirValue(const Value& start, const Value& end, uint64_t number_steps)
        : current_(start)
        , delta_((end - start) / std::max(number_steps - 1, 1ull)) {
    }

    const Value& Get() const {
        return current_;
    }

    void Increment() {
        current_ += delta_;
    }

private:
    Value current_;
    Value delta_;
};

struct VertexInfo {
    int64_t x;
    int64_t y;
    Interpolation interpolation;
};

class HorizontalLine {
public:
    HorizontalLine(const VertexInfo& vertex_a, const VertexInfo& vertex_b);

    bool Finished() const;

    VertexInfo GetVertex() const;

    void Increment();

private:
    DirValue<int64_t> x_;
    int64_t y_;
    uint64_t number_pixels_;
    DirValue<Interpolation> interpolation_;
};

}  // namespace null_engine::detail
