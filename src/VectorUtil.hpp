#pragma once

namespace Util {

    inline static RE::NiPoint3 Vec4_To_Vec3(RE::hkVector4 vec) {
        return RE::NiPoint3(vec.quad.m128_f32[0], vec.quad.m128_f32[1], vec.quad.m128_f32[2]);
    }

    inline static RE::hkVector4 Vec3_To_Vec4(RE::NiPoint3 vec) {
        return RE::hkVector4(vec.x, vec.y, vec.z, 0);
    }

    inline static void Normalize2D(RE::NiPoint3 &v) {
        float len = std::sqrt(v.x * v.x + v.y * v.y);
        if (len > 1e-4f) {
            v.x /= len;
            v.y /= len;
        }
    }
}  // namespace Util