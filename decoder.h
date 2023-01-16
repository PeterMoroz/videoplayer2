#pragma once

#include <functional>

struct AVCodecContext;
struct AVPacket;
struct AVFrame;

class Decoder final
{
public:
	Decoder(const Decoder&) = delete;
	Decoder& operator=(const Decoder&) = delete;

	Decoder();
	~Decoder();

	explicit Decoder(AVCodecContext* codec_context);

	template <typename T>
	bool getParameter(const char* key, T& value) const;

	bool sendPacket(const AVPacket *packet);

	void setFrameReceiver(std::function<void(AVFrame*)>&& onFrameReady);

protected:
	AVCodecContext* _codec_context;
	AVFrame* _frame;
	std::function<void(AVFrame*)> _onFrameReady;
};
