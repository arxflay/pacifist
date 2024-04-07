#include <arxengine/ui/ImageControl.h>
#include <arxengine/media/Image.h>
#include <arxengine/misc/Utils.h>
#include <arxengine/ui/BitmapButton.h>
#include <arxengine/Timer.h>
#include "PacifistGameWindow.h"
#include "PacifistGame.h"
#include "FontManager.h"
#include "ImageManager.h"
#include "Config.h"
#include "CheckBox.h"
#include "Player.h"
#include <arxengine/ui/Painter.h>
#include <arxengine/ui/Label.h>
#include <arxengine/media/Sound.h>
#include <arxengine/ui/KeyEvent.h>

#include <fstream>
#include <iostream>
#include <map>
#include <thread>

class CollisionEvent : public arx::Event
{
private:
    void HandleEvent() override {}
};

class CollisionCheckEvent : public arx::Event
{
public:
    CollisionCheckEvent(const std::chrono::time_point<std::chrono::steady_clock> &collisionCheckerTimePoint, arx::UIControl **checkableObject)
        : m_collisionCheckerTimePoint(collisionCheckerTimePoint)
        , m_collisionCheckEventTimePoint(collisionCheckerTimePoint)
        , m_checkableObject(checkableObject)
    {
    }

private:
    void HandleEvent() override
    {
        if (m_collisionCheckerTimePoint != m_collisionCheckEventTimePoint)
            return;
        
        arx::UIObject *sender = static_cast<arx::UIObject*>(GetSender());
        arx::UIControl *checkableObject = *m_checkableObject;

        if (checkableObject != nullptr && sender->GetOwnerUIControl()->GetRect().IntersectWith((checkableObject)->GetRect()))
        {
            CollisionEvent collisionEvt;
            sender->GetEventManager().ProcessEvent<CollisionEvent>(collisionEvt);
        } 
        ScheduleAfterProcessing(true);
    }

    const std::chrono::time_point<std::chrono::steady_clock> &m_collisionCheckerTimePoint;
    std::chrono::time_point<std::chrono::steady_clock> m_collisionCheckEventTimePoint;
    arx::UIControl **m_checkableObject;
};

class CollisionChecker : public arx::UIObject
{
public:
    CollisionChecker(arx::UIControl *parent)
        : arx::UIObject(parent)
        , m_checkable(nullptr)
    {
        m_collisionCheckerTimePoint = std::chrono::steady_clock::now();
        GetEventManager().QueueEvent<CollisionCheckEvent>(std::make_unique<CollisionCheckEvent>(m_collisionCheckerTimePoint, &m_checkable));
    }

    void SetCheckableObject(arx::UIControl *checkable)
    {
        m_checkable = checkable;
    }

    void SetCanCollide(bool canCollide)
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        m_collisionCheckerTimePoint = std::chrono::steady_clock::now();
        if (canCollide)
            GetEventManager().QueueEvent<CollisionCheckEvent>(std::make_unique<CollisionCheckEvent>(m_collisionCheckerTimePoint, &m_checkable));
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> m_collisionCheckerTimePoint;
    arx::UIControl *m_checkable;
};

class TriggerControl : public arx::UIControl
{
public:
    TriggerControl(arx::UIControl *parent)
        : arx::UIControl(parent)
        , m_enableDraw(false)
    {
        m_collisionChecker = new CollisionChecker(this);
        SetBackgroundColor(arx::Color(0xFF, 0, 0));
    }

    void EnableDraw(bool enable)
    {
        m_enableDraw = enable;
    }

    CollisionChecker *GetCollisonChecker()
    {
        return m_collisionChecker;
    }
private:
    void OnDraw(arx::DrawEvent &e) override 
    {
        if (m_enableDraw)
        {
            arx::Painter p(e);
            p.SetBrush(arx::Brush(GetColor()));
            p.DrawRectangle(arx::Position(0, 0), GetSize());
        }
    }

    CollisionChecker *m_collisionChecker; 
    bool m_enableDraw;
};

