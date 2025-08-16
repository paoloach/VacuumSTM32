/*
 * Commands.cpp
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */


#include <string.h>
#include "Commands.h"
#include "CommandTokens.h"
#include "Move.h"

extern Move move;

Command createCommand(const CommandTokens & commandTokens) {
	if (commandTokens.empty()){
		return Command();
	}
	const char * commandName = commandTokens.getCommandName();
	if (stricmp(commandName, "SPEED") == 0){
		if (commandTokens.count() > 1){
			int32_t speed = commandTokens.getTokenAsInt(1);
			if (speed < 0)
				speed = 0;
			if (speed > 100)
				speed = 100;
			Command command(CommandType::SPEED);
			command.speed.velocity = speed;
			return command;
		}
	} else if (stricmp(commandName, "MOVE") == 0){
		if (commandTokens.count() > 1){
			Command command(CommandType::MOVE);
			if (stricmp(commandTokens.getTokenAsString(1), "ON") == 0){
				command.move.on=true;
			} else {
				command.move.on=false;
			}
			return command;
		}
	} else if (stricmp(commandName, "GET") == 0){
		if (commandTokens.count() > 2){
			const char * object = commandTokens.getTokenAsString(1);
			const char * info = commandTokens.getTokenAsString(2);
			Command command(CommandType::MOTOR);
			if (stricmp(object, "MOTOR1")==0){
				command.motor.move = &move;
				if (stricmp(info, "SPEED") == 0){
					command.motor.info = move.getSpeedMotor1();
				}else {
					command = Command();
				}
			}else if (stricmp(object, "MOTOR2")==0){
				command.motor.move = &move;
				if (stricmp(info, "SPEED") == 0){
					command.motor.info = move.getSpeedMotor2();
				} else {
					command = Command();
				}
			} else {
				command = Command();
			}

			return command;
		}
	}
	return Command();

}
