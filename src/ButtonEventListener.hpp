#pragma once
#include "ModSettings.h"
#include "VectorUtil.hpp"
#include "HitEventListener.hpp"
#include "MCM_Translation.h"

namespace Listeners {

    class ButtonEventListener : public RE::BSTEventSink<RE::InputEvent *> {
        public:
            static ButtonEventListener *GetSingleton() {
                static ButtonEventListener singleton;
                return &singleton;
            }

            static void Register();
            static void Unregister();

            bool SinkRegistered = false;

        private:
            virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent *const *a_event, RE::BSTEventSource<RE::InputEvent *> *) override;

            ButtonEventListener() = default;
            ~ButtonEventListener() = default;
    };

    void ButtonEventListener::Register() {
        auto inputManager = RE::BSInputDeviceManager::GetSingleton();
        if (inputManager) {
            inputManager->AddEventSink(ButtonEventListener::GetSingleton());
            ButtonEventListener::GetSingleton()->SinkRegistered = true;
        }
    }
    void ButtonEventListener::Unregister() {
        auto inputManager = RE::BSInputDeviceManager::GetSingleton();
        if (inputManager) {
            inputManager->RemoveEventSink(ButtonEventListener::GetSingleton());
            ButtonEventListener::GetSingleton()->SinkRegistered = false;
        }
    }

    RE::BSEventNotifyControl ButtonEventListener::ProcessEvent(RE::InputEvent *const *a_event, RE::BSTEventSource<RE::InputEvent *> *) {
        if (!a_event) return RE::BSEventNotifyControl::kContinue;
        bool left{false};
        bool right{false};

        const auto pl = RE::PlayerCharacter::GetSingleton();
        if (!pl->IsOnMount()) return RE::BSEventNotifyControl::kContinue;

        RE::ActorPtr mnt;
        if (!pl->GetMount(mnt)) RE::BSEventNotifyControl::kContinue;
        const auto horse = mnt.get();
        const auto ctrl = horse->GetCharController();
        if (!ctrl) RE::BSEventNotifyControl::kContinue;

        for (auto event = *a_event; event; event = event->next) {
            const auto UE = RE::UserEvents::GetSingleton();

            if (ModSettings::ManualHorseAttacks.GetValue()) {
                if (pl->AsActorState()->IsWeaponDrawn()) {
                    constexpr auto heldThreshold = 0.1f;
                    if (event->QUserEvent() == UE->leftAttack) {
                        if (event->AsButtonEvent()->HeldDuration() < heldThreshold) left = true;
                    }
                    if (event->QUserEvent() == UE->rightAttack) {
                        if (event->AsButtonEvent()->HeldDuration() < heldThreshold) right = true;
                    }
                }
            }

            /* Sprint Knock System */
            if (ModSettings::SprintJumpKnock.GetValue()) {
                if (auto as = horse->AsActorState(); as->IsSprinting() && horse->IsInJumpState()) {
                    int fed{0};
                    horse->GetGraphVariableInt(FEED_COUNTER, fed);

                    if (fed > 0 || !ModSettings::GrazeSystem.GetValue()) {
                        [horse] {
                            const auto bumped = horse->GetCharController()->bumpedCharCollisionObject;
                            if (!bumped) return;

                            const auto ref = RE::TESHavokUtilities::FindCollidableRef(bumped.get()->collidable);
                            if (!ref) return;

                            if (!ref->IsActor()) return;
                            const auto act = ref->As<RE::Actor>();

                            if (act->GetKnockState() != RE::KNOCK_STATE_ENUM::kNormal) return;
                            const auto curProc = horse->GetActorRuntimeData().currentProcess;
                            if (!curProc) return;

                            curProc->KnockExplosion(act, horse->GetPosition(), 3.f);
                            RE::PlaySound("PHYBodyMediumDirtH");
                        }();
                    }
                }
            }

            /* Feed system */
            if (event->QUserEvent() == UE->sneak) {
                if (!ModSettings::GrazeSystem.GetValue()) {
                    if (ModSettings::ManualPetting.GetValue()) {
                        horse->NotifyAnimationGraph("IdlePet");
                    }
                }
                else {
                    bool valid = [pl, event, horse, ctrl, UE] {
                        if (pl->AsActorState()->IsWeaponDrawn()) return false;

                        bool idle;
                        horse->GetGraphVariableBool("_Horse_IsStandingIdle", idle);
                        if (idle) {
                            if (ModSettings::ManualPetting.GetValue()) {
                                int32_t hunger;
                                horse->GetGraphVariableInt(FEED_COUNTER, hunger);

                                if (hunger > 4) {
                                    const auto msg = this_plugin::CachedStrings::GetSingleton()->upStage5.CachedValue;
                                    RE::SendHUDMessage::ShowHUDMessage(fmt::format("{} {}", horse->GetName(), msg).c_str());
                                    return horse->NotifyAnimationGraph("IdlePet");
                                }
                            }

                            // RE::TESLandTexture *landTex = RE::TES::GetSingleton()->GetLandTexture(pos);
                            auto pos = horse->GetPosition();
                            RE::MATERIAL_ID mat = RE::TES::GetSingleton()->GetLandMaterialType(pos);
                            // LOG("{}", mat);

                            // constexpr float maxSubmerge = 0.15f;
                            // const auto wld = horse->GetParentCell();
                            // const float submerged = wld ? horse->GetSubmergedLevel(horse->GetPositionZ(), wld) : 0;
                            // if (landTex && !landTex->textureGrassList.empty()) {
                            switch (mat) {
                                case RE::MATERIAL_ID::kGrass:
                                case RE::MATERIAL_ID::kDirt:

                                    // Feed
                                    return horse->NotifyAnimationGraph("idleGrazing");

                                default:
                                    if (ModSettings::ManualPetting.GetValue()) {
                                        // Pet
                                        return horse->NotifyAnimationGraph("IdlePet");
                                    }
                            }
                        }
                        return false;
                    }();
                    if (valid) continue;
                }
            }

            /* Canned 180 Turn logic, relative to camera, horse facing direction and input direction */
            if (horse->AsActorState()->IsSprinting()) continue;

            bool turning;
            horse->GetGraphVariableBool("_Horse_IsCannedTurn", turning);

            const auto horseFwd = Util::Vec4_To_Vec3(ctrl->forwardVec * -1);  // This shit is inverted for some reason

            auto horseCam = [horseFwd] -> RE::NiPoint3 {
                const auto cam = RE::PlayerCamera::GetSingleton();
                const auto camNode = cam->cameraRoot;
                RE::NiPoint3 camForward = camNode->world.rotate * RE::NiPoint3{0, 1, 0};
                RE::NiPoint3 camRight = camNode->world.rotate * RE::NiPoint3{1, 0, 0};

                // horse forward in camera space
                RE::NiPoint3 horseCam{horseFwd.Dot(camRight), horseFwd.Dot(camForward), 0.0f};

                return horseCam;
            }();

            auto inputRaw = RE::PlayerControls::GetSingleton()->data.moveInputVec;

            /* Not Pressing the button & should cancel early */
            if (inputRaw.Length() < 0.05f) {
                if (turning) {
                    horse->NotifyAnimationGraph("_CannedEarlyExit");  // Early exit
                }

                continue;
            }

            auto input = RE::NiPoint3(inputRaw.x, inputRaw.y, 0);

            Util::Normalize2D(input);
            Util::Normalize2D(horseCam);

            // signed angle
            float dot = std::clamp(horseCam.Dot(input), -1.0f, 1.0f);
            float crossZ = horseCam.x * input.y - horseCam.y * input.x;

            // LOG("{}", dot);

            const auto fwdVel = [horse, horseFwd] -> float {
                RE::NiPoint3 vel;
                horse->GetLinearVelocity(vel);
                vel.z = 0;
                vel.x *= horseFwd.x;
                vel.y *= horseFwd.y;

                return vel.Length();
            };

            /* Still pressing the button and should cancel early */
            if (turning && dot > 0.8f) {
                horse->NotifyAnimationGraph("_CannedEarlyExit");  // Early exit
            }

            /* DOT: ~120 = -0.5 / ~75 = 0.2588 / ~60 = 0.5 / ~90 = 0 */
            else if (fwdVel() < 200.f) {
                if (dot <= -0.5) {
                    if (crossZ > 0.0f)
                        horse->NotifyAnimationGraph("cannedTurnLeft180");
                    else
                        horse->NotifyAnimationGraph("cannedTurnRight180");
                }
                else if (dot <= 0.2588f) {
                    if (crossZ > 0.0f)
                        horse->NotifyAnimationGraph("cannedTurnLeft90");
                    else
                        horse->NotifyAnimationGraph("cannedTurnRight90");
                }
            }
        }
        /* BUG: Horse hitting is not considered assault for rider, also horse can hit player */
        if (left && right) {
            // bool idle;
            // horse->GetGraphVariableBool("_Horse_IsStandingIdle", idle);
            // if (idle)
            //     horse->NotifyAnimationGraph("attackStart_attack1");
            // else
            horse->NotifyAnimationGraph("attackStart_attack2");

            pl->NotifyAnimationGraph("standingRearup");
        }
        return RE::BSEventNotifyControl::kContinue;
    }

}  // namespace Listeners