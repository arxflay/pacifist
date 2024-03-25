#include "PacifistGame.h"
#include <arxengine/logging/FileLogger.h>
#include <memory>
#include <filesystem>
#include <cstdlib>
#include <arxengine/ui/ArxWindow.h>

std::string CreateLogPath()
{
    std::filesystem::path logPath;
#ifdef WIN32
    logPath = getenv("PROGRAMDATA");
#else
    logPath = getenv("HOME");
    logPath = logPath / std::filesystem::path(".local/share");
#endif
    return logPath.u8string();
}

const std::string &GetLogPath()
{
    static std::string logPath = CreateLogPath();
    return logPath;
}

void PacifistGame::OnRun() 
{
    arx::ArxWindow *window = new arx::ArxWindow("pacifist", arx::SizeF(800, 600));
    window->EnableVSync(true);
    window->Show();
    window->SetWindowAspectRatio(16, 9);
}

IMPLEMENT_GAMEAPP_WITH_LOGGER(PacifistGame, std::make_unique<arx::FileLogger>(arx::Logger::LoggingLevel::Info, GetLogPath()));
