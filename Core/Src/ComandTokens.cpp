/*
 * CommandTokens.cpp
 *
 *  Created on: Aug 2, 2025
 *      Author: paolo
 */

#include <stdlib.h>
#include <memory.h>
#include "CommandTokens.h"
#include "Token.h"

CommandTokens::CommandTokens():tokensCount(0){
	for(int i=0; i < MAX_TOKENS; i++){
		tokens[i]=nullptr;
	}
}

void CommandTokens::free() {
	for(uint8_t i =0; i < tokensCount; i++){
		delete tokens[i];
	}
	tokensCount=0;
}
void CommandTokens::addToken(const Token & token){
	if (tokensCount < MAX_TOKENS){
		uint8_t * line = new uint8_t[token.size()+1];
		if (line != nullptr){
			memcpy(line, token.data(), token.size());
			line[token.size()] = 0;
			tokens[tokensCount] = line;
			tokensCount++;
		}
	}
}

int32_t CommandTokens::getTokenAsInt(int index) const{
	if (index >= tokensCount)
		return 0;
	return atoi((const char*)tokens[index]);
}

