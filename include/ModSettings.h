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
    inline REX::INI::Bool SprintStop{"Settings", "bSprintStop", true};
    inline REX::INI::Bool SlopeSlide{"Settings", "bSlopeSliding", true};

    // inline REX::INI::F32 _MT_WalkSlow{"MovementTypes", "fWalkSlow", 5.f};                        // Not Set
    inline REX::INI::F32 _MT_WalkForward{"MovementTypes", "fWalkForward", 125.112000f};
    // inline REX::INI::F32 _MT_TrotForwardSlow{"MovementTypes", "fTrotForwardSlow", 214.000000f};  // Not Set
    // inline REX::INI::F32 _MT_TrotForward{"MovementTypes", "fTrotForward", 303.906006f};          // Not Set
    inline REX::INI::F32 _MT_RunForward{"MovementTypes", "fRunForward", 500.000000f};
    inline REX::INI::F32 _MT_Sprint{"MovementTypes", "fSprint", 1000.000000f};

    void ReadINI() {
        const auto ini = REX::INI::SettingStore::GetSingleton();
        ini->Init("Data/SKSE/Plugins/Horsepower.ini", "Data/SKSE/Plugins/HorsepowerCustom.ini");
        ini->Load();
    }

}  // namespace ModSettings