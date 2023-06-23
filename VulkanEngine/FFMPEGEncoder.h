#pragma once
#include "VEInclude.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <iostream>




extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include "MyUDPSend6.h"
}


class FFMPEGEncoder
{
private:
	uint32_t height;
	uint32_t width;
	


	const char* filename = "file.mp4";
	const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MPEG2VIDEO);
	AVCodecContext* c = NULL;
	int  ret;
	FILE* f;
	AVFrame* frame;
	AVPacket* pkt;
	uint8_t endcode[4] = { 0, 0, 1, 0xb7 };
	struct SwsContext* sws_context = NULL;
	MyUDPSend6 sender;

	
public:
	int cter = 0;
	FFMPEGEncoder() {
		
	}
	void setHeight(const uint32_t h) {
		height = h;
	}
	void setWidth(const uint32_t w) {
		width = w;
	}
	void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt,
		FILE* outfile)
	{
		int ret;

		/* send the frame to the encoder */
		//printf("Send frame %3"PRId64"\n", frame->pts);

		ret = avcodec_send_frame(enc_ctx, frame);
		if (ret < 0) {
			fprintf(stderr, "Error sending a frame for encoding\n");
			exit(1);
		}

		while (ret >= 0) {
			ret = avcodec_receive_packet(enc_ctx, pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return;
			else if (ret < 0) {
				fprintf(stderr, "Error during encoding\n");
				exit(1);
			}

			//printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
			//std::cout << "Write packet " << pkt->pts << " (size=" << pkt->size << ")\n";
			
			//fwrite(pkt->data, 1, pkt->size, outfile);

			sender.send(pkt->data, pkt->size);
			av_packet_unref(pkt);
			
		}
	}
	void setup() {

		std::cout << "setup" << std::endl;

		sender.start();


		if (!codec) {
			fprintf(stderr, "Codec '%s' not found\n");
			exit(1);
		}

		c = avcodec_alloc_context3(codec);
		if (!c) {
			fprintf(stderr, "Could not allocate video codec context\n");
			exit(1);
		}
		pkt = av_packet_alloc();
		if (!pkt)
			exit(1);

	
		c->bit_rate = 650000;
		
		c->width = 704;
		c->height = 576;
		
		c->time_base = { 1, 30 };
		c->framerate = { 30, 1 };

		
		c->gop_size = 10;
		c->max_b_frames = 1;
		c->pix_fmt = AV_PIX_FMT_YUV420P;
		
		

		if (codec->id == AV_CODEC_ID_H264)
			av_opt_set(c->priv_data, "preset", "slow", 0);

		/* open it*/
		ret = avcodec_open2(c, codec, NULL);
		if (ret < 0) {
			std::cout << "Could not open codec" << std::endl;
			//fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
			exit(1);
		}

		f = fopen(filename, "wb");
		if (!f) {
			fprintf(stderr, "Could not open %s\n", filename);
			exit(1);
		}


		frame = av_frame_alloc();
		if (!frame) {
			fprintf(stderr, "Could not allocate video frame\n");
			exit(1);
		}
		frame->format = c->pix_fmt;
		frame->width = c->width;
		frame->height = c->height;

		ret = av_frame_get_buffer(frame, 0);
		if (ret < 0) {
			fprintf(stderr, "Could not allocate the video frame data\n");
			exit(1);
		}
	}
	virtual void FFMPEGencode(uint8_t* data);
	virtual ~FFMPEGEncoder() {
		encode(c, NULL, pkt, f);

		if (codec->id == AV_CODEC_ID_MPEG1VIDEO || codec->id == AV_CODEC_ID_MPEG2VIDEO) {
			std::cout << "endcode sent" << std::endl;
			sender.send(endcode, sizeof(endcode));
			//fwrite(endcode, 1, sizeof(endcode), f);
		}
		sender.close();
		
		fclose(f);

		avcodec_free_context(&c);
		av_frame_free(&frame);
		av_packet_free(&pkt);
	};

};

