#include "ImageManager.h"
#include <arxengine/ArxException.h>
#include <arxengine/misc/Utils.h>
ImageManager g_manager;

/*static*/ ImageManager &ImageManager::GetImageManager()
{
    return g_manager;
}
bool ImageManager::LoadFromFile(std::string_view name, std::string_view filename)
{
    if (m_images.count(name.data()))
        return false;

    arx::Image image = arx::Image::LoadFromFile(filename);
    return m_images.emplace(name, std::move(image)).second;
}

const arx::Image &ImageManager::Retrieve(std::string_view name)
{
    auto it = m_images.find(name.data());
    if (it == m_images.end())
        throw arx::ArxException(arx::ArxException::ErrorCode::GenericError, arx::Utils::Format("Failed to find image with name '%s'", name.data()));
    return it->second;
}
