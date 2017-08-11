## Before installation
1. modify `./torcs-1.3.6/src/libs/raceengineclient/raceengine.cpp: 683`
	
		sprintf(filename, "/home/users/TORCS/data/%d.tga", img_num);
		
	by which directory you want to put the images, e.g., if you want to place the images into `/home/name/Desktop/balabala`, you should modify this into
	
		sprintf(filename, "/home/name/Desktop/balabala/%d.tga", img_num);

2. modify `./torcs-1.3.6/src/libs/raceengineclient/racestate.cpp: 113`
	
		data_out.open("/home/users/TORCS/data/data.txt");
		
	by which directory you want to put the steerangle information, e.g., if you want to place the information into `/home/name/Desktop/balabala`, you should modify this into
	
		data_out.open("/home/name/Desktop/balabala/data.txt");

3. Make sure you uninstall previous installed TORCS and its dependencies by

        sudo rm -rf /usr/local/share/games/torcs
        sudo rm -rf /usr/local/lib/torcs
        sudo apt-get remove --auto-remove torcs
        sudo apt-get purge --auto-remove torcs

## Installation
Just `./install.sh` , it will need you enter password in the middle. Then the torcs will be compiled in the current directory and installed.

## Collect data
1. type `torcs` in command line
2. `Race` -> `quickrace` -> `Configure Race` -> choose a track, click accept -> click the `Player` in the right column, click `(De)Select`, click accept -> click accept
3. `Newrace` and the race will begin, drive the car use `up, down, left, right`.
4. It will automatically begin to collect data. Enter `f2` to switch from different view point.
5. exit with `esc`
6. you will find corresponding data in the directory you modified in *Before installation* section


## Data
* images are stored in `.tga` format, and named with a number
* steerangles are stored in file `data.txt`, each line is composed of `imageNumber, steerangle`. Steer angle is stored in rad.

## Note
* if you change anything in the source code (`./torcs-1.3.6/`) and want to make this take effect, you need to reinstall torcs.

* In case any stuck happens when you click `Configure Race` while initialization, put the quickrace.xml under TORCS root folder`./torcs/config/raceman/`
