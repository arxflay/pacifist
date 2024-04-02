#include "Config.h"

void Config::FromJson(const arx::Json &jsonObject) 
{
    fullscreen = jsonObject["fullscreen"].Get<bool>();
    resX = jsonObject["resX"].Get<int>();
    resY = jsonObject["resY"].Get<int>();
}

void Config::ToJson(arx::Json &jsonObject) const 
{
    jsonObject["fullscreen"] = fullscreen;
    jsonObject["resX"] = resX;
    jsonObject["resY"] = resY;
}

Config *GetGlobalConfig()
{
    static Config cfg;
    return &cfg;
}

