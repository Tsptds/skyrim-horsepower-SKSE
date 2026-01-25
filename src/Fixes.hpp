#pragma once
#include "REX/REX/INI.h"
#include "ModSettings.h"

namespace Fixes {
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

    using fix_t = void (*)(const RE::BSFixedString &a_eventName);
    fix_t ApplyFix = nullptr;

    void ReadINI() {
        const auto ini = REX::INI::SettingStore::GetSingleton();
        ini->Init("Data/SKSE/Plugins/Horsepower.ini", "Data/SKSE/Plugins/HorsepowerCustom.ini");
        ini->Load();

        bool swappingHands = ModSettings::SwapHands.GetValue();
        ApplyFix = swappingHands ? FixAttackAnnotationsAndHands : FixLeftAttackAnnotationsOnly;

        LOG("Left Hand Attack Event Fix Installed, Swapped Attack Inputs: {}", swappingHands);
    }
}  // namespace Fixes