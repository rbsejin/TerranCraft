#pragma once

#include "../BWLib/OrderType.h"
#include "Target.h"

class Unit;

class Order
{
public:
	eOrder OrderType = eOrder::None;
	Target Target = { 0, };
};
