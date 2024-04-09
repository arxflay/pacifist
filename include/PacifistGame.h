/*
 * @author Alexej Fedorenko
 */
#ifndef PACIFISTGAME_H
#define PACIFISTGAME_H
#include <arxengine/GameApp.h>
#include <filesystem>

std::string GetResourcePathStr(std::string_view pathToResource);
const std::string &GetConfigPathStr();

class PacifistGame : public arx::GameApp
{
protected:
    void OnRun() override;
    void OnAfterInit() override;

    void InitResources();
    void LoadConfig();
    void CreateDefaultConfig();
};
#endif
