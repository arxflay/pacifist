#include "PacifistGame.h"
#include <arxengine/logging/FileLogger.h>
#include <memory>
#include <filesystem>
#include <cstdlib>
#include "PacifistGameWindow.h"
#include "FontManager.h"
#include "ImageManager.h"
#include "Config.h"
#include <arxengine/ui/Monitor.h>
#include <fstream>


std::filesystem::path GetGameConfigPath()
{
    std::filesystem::path gameDataPath ;
#ifdef WIN32
    gameDataPath = getenv("PROGRAMDATA");
#else
    gameDataPath = getenv("HOME");
    gameDataPath = gameDataPath / std::filesystem::path(".local/share");
#endif
    gameDataPath /= "pacifist";
    return gameDataPath;
}

std::filesystem::path GetLogPath()
{
    std::filesystem::path logPath = (GetGameConfigPath() / "log.txt");
    return logPath;
}

#ifndef PAC_USE_RELATIVE_PATH
const std::filesystem::path &GetResourcesPath()
{
    #ifdef WIN32
    //C:ProgramFiles/Pacifist/bin/pacifist.exe -> C:ProgramFiles/Pacifist/resources
    static std::filesystem::path path = (std::filesystem::path(arx::Utils::GetExecutablePath()).parent_path().parent_path() / "resources");
    #else
    //usr/bin/pacifist.exe -> ../share/pacifist/resources/
    static std::filesystem::path path = (std::filesystem::path(arx::Utils::GetExecutablePath()).parent_path().parent_path() / "share/pacifist/resources");
    #endif
    return path;
}

const std::string &GetConfigPathStr()
{   
    static std::string configPath = (GetGameConfigPath() / "config.json").u8string();
    return configPath;
}
#else
const std::string &GetConfigPathStr()
{
    static std::string configPath = "config.json";
    return configPath;
}

const std::filesystem::path &GetResourcesPath() { 
    static std::filesystem::path path("resources");
    return path;
}
#endif

std::string GetResourcePathStr(std::string_view pathToResource)
{
    return (GetResourcesPath() / pathToResource).u8string();
}

void PacifistGame::OnAfterInit()
{
    if (!std::filesystem::exists(GetConfigPathStr())) 
        CreateDefaultConfig();
    else
        LoadConfig();

    InitResources();
}

