/*
 * RobotControl.h
 *
 *  Created on: Sep 27, 2013
 *      Author: mbrown
 */

#ifndef ROBOTCONTROL_H_
#define ROBOTCONTROL_H_

#include <memory>

class IPort;

class RobotControl
{
public:
	static std::shared_ptr<RobotControl> Create();

	void set_speed_right(float speed); // Speed can be from -1.0f to 1.0f
	float speed_right();

	void set_speed_left( float speed);
	float speed_left();

protected:
	RobotControl();
	virtual ~RobotControl();

private:
	std::shared_ptr<IPort> direction_right_;
	std::shared_ptr<IPort> direction_left_;

	std::shared_ptr<IPort> speed_right_;
	std::shared_ptr<IPort> speed_left_;
};

#endif /* ROBOTCONTROL_H_ */
