#pragma once
#include "VectorUtil.hpp"
#include "RayCastUtil.hpp"
#include "ButtonEventListener.hpp"
#include "Fixes.hpp"
#include "Util.hpp"
#include "MCM_Translation.h"

namespace Hooks {
    class AnimationEventHook {
        public:
            static bool InstallAnimEventHook() {
                // This is the Event notify hook, equivalent of an event sink. Event will go regardless. Don't return anything in this except the OG func.
                // Sink gets destroyed when graph deletes, so using this
                auto vtbl = REL::Relocation<std::uintptr_t>(RE::VTABLE_BSAnimationGraphManager[0]);
                constexpr std::size_t idx = 0x1;
                _ProcessEvent = vtbl.write_vfunc(idx, &Hook);

                if (!_ProcessEvent.address()) {
                    logger::critical("AnimEvent Hook Not Installed");
                    return false;
                }
                return true;
            }

        private:
            static inline RE::BSEventNotifyControl Hook(RE::BSAnimationGraphManager *a_this, const RE::BSAnimationGraphEvent *a_event,
                                                        RE::BSTEventSource<RE::BSAnimationGraphEvent> *a_eventSource) {
                const auto actor = a_this->graphs[a_this->GetRuntimeData().activeGraph]->holder;
                if (!actor) return _ProcessEvent(a_this, a_event, a_eventSource);

                if (actor->IsPlayerRef()) {
                    /* Stuck swimming bool fix, can't draw weapons bug */
                    if (a_event->tag == "MountEnd") {
                        actor->AsActorState()->actorState1.swimming = false;

                        /* Horse walking in water fix */
                        RE::ActorPtr horse;
                        if (actor->GetMount(horse)) {
                            if (horse->AsActorState()->actorState1.swimming) horse->NotifyAnimationGraph("MountedSwimStart");
                        }
                    }
                    return _ProcessEvent(a_this, a_event, a_eventSource);
                }
                if (!actor->IsHorse()) return _ProcessEvent(a_this, a_event, a_eventSource);
                const auto ev = a_event->tag;

                /* Ragdoll enable activation */
                if (ev == "GetUpEnd") {
                    if (actor->IsActivationBlocked()) {
                        DEBUG("GetUpEnd, clear block on {}", actor->GetDisplayFullName());
                        actor->SetActivationBlocked(false);
                    }
                }

                /* Sprinting force stop if stuck to an object */
                else if (ev == "FootFront" || ev == "FootBack") {
                    if (ModSettings::SprintInterruption.GetValue()) {
                        bool isSprinting;
                        actor->GetGraphVariableBool("IsSprinting", isSprinting);

                        if (!isSprinting) return _ProcessEvent(a_this, a_event, a_eventSource);

                        const auto fwdDir = Util::Vec4_To_Vec3(actor->GetCharController()->forwardVec * -1);

                        RE::NiPoint3 vel;
                        actor->GetLinearVelocity(vel);
                        vel.z = 0;
                        vel.x *= fwdDir.x;
                        vel.y *= fwdDir.y;

                        const auto fwdVel = vel.Length();

                        if (fwdVel > 0 && fwdVel < 50) {
                            actor->NotifyAnimationGraph("IdleRearUp");
                        }
                    }
                }

                /* Horse Sliding fix */
                else if (ev == "_StillFalling") {
                    const Util::RayCastResult ray =
                        Util::RayCast(actor->GetPosition(), RE::NiPoint3(0, 0, -1), 35.f, RE::COL_LAYER::kTransparent, actor);

                    if (ray.didHit) {
                        actor->NotifyAnimationGraph("LandStart");
                    }
                }

                else if (ev == "jumpBegin") {
                    Fixes::Compatibility::ModJump(actor);
                }

                else if (ev == "_KnockRider") {
                    RE::ActorPtr riderPtr;
                    if (actor->GetMountedBy(riderPtr)) {
                        // Knock rider
                        actor->NotifyAnimationGraph("idleRearUp");
                        // Seems to be annoying for now
                        // RE::Actor *rider = riderPtr.get();
                        // actor->GetActorRuntimeData().currentProcess->KnockExplosion(rider, rider->GetPosition(), 0.f);
                    }
                }

                else if (ev == FEED_COUNTER_DEC) {
                    int32_t ctr;
                    RE::BSFixedString var{FEED_COUNTER};
                    actor->GetGraphVariableInt(var, ctr);

                    if (ctr > 0) {
                        ctr -= 1;
                        actor->SetGraphVariableInt(var, ctr);
                    }

                    /* Notifications */
                    if (ModSettings::GrazeSystem.GetValue()) {
                        if (RE::ActorPtr rider; actor->GetMountedBy(rider) && rider->IsPlayerRef()) {
                            auto trns = this_plugin::CachedStrings::GetSingleton();

                            std::string msg{""};
                            switch (ctr) {
                                case 2:
                                    msg = trns->downStage2.CachedValue;
                                    break;
                                case 1:
                                    msg = trns->downStage1.CachedValue;
                                    break;
                                case 0:
                                    msg = trns->downStage0.CachedValue;
                                    break;
                            }

                            if (msg != "")
                                RE::SendHUDMessage::ShowHUDMessage((fmt::format("{} {}", actor->GetName(), msg)).c_str(), nullptr, false);
                        }
                    }
                }
                else if (ev == FEED_COUNTER_INC) {
                    int32_t ctr;
                    RE::BSFixedString var{FEED_COUNTER};
                    actor->GetGraphVariableInt(var, ctr);

                    if (ctr < 5) {
                        ctr += 1;
                        actor->SetGraphVariableInt(var, ctr);
                    }

                    /* Notifications */
                    if (ModSettings::GrazeSystem.GetValue()) {
                        if (RE::ActorPtr rider; actor->GetMountedBy(rider) && rider->IsPlayerRef()) {
                            auto trns = this_plugin::CachedStrings::GetSingleton();

                            std::string msg{""};
                            switch (ctr) {
                                case 5:
                                    msg = trns->upStage5.CachedValue;
                                    break;
                                case 4:
                                    msg = trns->upStage4.CachedValue;
                                    break;
                                case 3:
                                    msg = trns->upStage3.CachedValue;
                                    break;
                                case 1:
                                    msg = trns->upStage1.CachedValue;
                                    break;
                            }

                            if (msg != "")
                                RE::SendHUDMessage::ShowHUDMessage((fmt::format("{} {}", actor->GetName(), msg)).c_str(), nullptr, false);
                        }
                    }
                }

                return _ProcessEvent(a_this, a_event, a_eventSource);
            }

