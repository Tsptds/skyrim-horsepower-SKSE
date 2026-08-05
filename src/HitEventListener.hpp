#pragma once
#include "Util.hpp"

namespace Listeners {
    class HitEventListener : public RE::BSTEventSink<RE::TESHitEvent> {
        public:
            static HitEventListener *GetSingleton();
            void Register();
            void Unregister();

            bool sinkRegistered = false;

            RE::BSEventNotifyControl ProcessEvent(const RE::TESHitEvent *a_event, RE::BSTEventSource<RE::TESHitEvent> *) override {
                if (!a_event) return RE::BSEventNotifyControl::kContinue;

                const auto attacker = a_event->cause;
                const auto target = a_event->target;
                if (attacker && target) {
                    if (!attacker->IsActor() || !target->IsActor()) return RE::BSEventNotifyControl::kContinue;

                    if (attacker->IsHorse()) {
                        auto horse = attacker->As<RE::Actor>();

                        if (RE::ActorPtr rider; horse->GetMountedBy(rider)) {
                            // RE::ConsoleLog::GetSingleton()->Print("hit by horse");
                            // const_cast<RE::TESObjectREFRPtr &>(a_event->cause) = rider;
                            Util::StartCombat(target.get(), rider.get());
                        }
                    }
                    else if (target->IsHorse()) {
                        if (!ModSettings::HorseHitToleration.GetValue()) return RE::BSEventNotifyControl::kContinue;
                        auto horse = target->As<RE::Actor>();

                        if (RE::ActorPtr rider; horse->GetMountedBy(rider)) {
                            bool canHit;
                            if (horse->GetGraphVariableBool("_Horse_CanHitCourage", canHit) && canHit) {
                                horse->NotifyAnimationGraph("_HitCourage");
                            }
                        }
                    }
                }

                return RE::BSEventNotifyControl::kContinue;
            }
    };

    HitEventListener *HitEventListener::GetSingleton() {
        static HitEventListener singleton;
        return &singleton;
    }

    void HitEventListener::Register() {
        RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESHitEvent>(HitEventListener::GetSingleton());
        sinkRegistered = true;
    }

    void HitEventListener::Unregister() {
        RE::ScriptEventSourceHolder::GetSingleton()->RemoveEventSink<RE::TESHitEvent>(HitEventListener::GetSingleton());
        sinkRegistered = false;
    }

}  // namespace Listeners