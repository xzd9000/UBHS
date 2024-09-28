#pragma once

struct Timer
{
	static double accumulator;
	static double currentTime;
	static double timeWorld;
	static double timeEvent;
	static bool eventTimeFrozen;
	static double dt;
	static double newTime;
	static double frameTime;
};
