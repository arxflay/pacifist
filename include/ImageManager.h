#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H
#include <arxengine/media/Image.h>
#include <map>
#include <optional>
#include <string>
#include <string_view>
class ImageManager final
{
public:
    static ImageManager &GetImageManager();
    bool LoadFromFile(std::string_view name, std::string_view filename);
    const arx::Image &Retrieve(std::string_view name);
private:
    std::map<std::string, arx::Image> m_images;
};

#endif
