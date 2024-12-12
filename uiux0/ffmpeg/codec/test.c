#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>


void print_avcodeccontext(AVCodecContext* ctx) {
    if (ctx == 0) {
        printf("AVCodecContext is null\n");
        return;
    }

    printf("AVCodecContext:\n");
    printf("  codec_id: %d\n", ctx->codec_id); // 编解码器ID
    printf("  codec_type: %d\n", ctx->codec_type); // 流类型（视频、音频等）
    printf("  bits/s: %lld\n", ctx->bit_rate); // 比特率
    printf("  width x height: %dx%d\n", ctx->width, ctx->height); // 视频宽度和高度
    printf("  gop_size: %d\n", ctx->gop_size); // 关键帧间隔
    printf("  pix_fmt: %d\n", ctx->pix_fmt); // 像素格式
    printf("  time_base: %d/%d\n", ctx->time_base.num, ctx->time_base.den); // 时间基准
}

// 初始化解码器
int init_decoder(AVCodecContext **dec_ctx) {
    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        printf("Codec not found\n");
        return -1;
    }

    *dec_ctx = avcodec_alloc_context3(codec);
    if (!*dec_ctx) {
        printf("Could not allocate video codec context\n");
        return -1;
    }

    if (avcodec_open2(*dec_ctx, codec, 0) < 0) {
        printf("Could not open codec\n");
        return -1;
    }

    return 0;
}

// 解码一包数据
int decode_packet(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, int* frame_count) {
    int ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        printf("Error sending a packet for decoding\n");
        return -1;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            printf("Error during decoding\n");
            return -1;
        }

        // 在这里处理解码后的帧，例如保存为图片
        printf("Decoded frame =%d\n", *frame_count);
        *frame_count += 1;

        // 例如，可以使用OpenCV保存帧（此处未展示）
    }
    return 0;
}



// 初始化H.265编码器
int init_encoder(AVCodecContext **enc_ctx, int width, int height) {
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
    if (!codec) {
        printf("Codec not found\n");
        return -1;
    }

    *enc_ctx = avcodec_alloc_context3(codec);
    if (!*enc_ctx) {
        printf("Could not allocate video codec context\n");
        return -1;
    }

    (*enc_ctx)->bit_rate = 400000;
    (*enc_ctx)->width = width;
    (*enc_ctx)->height = height;
    (*enc_ctx)->time_base = (AVRational){1, 25};
    (*enc_ctx)->framerate = (AVRational){25, 1};
    (*enc_ctx)->gop_size = 10;
    (*enc_ctx)->max_b_frames = 1;
    (*enc_ctx)->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(*enc_ctx, codec, 0) < 0) {
        printf("Could not open codec\n");
        return -1;
    }

    return 0;
}

// 编码一帧数据
int encode_frame(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt) {
    int ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        printf("Error sending a frame for encoding\n");
        return -1;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return -1;
        } else if (ret < 0) {
            printf("Error during encoding\n");
            return -1;
        }

        // 在这里处理编码后的数据包，例如写入文件或发送到网络
        printf("Encoded packet\n");

        av_packet_unref(pkt);
    }

    return 0;
}

int feed_data(AVPacket *pkt){
    return 0;
}
int main() {
    AVCodecContext *dec_ctx = 0;
    if (init_decoder(&dec_ctx)) {
        printf("Failed to initialize the decoder\n");
        return -1;
    }
    print_avcodeccontext(dec_ctx);

    // 假设你已经知道视频流的分辨率
    int width = 1920, height = 1080;
    AVCodecContext *enc_ctx = 0;
    if (init_encoder(&enc_ctx, width, height)) {
        printf("Failed to initialize the encoder\n");
        return -1;
    }
    print_avcodeccontext(enc_ctx);

    AVPacket *pkt = av_packet_alloc();
    if (!pkt) {
        printf("Failed to allocate the packet\n");
        return -1;
    }

    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        printf("Failed to allocate the frame\n");
        return -1;
    }

    int frame_count = 0;
    // 假设你有一个函数feed_data()用于填充pkt，这里只是一个示意
    while (feed_data(pkt)) {
        if (decode_packet(dec_ctx, pkt, frame, &frame_count)) {
            printf("Failed to decode packet\n");
            break;
        }

        // 对解码后的帧进行编码
        if (encode_frame(enc_ctx, frame, pkt)) {
            printf("Failed to encode frame\n");
            break;
        }
    }

    // 发送NULL帧到编码器以刷新剩余的帧
    encode_frame(enc_ctx, 0, pkt);

    // 清理
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&dec_ctx);
    avcodec_free_context(&enc_ctx);

    return 0;
}

