#pragma once
#include "SKSEMenuFramework.h"
#include "ModSettings.h"
#include "Fixes.hpp"
#include "MCM_Translation.h"

namespace ModConfigMenu {

    namespace ms = ModSettings;
    using cached = this_plugin::CachedStrings;
    auto CS = cached::GetSingleton();

/**/ #define CV(x) CS->x.CachedValue.c_str()  // cached value

    void Settings() {
        auto AddSetting = [&](const char *label, const char *desc, bool &value, std::function<void()> onUpdate = nullptr) {
            ImGuiMCP::BeginGroup();
            if (ImGuiMCPComponents::ToggleButton(label, &value)) {
                if (onUpdate) onUpdate();
                ms::g_settingStore.GetSingleton()->Save();
            }
            ImGuiMCP::Text(desc);
            ImGuiMCP::EndGroup();

            ImGuiMCP::Spacing();
            // Pushing a visible color ensures the separator shows up regardless of theme
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Separator, ImGuiMCP::ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGuiMCP::Separator();
            ImGuiMCP::PopStyleColor();
            ImGuiMCP::Spacing();
        };

        bool sprint = ms::SprintInterruption.GetValue();
        AddSetting(CV(cached::mcmSprintInterruptLabel), CV(cached::mcmSprintInterruptDesc), sprint,
                   [&] { ms::SprintInterruption.SetValue(sprint); });

        bool jump = ms::DisableModMovingJumpHeight.GetValue();
        AddSetting(CV(cached::mcmMinJumpClampLabel), CV(cached::mcmMinJumpClampDesc), jump, [&] {
            ms::DisableModMovingJumpHeight.SetValue(jump);
            Fixes::Compatibility::SetModJump();
        });

        bool horseAttacks = ms::ManualHorseAttacks.GetValue();
        AddSetting(CV(cached::mcmHorseAttackLabel), CV(cached::mcmHorseAttackDesc), horseAttacks,
                   [&] { ms::ManualHorseAttacks.SetValue(horseAttacks); });

        bool swap = ms::SwapHands.GetValue();
        AddSetting(CV(cached::mcmSwapInputLabel), CV(cached::mcmSwapInputDesc), swap, [&] {
            ms::SwapHands.SetValue(swap);
            Fixes::Attacks::SetHandSwapping();
        });

        bool petting = ms::ManualPetting.GetValue();
        AddSetting(CV(cached::mcmPettingLabel), CV(cached::mcmPettingDesc), petting, [&] { ms::ManualPetting.SetValue(petting); });

        bool grazing = ms::GrazeSystem.GetValue();
        AddSetting(CV(cached::mcmGrazeLabel), CV(cached::mcmGrazeDesc), grazing, [&] {
            ms::GrazeSystem.SetValue(grazing);
            Util::SyncGlobalGrazeValue(grazing);
        });

        bool sprintKnock = ms::SprintJumpKnock.GetValue();
        AddSetting(CV(cached::mcmSprintKnockLabel), CV(cached::mcmSprintKnockDesc), sprintKnock,
                   [&] { ms::SprintJumpKnock.SetValue(sprintKnock); });

        bool hitToleration = ms::HorseHitToleration.GetValue();
        AddSetting(CV(cached::mcmHitTolerationLabel), CV(cached::mcmHitTolerationDesc), hitToleration,
                   [&] { ms::HorseHitToleration.SetValue(hitToleration); });

#undef CV
    }
    void Experimental() {
        auto AddSetting = [&](const char *label, const char *desc, bool &value, std::function<void()> onUpdate = nullptr) {
            ImGuiMCP::BeginGroup();
            if (ImGuiMCPComponents::ToggleButton(label, &value)) {
                if (onUpdate) onUpdate();
                ms::g_settingStore.GetSingleton()->Save();
            }
            ImGuiMCP::Text(desc);
            ImGuiMCP::EndGroup();

            ImGuiMCP::Spacing();
            // Pushing a visible color ensures the separator shows up regardless of theme
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Separator, ImGuiMCP::ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGuiMCP::Separator();
            ImGuiMCP::PopStyleColor();
            ImGuiMCP::Spacing();
        };
    }
    void Debug() {
        if (ImGuiMCP::Button("Test Feed Message")) {
            std::string msg{"$Horsepower_FeedTutorial"};
            SKSE::Translation::Translate(msg, msg);
            RE::DebugMessageBox(msg.c_str());
        }
    }

    void SetupMenu() {
        if (!SKSEMenuFramework::IsInstalled()) return;
        this_plugin::CachedStrings::Initialize();

        SKSEMenuFramework::SetSection("HorsePower");
        SKSEMenuFramework::AddSectionItem("Settings", Settings);
        // SKSEMenuFramework::AddSectionItem("Experimental", Experimental);
        SKSEMenuFramework::AddSectionItem("Debug", Debug);
    }
}  // namespace ModConfigMenu