PacifisitGameWindow::PacifisitGameWindow(const arx::SizeF &size, const arx::Position &position, int attributes)
    : arx::ArxWindow("Pacifist", size, position, attributes)
{
    SetBackgroundColor(arx::constants::COLOR_BLACK); 
    SetFixedViewport(640.0f, 360.0f);
    CreateMainMenu();
    m_btnClickPlayer.LoadSound(arx::Sound::LoadFromFile(GetResourcePathStr("sound/click.wav"), arx::Sound::Format::UncompressedWAV));
    m_musicPlayer.LoadSound(arx::Sound::LoadFromFile(GetResourcePathStr("sound/music_menu.wav"), arx::Sound::Format::UncompressedWAV));
    m_musicPlayer.SetVolumeInPercentage(30.0f);
    m_musicPlayer.Play(arx::SoundPlayer::PlayMode::Async, true);
    GetEventManager().Bind<arx::KeyDownEvent>(&PacifisitGameWindow::OnKeyDownInMenu, this);
}

void PacifisitGameWindow::ReturnToMenu()
{
    m_musicPlayer.LoadSound(arx::Sound::LoadFromFile(GetResourcePathStr("sound/music_menu.wav"), arx::Sound::Format::UncompressedWAV));
    m_musicPlayer.Play(arx::SoundPlayer::PlayMode::Async, true);
    m_mainMenu->Show(true); 
    m_gameScene->RequestDelete();
    GetEventManager().Unbind<arx::KeyDownEvent>(&PacifisitGameWindow::OnKeyDownInGame, this);
    GetEventManager().Unbind<arx::KeyHoldEvent>(&PacifisitGameWindow::OnKeyHoldInGame, this);
    GetEventManager().Unbind<arx::KeyUpEvent>(&PacifisitGameWindow::OnKeyUpInGame, this);
    SetCameraPos(arx::Position(0.0f, 0.0f));
}

void PacifisitGameWindow::StartGame()
{
    m_mainMenu->Hide();
    m_gameScene = new arx::UIControl(this);
    m_gameScene->SetBackgroundColor(arx::Color(255, 0.0f, 0.0f)); 
    m_gameScene->SetPosition(arx::Position(0.0f, 0.0f));
    m_character = new Player(m_gameScene);
    SetUpLevel();
    m_character->MoveToFront();

    m_musicPlayer.LoadSound(arx::Sound::LoadFromFile(GetResourcePathStr("sound/music_level0.wav"), arx::Sound::Format::UncompressedWAV));
    m_musicPlayer.Play(arx::SoundPlayer::PlayMode::Async, true);
    GetEventManager().Bind<arx::KeyDownEvent>(&PacifisitGameWindow::OnKeyDownInGame, this);
    GetEventManager().Bind<arx::KeyHoldEvent>(&PacifisitGameWindow::OnKeyHoldInGame, this);
    GetEventManager().Bind<arx::KeyUpEvent>(&PacifisitGameWindow::OnKeyUpInGame, this);
}

void PacifisitGameWindow::OnKeyDownInEndScreen(arx::KeyDownEvent &e)
{
    if (e.GetKey() == arx::KeyEvent::Key::Esc)
    {
        m_endScreen->RequestDelete();
        GetEventManager().Unbind<arx::KeyDownEvent>(&PacifisitGameWindow::OnKeyDownInEndScreen, this);
        m_mainMenu->Show();
        m_musicPlayer.LoadSound(arx::Sound::LoadFromFile(GetResourcePathStr("sound/music_level0.wav"), arx::Sound::Format::UncompressedWAV));
        m_musicPlayer.Play(arx::SoundPlayer::PlayMode::Async, true);
        SetCameraPos(arx::Position(0.0f, 0.0f));
    }
}

