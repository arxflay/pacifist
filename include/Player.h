#ifndef PLAYER_H
#define PLAYER_H
#include "arxengine/ui/ImageControl.h"
#include <arxengine/animation/AnimationManager.h>

class Player : public arx::ImageControl
{
public:
    Player(arx::UIControl *gameScene);

    void SetIdleFacingDirection(bool rightDirection);
    void SetIdleState();

    void MoveToLeft();
    void MoveToRight();

private:
    float GetSidecrollingThresholdRight();
    float GetSidecrollingThresholdLeft();

    arx::AnimationManager *m_animManager;
    bool m_facingToRight;
    static constexpr float CHARACTER_SPEED = 150.0f;
    static constexpr float SIDESCROLLING_THRESHOLD_FACTOR = 1.5f;
};

#endif
