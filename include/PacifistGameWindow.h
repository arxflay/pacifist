#ifndef PACIFISTGAMEWINDOW_H
#define PACIFISTGAMEWINDOW_H
#include <arxengine/ui/ArxWindow.h>
#include <map>
#include <arxengine/ui/Monitor.h>
#include <arxengine/ui/Label.h>
#include <arxengine/media/SoundPlayer.h>
class PacifisitGameWindow final : public arx::ArxWindow
{
public:
    PacifisitGameWindow(const arx::SizeF &size = arx::SizeF::DEFAULT_SIZE, const arx::Position &position = arx::constants::DEFAULT_POSITION, int attributes = arx::ArxWindow::DISPLAY_ON_TOP);
private:
    void CreateMainMenu();
    std::vector<arx::ResolutionMode> m_resolutions;
    arx::SoundPlayer m_btnClickPlayer;
    arx::SoundPlayer m_mainMenuMusicPlayer;
    void UpdateResolutions();
    void SetCurrentResolutionText(arx::Label *label);
    long m_resPos;
};
#endif
