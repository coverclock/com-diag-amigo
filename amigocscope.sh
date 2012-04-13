cd ~/src
exec ~/bin/kcscope \
	Amigo \
	/usr/local/avr/avr/include \
	/usr/local/avr/lib/gcc/avr/4.5.1/include \
	/usr/local/avr/lib/gcc/avr/4.5.1/include-fixed \
	/usr/local/avr/lib/gcc/avr/4.5.1/install-tools/include \
	/usr/local/avr/avr/lib/ldscripts \
	avr-libc-1.8.0 \
	gcc-4.5.1/gcc/config/avr
