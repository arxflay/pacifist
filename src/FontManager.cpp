#include "FontManager.h"
#include <arxengine/ArxException.h>
#include <arxengine/misc/Utils.h>
FontManager g_fontManager;

/*static*/ FontManager &FontManager::GetFontManager()
{
    return g_fontManager;
}

bool FontManager::LoadFromFile(std::string_view name, std::string_view filename)
{
    if (m_fonts.count(name.data()))
        return false;
    
    arx::Font font = arx::Font::LoadFromFile(filename);
    return m_fonts.emplace(name, std::move(font)).second;
}

const arx::Font &FontManager::Retrieve(std::string_view name)
{
    auto it = m_fonts.find(name.data());
    if (it == m_fonts.end())
        throw arx::ArxException(arx::ArxException::ErrorCode::GenericError, arx::Utils::Format("Failed to find font with name '%s'", name.data()));

    return it->second;
}

