/*
 * Move.h
 *
 *  Created on: May 24, 2025
 *      Author: paolo
 */

#pragma once

class Move {
public:
	void init();
	void on();
	void off();
	void setSpeed(uint32_t speed);
	const uint32_t* getSpeedMotor1() const {return motor1;}
	const uint32_t* getSpeedMotor2() const {return motor2;}
	uint32_t motor1[10];
	uint32_t motor2[10];

private:
	uint32_t speed=50;

};

