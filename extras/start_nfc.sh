#bin/bash!

# This script file will be copied to /home/pi folder path
# It will change directore to project path and run nfc application.
# It will restart application if it fail and return back.

sudo chmod 777 /sys/class/leds/led0/trigger
sudo chmod 777 /sys/class/leds/led0/brightness

echo none > /sys/class/leds/led0/trigger
sudo echo 0 > /sys/class/leds/led0/brightness

cd /home/pi/NFC/NFC_device/build
echo "start_nfc : current directory : ${PWD}"
echo "Running nfc SERVER..."
 ../MyLib/libllcp/examples/snep-server/snep-server -o l

sudo echo 1 > /sys/class/leds/led0/brightness
while true;
do
echo "Running nfc application..."
./nfc ../../../share/
done
