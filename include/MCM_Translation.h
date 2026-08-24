#pragma once

namespace this_plugin {
    namespace translation = SKSE::Translation;

    template <const char *KeyStr>
    class TranslationKey {
        public:
            static constexpr const char *Key = KeyStr;
            std::string CachedValue = KeyStr;
    };

    inline constexpr char s_UpStage1[] = "$UpStage1";
    inline constexpr char s_UpStage3[] = "$UpStage3";
    inline constexpr char s_UpStage4[] = "$UpStage4";
    inline constexpr char s_UpStage5[] = "$UpStage5";
    inline constexpr char s_DownStage2[] = "$DownStage2";
    inline constexpr char s_DownStage1[] = "$DownStage1";
    inline constexpr char s_DownStage0[] = "$DownStage0";
    inline constexpr char s_HorsepowerFeedTutorial[] = "$Horsepower_FeedTutorial";
    inline constexpr char s_HorsepowerFeedTutorialHeader[] = "$Horsepower_FeedTutorialHeader";
    inline constexpr char s_MCMSprintInterruptLabel[] = "$MCM_SprintInterrupt_Label";
    inline constexpr char s_MCMSprintInterruptDesc[] = "$MCM_SprintInterrupt_Desc";
    inline constexpr char s_MCMMinJumpClampLabel[] = "$MCM_MinJumpClamp_Label";
    inline constexpr char s_MCMMinJumpClampDesc[] = "$MCM_MinJumpClamp_Desc";
    inline constexpr char s_MCMHorseAttackLabel[] = "$MCM_HorseAttack_Label";
    inline constexpr char s_MCMHorseAttackDesc[] = "$MCM_HorseAttack_Desc";
    inline constexpr char s_MCMSwapInputLabel[] = "$MCM_SwapInput_Label";
    inline constexpr char s_MCMSwapInputDesc[] = "$MCM_SwapInput_Desc";
    inline constexpr char s_MCMPettingLabel[] = "$MCM_Petting_Label";
    inline constexpr char s_MCMPettingDesc[] = "$MCM_Petting_Desc";
    inline constexpr char s_MCMGrazeLabel[] = "$MCM_Graze_Label";
    inline constexpr char s_MCMGrazeDesc[] = "$MCM_Graze_Desc";
    inline constexpr char s_MCMSprintKnockLabel[] = "$MCM_SprintKnock_Label";
    inline constexpr char s_MCMSprintKnockDesc[] = "$MCM_SprintKnock_Desc";
    inline constexpr char s_MCMHitTolerationLabel[] = "$MCM_HitToleration_Label";
    inline constexpr char s_MCMHitTolerationDesc[] = "$MCM_HitToleration_Desc";
    inline constexpr char s_MCMAllowRagdollLabel[] = "$MCM_AllowRagdoll_Label";
    inline constexpr char s_MCMAllowRagdollDesc[] = "$MCM_AllowRagdoll_Desc";

    class CachedStrings {
        public:
            static CachedStrings *GetSingleton() {
                static CachedStrings instance;
                return &instance;
            }

            TranslationKey<s_UpStage1> upStage1;
            TranslationKey<s_UpStage3> upStage3;
            TranslationKey<s_UpStage4> upStage4;
            TranslationKey<s_UpStage5> upStage5;
            TranslationKey<s_DownStage2> downStage2;
            TranslationKey<s_DownStage1> downStage1;
            TranslationKey<s_DownStage0> downStage0;
            TranslationKey<s_HorsepowerFeedTutorial> horsepowerFeedTutorial;
            TranslationKey<s_HorsepowerFeedTutorialHeader> horsepowerFeedTutorialHeader;
            TranslationKey<s_MCMSprintInterruptLabel> mcmSprintInterruptLabel;
            TranslationKey<s_MCMSprintInterruptDesc> mcmSprintInterruptDesc;
            TranslationKey<s_MCMMinJumpClampLabel> mcmMinJumpClampLabel;
            TranslationKey<s_MCMMinJumpClampDesc> mcmMinJumpClampDesc;
            TranslationKey<s_MCMHorseAttackLabel> mcmHorseAttackLabel;
            TranslationKey<s_MCMHorseAttackDesc> mcmHorseAttackDesc;
            TranslationKey<s_MCMSwapInputLabel> mcmSwapInputLabel;
            TranslationKey<s_MCMSwapInputDesc> mcmSwapInputDesc;
            TranslationKey<s_MCMPettingLabel> mcmPettingLabel;
            TranslationKey<s_MCMPettingDesc> mcmPettingDesc;
            TranslationKey<s_MCMGrazeLabel> mcmGrazeLabel;
            TranslationKey<s_MCMGrazeDesc> mcmGrazeDesc;
            TranslationKey<s_MCMSprintKnockLabel> mcmSprintKnockLabel;
            TranslationKey<s_MCMSprintKnockDesc> mcmSprintKnockDesc;
            TranslationKey<s_MCMHitTolerationLabel> mcmHitTolerationLabel;
            TranslationKey<s_MCMHitTolerationDesc> mcmHitTolerationDesc;
            TranslationKey<s_MCMAllowRagdollLabel> mcmAllowRagdollLabel;
            TranslationKey<s_MCMAllowRagdollDesc> mcmAllowRagdollDesc;

            inline static void Initialize() {
                auto Fetch = [](auto &target) { translation::Translate(target.Key, target.CachedValue); };
                auto cs = CachedStrings::GetSingleton();
                Fetch(cs->upStage1);
                Fetch(cs->upStage3);
                Fetch(cs->upStage4);
                Fetch(cs->upStage5);
                Fetch(cs->downStage2);
                Fetch(cs->downStage1);
                Fetch(cs->downStage0);
                Fetch(cs->horsepowerFeedTutorial);
                Fetch(cs->horsepowerFeedTutorialHeader);
                Fetch(cs->mcmSprintInterruptLabel);
                Fetch(cs->mcmSprintInterruptDesc);
                Fetch(cs->mcmMinJumpClampLabel);
                Fetch(cs->mcmMinJumpClampDesc);
                Fetch(cs->mcmHorseAttackLabel);
                Fetch(cs->mcmHorseAttackDesc);
                Fetch(cs->mcmSwapInputLabel);
                Fetch(cs->mcmSwapInputDesc);
                Fetch(cs->mcmPettingLabel);
                Fetch(cs->mcmPettingDesc);
                Fetch(cs->mcmGrazeLabel);
                Fetch(cs->mcmGrazeDesc);
                Fetch(cs->mcmSprintKnockLabel);
                Fetch(cs->mcmSprintKnockDesc);
                Fetch(cs->mcmHitTolerationLabel);
                Fetch(cs->mcmHitTolerationDesc);
                Fetch(cs->mcmAllowRagdollLabel);
                Fetch(cs->mcmAllowRagdollDesc);
            }

        private:
            CachedStrings() = default;
    };
}  // namespace this_plugin