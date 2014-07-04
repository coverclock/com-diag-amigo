#!/bin/bash
stty sane dec
stty -f /dev/tty.usbserial-A800f3aA hupcl
screen -c .screenrc4xbeeconsole -L /dev/tty.usbserial-A800f3aA 9600 cs8
# ^a: exec ./xbeelogger.sh
stty sane dec
