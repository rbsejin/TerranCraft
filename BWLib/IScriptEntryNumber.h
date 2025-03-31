#pragma once
#include "../Common/typedef.h"

namespace BW
{
    enum class IScriptEntryNumber : uint16
    {
        Marine = 78,
		Marine_Remnants = 79,
		Marine_Death = 80,
        SCV = 84,
		Command_Center = 102,
        Command_Center_Overlay = 103,
		Shadow_Header = 275,
		Cursor_Marker = 288,
        Circle_Marker = 289,
        None = 412
    };
}