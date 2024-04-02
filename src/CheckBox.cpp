#include "CheckBox.h"
#include "ImageManager.h"
#include <arxengine/ui/MouseEvent.h>

CheckBoxEvent::CheckBoxEvent()
    : m_checked(false)
{
}

void CheckBoxEvent::SetChecked(bool checked)
{
    m_checked = checked;
}

bool CheckBoxEvent::WasChecked() const
{
    return m_checked;
}

void CheckBoxEvent::HandleEvent() {}


CheckBox::CheckBox(arx::UIControl *parent, const arx::Position &position)
    : arx::BitmapButton(parent, {}, arx::SizeF(13.0f, 12.0f), position)
    , m_checked(false)
{
    UpdateTextures();
    GetEventManager().Bind<arx::MouseUpEvent>([this](arx::MouseUpEvent &e){
        if (e.GetMouseButtonType() == arx::MouseButtonEvent::ButtonType::Left)
        {
            Check(!IsChecked());
            e.Skip();
        }
    });
}

void CheckBox::Check(bool check)
{
    if (m_checked == check)
        return;
    m_checked = check;
    if (GetEventManager().HasNonDefaultEventHandler<CheckBoxEvent>())
    {
        std::unique_ptr<CheckBoxEvent> evt(std::make_unique<CheckBoxEvent>());
        evt->SetChecked(check);
        GetEventManager().QueueEvent<CheckBoxEvent>(std::move(evt));
    }
    UpdateTextures();
}

bool CheckBox::IsChecked() const
{
    return m_checked;
}
void CheckBox::UpdateTextures()
{
    if (!m_checked)
    {
        SetNormalImage(ImageManager::GetImageManager().Retrieve("check_btn"));
        SetMouseEnterImage(ImageManager::GetImageManager().Retrieve("check_hover_btn"));
        SetMouseHoldImage(ImageManager::GetImageManager().Retrieve("check_clicked_btn"));
    }
    else         
    {
        SetNormalImage(ImageManager::GetImageManager().Retrieve("check_btn_checked"));
        SetMouseEnterImage(ImageManager::GetImageManager().Retrieve("check_hover_btn_checked"));
        SetMouseHoldImage(ImageManager::GetImageManager().Retrieve("check_clicked_btn_checked"));
    }
}
