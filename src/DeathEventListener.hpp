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

                if (!ref->IsHorse()) return RE::BSEventNotifyControl::kContinue;

                if (ref->IsDead() || ref->IsDead(false)) {
#ifdef _DEBUG
                    LOG("dead: {}", ref->GetDisplayFullName());
#endif
                    return RE::BSEventNotifyControl::kContinue;  // It seems to fire both on dying and on dead
                }
#ifdef _DEBUG
                LOG("dying: {}", ref->GetDisplayFullName());
#endif

                if (ref->IsActivationBlocked()) {
#ifdef _DEBUG
                    LOG("Unblocking {}", ref->GetDisplayFullName());
#endif
                    ref->SetActivationBlocked(false);
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