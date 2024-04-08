#ifndef PACIFISTGAMEWINDOW_H
#define PACIFISTGAMEWINDOW_H
#include <arxengine/ui/ArxWindow.h>
#include <map>
#include <arxengine/ui/Monitor.h>
#include <arxengine/ui/Label.h>
#include <arxengine/media/SoundPlayer.h>
#include <arxengine/ui/KeyEvent.h>
#include <arxengine/ui/ImageControl.h>

class Player;

class PacifisitGameWindow final : public arx::ArxWindow
{
public:
    PacifisitGameWindow(const arx::SizeF &size = arx::SizeF::DEFAULT_SIZE, const arx::Position &position = arx::constants::DEFAULT_POSITION, int attributes = 0);
private:
    void CreateMainMenu();
    void SetUpLevel();
    void StartGame();

    void UpdateResolutions();
    void SetCurrentResolutionText(arx::Label *label);

    void OnKeyDownInGame(arx::KeyDownEvent &e);
    void OnKeyHoldInGame(arx::KeyHoldEvent &e);
    void OnKeyUpInGame(arx::KeyUpEvent &e);
    void OnKeyDownInMenu(arx::KeyDownEvent &e);
    void HandleCharacterMovement(arx::Position newPosition);
    void ReturnToMenu();
    void ShowEndScreen();

    void OnKeyDownInEndScreen(arx::KeyDownEvent &e);

private:
    std::vector<arx::ResolutionMode> m_resolutions;
    arx::SoundPlayer m_btnClickPlayer;
    arx::SoundPlayer m_musicPlayer;

    long long m_resPos;
    arx::ImageControl *m_mainMenu;
    arx::UIControl *m_gameScene;
    arx::UIControl *m_endScreen;
    Player *m_character;
};
#endif
