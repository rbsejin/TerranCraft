#pragma once

#include "../BWLib/OrderType.h"
#include "Target.h"

class Unit;

class Order
{
public:
	BW::OrderType OrderType = BW::OrderType::None;
	Target Target = { 0, };
};
