#pragma once

namespace Listeners {

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

    class ButtonEventListener : public RE::BSTEventSink<RE::InputEvent *> {
        public:
            static ButtonEventListener *GetSingleton() {
                static ButtonEventListener singleton;
                return &singleton;
            }

            static void Register();
            static void Unregister();

            bool SinkRegistered;

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
            logger::info("Buttons - Listening");
        }
    }
    void ButtonEventListener::Unregister() {
        auto inputManager = RE::BSInputDeviceManager::GetSingleton();
        if (inputManager) {
            inputManager->RemoveEventSink(ButtonEventListener::GetSingleton());
            ButtonEventListener::GetSingleton()->SinkRegistered = false;
            //LOG("Buttons - Not Listening");
        }
    }

    RE::BSEventNotifyControl ButtonEventListener::ProcessEvent(RE::InputEvent *const *a_event, RE::BSTEventSource<RE::InputEvent *> *) {
        if (!a_event) return RE::BSEventNotifyControl::kContinue;

        // const auto &UE = RE::UserEvents::GetSingleton();
        const auto &pl = RE::PlayerCharacter::GetSingleton();

        for (auto event = *a_event; event; event = event->next) {
            if (!pl->GetCharController()) continue;

            RE::ActorPtr mnt;
            if (!pl->GetMount(mnt)) continue;

            const auto horse = mnt.get();

            // if (horse->IsInMidair() && event->AsButtonEvent()->GetUserEvent() == RE::UserEvents::GetSingleton()->jump) {
            //     const auto &ctrl = horse->GetCharController();
            //     ctrl->jumpHeight = 5.0f;

            //     horse->SetGraphVariableInt("TurnDeltaDamped", 0);
            //     ctrl->context.currentState = RE::hkpCharacterStateType::kJumping;
            //     ctrl->forwardVec = horse->NotifyAnimationGraph("forwardJumpStart");
            // }

            // const auto facingDir = Vec4_To_Vec3(horse->GetCharController()->forwardVec * -1);
            // logger::info("Facing: {} {}", facingDir.x, facingDir.y);
            // const auto inputDir = RE::PlayerControls::GetSingleton()->data.moveInputVec;
            // logger::info("Inputting: {} {}", inputDir.x, inputDir.y);

            // auto facing = Vec4_To_Vec3(horse->GetCharController()->forwardVec);

            auto horseFwd = Vec4_To_Vec3(horse->GetCharController()->forwardVec * -1);  // This shit is inverted for some reason

            auto cam = RE::PlayerCamera::GetSingleton();
            auto camNode = cam->cameraRoot;
            RE::NiPoint3 camForward = camNode->world.rotate * RE::NiPoint3{0, 1, 0};
            RE::NiPoint3 camRight = camNode->world.rotate * RE::NiPoint3{1, 0, 0};

            // horse forward in camera space
            RE::NiPoint3 horseCam{horseFwd.Dot(camRight), horseFwd.Dot(camForward), 0.0f};

            auto inputRaw = RE::PlayerControls::GetSingleton()->data.moveInputVec;
            if (inputRaw.Length() < 0.05f) continue;

            auto input = RE::NiPoint3(inputRaw.x, inputRaw.y, 0);

            Normalize2D(input);
            Normalize2D(horseCam);

            // signed angle
            float dot = std::clamp(horseCam.Dot(input), -1.0f, 1.0f);
            float crossZ = horseCam.x * input.y - horseCam.y * input.x;

            if (dot <= -0.5) {  // ~120°
                if (crossZ > 0.0f)
                    horse->NotifyAnimationGraph("cannedTurnLeft180");
                else
                    horse->NotifyAnimationGraph("cannedTurnRight180");
            }
            // else if (dot < -0.087) {  // ~95°
            //     if (crossZ > 0.0f)
            //         horse->NotifyAnimationGraph("cannedTurnLeft90");
            //     else
            //         horse->NotifyAnimationGraph("cannedTurnRight90");
            // }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

}  // namespace Listeners