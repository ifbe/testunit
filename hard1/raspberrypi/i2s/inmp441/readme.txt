1:
	sudo vi /boot/config.txt
	dtparam=i2s=on

2:
	sudo vi /etc/modules
	snd-bcm2835

3:
	sudo reboot
	lsmod | grep snd

4:
	sudo apt-get install git bc libncurses5-dev bison flex libssl-dev
	sudo wget https://raw.githubusercontent.com/notro/rpi-source/master/rpi-source -O /usr/bin/rpi-source
	sudo chmod +x /usr/bin/rpi-source
	/usr/bin/rpi-source -q --tag-update
	rpi-source --skip-gcc

5:
	sudo mount -t debugfs debugs /sys/kernel/debug
	Pi 3 or 2 - make sure the module name is 3f203000.i2s 
	Pi Zero - the module name is 20203000.i2s

6:
	git clone https://github.com/PaulCreaser/rpi-i2s-audio
	cd rpi-i2s-audio
	if pi0: vi my_loader.c{
		.platform = "20203000.i2s",
		.name = "20203000.i2s",
	}

7:
	make -C /lib/modules/$(uname -r )/build M=$(pwd) modules
	sudo insmod my_loader.ko	
	lsmod | grep my_loader
	dmesg | tail

7.5(optional):
	sudo cp my_loader.ko /lib/modules/$(uname -r)
	echo 'my_loader' | sudo tee --append /etc/modules > /dev/null
	sudo depmod -a
	sudo modprobe my_loader

8:
	arecord -l
	arecord -D plughw:1 -c1 -r 48000 -f S32_LE -t wav -V mono -v file.wav
	#arecord -D plughw:1 -c2 -r 48000 -f S32_LE -t wav -V stereo -v file_stereo.wav

8.5(optional):
	sudo vi ~/.asoundrc
	copy and paste it:
pcm.dmic_hw {
	type hw
	card sndrpisimplecar
	channels 2
	format S32_LE
}
pcm.dmic_sv {
	type softvol
	slave.pcm dmic_hw
	control {
		name "Boost Capture Volume"
		card sndrpisimplecar
	}
	min_dB -3.0
	max_dB 30.0
}
    
