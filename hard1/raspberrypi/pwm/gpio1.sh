gpio mode 1 pwm		#pin18, gpio12, right6
gpio pwm-ms		#mark space
gpio pwmc 192		#each = 192/19.2m = 10us
gpio pwmr 2000		#period = 10us*2000 = 20ms
gpio pwm 1 100		#width = 10us*100 = 1ms
