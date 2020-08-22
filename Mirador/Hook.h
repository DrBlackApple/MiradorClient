#pragma once
#include "common.h"

class Hook
{
	public: 
		virtual void HandleHook(SOCKET, std::string const &) = 0;
};