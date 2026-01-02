#pragma once

namespace Util {
    struct RayCastResult {
            float distance = -1.0f;
            RE::COL_LAYER layer = RE::COL_LAYER::kUnidentified;
            RE::hkVector4 normalOut = RE::hkVector4(0, 0, 0, 0);
            bool didHit = false;

            // Do a null check before using this
            RE::TESObjectREFR *hitObjectRef = nullptr;

            RayCastResult() = default;

            RayCastResult(float d, RE::COL_LAYER l, const RE::hkVector4 &n, bool h, RE::TESObjectREFR *r)
                : distance(d), layer(l), normalOut(n), didHit(h), hitObjectRef(r) {}
    };

    RayCastResult RayCast(RE::NiPoint3 rayStart, RE::NiPoint3 rayDir, float maxDist, RE::COL_LAYER layerMask,
                                          RE::Actor *actor) {
        RayCastResult result{};
        result.distance = maxDist;

        if (!actor) {
            return result;
        }
        const auto &cell = actor->GetParentCell();
        if (!cell) {
            return result;
        }
        const auto &bhkWorld = cell->GetbhkWorld();
        if (!bhkWorld) {
            return result;
        }

        RE::bhkPickData pickData;
        const auto &havokWorldScale = RE::bhkWorld::GetWorldScale();

        // Set ray start and end points (scaled to Havok world)
        pickData.rayInput.from = rayStart * havokWorldScale;
        pickData.rayInput.to = (rayStart + rayDir * maxDist) * havokWorldScale;

        // Set the collision filter info to exclude the player
        /* hkpCollidable.h, lower 4 bits: CollidesWith, higher 4 bits: BelongsTo */

        //static_cast<uint32_t>(COL_LAYER::kAnimStatic) & ~static_cast<uint32_t>(COL_LAYER::kDoorDetection)

        RE::CFilter cFilter;
        actor->GetCollisionFilterInfo(cFilter);
        cFilter.SetCollisionLayer(layerMask);
        pickData.rayInput.filterInfo = cFilter;
        // static_cast<RE::CFilter>(cFilter.filter | static_cast<uint32_t>(layerMask));

        // Perform the raycast
        if (bhkWorld->PickObject(pickData) && pickData.rayOutput.HasHit()) {
            result.didHit = true;
            result.distance = maxDist * pickData.rayOutput.hitFraction;
            result.normalOut = pickData.rayOutput.normal;

            result.layer = pickData.rayOutput.rootCollidable->GetCollisionLayer();

            result.hitObjectRef = RE::TESHavokUtilities::FindCollidableRef(*pickData.rayOutput.rootCollidable);
        }

        return result;
    }
}  // namespace Util