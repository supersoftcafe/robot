
#include <iostream>
#include <memory>
#include <cmath>

#include "interfaces/IPort.h"

#ifdef TARGET_RASPBERRY_PI 
#include "providers/GPIOPortManager.h"
#else
#include "providers/FakePortManager.h"
#define GPIOPortManager	FakePortManager
#endif


#include "RobotControl.h"


namespace { // Begin anonymous namespace

class _RobotControl : public RobotControl
{
public:
    _RobotControl() { }
    ~_RobotControl() { }
};

} // End anonymous namespace


RobotControl::RobotControl()
{
    auto manager = std::static_pointer_cast<IPortManager>(GPIOPortManager::Create());

    direction_left_ = manager->FindPort("gpio/8");
    direction_left_->set_writable(true);

    speed_left_ = manager->FindPort("gpio/7");
    speed_left_->set_writable(true);
    //  speed_left_ = translator->TranslatePort(speed_left));

    direction_right_ = manager->FindPort("gpio/10");
    direction_right_->set_writable(true);

    speed_right_ = manager->FindPort("gpio/9");
    speed_right_->set_writable(true);
    //  speed_right_ = translator->TranslatePort(speed_right_);
}

RobotControl::~RobotControl()
{
}

std::shared_ptr<RobotControl> RobotControl::Create()
{
    return std::static_pointer_cast<RobotControl>(std::make_shared<_RobotControl>());
}

void RobotControl::set_speed_left(float value)
{
    direction_left_->set_binary_level(value < 0.0f);
    speed_left_->set_analogue_level(::fabsf(value));
}

float RobotControl::speed_left()
{
    float speed = speed_left_->analogue_level();
    return direction_left_->binary_level() ? -speed : speed;
}

void RobotControl::set_speed_right(float value)
{
    direction_right_->set_binary_level(value < 0.0f);
    speed_right_->set_analogue_level(::fabsf(value));
}

float RobotControl::speed_right()
{
    float speed = speed_right_->analogue_level();
    return direction_right_->binary_level() ? -speed : speed;
}

