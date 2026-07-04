#pragma once

namespace this_plugin {
    namespace translation = SKSE::Translation;

    template <const char *KeyStr>
    class TranslationKey {
        public:
            static constexpr const char *Key = KeyStr;
            std::string CachedValue;
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

    class CachedStrings {
        public:
            inline static TranslationKey<s_UpStage1> upStage1;
            inline static TranslationKey<s_UpStage3> upStage3;
            inline static TranslationKey<s_UpStage4> upStage4;
            inline static TranslationKey<s_UpStage5> upStage5;
            inline static TranslationKey<s_DownStage2> downStage2;
            inline static TranslationKey<s_DownStage1> downStage1;
            inline static TranslationKey<s_DownStage0> downStage0;
            inline static TranslationKey<s_HorsepowerFeedTutorial> horsepowerFeedTutorial;
            inline static TranslationKey<s_HorsepowerFeedTutorialHeader> horsepowerFeedTutorialHeader;
            inline static TranslationKey<s_MCMSprintInterruptLabel> mcmSprintInterruptLabel;
            inline static TranslationKey<s_MCMSprintInterruptDesc> mcmSprintInterruptDesc;
            inline static TranslationKey<s_MCMMinJumpClampLabel> mcmMinJumpClampLabel;
            inline static TranslationKey<s_MCMMinJumpClampDesc> mcmMinJumpClampDesc;
            inline static TranslationKey<s_MCMHorseAttackLabel> mcmHorseAttackLabel;
            inline static TranslationKey<s_MCMHorseAttackDesc> mcmHorseAttackDesc;
            inline static TranslationKey<s_MCMSwapInputLabel> mcmSwapInputLabel;
            inline static TranslationKey<s_MCMSwapInputDesc> mcmSwapInputDesc;
            inline static TranslationKey<s_MCMPettingLabel> mcmPettingLabel;
            inline static TranslationKey<s_MCMPettingDesc> mcmPettingDesc;
            inline static TranslationKey<s_MCMGrazeLabel> mcmGrazeLabel;
            inline static TranslationKey<s_MCMGrazeDesc> mcmGrazeDesc;
            inline static TranslationKey<s_MCMSprintKnockLabel> mcmSprintKnockLabel;
            inline static TranslationKey<s_MCMSprintKnockDesc> mcmSprintKnockDesc;
            inline static TranslationKey<s_MCMHitTolerationLabel> mcmHitTolerationLabel;
            inline static TranslationKey<s_MCMHitTolerationDesc> mcmHitTolerationDesc;

            inline static void Initialize() {
                auto Fetch = [](auto &target) { translation::Translate(target.Key, target.CachedValue); };

                Fetch(upStage1);
                Fetch(upStage3);
                Fetch(upStage4);
                Fetch(upStage5);
                Fetch(downStage2);
                Fetch(downStage1);
                Fetch(downStage0);
                Fetch(horsepowerFeedTutorial);
                Fetch(horsepowerFeedTutorialHeader);
                Fetch(mcmSprintInterruptLabel);
                Fetch(mcmSprintInterruptDesc);
                Fetch(mcmMinJumpClampLabel);
                Fetch(mcmMinJumpClampDesc);
                Fetch(mcmHorseAttackLabel);
                Fetch(mcmHorseAttackDesc);
                Fetch(mcmSwapInputLabel);
                Fetch(mcmSwapInputDesc);
                Fetch(mcmPettingLabel);
                Fetch(mcmPettingDesc);
                Fetch(mcmGrazeLabel);
                Fetch(mcmGrazeDesc);
                Fetch(mcmSprintKnockLabel);
                Fetch(mcmSprintKnockDesc);
                Fetch(mcmHitTolerationLabel);
                Fetch(mcmHitTolerationDesc);
            }
    };
}  // namespace this_plugin