#include "demuxer.h"
#include "decoder.h"

extern "C"
{
#include <libavformat/avformat.h>
}

#include <iostream>


Demuxer::Demuxer()
	: _format_ctx(NULL)
{

}

Demuxer::~Demuxer()
{
	Close();
}


bool Demuxer::Open(const char* url)
{
	if (_format_ctx)
	{
		Close();
	}

	char errbuf[AV_ERROR_MAX_STRING_SIZE + 1];
	int ret = 0;
	ret = avformat_open_input(&_format_ctx, url, NULL, NULL);
	if (ret < 0)
	{
		av_strerror(ret, errbuf, sizeof(errbuf));
		std::cerr << "avformat_open_input() failed. ret: " 
			<< ret << "(" << errbuf << ")" << std::endl;
		return false;
	}

	ret = avformat_find_stream_info(_format_ctx, NULL);
	if (ret < 0)
	{
		av_strerror(ret, errbuf, sizeof(errbuf));
		std::cerr << "avformat_find_stream_info() failed. ret: " 
			<< ret << "(" << errbuf << ")" << std::endl;
		return false;
	}

	for (int i = 0; i < static_cast<int>(_format_ctx->nb_streams); i++)
	{
		av_dump_format(_format_ctx, i, url, 0);
	}

	return true;
}

void Demuxer::Close()
{
	if (_format_ctx)
	{
		avformat_close_input(&_format_ctx);
		_format_ctx = NULL;
	}
}

bool Demuxer::readFrame(AVPacket* packet)
{
	bool result = true;
	_readFailed = false;
	int ret = av_read_frame(_format_ctx, packet);
	if (ret < 0)
	{
		result = false;
		if (_format_ctx->pb->error != 0) 
		{
			_readFailed = true;
			char errbuf[AV_ERROR_MAX_STRING_SIZE + 1];
			av_strerror(ret, errbuf, sizeof(errbuf));
			std::cerr << "av_read_frame() failed. ret = " 
				<< ret << "(" << errbuf << ")" << std::endl;
		}
	}
	return result;
}

int Demuxer::getStreamIndex(StreamType type) const
{
	if (!_format_ctx)
	{
		std::cerr << "Format context is not opened yet. " << std::endl;
		return -1;
	}

	int index = -1;
	switch (type)
	{
	case StreamType::Audio:
		index = av_find_best_stream(_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
		break;
	case StreamType::Video:
		index = av_find_best_stream(_format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
		break;
	default:
		std::cerr << "Unsupported stream type " << static_cast<int>(type) << std::endl;
	}

	return index;
}

std::unique_ptr<Decoder> Demuxer::createDecoder(int streamIndex)
{
	if (!_format_ctx)
	{
		std::cerr << "Format context is not opened yet. " << std::endl;
		return nullptr;
	}

	if (streamIndex == -1 || streamIndex >= static_cast<int>(_format_ctx->nb_streams))
	{
		std::cerr << "Invalid stream index " << streamIndex << std::endl;
		return nullptr;
	}

	AVStream* stream = _format_ctx->streams[streamIndex];
	AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
	if (!codec)
	{
		std::cerr << "Could not find decoder. codec id = " << stream->codecpar->codec_id << std::endl;
		return nullptr;
	}

	AVCodecContext* codec_context = avcodec_alloc_context3(codec);
	if (!codec_context)
	{
		std::cerr << "Could not allocate codec context." << std::endl;
		return nullptr;
	}

	char errbuf[AV_ERROR_MAX_STRING_SIZE + 1];
	int ret = -1;
	ret = avcodec_parameters_to_context(codec_context, stream->codecpar);
	if (ret < 0)
	{
		av_strerror(ret, errbuf, sizeof(errbuf));
		std::cerr << "Could not copy codec parameters to decoder context. ret = " 
			<< ret << "(" << errbuf << ")" << std::endl;
		avcodec_free_context(&codec_context);
		return nullptr;
	}

	ret = avcodec_open2(codec_context, codec, NULL);
	if (ret < 0)
	{
		av_strerror(ret, errbuf, sizeof(errbuf));
		std::cerr << "Could not open  codec. ret = " 
			<< ret << "(" << errbuf << ")" << std::endl;
		avcodec_free_context(&codec_context);
		return nullptr;
	}

	return std::make_unique<Decoder>(codec_context);
}
