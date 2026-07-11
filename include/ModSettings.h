#pragma once
#include "REX/REX/INI.h"

namespace ModSettings {
    REX::INI::SettingStore g_settingStore;
    inline REX::INI::Bool SprintInterruption{"Settings", "bEnableSprintInterruption", true};
    inline REX::INI::Bool ManualPetting{"Settings", "bManualPetting", true};
    inline REX::INI::Bool ManualHorseAttacks{"Settings", "bEnableManualHorseAttacks", true};
    inline REX::INI::Bool SwapHands{"Settings", "bSwapAttackInputs", false};
    inline REX::INI::Bool DisableModMovingJumpHeight{"Settings", "bDisableMovingJumpHeightChange", false};
    inline REX::INI::Bool GrazeSystem{"Settings", "bGrazeSystem", true};
    inline REX::INI::Bool SprintJumpKnock{"Settings", "bSprintJumpKnock", true};
    inline REX::INI::Bool HorseHitToleration{"Settings", "bHorseHitToleration", true};
    inline REX::INI::Bool RemoveNoKnockdownFlag{"Settings", "bRemoveNoKnockdownFlag", true};

    void ReadINI() {
        const auto ini = REX::INI::SettingStore::GetSingleton();
        ini->Init("Data/SKSE/Plugins/Horsepower.ini", "Data/SKSE/Plugins/HorsepowerCustom.ini");
        ini->Load();
    }
}  // namespace ModSettings