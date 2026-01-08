#include "Hooks.hpp"
#include "ButtonEventListener.hpp"
#include "DeathEventListener.hpp"
#include "Plugin.h"

using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace plugin {
    std::optional<std::filesystem::path> getLogDirectory() {
        using namespace std::filesystem;
        PWSTR buf;
        SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &buf);
        std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> documentsPath{buf, CoTaskMemFree};
        path directory{documentsPath.get()};
        directory.append("My Games"sv);

        if (exists("steam_api64.dll"sv)) {
            if (exists("openvr_api.dll") || exists("Data/SkyrimVR.esm")) {
                directory.append("Skyrim VR"sv);
            }
            else {
                directory.append("Skyrim Special Edition"sv);
            }
        }
        else if (exists("Galaxy64.dll"sv)) {
            directory.append("Skyrim Special Edition GOG"sv);
        }
        else if (exists("eossdk-win64-shipping.dll"sv)) {
            directory.append("Skyrim Special Edition EPIC"sv);
        }
        else {
            return current_path().append("skselogs");
        }
        return directory.append("SKSE"sv).make_preferred();
    }

    void initializeLogging() {
        auto path = getLogDirectory();
        if (!path) {
            report_and_fail("Can't find SKSE log directory");
        }
        *path /= std::format("{}.log"sv, Plugin::Name);

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        }
        else {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern(PLUGIN_LOGPATTERN_DEFAULT);
    }
}  // namespace plugin

using namespace plugin;

extern "C" DLLEXPORT bool SKSEPlugin_Load(const LoadInterface *skse) {
    initializeLogging();

    LOG("'{} By {}' / Skyrim '{}'", Plugin::Name, Plugin::Author, REL::Module::get().version().string());
    Init(skse, false);

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *msg) {
        switch (msg->type) {
            case SKSE::MessagingInterface::kPostLoad:
                Hooks::AnimationEventHook::InstallAnimEventHook();
                Hooks::NotifyGraphHandler::InstallGraphNotifyHook();
                break;

            case SKSE::MessagingInterface::kDataLoaded:
                Listeners::DeathEventListener::GetSingleton()->Register();
                break;

            case SKSE::MessagingInterface::kPostLoadGame:
                if (RE::PlayerCharacter::GetSingleton()->IsOnMount()) {
                    Listeners::ButtonEventListener::GetSingleton()->Register();
                }

            case SKSE::MessagingInterface::kPreLoadGame:
                if (RE::PlayerCharacter::GetSingleton()->IsOnMount()) {
                    Listeners::ButtonEventListener::GetSingleton()->Unregister();
                }

                break;
        }
    });

    LOG(">_{} loaded_<|", Plugin::Name);
    return true;
}