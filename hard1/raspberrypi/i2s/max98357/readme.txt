1:
	sudo vi /etc/modprobe.d/raspi-blacklist.conf
	if(empty)goto 2
	else{
		#blacklist i2c-bcm2708
		#blacklist snd-soc-pcm512x 
		#blacklist snd-soc-wm8804
	}

2:
	sudo vi /etc/modules
	if(empty)goto 3
	else{
		#snd_bcm2835
	}

3:
	sudo vi /etc/asound.conf
	copy and paste this:

pcm.speakerbonnet {
   type hw card 0
}
 
pcm.dmixer {
   type dmix
   ipc_key 1024
   ipc_perm 0666
   slave {
     pcm "speakerbonnet"
     period_time 0
     period_size 1024
     buffer_size 8192
     rate 44100
     channels 2
   }
}
 
ctl.dmixer {
    type hw card 0
}
 
pcm.softvol {
    type softvol
    slave.pcm "dmixer"
    control.name "PCM"
    control.card 0
}
 
ctl.softvol {
    type hw card 0
}
 
pcm.!default {
    type             plug
    slave.pcm       "softvol"
}

4:
	sudo vi /boot/config.txt
	do this:
#dtparam=audio=on
dtoverlay=hifiberry-dac
dtoverlay=i2s-mmap

5:
	sudo reboot

6:
	speaker-test -c2
	speaker-test -c2 --test=wav -w /usr/share/sounds/alsa/Front_Center.wav
	mpg123 http://ice1.somafm.com/u80s-128-mp3
