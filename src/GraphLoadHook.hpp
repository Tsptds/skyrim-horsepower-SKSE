#pragma once

namespace Hooks {
    class GraphLoadHook {
        public:
            inline static bool InstallGraphManagerHooks() {
                bool res{true};
                res &= Install::PostCreate();

                return res;
            }

        private:
            struct Signatures {
                    using PostCreate_t = void(RE::IAnimationGraphManagerHolder *a_this,
                                              RE::BSTSmartPointer<RE::BSAnimationGraphManager> &a_animGraphMgr);
            };

            struct Install {
                    static bool PostCreate();
            };

            struct Callback {
                    static Signatures::PostCreate_t PostCreate;
            };

            struct OG {
                    static inline REL::Relocation<Signatures::PostCreate_t *> _PostCreate;
            };
    };

    bool GraphLoadHook::Install::PostCreate() {
        REL::Relocation<std::uintptr_t> vtbl{
            RE::VTABLE_Character[3]};  // Player and other actors have different vtables, VTABLE_PlayerCharacter for player hook.
        OG::_PostCreate = vtbl.write_vfunc(0xB, Callback::PostCreate);

        bool res = OG::_PostCreate.address() != 0;
        if (!res) ERROR("GraphManager Hook Not Installed");
        return res;
    }

    void GraphLoadHook::Callback::PostCreate(RE::IAnimationGraphManagerHolder *a_this,
                                             RE::BSTSmartPointer<RE::BSAnimationGraphManager> &a_animGraphMgr) {
        OG::_PostCreate(a_this, a_animGraphMgr);

        const auto actor = a_animGraphMgr->graphs[a_animGraphMgr->GetRuntimeData().activeGraph].get()->holder;

        if (actor && actor->IsHorse()) {
            using flag = RE::RACE_DATA::Flag;
            bool enabled = ModSettings::RemoveNoKnockdownFlag.GetValue();
            auto race = actor->GetRace();
            if (race)
                if (enabled) {
                    if (race->data.flags.any(flag::kNoKnockdowns)) {
                        INFO("Removing NoKnockdown flag of: {}", race->GetName());
                        race->data.flags.reset(flag::kNoKnockdowns);
                    }
                }
                else {
                    if (!race->data.flags.any(flag::kNoKnockdowns)) {
                        INFO("Restored NoKnockdown flag of: {}", race->GetName());
                        race->data.flags.set(RE::RACE_DATA::Flag::kNoKnockdowns);
                    }
                }
        }
    }
}  // namespace Hooks