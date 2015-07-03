convert -rotate 90 $1 $1.bgr
if [ -f ${1%.*}.bin ]
then
	echo Removing old file...
	rm ${1%.*}.bin
fi
mv $1.bgr ${1%.*}.bin
