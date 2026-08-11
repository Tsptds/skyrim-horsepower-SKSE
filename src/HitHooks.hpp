namespace Hooks {
    struct ProcessHitEvent {
            static void thunk(RE::Actor *a_target, RE::HitData *a_hitData) {
                // INFO("{}", a_target->GetName());
                auto aggressor = a_hitData->aggressor.get();
                if (aggressor && aggressor->IsAMount()) {
                    RE::ActorPtr riderPtr;
                    bool hasRider = aggressor->GetMountedBy(riderPtr);
                    if (hasRider) {
                        // Horse hit rider, stop
                        if (riderPtr.get() == a_target) {
                            return;
                        }
                        else {
                            // Horse has rider and hit someone, swap attacker to rider so combat initiation is with rider
                            // This also fixes not getting bounty for assult
                            a_hitData->aggressor = riderPtr.get();
                        }
                    }
                }

                func(a_target, a_hitData);
            }

            static inline REL::Relocation<decltype(thunk)> func;

            static inline void Install() {
                hooking::write_thunk_call<ProcessHitEvent>(RELOCATION_ID(37673, 38627).address() + REL::Relocate(0x3C0, 0x4A8));
            }
    };

    // struct Hit_CdPointCollector {
    //         static bool thunk(RE::hkpAllCdPointCollector *a_collector, RE::bhkWorld *a_world, RE::NiPoint3 &a_origin,
    //                           RE::NiPoint3 &a_direction, float a_length) {
    //             bool result = func(a_collector, a_world, a_origin, a_direction, a_length);

    //             auto &hits = a_collector->hits;
    //             decltype(hits.size()) writeIdx = 0;

    //             for (decltype(writeIdx) readIdx = 0; readIdx < hits.size(); ++readIdx) {
    //                 auto &hit = hits[readIdx];

    //                 auto attackerRef = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableA);
    //                 auto targetRef = RE::TESHavokUtilities::FindCollidableRef(*hit.rootCollidableB);
    //                 auto attacker = attackerRef ? attackerRef->As<RE::Actor>() : nullptr;
    //                 auto target = targetRef ? targetRef->As<RE::Actor>() : nullptr;

    //                 bool drop = false;
    //                 if (attacker && target && attacker->IsAMount()) {
    //                     RE::ActorPtr riderPtr;
    //                     drop = attacker->GetMountedBy(riderPtr) && riderPtr.get() == target;
    //                 }

    //                 if (drop) continue;
    //                 if (writeIdx != readIdx) hits[writeIdx] = hits[readIdx];
    //                 ++writeIdx;
    //             }
    //             if (!hits.empty()) INFO("Hit smth");
    //             hits.resize(writeIdx);
    //             return result;
    //         }

    //         static inline REL::Relocation<decltype(thunk)> func;

    //         static inline void Install() {
    //             hooking::write_thunk_call<Hit_CdPointCollector>(RELOCATION_ID(37674, 38628).address() + REL::Relocate(0x26A, 0x294));
    //         }
    // };

}  // namespace Hooks