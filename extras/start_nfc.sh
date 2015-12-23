#bin/bash!

# This script file will be copied to /home/pi folder path
# It will change directore to project path and run nfc application.
# It will restart application if it fail and return back.


cd /home/pi/NFC/NFC_device/build
echo "start_nfc : current directory : ${PWD}"

while true;
do
echo "Running nfc application..."
./nfc ../../../share/
done
