/*
 * Commands.h
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#pragma once

#include <stdint.h>
class CommandTokens;

enum class CommandType {
	NOTHING,
	SPEED,
	MOVE,
	MOTOR
};

class Move;

class CommandSpeed {
public:
	uint32_t velocity;
};

class CommandMove {
public:
	bool on;
};

class CommandMotor {
public:
	Move * move;
	const uint32_t * info;

};

class Command {
public:
	Command():commandType(CommandType::NOTHING){}
	Command(CommandType type):commandType(type){}
public:
	CommandType commandType;
	union {
		CommandSpeed speed;
		CommandMove move;
		CommandMotor motor;
	};
};


Command createCommand(const CommandTokens & commandTokens);

