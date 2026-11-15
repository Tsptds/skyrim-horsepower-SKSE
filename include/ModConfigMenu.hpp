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

    void ShowTooltip(const char *text) {
        if (ImGuiMCP::IsItemHovered()) {
            ImGuiMCP::SetTooltip("%s", text);
        }
    }
    void Settings() {
        auto Divider = [] {
            ImGuiMCP::Spacing();
            // Pushing a visible color ensures the separator shows up regardless of theme
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Separator, ImGuiMCP::ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            ImGuiMCP::Separator();
            ImGuiMCP::PopStyleColor();
            ImGuiMCP::Spacing();
        };

        auto AddToggle = [&](const char *label, const char *desc, auto &setting, std::function<void(bool)> onUpdate = nullptr) {
            bool value = setting.GetValue();
            ImGuiMCP::BeginGroup();
            if (ImGuiMCP::Checkbox(label, &value)) {
                setting.SetValue(value);
                if (onUpdate) onUpdate(value);
                ms::g_settingStore.GetSingleton()->Save();
            }
            if (desc) ShowTooltip(desc);
            ImGuiMCP::EndGroup();
            Divider();
        };

        auto AddButton = [&](const char *label, const char *desc, std::function<void()> onClick, float width = 0.0f,
                             bool saveAfter = true) {
            ImGuiMCP::BeginGroup();
            if (ImGuiMCP::Button(label, ImGuiMCP::ImVec2(width, 0.0f))) {
                if (onClick) onClick();
                if (saveAfter) ms::g_settingStore.GetSingleton()->Save();
            }
            if (desc) ShowTooltip(desc);
            ImGuiMCP::EndGroup();
            Divider();
        };

        auto AddMTSlider = [&](const char *label, const char *desc, auto &setting, MT_Ptr::Type type, float min, float max) {
            float value = setting.GetValue();
            ImGuiMCP::BeginGroup();
            ImGuiMCP::SetNextItemWidth(250.0f);
            if (ImGuiMCP::SliderFloat(label, &value, min, max, "%.2f", 0)) {
                if (HavokUtil::SetBoundValue(value, type)) {
                    setting.SetValue(value);
                }
            }
            // Save once when the drag ends instead of every frame
            if (ImGuiMCP::IsItemDeactivatedAfterEdit()) {
                ms::g_settingStore.GetSingleton()->Save();
            }
            if (desc) ShowTooltip(desc);
            ImGuiMCP::EndGroup();
            Divider();
        };

        if (ImGuiMCP::BeginTabBar("##SettingsTabs", 0)) {
            if (ImGuiMCP::BeginTabItem(CV(cached::mcmTabMovement), nullptr, 0)) {
                AddToggle(CV(cached::mcmSprintInterruptLabel), CV(cached::mcmSprintInterruptDesc), ms::SprintInterruption);
                AddToggle(CV(cached::mcmSprintStopLabel), CV(cached::mcmSprintStopDesc), ms::SprintStop);
                AddToggle(CV(cached::mcmSlopeSlideLabel), CV(cached::mcmSlopeSlideDesc), ms::SlopeSlide);
                AddToggle(CV(cached::mcmMinJumpClampLabel), CV(cached::mcmMinJumpClampDesc), ms::DisableModMovingJumpHeight,
                          [](bool) { Fixes::Compatibility::SetModJump(); });
                ImGuiMCP::EndTabItem();
            }

            if (ImGuiMCP::BeginTabItem(CV(cached::mcmTabCombat), nullptr, 0)) {
                AddToggle(CV(cached::mcmHorseAttackLabel), CV(cached::mcmHorseAttackDesc), ms::ManualHorseAttacks);
                AddToggle(CV(cached::mcmSwapInputLabel), CV(cached::mcmSwapInputDesc), ms::SwapHands,
                          [](bool) { Fixes::Attacks::SetHandSwapping(); });
                AddToggle(CV(cached::mcmSprintKnockLabel), CV(cached::mcmSprintKnockDesc), ms::SprintJumpKnock);
                AddToggle(CV(cached::mcmHitTolerationLabel), CV(cached::mcmHitTolerationDesc), ms::HorseHitToleration);
                AddToggle(CV(cached::mcmAllowRagdollLabel), CV(cached::mcmAllowRagdollDesc), ms::RemoveNoKnockdownFlag);
                ImGuiMCP::EndTabItem();
            }

            if (ImGuiMCP::BeginTabItem(CV(cached::mcmTabInteract), nullptr, 0)) {
                AddToggle(CV(cached::mcmPettingLabel), CV(cached::mcmPettingDesc), ms::ManualPetting);
                AddToggle(CV(cached::mcmGrazeLabel), CV(cached::mcmGrazeDesc), ms::GrazeSystem,
                          [](bool on) { Util::SyncGlobalGrazeValue(on); });
                ImGuiMCP::EndTabItem();
            }

            if (ImGuiMCP::BeginTabItem(CV(cached::mcmTabAnim), nullptr, 0)) {
                AddMTSlider(CV(cached::mcmWalkSpeedLabel), CV(cached::mcmWalkSpeedDesc), ModSettings::_MT_WalkForward, MT_Ptr::Type::kWalk,
                            10.0f, 210.f);
                AddMTSlider(CV(cached::mcmRunSpeedLabel), CV(cached::mcmRunSpeedDesc), ModSettings::_MT_RunForward, MT_Ptr::Type::kRun,
                            310.f, ModSettings::_MT_Sprint.GetValue() - 50.f);
                AddMTSlider(CV(cached::mcmSprintSpeedLabel), CV(cached::mcmSprintSpeedDesc), ModSettings::_MT_Sprint, MT_Ptr::Type::kSprint,
                            ModSettings::_MT_RunForward.GetValue() + 100.f, 2000.f);

                AddButton(
                    CV(cached::mcmResetSpeedMatchLabel), nullptr,
                    [] {
                        const float kWalkDefault = ModSettings::_MT_WalkForward.GetValueDefault();
                        const float kRunDefault = ModSettings::_MT_RunForward.GetValueDefault();
                        const float kSprintDefault = ModSettings::_MT_Sprint.GetValueDefault();

                        if (HavokUtil::SetBoundValue(kWalkDefault, MT_Ptr::Type::kWalk)) {
                            ModSettings::_MT_WalkForward.SetValue(kWalkDefault);
                        }
                        if (HavokUtil::SetBoundValue(kRunDefault, MT_Ptr::Type::kRun)) {
                            ModSettings::_MT_RunForward.SetValue(kRunDefault);
                        }
                        if (HavokUtil::SetBoundValue(kSprintDefault, MT_Ptr::Type::kSprint)) {
                            ModSettings::_MT_Sprint.SetValue(kSprintDefault);
                        }
                    },
                    160.0f);

                ImGuiMCP::EndTabItem();
            }

            ImGuiMCP::EndTabBar();
        }
    }
    void Experimental() {
        auto AddSetting = [&](const char *label, const char *desc, bool &value, std::function<void()> onUpdate = nullptr) {
            ImGuiMCP::BeginGroup();
            if (ImGuiMCPComponents::ToggleButton(label, &value)) {
                if (onUpdate) onUpdate();
                ModSettings::g_settingStore.GetSingleton()->Save();
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
        SKSEMenuFramework::AddSectionItem(CV(cached::mcmSectSettings), Settings);
        // SKSEMenuFramework::AddSectionItem("Experimental", Experimental);
        SKSEMenuFramework::AddSectionItem(CV(cached::mcmSectDebug), Debug);
    }

/**/ #undef CV
}  // namespace ModConfigMenu