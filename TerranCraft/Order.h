#pragma once

#include "../BWLib/OrderType.h"

class Unit;

class Order
{
public:
	BW::OrderType OrderType;
	Unit* Target;
};
