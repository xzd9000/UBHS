#pragma once
#include"UBHS_Definitions.h"

enum Timeline
{
	TL_Global,
	TL_Event
};

struct Event
{
	Timeline timeline;
	double activationTime;
	FuncTDT action;
};

