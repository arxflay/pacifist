#include "Player.h"
#include <arxengine/animation/AbstractAnimationAction.h>
#include <arxengine/animation/Animation.h>
#include <arxengine/animation/EmptyAnimationAction.h>
#include "ImageManager.h"
#include <arxengine/ui/ArxWindow.h>

class SetImageAnimation : public arx::AbstractAnimationAction
{
public:
    SetImageAnimation(std::unique_ptr<AbstractAnimationAction> &&next, arx::ImageControl *imageControl, std::string_view imageId)
        : AbstractAnimationAction(std::move(next)), m_imgControl(imageControl), m_imageId(imageId)
    {
    }
private:
    void PerformActionImpl() override 
    {
        m_imgControl->Load(ImageManager::GetImageManager().Retrieve(m_imageId));
    }
    arx::ImageControl *m_imgControl;
    std::string m_imageId;
};

Player::Player(arx::UIControl *gameScene)
    : arx::ImageControl(gameScene)
    , m_facingToRight(true)
{
    m_animManager = new arx::AnimationManager(this);
    arx::Animation *moveAnimation = new arx::Animation;
    //animation->AddKeyFrame(0, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle"));
    moveAnimation->AddKeyFrame(0, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_0"));
    moveAnimation->AddKeyFrame(3, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_1"));
    moveAnimation->AddKeyFrame(6, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_2"));
    moveAnimation->AddKeyFrame(9, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_3"));
    moveAnimation->AddKeyFrame(12, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_4"));
    moveAnimation->AddKeyFrame(15, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_5"));
    moveAnimation->AddKeyFrame(18, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_6"));
    moveAnimation->AddKeyFrame(21, std::make_unique<arx::EmptyAnimationAction>(nullptr));
    m_animManager->ClaimAnimation("move_right", moveAnimation);
    
    arx::Animation *moveLeftAnimation = new arx::Animation;
    moveLeftAnimation->AddKeyFrame(0, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_0_left"));
    moveLeftAnimation->AddKeyFrame(3, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_1_left"));
    moveLeftAnimation->AddKeyFrame(6, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_2_left"));
    moveLeftAnimation->AddKeyFrame(9, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_3_left"));
    moveLeftAnimation->AddKeyFrame(12, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_4_left"));
    moveLeftAnimation->AddKeyFrame(15, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_5_left"));
    moveLeftAnimation->AddKeyFrame(18, std::make_unique<SetImageAnimation>(nullptr, this, "character_move_6_left"));
    moveLeftAnimation->AddKeyFrame(21, std::make_unique<arx::EmptyAnimationAction>(nullptr));
    m_animManager->ClaimAnimation("move_left", moveLeftAnimation);

    arx::Animation *idleAnimation = new arx::Animation;
    idleAnimation->AddKeyFrame(0, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_0"));
    idleAnimation->AddKeyFrame(3, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_1"));
    idleAnimation->AddKeyFrame(6, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_2"));
    idleAnimation->AddKeyFrame(9, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_3"));
    idleAnimation->AddKeyFrame(12, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_4"));
    idleAnimation->AddKeyFrame(15, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_5"));
    idleAnimation->AddKeyFrame(18, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_6"));
    idleAnimation->AddKeyFrame(21, std::make_unique<arx::EmptyAnimationAction>(nullptr));
    m_animManager->ClaimAnimation("idle_right", idleAnimation);
    
    arx::Animation *idleLeftAnimation = new arx::Animation;
    idleLeftAnimation->AddKeyFrame(0, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_0_left"));
    idleLeftAnimation->AddKeyFrame(3, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_1_left"));
    idleLeftAnimation->AddKeyFrame(6, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_2_left"));
    idleLeftAnimation->AddKeyFrame(9, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_3_left"));
    idleLeftAnimation->AddKeyFrame(12, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_4_left"));
    idleLeftAnimation->AddKeyFrame(15, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_5_left"));
    idleLeftAnimation->AddKeyFrame(18, std::make_unique<SetImageAnimation>(nullptr, this, "character_idle_6_left"));
    idleLeftAnimation->AddKeyFrame(21, std::make_unique<arx::EmptyAnimationAction>(nullptr));
    m_animManager->ClaimAnimation("idle_left", idleLeftAnimation);
    m_animManager->LoadAndRun("idle_right", true);

    SetSize(arx::SizeF(20.0f, 32.0f));
}

void Player::SetIdleFacingDirection(bool rightDirection)
{
    if (rightDirection)
        m_animManager->LoadAndRun("idle_right", true);
    else
        m_animManager->LoadAndRun("idle_left", true);
}

void Player::SetIdleState()
{
    SetIdleFacingDirection(m_facingToRight);
}

void Player::MoveToLeft()
{
    arx::UIControl *parent = static_cast<arx::UIControl*>(GetParent());
    auto characterRect = GetRect(false);
    auto gameSceneRect = parent->GetRect(false);
    float x = -(float)arx::GameApp::GetGlobalApp()->GetDeltaTime() * CHARACTER_SPEED;
    float newPos = x;
    if (characterRect.GetLeft() + x < gameSceneRect.GetLeft())
        newPos = -characterRect.GetLeft();
    characterRect.SetPosition(arx::Position(characterRect.GetPosition().x + newPos, characterRect.GetTop()));
    
    const arx::Position &cameraPos = GetWindow()->GetCameraPos();
    if (cameraPos.x > 0 && characterRect.GetLeft() - cameraPos.x <= GetSidecrollingThresholdLeft())
    {
        arx::Position newCameraPos = arx::Position(std::max(cameraPos.x + x, 0.0f), cameraPos.y);
        GetWindow()->SetCameraPos(newCameraPos);
    }

    SetPosition(arx::Position(characterRect.GetPosition().x, characterRect.GetPosition().y));
    if (m_animManager->GetCurrentAnimationName() != "move_left" || !m_animManager->IsRunning())
        m_animManager->LoadAndRun("move_left", true);

    m_facingToRight = false;
}

void Player::MoveToRight()
{
    arx::UIControl *parent = static_cast<arx::UIControl*>(GetParent());
    auto characterRect = GetRect(false);
    auto gameSceneRect = parent->GetRect(false);
    float x = (float)arx::GameApp::GetGlobalApp()->GetDeltaTime() * CHARACTER_SPEED;
    float newPos = x;
    if (characterRect.GetRight() + newPos > gameSceneRect.GetRight())
        newPos = -characterRect.GetLeft() + (gameSceneRect.GetRight() - characterRect.GetSize().width);

    characterRect.SetPosition(arx::Position(characterRect.GetPosition().x + newPos, characterRect.GetTop()));
    SetPosition(arx::Position(characterRect.GetPosition().x, characterRect.GetPosition().y));
    
    float cameraRightBorder = gameSceneRect.GetRight() - GetWindow()->GetViewport().size.width;
    const arx::Position &cameraPos = GetWindow()->GetCameraPos();
    if (cameraPos.x < cameraRightBorder && characterRect.GetRight() - cameraPos.x >= GetSidecrollingThresholdRight())
    {
        arx::Position newCameraPos(std::min(cameraPos.x + x, cameraRightBorder), cameraPos.y);
        GetWindow()->SetCameraPos(newCameraPos);
    }

    //Load(ImageManager::GetImageManager().Retrieve("character_idle"));
    if (m_animManager->GetCurrentAnimationName() != "move_right" || !m_animManager->IsRunning())
        m_animManager->LoadAndRun("move_right", true);

    m_facingToRight = true;
}


float Player::GetSidecrollingThresholdRight()
{
    return GetWindow()->GetViewport().size.width / SIDESCROLLING_THRESHOLD_FACTOR;
}
float Player::GetSidecrollingThresholdLeft()
{
    return GetWindow()->GetViewport().size.width - GetSidecrollingThresholdRight();
}

