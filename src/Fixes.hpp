#pragma once
#include "REX/REX/INI.h"
#include "ModSettings.h"

namespace Fixes {

    inline static void FixBowZoom(RE::Actor *actor) {
        float bowZoom;
        actor->GetGraphVariableFloat("bowZoom", bowZoom);
        if (bowZoom > 0.f) actor->SetGraphVariableFloat("bowZoom", 0.f);
    }

    class Attacks {
        private:
            using fix_t = void (*)(const RE::BSFixedString &a_eventName);

            inline static void FixAttackAnnotationsAndHands(const RE::BSFixedString &a_eventName) {
                if (a_eventName == "blockStart") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMRight";
                }

                else if (a_eventName == "blockStop") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackRelease";
                }

                /* Swap hands, right click to attack right, left click to attack left. Duh */
                else if (a_eventName == "attackStart_MC_1HMLeft") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMRight";
                }
                else if (a_eventName == "attackStart_MC_1HMRight") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMLeft";
                }
                else if (a_eventName == "attackPowerStart_MC_1HMRight") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackPowerStart_MC_1HMLeft";
                }
                else if (a_eventName == "attackPowerStart_MC_1HMLeft") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackPowerStart_MC_1HMRight";
                }
                /* Release events */
                else if (a_eventName == "attackReleaseL") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackRelease";
                }
                else if (a_eventName == "attackRelease") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackReleaseL";
                }
            }

            inline static void FixLeftAttackAnnotationsOnly(const RE::BSFixedString &a_eventName) {
                if (a_eventName == "blockStart") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMLeft";
                }

                else if (a_eventName == "blockStop") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackReleaseL";
                }
            }

        public:
            inline static fix_t ApplyFix = nullptr;

            inline static void SetHandSwapping() {
                bool swappingHands = ModSettings::SwapHands.GetValue();
                Attacks::ApplyFix = swappingHands ? Attacks::FixAttackAnnotationsAndHands : Attacks::FixLeftAttackAnnotationsOnly;
                LOG("Left Hand Attack Event Fix Installed, Swapped Attack Inputs: {}", swappingHands);
            }
    };

}  // namespace Fixes