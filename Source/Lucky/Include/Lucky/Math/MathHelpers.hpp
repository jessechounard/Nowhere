#pragma once

#include <algorithm>
#include <math.h>

#include <glm/vec2.hpp>

namespace Lucky
{
    template <typename ValueType, typename InterpolantType>
    ValueType Lerp(const ValueType &v0, const ValueType &v1, InterpolantType t)
    {
        return v0 + t * (v1 - v0);
    }

    template <typename T>
    int Sign(T val)
    {
        return (int)((T(0) < val) - (val < T(0)));
    }

    template <typename T>
    T Clamp(const T &v, const T &lo, const T &hi)
    {
        T result = std::min(v, hi);
        return std::max(result, lo);
    }

    template <typename T>
    T MoveTowards(const T &current, const T &target, const T &delta)
    {
        if (target > current)
        {
            return std::min(current + delta, target);
        }
        else if (target < current)
        {
            return std::max(current - delta, target);
        }

        return target;
    }

    float FindAngle(const glm::vec2 &vector, const glm::vec2 &reference);

    bool ApproximatelyEqual(float floatA, float floatB, float tolerance = 0.0001f);
    bool ApproximatelyZero(float f, float tolerance = 0.0001f);
} // namespace Lucky