void PacifisitGameWindow::ShowEndScreen()
{
    m_endScreen = new arx::UIControl(this);
    m_endScreen->SetBackgroundColor(arx::constants::COLOR_BLACK);
    m_endScreen->SetSize(arx::SizeF(640, 360));
    arx::Label *endLabel = new arx::Label(m_endScreen, "The end");
    endLabel->SetHorizontalTextAlignment(arx::HorizontalTextAlignment::Center);
    endLabel->SetFont(FontManager::GetFontManager().Retrieve("dejavu_serif"));
    endLabel->EnableTextAntialising(false);
    endLabel->SetPosition(arx::Position(40, 40));
    endLabel->GetFont().SetSizeInPixels(30);
    endLabel->EnableAutoresize(true);
    endLabel->SetTextColor(arx::Color("#ffffc2"));

    arx::Label *creditsLabel = new arx::Label(m_endScreen, R"(Graphics and programming by Alexej Fedorenko (37676)
Neon Pixel-7 font made by third-party (Sizenko Alexander)
DejaVuSerif font made by third-party (Bitstream Vera)
Character spritesheet made by third-party (Mucho pixels)
Most images were generated by ChatGPT 4 AI
Music was generated by Suno AI


Press Escape(Esc) to return to main menu
)");
    creditsLabel->SetHorizontalTextAlignment(arx::HorizontalTextAlignment::Center);
    creditsLabel->SetFont(FontManager::GetFontManager().Retrieve("dejavu_serif"));
    creditsLabel->EnableTextAntialising(false);
    creditsLabel->SetPosition(arx::Position(40, endLabel->GetRect().GetBottom() + 50));
    creditsLabel->GetFont().SetSizeInPixels(16);
    creditsLabel->EnableAutoresize(true);
    creditsLabel->SetTextColor(arx::Color("#ffffc2"));

    m_gameScene->RequestDelete();
    
    GetEventManager().Unbind<arx::KeyDownEvent>(&PacifisitGameWindow::OnKeyDownInGame, this);
    GetEventManager().Unbind<arx::KeyHoldEvent>(&PacifisitGameWindow::OnKeyHoldInGame, this);
    GetEventManager().Unbind<arx::KeyUpEvent>(&PacifisitGameWindow::OnKeyUpInGame, this);

    GetEventManager().Bind<arx::KeyDownEvent>(&PacifisitGameWindow::OnKeyDownInEndScreen, this);
}

void PacifisitGameWindow::OnKeyUpInGame(arx::KeyUpEvent &e)
{
    if (e.GetKey() == arx::KeyEvent::Key::A || e.GetKey() == arx::KeyEvent::Key::D)
        m_character->SetIdleState();
}

void PacifisitGameWindow::SetUpLevel()
{
    m_character->SetPosition(arx::Position(8.0f,310.0f));
    m_gameScene->SetSize(arx::SizeF(640.0f * 3.0f, 360.0f));
    arx::ImageControl *bg = new arx::ImageControl(m_gameScene, arx::Image::LoadFromFile(GetResourcePathStr("images/backgrounds/bg_level0.jpg")));
    bg->SetSize(arx::SizeF(640.0f, 344.0f));

    arx::ImageControl *city_bg0 = new arx::ImageControl(m_gameScene, arx::Image::LoadFromFile(GetResourcePathStr("images/backgrounds/city0.png")));
    city_bg0->SetCanBeAffectedByCamera(true);
    city_bg0->SetSize(arx::SizeF(512.0f, 311.0f));
    city_bg0->SetPosition(arx::Position(640.0f, 33.0f));
    arx::ImageControl *city_bg1 = new arx::ImageControl(m_gameScene, arx::Image::LoadFromFile(GetResourcePathStr("images/backgrounds/city1.png")));
    city_bg1->SetCanBeAffectedByCamera(true);
    city_bg1->SetSize(arx::SizeF(345.0f, 339.0f));
    city_bg1->SetPosition(arx::Position(20.0f, 5.0f));

    arx::ImageControl *ground0 = new arx::ImageControl(m_gameScene, arx::Image::LoadFromFile(GetResourcePathStr("images/blocks/bottom_tile_1.jpg")));
    ground0->SetSize(arx::SizeF(m_gameScene->GetSize().width, 16.0f));
    ground0->SetPosition(arx::Position(0.0f, 344.0f));
    ground0->EnableTilingMode(arx::TileData{static_cast<int>(m_gameScene->GetSize().width / 16), 1});
    ground0->SetCanBeAffectedByCamera(true);
    m_character->SetPosition(arx::Position(8.0f, ground0->GetPosition().y - m_character->GetClientSize().height));
    m_character->SetCanBeAffectedByCamera(true);

    TriggerControl *endTrigger = new TriggerControl(m_gameScene);
    endTrigger->SetSize(arx::SizeF(100, 100));
    endTrigger->SetPosition(arx::Position(m_gameScene->GetSize().width - endTrigger->GetSize().width - 100, 244.0f));
    endTrigger->GetCollisonChecker()->GetEventManager().Bind<CollisionEvent>([this](CollisionEvent &){    
        ShowEndScreen();
    });
    endTrigger->SetCanBeAffectedByCamera(true);
    endTrigger->GetCollisonChecker()->SetCheckableObject(m_character);

    arx::ImageControl *water_bg = new arx::ImageControl(m_gameScene, arx::Image::LoadFromFile(GetResourcePathStr("images/backgrounds/water.png")));
    water_bg->SetCanBeAffectedByCamera(true);
    water_bg->SetSize(arx::SizeF(323.0f, 16.0f));
    water_bg->SetPosition(arx::Position(m_gameScene->GetSize().width - water_bg->GetSize().width - 280, 344.0f));
    arx::ImageControl *bridge = new arx::ImageControl(m_gameScene, arx::Image::LoadFromFile(GetResourcePathStr("images/blocks/bridge.png")));
    bridge->SetCanBeAffectedByCamera(true);
    bridge->SetSize(arx::SizeF(323.0f, 49.0f));
    bridge->SetPosition(arx::Position(m_gameScene->GetSize().width - water_bg->GetSize().width - 280, 311.0f));
}

