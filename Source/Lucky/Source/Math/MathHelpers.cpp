#include <cmath>

#include <Lucky/Math/MathHelpers.hpp>

namespace Lucky
{
    float FindAngle(const glm::vec2 &vector, const glm::vec2 &reference)
    {
        float angle = std::atan2(vector.y, vector.x) - std::atan2(reference.y, reference.x);

        if (angle < 0)
            angle += 2 * 3.14159f;

        return angle;
    }

    bool ApproximatelyEqual(float floatA, float floatB, float tolerance)
    {
        // See the algorithm here on Christer Ericson's blog
        // http://realtimecollisiondetection.net/blog/?p=89
        if (std::abs(floatA - floatB) <=
            tolerance * std::max(1.0f, std::max(std::abs(floatA), std::abs(floatB))))
        {
            return true;
        }
        return false;
    }

    bool ApproximatelyZero(float f, float tolerance)
    {
        if (std::abs(f) < tolerance)
        {
            return true;
        }
        return false;
    }
} // namespace Lucky
