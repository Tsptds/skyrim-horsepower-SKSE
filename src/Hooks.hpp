#pragma once

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

                if (!actor) return RE::BSEventNotifyControl::kContinue;

                if (a_event->tag == "GetUpEnd") {
                    if (actor->IsActivationBlocked()) {
                        // logger::info("Clearing block");
                        actor->SetActivationBlocked(false);
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
    if (a_eventName == "Ragdoll") {
        // LOG("RAGDOLL");
        RE::BSAnimationGraphManagerPtr mngr;
        a_this->GetAnimationGraphManager(mngr);
        const auto &actor = mngr->graphs[0]->holder;

        if (actor->IsHorse()) {
            RE::ActorPtr riderPtr;
            if (actor->GetMountedBy(riderPtr) && riderPtr) {
                // logger::info("Knocked rider {}", riderPtr->GetDisplayFullName());

                RE::Actor *rider = riderPtr.get();

                actor->GetActorRuntimeData().currentProcess->KnockExplosion(rider, rider->GetPosition(), 10.f);
            }

            if (!actor->IsActivationBlocked()) {
                actor->SetActivationBlocked(true);
                // logger::info("blocked activation on {}", actor->GetDisplayFullName());
            }
        }
    }

    // LOG(">> Char Anim Event: {}", a_eventName.c_str());
    bool result = _origCharacter(a_this, a_eventName);
    return result;
}