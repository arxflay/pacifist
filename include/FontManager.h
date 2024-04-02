#ifndef FONTMANAGER_H
#define FONTMANAGER_H
#include <arxengine/media/Font.h>
#include <map>
#include <optional>
#include <string>
#include  <string_view>
class FontManager final
{
public:
    static FontManager &GetFontManager();
    bool LoadFromFile(std::string_view name, std::string_view filename);
    const arx::Font &Retrieve(std::string_view name);
private:
    std::map<std::string, arx::Font> m_fonts;
};

#endif
