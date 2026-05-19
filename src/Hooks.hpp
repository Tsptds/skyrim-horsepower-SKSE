#pragma once
#include "VectorUtil.hpp"
#include "RayCastUtil.hpp"
#include "ButtonEventListener.hpp"
#include "Fixes.hpp"
#include "Util.hpp"

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
                const auto &actor = a_this->graphs[a_this->GetRuntimeData().activeGraph]->holder;
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
                        // LOG("speed: {}", fwdVel);

                        if (fwdVel > 0 && fwdVel < 50) {
                            actor->NotifyAnimationGraph("IdleRearUp");
                        }
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
                    Fixes::Compatibility::ModJump(actor);
                }

                else if (ev == "_KnockRider") {
                    RE::ActorPtr riderPtr;
                    if (actor->GetMountedBy(riderPtr)) {
                        RE::Actor *rider = riderPtr.get();

                        // Knock rider
                        actor->NotifyAnimationGraph("idleRearUp");
                        actor->GetActorRuntimeData().currentProcess->KnockExplosion(rider, rider->GetPosition(), 0.f);
                    }
                }

                else if (ev == "_Horse_DepleteFeedCounter") {
                    int32_t ctr;
                    RE::BSFixedString var{"_Horse_FeedCounter"};
                    actor->GetGraphVariableInt(var, ctr);

                    if (ctr > 0) {
                        ctr -= 1;
                        actor->SetGraphVariableInt(var, ctr);
                    }

                    if (RE::ActorPtr rider; actor->GetMountedBy(rider) && rider->IsPlayerRef()) {
                        constexpr std::string trns2 = "$DownStage2";
                        constexpr std::string trns1 = "$DownStage1";
                        constexpr std::string trns0 = "$DownStage0";

                        std::string msg{""};
                        std::string trns_out{""};
                        switch (ctr) {
                            case 2:
                                SKSE::Translation::Translate(trns2, trns_out);
                                msg = fmt::format("{} {}", actor->GetName(), trns_out);
                                break;
                            case 1:
                                SKSE::Translation::Translate(trns1, trns_out);
                                msg = fmt::format("{} {}", actor->GetName(), trns_out);
                                break;
                            case 0:
                                SKSE::Translation::Translate(trns0, trns_out);
                                msg = fmt::format("{} {}", actor->GetName(), trns_out);
                                break;
                        }

                        if (msg != "") RE::SendHUDMessage::ShowHUDMessage(msg.c_str(), nullptr, false);
                    }
                }
                else if (ev == "_Horse_IncreaseFeedCounter") {
                    int32_t ctr;
                    RE::BSFixedString var{"_Horse_FeedCounter"};
                    actor->GetGraphVariableInt(var, ctr);

                    if (ctr < 5) {
                        ctr += 1;
                        actor->SetGraphVariableInt(var, ctr);
                    }

                    if (RE::ActorPtr rider; actor->GetMountedBy(rider) && rider->IsPlayerRef()) {
                        constexpr std::string trns5 = "$UpStage5";
                        constexpr std::string trns4 = "$UpStage4";
                        constexpr std::string trns3 = "$UpStage3";
                        constexpr std::string trns1 = "$UpStage1";

                        std::string msg{""};
                        std::string trns_out{""};
                        switch (ctr) {
                            case 5:
                                SKSE::Translation::Translate(trns5, trns_out);
                                msg = fmt::format("{} {}", actor->GetName(), trns_out);
                                break;
                            case 4:
                                SKSE::Translation::Translate(trns4, trns_out);
                                msg = fmt::format("{} {}", actor->GetName(), trns_out);
                                break;
                            case 3:
                                SKSE::Translation::Translate(trns3, trns_out);
                                msg = fmt::format("{} {}", actor->GetName(), trns_out);
                                break;
                            case 1:
                                SKSE::Translation::Translate(trns1, trns_out);
                                msg = fmt::format("{} {}", actor->GetName(), trns_out);
                                break;
                        }

                        if (msg != "") RE::SendHUDMessage::ShowHUDMessage(msg.c_str(), nullptr, false);
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

    const auto &graph = mngr->graphs[0];

    if (!graph) return _origCharacter(a_this, a_eventName);

    const auto &actor = graph->holder;

    if (actor->IsOnMount()) {
        if (Util::IsAllowedToReplaceEvent(actor)) {
            Fixes::Attacks::ApplyFix(a_eventName);
        }
    }

    if (!actor->IsHorse()) return _origCharacter(a_this, a_eventName);

    /* Ragdoll block activation */
    if (a_eventName == "Ragdoll") {
        // LOG("RAGDOLL");

        if (actor->IsDead() || actor->IsDead(false)) return _origCharacter(a_this, a_eventName);

        RE::ActorPtr riderPtr;
        if (actor->GetMountedBy(riderPtr)) {
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
    else if (a_eventName == "idleGrazing") {
        const auto ctrl = actor->GetCharController();
        const auto mat = ctrl->surfaceMaterial;
        using mi = RE::MATERIAL_ID;

        constexpr float maxSubmerge = 0.15f;
        const auto wld = actor->GetParentCell();
        const float submerged = wld ? actor->GetSubmergedLevel(actor->GetPositionZ(), wld) : 0;
        switch (mat) {
            case mi::kNone:
            case mi::kGrass:
            case mi::kDirt:
                if (submerged > maxSubmerge) return false;
                break;

            default:
                return false;
        }
    }

    // LOG(">> Char Anim Event: {}", a_eventName.c_str());
    return _origCharacter(a_this, a_eventName);
}

bool Hooks::NotifyGraphHandler::OnPlayer(RE::IAnimationGraphManagerHolder *a_this, const RE::BSFixedString &a_eventName) {
    if (a_eventName == "HorseEnter" || a_eventName == "HorseEnterSwim") {
        bool res = _origPlayer(a_this, a_eventName);
        if (res) {
            Listeners::ButtonEventListener::GetSingleton()->Register();
            Listeners::HitEventListener::GetSingleton()->Register();
#ifdef _DEBUG
            LOG("HORSE ENTER {}", Listeners::ButtonEventListener::GetSingleton()->SinkRegistered);
#endif
        }

        return res;
    }

    if (a_eventName == "HorseExit" || a_eventName == "HorseExitSwim") {
        bool res = _origPlayer(a_this, a_eventName);
        if (res) {
            Listeners::ButtonEventListener::GetSingleton()->Unregister();
            Listeners::HitEventListener::GetSingleton()->Unregister();
#ifdef _DEBUG
            LOG("HORSE EXIT {}", Listeners::ButtonEventListener::GetSingleton()->SinkRegistered);
#endif
        }

        return res;
    }

    const auto &pl = RE::PlayerCharacter::GetSingleton();
    if (pl->IsOnMount()) {
        if (Util::IsAllowedToReplaceEvent(pl)) {
            Fixes::Attacks::ApplyFix(a_eventName);
        }
    }

    return _origPlayer(a_this, a_eventName);
}