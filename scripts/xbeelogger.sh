STAMP=`date +'%Y-%m-%dT%H:%M:%S'`
echo ${STAMP} START > /dev/tty
while true; do
	read LINE
	STAMP=`date +'%Y-%m-%dT%H:%M:%S'`
	echo ${STAMP} ${LINE} > /dev/tty
done
