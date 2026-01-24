#pragma once

namespace Fixes {
    /* This is buggy, spamming the other hand during the wind up animation causes hitting to the other side, after committing to one */
    void FixAttackAnnotationsAndHands(const RE::BSFixedString &a_eventName) {
        if (a_eventName == "blockStart") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMRight";
        }

        else if (a_eventName == "blockStop") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackRelease";
        }

        /* Swap hands, right click to attack right, left click to attack left. Duh */
        else if (a_eventName == "attackStart_MC_1HMLeft") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMRight";
        }
        else if (a_eventName == "attackStart_MC_1HMRight") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMLeft";
        }
        else if (a_eventName == "attackPowerStart_MC_1HMRight") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackPowerStart_MC_1HMLeft";
        }
        else if (a_eventName == "attackPowerStart_MC_1HMLeft") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackPowerStart_MC_1HMRight";
        }
        /* Release events */
        else if (a_eventName == "attackReleaseL") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackRelease";
        }
        else if (a_eventName == "attackRelease") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackReleaseL";
        }
    }

    void FixLeftAttackAnnotationsOnly(const RE::BSFixedString &a_eventName) {
        if (a_eventName == "blockStart") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackStart_MC_1HMLeft";
        }

        else if (a_eventName == "blockStop") {
            const_cast<RE::BSFixedString &>(a_eventName) = "attackReleaseL";
        }
    }
}  // namespace Fixes