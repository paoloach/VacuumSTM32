/*
 * Token.cpp
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#include "Token.h"

void Token::addByte(uint8_t byte){
	if (token_size < MAX_TOKEN_SIZE){
		token[token_size]=byte;
		token_size++;
	}
}


