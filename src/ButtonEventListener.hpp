#pragma once
#include "VectorUtil.hpp"

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

        /* Canned 180 Turn logic, relative to camera, horse facing direction and input direction */

        const auto &pl = RE::PlayerCharacter::GetSingleton();
        if (!pl->IsOnMount()) return RE::BSEventNotifyControl::kContinue;

        for (auto event = *a_event; event; event = event->next) {
            RE::ActorPtr mnt;
            if (!pl->GetMount(mnt)) continue;

            const auto &horse = mnt.get();
            const auto &ctrl = horse->GetCharController();

            if (!ctrl) continue;

            bool turning;
            horse->GetGraphVariableBool("_Horse_IsCannedTurn", turning);

            const auto &horseFwd = Util::Vec4_To_Vec3(ctrl->forwardVec * -1);  // This shit is inverted for some reason

            const auto &fwdVel = [horse, horseFwd] -> float {
                RE::NiPoint3 vel;
                horse->GetLinearVelocity(vel);
                vel.z = 0;

                return vel * horseFwd;
            }();

            auto &horseCam = [horseFwd] -> RE::NiPoint3 & {
                const auto &cam = RE::PlayerCamera::GetSingleton();
                const auto &camNode = cam->cameraRoot;
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
                    horse->NotifyAnimationGraph("cannedTurnStop");  // Early exit
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

            /* Still pressing the button and should cancel early */
            if (turning && dot > 0.4f) {
                horse->NotifyAnimationGraph("cannedTurnStop");  // Early exit
            }

            else if (fwdVel < 50.f) {
                if (dot <= -0.5) {  // ~120°
                    if (crossZ > 0.0f)
                        horse->NotifyAnimationGraph("cannedTurnLeft180");
                    else
                        horse->NotifyAnimationGraph("cannedTurnRight180");
                }
                else if (dot <= 0) {  // ~90°
                    if (crossZ > 0.0f)
                        horse->NotifyAnimationGraph("cannedTurnLeft90");
                    else
                        horse->NotifyAnimationGraph("cannedTurnRight90");
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

}  // namespace Listeners