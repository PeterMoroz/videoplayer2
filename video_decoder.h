#pragma once

#include "decoder.h"

class VideoDecoder : public Decoder
{
public:
	explicit VideoDecoder(AVCodecContext* codec_context);
};
