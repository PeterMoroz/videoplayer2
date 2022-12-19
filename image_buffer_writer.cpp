#include "image_buffer_writer.h"
#include "image_buffer.h"

ImageBufferWriter::~ImageBufferWriter()
{

}

void ImageBufferWriter::init(ImageBuffer& imageBuffer)
{
	for (size_t i = 0; i < 4; i++)
	{
		_image_data[i] = imageBuffer._image_data[i];
		_linesize[i] = imageBuffer._linesize[i];
	}
}
