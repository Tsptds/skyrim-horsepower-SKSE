#pragma once
#include "VectorUtil.hpp"
#include "RayCastUtil.hpp"

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
                if (ev == "GetUpEnd" || ev == "RemoveCharacterControllerFromWorld") {
                    if (actor->IsActivationBlocked()) {
                        // logger::info("Clearing block");
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
                    // logger::info("speed: {}", fwdVel);

                    if (fwdVel < 50) {
                        actor->NotifyAnimationGraph("IdleRearUp");
                    }
                }

                /* Horse Sliding fix */
                else if (ev == "_StillFalling") {
                    const Util::RayCastResult ray =
                        Util::RayCast(actor->GetPosition(), RE::NiPoint3(0, 0, -1), 35.f, RE::COL_LAYER::kTransparent, actor);

                    // logger::info("{}", ray.distance);
                    if (ray.didHit) {
                        actor->NotifyAnimationGraph("LandStart");
                    }
                }

                else if (ev == "jumpBegin") {
                    const auto &ctrl = actor->GetCharController();
                    auto &JH = ctrl->jumpHeight;
                    // logger::info("{}", JH);
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

            // Originals
            static inline REL::Relocation<decltype(OnCharacter)> _origCharacter;
    };
}  // namespace Hooks

bool Hooks::NotifyGraphHandler::InstallGraphNotifyHook() {
    // Character
    REL::Relocation<uintptr_t> vtblChar{RE::VTABLE_Character[3]};
    _origCharacter = vtblChar.write_vfunc(0x1, OnCharacter);

    return true;
}

bool Hooks::NotifyGraphHandler::OnCharacter(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName) {
    RE::BSAnimationGraphManagerPtr mngr;
    a_this->GetAnimationGraphManager(mngr);

    if (!mngr) _origCharacter(a_this, a_eventName);

    const auto &graph = mngr->graphs[0];

    if (!graph) _origCharacter(a_this, a_eventName);

    const auto &actor = graph->holder;

    if (!actor->IsHorse()) return _origCharacter(a_this, a_eventName);

    /* Ragdoll block activation */
    if (a_eventName == "Ragdoll") {
        // LOG("RAGDOLL");

        RE::ActorPtr riderPtr;
        if (actor->GetMountedBy(riderPtr) && riderPtr) {
            // logger::info("Knocked rider {}", riderPtr->GetDisplayFullName());

            RE::Actor *rider = riderPtr.get();

            /* Knock rider or its animations go crazy */
            actor->GetActorRuntimeData().currentProcess->KnockExplosion(rider, rider->GetPosition(), 10.f);
        }

        if (!actor->IsActivationBlocked()) {
            actor->SetActivationBlocked(true);
            // logger::info("blocked activation on {}", actor->GetDisplayFullName());
        }
    }

    // LOG(">> Char Anim Event: {}", a_eventName.c_str());
    bool result = _origCharacter(a_this, a_eventName);
    return result;
}