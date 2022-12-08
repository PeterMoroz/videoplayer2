#pragma once

struct AVFrame;

class FrameReceiver
{
public:
	virtual ~FrameReceiver() = 0;
	virtual void acceptFrame(const AVFrame* frame) = 0;
	// virtual void operator()(const AVFrame* frame) = 0;
};