void PacifisitGameWindow::OnKeyHoldInGame(arx::KeyHoldEvent &e)
{
    auto itRight = e.GetHeldKeys().find(arx::KeyEvent::Key::D);
    auto itLeft = e.GetHeldKeys().find(arx::KeyEvent::Key::A);
    if (itRight != e.GetHeldKeys().end() && itLeft == e.GetHeldKeys().end())
        m_character->MoveToRight();
    if (itLeft != e.GetHeldKeys().end() && itRight == e.GetHeldKeys().end())
        m_character->MoveToLeft();
}


void PacifisitGameWindow::OnKeyDownInGame(arx::KeyDownEvent &e)
{
    switch(e.GetKey())
    {
        case arx::KeyEvent::Key::Esc:
            ReturnToMenu();
            break;
        default:
            break;
    }
}

void PacifisitGameWindow::OnKeyDownInMenu(arx::KeyDownEvent &e)
{
    if (e.GetKey() == arx::KeyEvent::Key::Esc)
        RequestDelete();
}

void PacifisitGameWindow::UpdateResolutions()
{
    m_resolutions = arx::Monitor::GetPrimaryMonitor().GetResolutions(arx::Monitor::ResolutionFilter().SetRetrieveWithMaxRefreshRate(true).SetAspectRatio(16, 9));
    auto it = std::find_if(m_resolutions.begin(), m_resolutions.end(), [](arx::ResolutionMode &resMode) { 
            return GetGlobalConfig()->resX == resMode.GetSize().width && GetGlobalConfig()->resY == resMode.GetSize().height; 
    });
    if (it == m_resolutions.end())
        m_resPos = 0;
    else
        m_resPos = std::distance(m_resolutions.begin(), it);
}

void PacifisitGameWindow::SetCurrentResolutionText(arx::Label *label)
{
    if (m_resolutions.empty())
        label->SetText("");
    else
        label->SetText(arx::Utils::Format("%d x %d", m_resolutions[(size_t)m_resPos].GetSize().width, m_resolutions[(size_t)m_resPos].GetSize().height));
}

