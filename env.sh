# Run `source env.sh` to set the env variables for esp32
# Must be in the current dir.

# for esp-idf
export PATH="$PATH:`pwd`/crosstool-NG/builds/xtensa-esp32-elf/bin/"
export IDF_PATH="`pwd`/esp-idf"

# for makeEspArduino to find arduino-esp32
export CHIP=esp32
export ESP_ROOT="`pwd`/arduino-esp32/"
alias espmake="make -f `pwd`/makeEspArduino/makeEspArduino.mk"
