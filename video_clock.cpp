#include "video_clock.h"

#include <cmath>

extern "C"
{
#include <libavutil\frame.h>
#include <libavutil\time.h>
}

const double VideoClock::SYNC_THRESHOLD = 0.01;
const double VideoClock::NOSYNC_THRESHOLD = 10.0;

double VideoClock::update(AVFrame* frame)
{
	double value = getValue();

	double ts = frame->best_effort_timestamp;
	ts *= _timebase;

	if (ts != 0.0)
	{
		setValue(ts);
		value = ts;
	}

	double delay = _timebase;
	delay += frame->repeat_pict * (delay * 0.5);
	increment(delay);

	return value;
}

void VideoClock::reset()
{
	_lastFrameDelay = 40e-3;
	_frameTimer = static_cast<double>(av_gettime()) / 1000000.0;
}

double VideoClock::calculateFrameDelay(const double& pts, const double& refClock)
{
	double delay = pts - _lastFramePTS;
	// fix value if something wrong
	if (delay <= 0 || delay >= 1.0)
	{
		delay = _lastFrameDelay;
	}

	_lastFrameDelay = delay;
	_lastFramePTS = pts;

	double diff = pts - refClock;

	double syncThreshold = (delay > SYNC_THRESHOLD) ? delay : SYNC_THRESHOLD;
	if (std::fabs(diff) < NOSYNC_THRESHOLD)
	{
		if (diff <= -syncThreshold)
		{
			delay = 0.0;
		}
		else if (diff >= syncThreshold)
		{
			delay = 2.0 * delay;
		}
	}

	_frameTimer += delay;
	double actualDelay = _frameTimer - (av_gettime() / 1000000.0);
	if (actualDelay < 0.010)
	{
		actualDelay = 0.010;
	}
	return actualDelay * 1000 + 0.5;
}
