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
#include <arxengine/ui/Painter.h>
#include <arxengine/ui/Label.h>
#include <arxengine/media/Sound.h>
#include <fstream>

PacifisitGameWindow::PacifisitGameWindow(const arx::SizeF &size, const arx::Position &position, int attributes)
    : arx::ArxWindow("Pacifist", size, position, attributes)
{
    SetFixedViewport(640.0f, 360.0f);
    CreateMainMenu();
    m_btnClickPlayer.LoadSound(arx::Sound::LoadFromFile(GetResourcePathStr("sound/click.wav"), arx::Sound::Format::UncompressedWAV));
    m_mainMenuMusicPlayer.LoadSound(arx::Sound::LoadFromFile(GetResourcePathStr("sound/music_menu.wav"), arx::Sound::Format::UncompressedWAV));
    m_btnClickPlayer.SetVolumeInPercentage(700.0f);
    m_mainMenuMusicPlayer.SetVolumeInPercentage(30.0f);
    m_mainMenuMusicPlayer.Play(arx::SoundPlayer::PlayMode::Async, true);
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
    arx::ImageControl *imagePanel = new arx::ImageControl(this, arx::Image::LoadFromFile(GetResourcePathStr("images/ui/Menu.png")), arx::SizeF(640.0f, 360.0f), arx::Position(0,0));
    imagePanel->SetFilteringMode(arx::Texture::TextureFilteringMode::Nearest);
    imagePanel->SetCanRecieveMouseEvents(true);

    arx::BitmapButton *settingsButton = new arx::BitmapButton(imagePanel, {}, arx::SizeF(22.0f, 79.0f), arx::Position(110,145));
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
    arx::ImageControl *settingsMenu = new arx::ImageControl(this, arx::Image::LoadFromFile(GetResourcePathStr("images/ui/settings_menu.jpg")), arx::SizeF(132.0f, 183.0f), arx::Position(239,27));
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

    settingsButton->GetEventManager().Bind<arx::MouseUpEvent>([settingsMenu, settingsButton, resSelLabel, this, getIncreaseResBtnX, increaseResBtn](arx::MouseUpEvent &e)
    {
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            settingsButton->SetTextColor(arx::Color("#ffffc2"));
            if (!settingsMenu->IsShown())
            {
                UpdateResolutions();
                SetCurrentResolutionText(resSelLabel);
                increaseResBtn->SetPosition(arx::Position(getIncreaseResBtnX(), increaseResBtn->GetPosition().y));
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
    arx::BitmapButton *startButton = new arx::BitmapButton(imagePanel, {}, arx::SizeF(147.0f, 32.0f), arx::Position(478.0f, 235.0f) );
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
    startButton->GetEventManager().Bind<arx::MouseUpEvent>([startButton](arx::MouseUpEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            startButton->SetTextColor(arx::Color("#ffffc2"));
            e.Skip();
        }
    });
    startButton->SetText("Start");

    arx::BitmapButton *exitButton = new arx::BitmapButton(imagePanel, {}, arx::SizeF(25.0f, 68.0f), arx::Position(134.0f, 161.0f));
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
