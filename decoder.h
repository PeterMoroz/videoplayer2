#pragma once

#include <list>

struct AVCodecContext;
struct AVPacket;
struct AVFrame;

class FrameReceiver;

class Decoder final
{
public:
	Decoder(const Decoder&) = delete;
	Decoder& operator=(const Decoder&) = delete;

	Decoder();
	~Decoder();

	explicit Decoder(AVCodecContext* codec_context);

	bool getParameter(const char* key, int& value) const;

	bool sendPacket(const AVPacket *packet);

	void addFrameReceiver(FrameReceiver& receiver);

protected:
	AVCodecContext* _codec_context;
	AVFrame* _frame;
	std::list<std::reference_wrapper<FrameReceiver>> _frameReceivers;
};