#pragma once


struct AVFormatContext;
struct AVPacket;

class Decoder;

#include <memory>

class Demuxer final
{
public:
	enum StreamType
	{
		Unknown = -1,
		Video = 0,
		Audio = 1,
	};

public:
	Demuxer();
	~Demuxer();

	Demuxer(const Demuxer&) = delete;
	Demuxer& operator=(const Demuxer&) = delete;

	bool Open(const char* url);
	
	void Close();

	bool readFrame(AVPacket* packet);
	bool isReadFailed() const { return _readFailed; }

	int getStreamIndex(StreamType type) const;
	
	std::unique_ptr<Decoder> createDecoder(int streamIndex);

private:
	AVFormatContext* _format_ctx;
	bool _readFailed = false;
};
