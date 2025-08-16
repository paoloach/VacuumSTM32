/*
 * Token.h
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#pragma once

#include <stdint.h>

constexpr uint8_t MAX_TOKEN_SIZE=20;

class Token {
public:
	Token():token_size(0){}
	void addByte(uint8_t byte);
	uint8_t size() const {return token_size;}
	const uint8_t * data() const {return token;}
	void reset(){token_size=0;}
private:
	uint8_t token[MAX_TOKEN_SIZE];
	uint8_t token_size;
};


