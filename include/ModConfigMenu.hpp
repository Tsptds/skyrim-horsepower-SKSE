#pragma once
#include "SKSEMenuFramework.h"
#include "ModSettings.h"
#include "Fixes.hpp"

namespace ModConfigMenu {

    namespace ms = ModSettings;

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
        AddSetting("Sprint Interruption", "Getting too slow with your horse will cause it to rear-up", sprint,
                   [&] { ms::SprintInterruption.SetValue(sprint); });

        bool jump = ms::DisableModMovingJumpHeight.GetValue();
        AddSetting("Allow Lower Jump Heights While Moving",
                   "By default, your minimum jump height is increased. For compatibility with other mods that lower the default jump "
                   "height, keep it off unless you see other issues.",
                   jump, [&] {
                       ms::DisableModMovingJumpHeight.SetValue(jump);
                       Fixes::Compatibility::SetModJump();
                   });

        bool horseAttacks = ms::ManualHorseAttacks.GetValue();
        AddSetting("Horse Attack", "Make your horse attack by pressing both attack buttons at once", horseAttacks,
                   [&] { ms::ManualHorseAttacks.SetValue(horseAttacks); });

        bool swap = ms::SwapHands.GetValue();
        AddSetting("Swap Horseback Attack Inputs", "Left click to attack left, right click to attack right", swap, [&] {
            ms::SwapHands.SetValue(swap);
            Fixes::Attacks::SetHandSwapping();
        });

        bool petting = ms::ManualPetting.GetValue();
        AddSetting("Manual Horseback Petting", "Pet the horse with sneak key on horseback", petting,
                   [&] { ms::ManualPetting.SetValue(petting); });

        bool grazing = ms::GrazeSystem.GetValue();
        AddSetting("Graze System",
                   "Enable the hunger / grazing system. Feed your horse when on grassy surfaces with sneak key. Horses will graze on their "
                   "own as well.",
                   grazing, [&] {
                       ms::GrazeSystem.SetValue(grazing);
                       auto dh = RE::TESDataHandler::GetSingleton();
                       auto form = dh->LookupForm(0x26, "Horsepower.esp");
                       if (form) {
                           auto glbl = form->As<RE::TESGlobal>();
                           if (glbl) {
                               glbl->value = grazing;
                           }
                       }
                   });

        bool sprintKnock = ms::SprintJumpKnock.GetValue();
        AddSetting("Sprint Jump Knock", "Knock actors with a sprinting jump, requires graze level 1 or above if grazing is enabled.",
                   sprintKnock, [&] { ms::SprintJumpKnock.SetValue(sprintKnock); });

        bool hitToleration = ms::HorseHitToleration.GetValue();
        AddSetting("Hit Toleration",
                   "Horses do a rearup after taking hits between 5-20 randomly, including spells. There's a cooldown window to prevent "
                   "stunlock. The horse has to be hit, not the rider.",
                   hitToleration, [&] { ms::HorseHitToleration.SetValue(hitToleration); });
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

        SKSEMenuFramework::SetSection("HorsePower");
        SKSEMenuFramework::AddSectionItem("Settings", Settings);
        // SKSEMenuFramework::AddSectionItem("Experimental", Experimental);
        SKSEMenuFramework::AddSectionItem("Debug", Debug);
    }
}  // namespace ModConfigMenu