void PacifisitGameWindow::PacifisitGameWindow::CreateMainMenu()
{
    m_mainMenu = new arx::ImageControl(this, arx::Image::LoadFromFile(GetResourcePathStr("images/ui/Menu.png")), arx::SizeF(640.0f, 360.0f), arx::Position(0,0));
    m_mainMenu->SetFilteringMode(arx::Texture::TextureFilteringMode::Nearest);
    m_mainMenu->SetCanRecieveMouseEvents(true);

    arx::BitmapButton *settingsButton = new arx::BitmapButton(m_mainMenu, {}, arx::SizeF(22.0f, 79.0f), arx::Position(110,145));
    settingsButton->SetNormalImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/settings_btn.png")));
    settingsButton->SetMouseEnterImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/settings_hover_btn.png")));
    settingsButton->SetMouseHoldImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/settings_clicked_btn.png")));
    settingsButton->SetFont(FontManager::GetFontManager().Retrieve("neon"));
    settingsButton->EnableTextAntialising(false);
    settingsButton->GetFont().SetSizeInPixels(18);
    settingsButton->SetTextColor(arx::Color("#ffffc2"));
    settingsButton->GetEventManager().Bind<arx::MouseDownEvent>([settingsButton, this](arx::MouseDownEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            settingsButton->SetTextColor(arx::constants::COLOR_BLACK);
            e.Skip();
        }
    });
    settingsButton->SetText("C\no\nn\nf\ni\ng");
    settingsButton->SetTextAlignmentPaddingY(-2);
   
    //settingsMenu begin
    arx::ImageControl *settingsMenu = new arx::ImageControl(m_mainMenu, arx::Image::LoadFromFile(GetResourcePathStr("images/ui/settings_menu.jpg")), arx::SizeF(132.0f, 183.0f), arx::Position(239,27));
    settingsMenu->Hide();
    settingsMenu->SetCanRecieveMouseEvents(true);

    CheckBox *fullscreenCheckbox = new CheckBox(settingsMenu, arx::Position(6, 36));
    fullscreenCheckbox->GetEventManager().Bind<arx::MouseDownEvent>([this](arx::MouseDownEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            e.Skip();
        };
    });
    fullscreenCheckbox->Check(GetGlobalConfig()->fullscreen);

    arx::Label *label = new arx::Label(settingsMenu, "Fullscreen");
    label->SetFont(FontManager::GetFontManager().Retrieve("dejavu_serif"));
    label->GetFont().SetSizeInPixels(9);
    label->EnableAutoresize(true);
    label->SetPosition(arx::Position(21, 38));    
    label->EnableTextAntialising(false);
    label->SetTextColor(arx::Color("#ffffc2"));

    arx::Label *resLabel = new arx::Label(settingsMenu, "Resolution :");
    resLabel->SetFont(FontManager::GetFontManager().Retrieve("dejavu_serif"));
    resLabel->GetFont().SetSizeInPixels(9);
    resLabel->EnableAutoresize(true);
    resLabel->SetPosition(arx::Position(6, 6));
    resLabel->EnableTextAntialising(false);
    resLabel->SetTextColor(arx::Color("#ffffc2"));

    arx::Label *resSelLabel = new arx::Label(settingsMenu, "");
    resSelLabel->SetFont(FontManager::GetFontManager().Retrieve("dejavu_serif"));
    resSelLabel->GetFont().SetSizeInPixels(9);
    resSelLabel->EnableAutoresize(true);
    resSelLabel->SetPosition(arx::Position(21, 20));
    resSelLabel->EnableTextAntialising(false);
    resSelLabel->SetTextColor(arx::Color("#ffffc2"));

    auto getIncreaseResBtnX = [resSelLabel]() { return resSelLabel->GetPosition().x + resSelLabel->GetSize().width; };

    arx::BitmapButton *increaseResBtn = new arx::BitmapButton(settingsMenu, {}, arx::SizeF(13.0f, 12.0f), arx::Position(0, 18.0f));
    increaseResBtn->SetNormalImage(ImageManager::GetImageManager().Retrieve("togglable_btn"));
    increaseResBtn->SetMouseEnterImage(ImageManager::GetImageManager().Retrieve("togglable_hover_btn"));
    increaseResBtn->SetMouseHoldImage(ImageManager::GetImageManager().Retrieve("togglable_hover_btn"));
    increaseResBtn->SetText(">");
    increaseResBtn->SetFont(FontManager::GetFontManager().Retrieve("neon"));
    increaseResBtn->EnableTextAntialising(false);
    increaseResBtn->GetFont().SetSizeInPixels(14);
    increaseResBtn->SetTextAlignmentPaddingX(1);

    increaseResBtn->GetEventManager().Bind<arx::MouseDownEvent>([increaseResBtn, this](arx::MouseDownEvent&e) {
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            increaseResBtn->SetTextColor(arx::Color("#a79292"));
            e.Skip();
        } 
    });
    
    increaseResBtn->GetEventManager().Bind<arx::MouseUpEvent>([increaseResBtn, resSelLabel, getIncreaseResBtnX, this, fullscreenCheckbox](arx::MouseUpEvent&e) {
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            increaseResBtn->SetTextColor(arx::constants::COLOR_BLACK);
            if ((size_t)m_resPos + 1 < m_resolutions.size() && !fullscreenCheckbox->IsChecked())
            {
                m_resPos++;
                SetCurrentResolutionText(resSelLabel);
                increaseResBtn->SetPosition(arx::Position(getIncreaseResBtnX(), increaseResBtn->GetPosition().y));
            }
            e.Skip();
        } 
    });
    
    arx::BitmapButton *decreaseResBtn = new arx::BitmapButton(settingsMenu, {}, arx::SizeF(13.0f, 12.0f), arx::Position(6,18));
    decreaseResBtn->SetNormalImage(ImageManager::GetImageManager().Retrieve("togglable_btn"));
    decreaseResBtn->SetMouseEnterImage(ImageManager::GetImageManager().Retrieve("togglable_hover_btn"));
    decreaseResBtn->SetMouseHoldImage(ImageManager::GetImageManager().Retrieve("togglable_hover_btn"));
    decreaseResBtn->SetText("<");
    decreaseResBtn->SetFont(FontManager::GetFontManager().Retrieve("neon"));
    decreaseResBtn->EnableTextAntialising(false);
    decreaseResBtn->GetFont().SetSizeInPixels(14);
    decreaseResBtn->SetTextAlignmentPaddingX(1);
    decreaseResBtn->GetEventManager().Bind<arx::MouseDownEvent>([decreaseResBtn, this](arx::MouseDownEvent&e) {
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            decreaseResBtn->SetTextColor(arx::Color("#a79292"));
            e.Skip();
        } 
    });
    decreaseResBtn->GetEventManager().Bind<arx::MouseUpEvent>([decreaseResBtn, this, resSelLabel, getIncreaseResBtnX, increaseResBtn, fullscreenCheckbox](arx::MouseUpEvent&e) {
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            if (m_resPos > 0 && !fullscreenCheckbox->IsChecked())
            {
                m_resPos--;
                SetCurrentResolutionText(resSelLabel);
                increaseResBtn->SetPosition(arx::Position(getIncreaseResBtnX(), increaseResBtn->GetPosition().y));
            }
            decreaseResBtn->SetTextColor(arx::constants::COLOR_BLACK);
            e.Skip();
        } 
    });

    settingsButton->GetEventManager().Bind<arx::MouseUpEvent>([settingsMenu, settingsButton, resSelLabel, this, getIncreaseResBtnX, increaseResBtn, fullscreenCheckbox](arx::MouseUpEvent &e)
    {
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            settingsButton->SetTextColor(arx::Color("#ffffc2"));
            if (!settingsMenu->IsShown())
            {
                UpdateResolutions();
                SetCurrentResolutionText(resSelLabel);
                increaseResBtn->SetPosition(arx::Position(getIncreaseResBtnX(), increaseResBtn->GetPosition().y));
                fullscreenCheckbox->Check(GetGlobalConfig()->fullscreen);
            }
            settingsMenu->Show(!settingsMenu->IsShown());
            e.Skip();
        }
    });

    arx::BitmapButton *acceptButton = new arx::BitmapButton(settingsMenu, {}, arx::SizeF(43.0f, 16.0f), arx::Position(6.0f, 161.0f) );
    acceptButton->SetText("Accept");
    acceptButton->EnableTextAntialising(false);
    acceptButton->SetFont(FontManager::GetFontManager().Retrieve("dejavu_serif"));
    acceptButton->GetFont().SetSizeInPixels(10);
    acceptButton->SetNormalImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/generic_btn.jpg")));
    acceptButton->SetMouseEnterImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/generic_hover_btn.jpg")));
    acceptButton->SetMouseHoldImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/generic_btn.jpg")));
    acceptButton->GetEventManager().Bind<arx::MouseDownEvent>([acceptButton, this](arx::MouseDownEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            acceptButton->SetTextColor(arx::Color("#a79292"));
            e.Skip();
        }
    });
    acceptButton->GetEventManager().Bind<arx::MouseUpEvent>([acceptButton, fullscreenCheckbox, this](arx::MouseUpEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            try
            {
                Config *cfg = GetGlobalConfig();
                cfg->fullscreen = fullscreenCheckbox->IsChecked();
                
                SetFullscreen(cfg->fullscreen);
                if (!cfg->fullscreen && !m_resolutions.empty())
                {
                    cfg->resX = m_resolutions[(size_t)m_resPos].GetSize().width;
                    cfg->resY = m_resolutions[(size_t)m_resPos].GetSize().height;
                    SetSize(arx::SizeF(static_cast<float>(cfg->resX), static_cast<float>(cfg->resY)));
                    Center();
                }
                
                arx::Json json;
                cfg->ToJson(json);
                std::ofstream file;
                file.open(GetConfigPathStr());
                file << arx::Json::ToJsonStr(json);
                file.close();
            }
            catch(...)
            {
                arx::UniversalExceptionHandler::HandleException();
            }

            acceptButton->SetTextColor(arx::constants::COLOR_BLACK);
            e.Skip();
        }
    });

    arx::BitmapButton *cancelButton = new arx::BitmapButton(settingsMenu, {}, arx::SizeF(43.0f, 16.0f), arx::Position(83.0f, 161.0f) );
    cancelButton->SetText("Cancel");
    cancelButton->EnableTextAntialising(false);
    cancelButton->SetFont(FontManager::GetFontManager().Retrieve("dejavu_serif"));
    cancelButton->GetFont().SetSizeInPixels(10);
    cancelButton->SetNormalImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/generic_btn.jpg")));
    cancelButton->SetMouseEnterImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/generic_hover_btn.jpg")));
    cancelButton->SetMouseHoldImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/generic_btn.jpg")));
    cancelButton->GetEventManager().Bind<arx::MouseDownEvent>([cancelButton, this](arx::MouseDownEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            cancelButton->SetTextColor(arx::Color("#a79292"));
            e.Skip();
        }
    });
    cancelButton->GetEventManager().Bind<arx::MouseUpEvent>([settingsMenu, cancelButton](arx::MouseUpEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            settingsMenu->Show(false);
            cancelButton->SetTextColor(arx::constants::COLOR_BLACK);
            e.Skip();
        }
    });
    //settingsMenu end
    arx::BitmapButton *startButton = new arx::BitmapButton(m_mainMenu, {}, arx::SizeF(147.0f, 32.0f), arx::Position(478.0f, 235.0f) );
    startButton->SetNormalImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/start_btn.png")));
    startButton->SetMouseEnterImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/start_hover_btn.png")));
    startButton->SetMouseHoldImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/start_clicked_btn.png")));
    startButton->SetFont(FontManager::GetFontManager().Retrieve("neon"));
    startButton->SetTextAlignmentPaddingY(1);
    startButton->EnableTextAntialising(false);
    startButton->GetFont().SetSizeInPixels(34);
    startButton->SetTextColor(arx::Color("#ffffc2"));
    startButton->GetEventManager().Bind<arx::MouseDownEvent>([startButton, this](arx::MouseDownEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            startButton->SetTextColor(arx::constants::COLOR_BLACK);
            e.Skip();
        }
    });
    startButton->GetEventManager().Bind<arx::MouseUpEvent>([startButton, this, settingsMenu](arx::MouseUpEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            startButton->SetTextColor(arx::Color("#ffffc2"));
            settingsMenu->Hide();
            StartGame();
            e.Skip();
        }
    });
    startButton->SetText("Start");

    arx::BitmapButton *exitButton = new arx::BitmapButton(m_mainMenu, {}, arx::SizeF(25.0f, 68.0f), arx::Position(134.0f, 161.0f));
    exitButton->SetNormalImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/exit_btn.png")));
    exitButton->SetMouseEnterImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/exit_hover_btn.png")));
    exitButton->SetMouseHoldImage(arx::Image::LoadFromFile(GetResourcePathStr("images/ui/exit_clicked_btn.png")));
    exitButton->SetFont(FontManager::GetFontManager().Retrieve("neon"));
    exitButton->EnableTextAntialising(false);
    exitButton->GetFont().SetSizeInPixels(22);
    exitButton->SetTextColor(arx::Color("#ffffc2"));
    exitButton->GetEventManager().Bind<arx::MouseDownEvent>([exitButton, this](arx::MouseDownEvent &e)
    {
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            m_btnClickPlayer.Play(arx::SoundPlayer::Async);
            exitButton->SetTextColor(arx::constants::COLOR_BLACK);
            e.Skip();
        }
    });
    exitButton->GetEventManager().Bind<arx::MouseUpEvent>([exitButton, this](arx::MouseUpEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            if (!e.WasForced())
                RequestDelete();
            exitButton->SetTextColor(arx::Color("#ffffc2"));
            e.Skip();
        }
    });
    exitButton->SetText("E\nx\ni\nt");

}
