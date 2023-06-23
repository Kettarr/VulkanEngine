#include "FFMPEGEncoder.h"
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
#include <time.h>
}







void FFMPEGEncoder::FFMPEGencode(uint8_t* data) {


    fflush(stdout);

    ret = av_frame_make_writable(frame);
    if (ret < 0)
        exit(1);


    const int in_linesize[1] = { 4 * width };
    sws_context = sws_getCachedContext(sws_context,
        width, height, AV_PIX_FMT_RGBA,
        c->width, c->height, AV_PIX_FMT_YUV420P,
        0, 0, 0, 0);
    sws_scale(sws_context, (const uint8_t* const*)&data, in_linesize, 0,
        height, frame->data, frame->linesize);


    frame->pts = this->cter;

    encode(c, frame, pkt, f);


}
