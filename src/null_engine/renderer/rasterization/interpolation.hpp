#pragma once

#include <null_engine/drawable_objects/vertex.hpp>

namespace null_engine::detail {

class Interpolation {
public:
    explicit Interpolation(const Vertex& point);

    FloatType GetZ() const;
    FloatType GetH() const;
    const VertexParams& GetParams() const;

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
    VertexParams params_;
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

class PixelCounted {
public:
    explicit PixelCounted(uint64_t number_pixels);

    bool HasPixels() const;

protected:
    void DecreasePixels();

private:
    uint64_t number_pixels_;
};

class TriangleBorders : public PixelCounted {
public:
    struct Border {
        DirValue<FloatType> border;
        DirValue<Interpolation> interpolation;
    };

    TriangleBorders(DirValue<int64_t> y, const Border& left, const Border& right, uint64_t number_pixels);

    int64_t GetY() const;

    FloatType GetLeftBorder() const;

    FloatType GetRightBorder() const;

    const Interpolation& GetLeftInterpolation() const;

    const Interpolation& GetRightInterpolation() const;

    void Increment();

private:
    DirValue<int64_t> y_;
    Border left_;
    Border right_;
};

class RsteriztionLine : public PixelCounted {
public:
    RsteriztionLine(
        DirValue<int64_t> x, int64_t y, const DirValue<Interpolation>& interpolation, uint64_t number_pixels
    );

    int64_t GetX() const;

    int64_t GetY() const;

    const Interpolation& GetInterpolation() const;

    void Increment();

private:
    DirValue<int64_t> x_;
    int64_t y_;
    DirValue<Interpolation> interpolation_;
};

}  // namespace null_engine::detail
