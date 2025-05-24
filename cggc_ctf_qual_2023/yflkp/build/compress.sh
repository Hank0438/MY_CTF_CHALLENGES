#!/bin/sh

rm yflkp.zip
sh ./build.sh
cd ..
zip -r yflkp.zip yflkp/share/bios yflkp/share/run.sh yflkp/share/bzImage yflkp/share/initramfs.cpio.gz yflkp/Dockerfile yflkp/xinetd yflkp/docker-compose.yml
mv yflkp.zip ./yflkp/