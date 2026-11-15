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

    RE::BGSAttackData *AddAttack(RE::TESRace *a_race, std::string_view a_templateEvent, std::string_view a_newEvent) {
        if (!a_race || !a_race->attackDataMap) {
            return nullptr;
        }

        auto &map = a_race->attackDataMap->attackDataMap;

        const RE::BSFixedString newKey(a_newEvent);
        const RE::BSFixedString templateKey(a_templateEvent);

        if (auto existing = map.find(newKey); existing != map.end()) {
            return existing->second.get();
        }

        auto src = map.find(templateKey);
        if (src == map.end() || !src->second) {
            return nullptr;
        }

        auto *data = new RE::BGSAttackData(*src->second);
        data->event = newKey;

        data->data.damageMult = 1.0f;
        data->data.knockDown = 5.0f;
        data->data.strikeAngle = 50.f;   // Sweep cone rad
        data->data.attackAngle = 180.f;  // Cone angle

        map.insert({newKey, RE::NiPointer<RE::BGSAttackData>(data)});
        return data;
    }

    void FillKickAttackData(RE::Actor *a_actor) {
        if (!a_actor) {
            return;
        }

        auto *proc = a_actor->GetActorRuntimeData().currentProcess;
        auto *high = proc ? proc->high : nullptr;
        auto *race = a_actor->GetRace();
        if (!high || !race || !race->attackDataMap) {
            return;
        }

        auto &map = race->attackDataMap->attackDataMap;
        auto it = map.find(RE::BSFixedString("Horse_BackKick"));
        if (it == map.end() || !it->second) {
            WARN("Horse_BackKick not in attack map of race {}", race->GetFormEditorID());
            return;
        }

        high->attackData = decltype(high->attackData)(it->second.get());
    }

}  // namespace Util