#!/bin/bash
printf '\n%50s\n' | tr ' ' '='
echo " updating repository..."
printf '%50s\n\n' | tr ' ' '='

echo "cloud4533" | sudo -S apt update

printf '\n%50s\n' | tr ' ' '='
echo " installing binaries to system folders..."
printf '%50s\n\n' | tr ' ' '='

echo "cloud4533" | sudo -S mv /root/THEMIS/*.so /usr/local/lib/
echo "cloud4533" | sudo -S mv /root/THEMIS/Themis_Host /root/Themis_Host/

printf '\n%50s\n' | tr ' ' '='
echo " killing background processes..."
printf '%50s\n\n' | tr ' ' '='

killall 'Xvfb'
killall 'xfce4-session'
kill $(ps aux | grep '/Themis_Host' | awk '{print $2}')

printf '\n%50s\n' | tr ' ' '='
echo " rebooting..."
printf '%50s\n\n' | tr ' ' '='

nohup sh run.sh &