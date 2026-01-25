#pragma once
#include "REX/REX/INI.h"

namespace ModSettings {
    REX::INI::SettingStore g_settingStore;
    inline REX::INI::Bool SwapHands{"Settings", "bSwapAttackInputs", true};
}