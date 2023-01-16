#pragma once

#include <cstddef>
#include "clock.h"

struct AVFrame;

class VideoClock final : public Clock
{
	static const double SYNC_THRESHOLD;
	static const double NOSYNC_THRESHOLD;

public:

	// update current value
	// return the previous one
	double update(AVFrame* frame);

	void reset();

	// calculate delay to the next displaying frame
	double calculateFrameDelay(const double& pts, const double& refClock);

private:

	double _lastFrameDelay = 0.0;
	double _lastFramePTS = 0.0;
	double _frameTimer = 0.0;
};
