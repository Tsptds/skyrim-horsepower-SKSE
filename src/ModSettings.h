#pragma once
#include "REX/REX/INI.h"

namespace ModSettings {
    REX::INI::SettingStore g_settingStore;
    inline REX::INI::Bool SprintInterruption{"Settings", "bEnableSprintInterruption", true};
    inline REX::INI::Bool ManualPetting{"Settings", "bManualPetting", true};
    inline REX::INI::Bool SwapHands{"Settings", "bSwapAttackInputs", false};
    inline REX::INI::Bool DisableModMovingJumpHeight{"Settings", "bDisableMovingJumpHeightChange", false};

    void ReadINI() {
        const auto ini = REX::INI::SettingStore::GetSingleton();
        ini->Init("Data/SKSE/Plugins/Horsepower.ini", "Data/SKSE/Plugins/HorsepowerCustom.ini");
        ini->Load();
    }
}