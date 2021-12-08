all:
	ffmpeg \
	-f s16le -ac 1 -ar 44100 -acodec pcm_s16le -i audio/audio.pcm \
	-r 10 -f image2 -i video/in%d.jpg \
	-acodec aac -vcodec libx264 -r 10 out.mp4