            static inline REL::Relocation<decltype(Hook)> _ProcessEvent;  // 01
    };

    class NotifyGraphHandler {
        public:
            static bool InstallGraphNotifyHook();

        private:
            // Callbacks
            static bool OnCharacter(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName);
            static bool OnPlayer(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName);

            // Originals
            static inline REL::Relocation<decltype(OnCharacter)> _origCharacter;
            static inline REL::Relocation<decltype(OnPlayer)> _origPlayer;
    };
}  // namespace Hooks

bool Hooks::NotifyGraphHandler::InstallGraphNotifyHook() {
    // Character
    REL::Relocation<uintptr_t> vtblChar{RE::VTABLE_Character[3]};
    _origCharacter = vtblChar.write_vfunc(0x1, OnCharacter);

    // Player
    REL::Relocation<uintptr_t> vtblPlayer{RE::VTABLE_PlayerCharacter[3]};
    _origPlayer = vtblPlayer.write_vfunc(0x1, OnPlayer);

    return true;
}

bool Hooks::NotifyGraphHandler::OnCharacter(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName) {
    RE::BSAnimationGraphManagerPtr mngr;
    a_this->GetAnimationGraphManager(mngr);

    if (!mngr) return _origCharacter(a_this, a_eventName);

    const auto graph = mngr->graphs[0];

    if (!graph) return _origCharacter(a_this, a_eventName);

    const auto actor = graph->holder;

    if (actor->IsOnMount()) {
        if (Util::IsAllowedToReplaceEvent(actor)) {
            Fixes::Attacks::ApplyFix(a_eventName);
        }
    }

    if (!actor->IsHorse()) return _origCharacter(a_this, a_eventName);

    /* Ragdoll block activation */
    if (a_eventName == "Ragdoll") {
        if (actor->IsDead() || actor->IsDead(false)) return _origCharacter(a_this, a_eventName);

        RE::ActorPtr riderPtr;
        if (actor->GetMountedBy(riderPtr)) {
            DEBUG("Knocked rider {}", riderPtr->GetDisplayFullName());

            RE::Actor *rider = riderPtr.get();

            /* Knock rider or its animations go crazy */
            actor->GetActorRuntimeData().currentProcess->KnockExplosion(rider, rider->GetPosition(), 10.f);
        }

        if (!actor->IsActivationBlocked()) {
            actor->SetActivationBlocked(true);
            DEBUG("blocked activation on {}", actor->GetDisplayFullName());
        }
    }
    else if (a_eventName == "idleGrazing") {
        auto pos = actor->GetPosition();
        using mi = RE::MATERIAL_ID;
        RE::MATERIAL_ID mat = RE::TES::GetSingleton()->GetLandMaterialType(pos);

        switch (mat) {
            case mi::kGrass:
            case mi::kDirt:
                break;

            default:
                return false;
        }
    }

    return _origCharacter(a_this, a_eventName);
}

bool Hooks::NotifyGraphHandler::OnPlayer(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName) {
    if (a_eventName == "HorseEnter" || a_eventName == "HorseEnterSwim") {
        bool res = _origPlayer(a_this, a_eventName);
        if (res) {
            Listeners::ButtonEventListener::GetSingleton()->Register();
            Listeners::HitEventListener::GetSingleton()->Register();
            DEBUG("HORSE ENTER {}", Listeners::ButtonEventListener::GetSingleton()->SinkRegistered);
            Util::ShowFeedTutorial();
        }

        return res;
    }

    if (a_eventName == "HorseExit" || a_eventName == "HorseExitSwim") {
        bool res = _origPlayer(a_this, a_eventName);
        if (res) {
            Listeners::ButtonEventListener::GetSingleton()->Unregister();
            Listeners::HitEventListener::GetSingleton()->Unregister();
            DEBUG("HORSE EXIT {}", Listeners::ButtonEventListener::GetSingleton()->SinkRegistered);
        }

        return res;
    }

    const auto pl = RE::PlayerCharacter::GetSingleton();
    if (pl->IsOnMount()) {
        if (Util::IsAllowedToReplaceEvent(pl)) {
            Fixes::Attacks::ApplyFix(a_eventName);
        }
    }

    return _origPlayer(a_this, a_eventName);
}