#include "ModSettings.h"
namespace MT_Ptr {
    inline RE::BSTSmartPointer<RE::BSAnimationGraphChannel> walk_ptr;
    inline RE::BSTSmartPointer<RE::BSAnimationGraphChannel> run_ptr;
    inline RE::BSTSmartPointer<RE::BSAnimationGraphChannel> sprint_ptr;

    enum Type : uint16_t { kWalk = 0, kRun, kSprint };
    inline RE::BSTSmartPointer<RE::BSAnimationGraphChannel> *GetPtr(Type type) {
        switch (type) {
            case kWalk:
                return &walk_ptr;
            case kRun:
                return &run_ptr;
            case kSprint:
                return &sprint_ptr;
            default:
                return nullptr;
        }
    }
}  // namespace MT_Ptr

namespace HavokUtil {
    class HorsePower_BoundChannel : public RE::BSAnimationGraphChannel {
        public:
            HorsePower_BoundChannel(const RE::BSFixedString &name, float a_value) {
                const_cast<RE::BSFixedString &>(channelName) = name;
                value = std::bit_cast<std::uint32_t>(a_value);
            }
            void ResetImpl() override {}

            void PollChannelUpdateImpl([[maybe_unused]] bool a_arg1) override {}
    };

    static inline void BindChannels(RE::Actor *act, RE::BSAnimationGraphManagerPtr mgr = nullptr) {
        if (!act) {
            WARN("{}", "Attempted bind channel with null actor, skipped");
            return;
        }
        if (!mgr) {
            if (!act->GetAnimationGraphManager(mgr)) {
                WARN("Attempted bind channel on {} with no graph manager, skipped", act->GetName());
                return;
            }
        }

        const auto boundChannels = mgr->boundChannels;

        RE::BSFixedString walk{"HPMHR_MT_WalkForward"};
        RE::BSFixedString run{"HPMHR_MT_RunForward"};
        RE::BSFixedString sprint{"HPMHR_MT_Sprint"};

        bool has_walk{false};
        bool has_run{false};
        bool has_sprint{false};

        for (auto &i: boundChannels) {
            if (!i) continue;
            if (i->channelName == walk)
                has_walk = true;
            else if (i->channelName == run)
                has_run = true;
            else if (i->channelName == sprint)
                has_sprint = true;
        }

        if (!has_walk) {
            if (!MT_Ptr::walk_ptr) {
                MT_Ptr::walk_ptr = RE::BSTSmartPointer<RE::BSAnimationGraphChannel>(
                    new HorsePower_BoundChannel(walk, ModSettings::_MT_WalkForward.GetValue()));
            }
            mgr->boundChannels.push_back(MT_Ptr::walk_ptr);
        }
        if (!has_run) {
            if (!MT_Ptr::run_ptr) {
                MT_Ptr::run_ptr = RE::BSTSmartPointer<RE::BSAnimationGraphChannel>(
                    new HorsePower_BoundChannel(run, ModSettings::_MT_RunForward.GetValue()));
            }
            mgr->boundChannels.push_back(MT_Ptr::run_ptr);
        }
        if (!has_sprint) {
            if (!MT_Ptr::sprint_ptr) {
                MT_Ptr::sprint_ptr = RE::BSTSmartPointer<RE::BSAnimationGraphChannel>(
                    new HorsePower_BoundChannel(sprint, ModSettings::_MT_Sprint.GetValue()));
            }
            mgr->boundChannels.push_back(MT_Ptr::sprint_ptr);
        }
    }

    [[nodiscard]] static inline bool SetBoundValue(float value, MT_Ptr::Type type) {
        auto *ptr = MT_Ptr::GetPtr(type);
        if (ptr && *ptr) {
            (*ptr)->value = std::bit_cast<std::uint32_t>(value);
            return true;
        }
        return false;  // channel not created yet
    }
}  // namespace HavokUtil
