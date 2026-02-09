#pragma once
#include "REX/REX/INI.h"
#include "ModSettings.h"

namespace Fixes {

    class Attacks {
        private:
            using fix_t = void (*)(const RE::BSFixedString &a_eventName);

            inline static void FixAttackAnnotationsAndHands(const RE::BSFixedString &a_eventName) {
                if (a_eventName == "blockStart") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMRight";
                    return;
                }

                if (a_eventName == "blockStop") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackRelease";
                    return;
                }

                /* Swap hands, right click to attack right, left click to attack left. Duh */
                if (a_eventName == "attackStart_MC_1HMLeft") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMRight";
                    return;
                }
                if (a_eventName == "attackStart_MC_1HMRight") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMLeft";
                    return;
                }
                if (a_eventName == "attackPowerStart_MC_1HMRight") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackPowerStart_MC_1HMLeft";
                    return;
                }
                if (a_eventName == "attackPowerStart_MC_1HMLeft") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackPowerStart_MC_1HMRight";
                    return;
                }
                /* Release events */
                if (a_eventName == "attackReleaseL") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackRelease";
                    return;
                }
                if (a_eventName == "attackRelease") {
                    const_cast<RE::BSFixedString &>(a_eventName) = "attackReleaseL";
                    return;
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
            // inline static fix_t ApplyFix = FixLeftAttackAnnotationsOnly;
            inline static fix_t ApplyFix = nullptr;

            inline static void SetHandSwapping() {
                bool swappingHands = ModSettings::SwapHands.GetValue();
                ApplyFix = swappingHands ? Attacks::FixAttackAnnotationsAndHands : Attacks::FixLeftAttackAnnotationsOnly;
                LOG("Left Hand Attack Event Fix Installed, Attack Inputs Swapped: {}", swappingHands);
            }
    };

    class Compatibility {
        private:
            using jumpHeightMod_t = void (*)(const RE::Actor *);

            inline static void ModifyStandingAndMovingJump(const RE::Actor *actor) {
                const auto &ctrl = actor->GetCharController();
                auto &JH = ctrl->jumpHeight;
                // LOG("{}", JH);
                // Luckily event fires after jump height is set, so I can overwrite it here, it's set for every jump individually
                bool isStandingjump;
                actor->GetGraphVariableBool("_HORSE_IncreasedJump", isStandingjump);

                JH = isStandingjump ? 2.5f : 1.2f;  // Default 1.08585
            }
            inline static void ModifyStandingJumpOnly(const RE::Actor *actor) {
                const auto &ctrl = actor->GetCharController();
                auto &JH = ctrl->jumpHeight;
                bool isStandingjump;
                actor->GetGraphVariableBool("_HORSE_IncreasedJump", isStandingjump);

                if (isStandingjump) JH = 2.5f;
            }

        public:
            inline static jumpHeightMod_t ModJump = nullptr;

            inline static void SetModJump() {
                bool standingOnly = ModSettings::DisableModMovingJumpHeight.GetValue();
                ModJump = standingOnly ? ModifyStandingJumpOnly : ModifyStandingAndMovingJump;
                LOG("Jump Height Mod Installed, Modify Standing Only: {}", standingOnly);
            }
    };

}  // namespace Fixes