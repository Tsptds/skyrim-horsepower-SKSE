#pragma once

namespace Util {
    bool IsAllowedToReplaceEvent(RE::Actor *actor) {
        // Graph sends bowZoomStart when blockStart fires, back into the game instead of handling on button event
        // blockStart-stop control slow motion, bowZoomStart-stop control camera zoom.

        /* Not bow or crossbow, not while zoomed with bow */
        int rightHandType;
        int wantBlock;
        actor->GetGraphVariableInt("iRightHandType", rightHandType);
        actor->GetGraphVariableInt("iWantBlock", wantBlock);  // Bow zoom is controlled with this, yeah...

        if (rightHandType == 7 || rightHandType == 12 || wantBlock) return false;

        /* Block unless drawn */
        return actor->AsActorState()->GetWeaponState() == RE::WEAPON_STATE::kDrawn;
    }

    /* Added to Commonlib as actor->startcombat */
    // void StartCombat(RE::TESObjectREFR *a_initiator, RE::TESObjectREFR *a_target) {
    //     using func_t = void (*)(RE::TaskQueueInterface *, RE::TESObjectREFR *, RE::TESObjectREFR *);
    //     REL::Relocation<func_t> func{RELOCATION_ID(35984, 36959)};

    //     const auto taskPool = RE::TaskQueueInterface::GetSingleton();
    //     return func(taskPool, a_initiator, a_target);
    // }

    void ShowFeedTutorial() {
        auto dh = RE::TESDataHandler::GetSingleton();
        if (dh) {
            auto frm = dh->LookupForm(0x25, "Horsepower.esp");
            if (frm) {
                auto msg = frm->As<RE::BGSMessage>();
                if (msg) {
                    RE::TutorialMenu::OpenMenu(msg);
                }
            }
        }
    }

    bool SyncGlobalGrazeValue(bool grazing) {
        auto dh = RE::TESDataHandler::GetSingleton();
        auto form = dh->LookupForm(0x26, "Horsepower.esp");

        if (!form) {
            ERROR("Global Value for Grazing System is not found");
            return false;
        }
        auto glbl = form->As<RE::TESGlobal>();
        if (!glbl) {
            ERROR("Graze System Global Form ID is wrong, it should be 0x26");
            return false;
        }

        glbl->value = grazing;
        return true;
    }

}  // namespace Util