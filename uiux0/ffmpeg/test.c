#include <libavformat/avformat.h>
int info()
{
	int ver = avformat_version();
	const char* config = avformat_configuration();
	const char* license = avformat_license();
	printf("ver=%x\n", ver);
	printf("config=%s\n", config);
	printf("license=%s\n", license);

	int cnt = 0;
	void* iter = 0;
	const AVInputFormat* ifmt = 0;
	const char* name = 0;
	printf("av_demuxer_iterate\n");
	while(1){
		ifmt = av_demuxer_iterate(&iter);
		if(0 == ifmt)break;

		name = ifmt->long_name;
		if(0 == name)name = ifmt->name;
		if(0 == name)name = "";
		printf("%d: ifmt=%p, name=%s\n", cnt, ifmt,
			name
		);
		cnt++;
	}

	const AVOutputFormat* ofmt = 0;
	iter = 0;
	cnt = 0;
	printf("av_muxer_iterate\n");
	while(1){
		ofmt = av_muxer_iterate(&iter);
		if(0 == ofmt)break;

		name = ofmt->long_name;
		if(0 == name)name = ofmt->name;
		if(0 == name)name = "";
		printf("%d: ofmt=%p, name=%s, %x,%x,%x\n", cnt, ofmt,
			name,
			ofmt->audio_codec, ofmt->video_codec, ofmt->subtitle_codec
		);
		cnt++;
	}
	return 0;
}
int main(int argc, char** argv)
{
	if(argc < 2){
		printf("./a.out xxxx.mp4\n");
		return -1;
	}

	AVFormatContext* ctx = avformat_alloc_context();

	int ret = avformat_open_input(&ctx, argv[1], NULL, NULL);
	if(0 != ret){
		printf("open dst file error!!!");
		goto fail;
	}

	AVDictionary *metadata = ctx->metadata;
	int count = av_dict_count(metadata);
	AVDictionaryEntry *prev = NULL;
	if (count) {
		for (int i = 0; i < count; i++) {
			prev = av_dict_get(metadata, "", prev, AV_DICT_IGNORE_SUFFIX);
			if(prev)printf("%s = %s \n", prev->key, prev->value);
		}
	}
	printf("duration=%lld\n", ctx->duration);
	printf("bitrate=%lld\n", ctx->bit_rate);

	AVStream **streams = ctx->streams;
	int stream_counts = ctx->nb_streams;
	if (stream_counts > 0) {
		for (int index = 0; index < stream_counts; index++) {
			AVStream *stream = streams[index];
			int stream_index = stream->index;
			printf("stream index %d\n", stream_index);
			printf("	time_base=%d/%d\n",stream->time_base.num, stream->time_base.den);
			printf("	duration=%lld\n",stream->duration);
			printf("	nb_frames=%lld\n",stream->nb_frames);
		}
	}

good:
	avformat_close_input(&ctx);
fail:
	avformat_free_context(ctx);
}
