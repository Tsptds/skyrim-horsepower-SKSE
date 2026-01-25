#pragma once

namespace Util {
    bool IsAllowedToReplaceEvent(RE::Actor *actor) {
        // Graph sends bowZoomStart when blockStart fires, back into the game instead of handling on button event
        // blockStart-stop control slow motion, bowZoomStart-stop control camera zoom.

        /* Not bow or crossbow, not while zoomed with bow */
        int rightHandType;
        int wantBlock;
        actor->GetGraphVariableInt("iRightHandType", rightHandType);
        actor->GetGraphVariableInt("iWantBlock", wantBlock);  // Box zoom is controlled with this, yeah...

        if (rightHandType == 7 || rightHandType == 12 || wantBlock) return false;

        /* Block unless drawn */
        return actor->AsActorState()->GetWeaponState() == RE::WEAPON_STATE::kDrawn;
    }
}  // namespace Util