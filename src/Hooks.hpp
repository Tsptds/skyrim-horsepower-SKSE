#pragma once
#include "VectorUtil.hpp"
#include "RayCastUtil.hpp"
#include "ButtonEventListener.hpp"
#include "Fixes.hpp"

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
                constexpr auto kContinue = RE::BSEventNotifyControl::kContinue;

                const auto &actor = a_this->graphs[a_this->GetRuntimeData().activeGraph]->holder;

                if (!actor || !actor->IsHorse()) return kContinue;
                const auto &ev = a_event->tag;

                /* Ragdoll enable activation */
                if (ev == "GetUpEnd") {
                    if (actor->IsActivationBlocked()) {
#ifdef _DEBUG
                        LOG("GetUpEnd, clear block on {}", actor->GetDisplayFullName());
#endif
                        actor->SetActivationBlocked(false);
                    }
                }

                /* Sprinting force stop if stuck to an object */
                else if (ev == "FootFront") {
                    bool isSprinting;
                    actor->GetGraphVariableBool("IsSprinting", isSprinting);

                    if (!isSprinting) return kContinue;

                    const auto fwdDir = Util::Vec4_To_Vec3(actor->GetCharController()->forwardVec * -1);

                    RE::NiPoint3 vel;
                    actor->GetLinearVelocity(vel);
                    vel.z = 0;

                    const auto fwdVel = vel * fwdDir;
                    // LOG("speed: {}", fwdVel);

                    if (fwdVel < 50) {
                        actor->NotifyAnimationGraph("IdleRearUp");
                    }
                }

                /* Horse Sliding fix */
                else if (ev == "_StillFalling") {
                    const Util::RayCastResult ray =
                        Util::RayCast(actor->GetPosition(), RE::NiPoint3(0, 0, -1), 35.f, RE::COL_LAYER::kTransparent, actor);

                    // LOG("{}", ray.distance);
                    if (ray.didHit) {
                        actor->NotifyAnimationGraph("LandStart");
                    }
                }

                else if (ev == "jumpBegin") {
                    const auto &ctrl = actor->GetCharController();
                    auto &JH = ctrl->jumpHeight;
                    // LOG("{}", JH);
                    // Luckily event fires after jump height is set, so I can overwrite it here, it's set for every jump individually
                    bool isStandingjump;
                    actor->GetGraphVariableBool("_HORSE_IncreasedJump", isStandingjump);

                    JH = isStandingjump ? 2.5f : 1.2f;  // Default 1.08585
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

    const auto &graph = mngr->graphs[0];

    if (!graph) return _origCharacter(a_this, a_eventName);

    const auto &actor = graph->holder;

    if (actor->IsOnMount()) {
        int rightHandType;
        actor->GetGraphVariableInt("iRightHandType", rightHandType);

        // Graph sends bowZoomStart when blockStart fires, back into the game instead of handling on button event
        if (rightHandType != 7 && rightHandType != 12) {
            Fixes::ApplyFix(a_eventName);
        }
    }

    if (!actor->IsHorse()) return _origCharacter(a_this, a_eventName);

    /* Ragdoll block activation */
    if (a_eventName == "Ragdoll") {
        // LOG("RAGDOLL");

        if (actor->IsDead() || actor->IsDead(false)) return _origCharacter(a_this, a_eventName);

        RE::ActorPtr riderPtr;
        if (actor->GetMountedBy(riderPtr) && riderPtr) {
#ifdef _DEBUG
            LOG("Knocked rider {}", riderPtr->GetDisplayFullName());
#endif

            RE::Actor *rider = riderPtr.get();

            /* Knock rider or its animations go crazy */
            actor->GetActorRuntimeData().currentProcess->KnockExplosion(rider, rider->GetPosition(), 10.f);
        }

        if (!actor->IsActivationBlocked()) {
            actor->SetActivationBlocked(true);
#ifdef _DEBUG
            LOG("blocked activation on {}", actor->GetDisplayFullName());
#endif
        }
    }

    // LOG(">> Char Anim Event: {}", a_eventName.c_str());
    return _origCharacter(a_this, a_eventName);
}

bool Hooks::NotifyGraphHandler::OnPlayer(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName) {
    if (a_eventName == "HorseEnter") {
        bool res = _origPlayer(a_this, a_eventName);
        if (res) {
            Listeners::ButtonEventListener::GetSingleton()->Register();
#ifdef _DEBUG
            LOG("HORSE ENTER {}", Listeners::ButtonEventListener::GetSingleton()->SinkRegistered);
#endif
        }

        return res;
    }

    if (a_eventName == "HorseExit") {
        bool res = _origPlayer(a_this, a_eventName);
        if (res) {
            Listeners::ButtonEventListener::GetSingleton()->Unregister();
#ifdef _DEBUG
            LOG("HORSE EXIT {}", Listeners::ButtonEventListener::GetSingleton()->SinkRegistered);
#endif
        }

        return res;
    }

    const auto &pl = RE::PlayerCharacter::GetSingleton();
    if (pl->IsOnMount()) {
        int rightHandType;
        pl->GetGraphVariableInt("iRightHandType", rightHandType);

        // Graph sends bowZoomStart when blockStart fires, back into the game instead of handling on button event
        if (rightHandType != 7 && rightHandType != 12) {
            Fixes::ApplyFix(a_eventName);
        }
    }

    return _origPlayer(a_this, a_eventName);
}