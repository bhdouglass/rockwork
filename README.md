# Rockwork, the Unofficial Ubuntu Pebble App

An unofficial Pebble App for Ubuntu

## Development

* Install the [Ubuntu SDK](https://developer.ubuntu.com/en/start/ubuntu-sdk/installing-the-sdk/)
* Setup a [click chroot](https://developer.ubuntu.com/en/apps/sdk/tutorials/click-targets-and-device-kits/) (for armhf compiling)
* Installing dependencies (in the armhf chroot)
    * Use the Ubuntu SDKs kit management interface to open up cli access to the chroot
    * OR access it via command line: `click chroot -a armhf -f ubuntu-sdk-15.04 maint`
    * Run: `apt-get install qtconnectivity5-dev:armhf libqt5bluetooth5:armhf libtelepathy-qt5-dev:armhf libqmenumodel-dev:armhf libquazip-qt5-dev:armhf liburl-dispatcher1-dev:armhf`
* Compile Rockwork
    * Build/Run using the Ubuntu SDK and the desired kit
    * OR from the cli: `qmake path/to/source && make`
* Compile Rockwork for Debugging
    * Edit the build configuration to include CONFIG+=testing in the qmake arguments, then build/run using the Ubuntu SDK and the desired kit
    * OR from the cli: `qmake path/to/source CONFIG+=testing && make`

## Snooping on Pebble communications

NOTE: This setup requires an Android device

* Install [Wireshark](https://www.wireshark.org/)
* Launch Wireshark with the [Pebble Dissector](https://github.com/bhdouglass/PebbleDissector)
    * Run: `wireshark -X lua_script:pebble.lua`
* Setup [developer options](https://wiki.cyanogenmod.org/w/Doc:_developer_options) on Android
* In the developer options enable "Enable Bluetooth HCI snoop log"
* Do something interesting with your Pebble
* Send the bluetooth log (`/sdcard/btsnoop_hci.log`) to your computer with Wireshark
* Load btsnoop_hci.log into Wireshark
    * If Wireshark does not automatically detect the Pebble portions of the log you will need to use the option "Decode As" and make sure options for "BT RFCOMM" are selected

Original guide found here: <http://wordpress.meulenhoff.org/?p=996>
