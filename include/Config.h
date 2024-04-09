/*
 * @author Alexej Fedorenko
 */
#ifndef CONFIG_H
#define CONFIG_H
#include <arxengine/Json.h>

struct Config : public arx::IJsonObject
{
    void FromJson(const arx::Json &jsonObject) override;
    void ToJson(arx::Json &jsonObject) const override;
    bool fullscreen;
    int resX;
    int resY;
};

Config *GetGlobalConfig();

#endif
