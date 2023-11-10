#pragma once
#include "pch.h"

class Helper
{
public:
	static uintptr_t PatternScan(uintptr_t moduleAdress, const char* signature);
};