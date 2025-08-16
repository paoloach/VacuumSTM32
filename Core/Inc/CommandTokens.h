/*
 * CommandTokens.h
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#pragma once

#include <stdint.h>

constexpr uint8_t MAX_TOKENS=5;

class Token;

class CommandTokens {
public:
	CommandTokens();

	void free() ;
	void addToken(const Token & token);
	const char * getCommandName() const {return (const char *)tokens[0];}
	uint8_t count() const {return tokensCount;}
	bool empty() const {return tokensCount == 0;}
	int32_t getTokenAsInt(int index) const;
	const char * getTokenAsString(int index) const {return (const char *)tokens[index];}
private:
	uint8_t * tokens[MAX_TOKENS];
	uint8_t tokensCount;
};


