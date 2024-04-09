/*
 * @author Alexej Fedorenko
 */
#ifndef CHECKBOX_H
#define CHECKBOX_H
#include <arxengine/ui/BitmapButton.h>

class CheckBoxEvent final : public arx::Event
{
public:
    CheckBoxEvent();
    void SetChecked(bool checked = true);
    bool WasChecked() const;

private:
    void HandleEvent() override;    
    bool m_checked;
};

class CheckBox : public arx::BitmapButton
{
public:
    CheckBox(arx::UIControl *parent, const arx::Position &position = arx::constants::DEFAULT_POSITION);
    void Check(bool check);
    bool IsChecked() const;

private:
    void UpdateTextures();
    bool m_checked;
};

#endif
