#pragma once
#include "REX/REX/INI.h"

namespace ModSettings {
    REX::INI::SettingStore g_settingStore;
    inline REX::INI::Bool SwapHands{"Settings", "bSwapAttackInputs", true};

    void ReadINI() {
        const auto ini = REX::INI::SettingStore::GetSingleton();
        ini->Init("Data/SKSE/Plugins/Horsepower.ini", "Data/SKSE/Plugins/HorsepowerCustom.ini");
        ini->Load();
    }
}