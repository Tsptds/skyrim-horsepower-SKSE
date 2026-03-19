#pragma once
#include "SKSEMenuFramework.h"
#include "ModSettings.h"
#include "Fixes.hpp"

namespace ModConfigMenu {

    namespace ms = ModSettings;

    void __stdcall Settings() {
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
        AddSetting("Sprint Interruption", "Getting too slow with your horse will cause it to rear-up", sprint,
                   [&] { ms::SprintInterruption.SetValue(sprint); });

        bool jump = ms::DisableModMovingJumpHeight.GetValue();
        AddSetting("Disable Minimum Moving Jump Height Clamp", "Disable the increased minimum jump height when moving", jump, [&] {
            ms::DisableModMovingJumpHeight.SetValue(jump);
            Fixes::Compatibility::SetModJump();
        });

        bool swap = ms::SwapHands.GetValue();
        AddSetting("Swap Horseback Attack Inputs", "Left click to attack left, right click to attack right", swap, [&] {
            ms::SwapHands.SetValue(swap);
            Fixes::Attacks::SetHandSwapping();
        });

        bool petting = ms::ManualPetting.GetValue();
        AddSetting("Manual Horseback Petting", "Pet the horse with sneak key on horseback", petting,
                   [&] { ms::ManualPetting.SetValue(petting); });
    }
    void __stdcall Experimental() {
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
        bool horseAttacks = ms::ManualHorseAttacks.GetValue();
        AddSetting("Horse Attack", "Make horse attack by pressing both attack buttons", horseAttacks,
                   [&] { ms::ManualHorseAttacks.SetValue(horseAttacks); });
    }

    void SetupMenu() {
        if (!SKSEMenuFramework::IsInstalled()) return;

        SKSEMenuFramework::SetSection("HorsePower");
        SKSEMenuFramework::AddSectionItem("Settings", Settings);
        SKSEMenuFramework::AddSectionItem("Experimental", Experimental);
    }
}  // namespace ModConfigMenu