void PacifistGame::InitResources()
{
    FontManager::GetFontManager().LoadFromFile("neon", GetResourcePathStr("fonts/neon_pixel-7.ttf"));
    FontManager::GetFontManager().LoadFromFile("dejavu_serif", GetResourcePathStr("fonts/DejaVuSerif.ttf"));
    ImageManager::GetImageManager().LoadFromFile("check_btn", GetResourcePathStr("images/ui/check_btn.jpg"));
    ImageManager::GetImageManager().LoadFromFile("check_hover_btn", GetResourcePathStr("images/ui/check_hover_btn.jpg"));
    ImageManager::GetImageManager().LoadFromFile("check_clicked_btn", GetResourcePathStr("images/ui/check_clicked_btn.jpg"));
    ImageManager::GetImageManager().LoadFromFile("check_btn_checked", GetResourcePathStr("images/ui/check_btn_checked.jpg"));
    ImageManager::GetImageManager().LoadFromFile("check_hover_btn_checked", GetResourcePathStr("images/ui/check_hover_btn_checked.jpg"));
    ImageManager::GetImageManager().LoadFromFile("check_clicked_btn_checked", GetResourcePathStr("images/ui/check_clicked_btn_checked.jpg"));
    ImageManager::GetImageManager().LoadFromFile("togglable_btn", GetResourcePathStr("images/ui/togglable_btn.jpg"));
    ImageManager::GetImageManager().LoadFromFile("togglable_hover_btn", GetResourcePathStr("images/ui/togglable_hover_btn.jpg"));
    ImageManager::GetImageManager().LoadFromFile("character_move_0", GetResourcePathStr("images/character/character_move_0.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_1", GetResourcePathStr("images/character/character_move_1.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_2", GetResourcePathStr("images/character/character_move_2.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_3", GetResourcePathStr("images/character/character_move_3.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_4", GetResourcePathStr("images/character/character_move_4.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_5", GetResourcePathStr("images/character/character_move_5.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_6", GetResourcePathStr("images/character/character_move_6.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_0_left", GetResourcePathStr("images/character/character_move_0_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_1_left", GetResourcePathStr("images/character/character_move_1_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_2_left", GetResourcePathStr("images/character/character_move_2_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_3_left", GetResourcePathStr("images/character/character_move_3_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_4_left", GetResourcePathStr("images/character/character_move_4_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_5_left", GetResourcePathStr("images/character/character_move_5_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_move_6_left", GetResourcePathStr("images/character/character_move_6_left.png"));

    ImageManager::GetImageManager().LoadFromFile("character_idle_0", GetResourcePathStr("images/character/character_idle_0.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_1", GetResourcePathStr("images/character/character_idle_1.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_2", GetResourcePathStr("images/character/character_idle_2.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_3", GetResourcePathStr("images/character/character_idle_3.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_4", GetResourcePathStr("images/character/character_idle_4.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_5", GetResourcePathStr("images/character/character_idle_5.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_6", GetResourcePathStr("images/character/character_idle_6.png"));

    ImageManager::GetImageManager().LoadFromFile("character_idle_0_left", GetResourcePathStr("images/character/character_idle_0_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_1_left", GetResourcePathStr("images/character/character_idle_1_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_2_left", GetResourcePathStr("images/character/character_idle_2_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_3_left", GetResourcePathStr("images/character/character_idle_3_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_4_left", GetResourcePathStr("images/character/character_idle_4_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_5_left", GetResourcePathStr("images/character/character_idle_5_left.png"));
    ImageManager::GetImageManager().LoadFromFile("character_idle_6_left", GetResourcePathStr("images/character/character_idle_6_left.png"));
}

bool IsValidAspectRatio(Config *cfg)
{
    return arx::AreNumsEqual<float>(static_cast<float>(cfg->resX) / static_cast<float>(cfg->resY), 16.0f / 9.0f);
}

void PacifistGame::LoadConfig()
{
    Config *cfg = GetGlobalConfig();
    arx::Json j(arx::Json::FromFile(GetConfigPathStr()));
    cfg->FromJson(j);
    arx::ResolutionMode resMode = arx::Monitor::GetPrimaryMonitor().GetResolutionMode();
    
    if (cfg->resX > resMode.GetSize().width || cfg->resY > resMode.GetSize().height || !IsValidAspectRatio(cfg))
    {
        auto resolutions = arx::Monitor::GetPrimaryMonitor().GetResolutions(arx::Monitor::ResolutionFilter().SetRetrieveWithMaxRefreshRate(true).SetAspectRatio(16, 9));
        if (!resolutions.empty())
        {
            arx::ResolutionMode &resMode = resolutions.back();
            arx::SizeI res = resMode.GetSize();
            cfg->resX = res.width;
            cfg->resY = res.height;
        }
        else
        {
            cfg->resX = 1024;
            cfg->resY = 576;
        }
    }
}

void PacifistGame::CreateDefaultConfig()
{
    try
    {
        arx::ResolutionMode resMode = arx::Monitor::GetPrimaryMonitor().GetResolutionMode();
        Config *cfg = GetGlobalConfig();
        cfg->fullscreen = false;
        cfg->resX = resMode.GetSize().width;
        cfg->resY = resMode.GetSize().height;
        arx::Json j;
        cfg->ToJson(j);
        std::ofstream file;
        file.open(GetConfigPathStr());
        if (!file)
        {
            GLOG->Error("Failed to create default config.json");
            return;
        }
        file << arx::Json::ToJsonStr(j);
        file.close();
    }
    catch(...)
    {
        arx::UniversalExceptionHandler::HandleException();
    }
}

void PacifistGame::OnRun() 
{
    arx::SizeF res = arx::SizeF(static_cast<float>(GetGlobalConfig()->resX), static_cast<float>(GetGlobalConfig()->resY));
    PacifisitGameWindow *window = new PacifisitGameWindow(res);
    window->SetFullscreen(GetGlobalConfig()->fullscreen);
    window->EnableVSync(true);
    window->Center();
    window->GetEventManager().Bind<arx::ShowEvent>([window, res](arx::ShowEvent &e){
        window->SetSize(res);
        e.Skip();
    });
    window->Show();
}

IMPLEMENT_GAMEAPP_WITH_LOGGER(PacifistGame, std::make_unique<arx::FileLogger>(arx::Logger::LoggingLevel::Info, []() -> std::string {
        if (!std::filesystem::exists(GetGameConfigPath()))
                std::filesystem::create_directory(GetGameConfigPath());
        return GetLogPath().u8string();
    }() )
);

