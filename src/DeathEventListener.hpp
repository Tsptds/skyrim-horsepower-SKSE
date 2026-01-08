#pragma once

namespace Listeners {
    class DeathEventListener : public RE::BSTEventSink<RE::TESDeathEvent> {
        public:
            static DeathEventListener *GetSingleton();
            void Register();
            void Unregister();

            bool sinkRegistered = false;

            RE::BSEventNotifyControl ProcessEvent(const RE::TESDeathEvent *a_event, RE::BSTEventSource<RE::TESDeathEvent> *) override {
                if (!a_event || !a_event->actorDying) return RE::BSEventNotifyControl::kContinue;

                const auto &ref = a_event->actorDying;
                if (!ref->IsActor()) return RE::BSEventNotifyControl::kContinue;

                const auto &victim = ref->As<RE::Actor>();
                if (!victim->IsHorse()) return RE::BSEventNotifyControl::kContinue;

                if (victim->IsDead()) {
                    // LOG("dead: {}", victim->GetDisplayFullName());
                    return RE::BSEventNotifyControl::kContinue;  // It seems to fire both on dying and on dead
                }

                // LOG("dying: {}", victim->GetDisplayFullName());

                if (victim->IsActivationBlocked()) {
                    victim->SetActivationBlocked(false);
                }

                return RE::BSEventNotifyControl::kContinue;
            }
    };

    DeathEventListener *DeathEventListener::GetSingleton() {
        static DeathEventListener singleton;
        return &singleton;
    }

    void DeathEventListener::Register() {
        RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESDeathEvent>(DeathEventListener::GetSingleton());
        sinkRegistered = true;
    }

    void DeathEventListener::Unregister() {
        RE::ScriptEventSourceHolder::GetSingleton()->RemoveEventSink<RE::TESDeathEvent>(DeathEventListener::GetSingleton());
        sinkRegistered = false;
    }

}  // namespace Listeners