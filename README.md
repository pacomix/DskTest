# DskTest (an Amstrad CPC disk drive tool)
This tool is a native Amstrad CPC tool that can help to diagnose a faulty disk drive.

Main purposes:
* Measure disk drive RPMs.
* Alignment of the motor.

# Requirements
Any Amstrad CPC with a working disk drive or cassette player to load the program.
Unfortunately the latest releases don't work in a 464/664 machines yet.

Head to the Releases section to download it.

# Features
* Disk drive selection.
* Start/stop the disk drive motor.
* Move head to track.
* Search for a Sector ID.
* Measure RPMs.

# Instructions
## Options
### Drive
Press Enter to discover and switch to the next available drive (A/B). Only two drives are supported.
The switch can be activated even while measuring the RPMs in realtime.

### Motor
Press Enter to start/stop the motor of the selected drive.
If a Sector ID wasn't found as a result of a previous search, the tool will start measuring RPMs. This is tipically the case when RPMs have been measuring and we stopped the motor intentionally, so activating it again will "resume" the measuring.

### Track
Moves the drive head to the currently selected track (0-41).

Select the track with the left-right cursors and activate the functionality with Enter.

### Sector
Try to find a Sector ID in the current track with the specified value.

Select the Sector ID with the left-right cursor keys and activate the functionality with Enter.

### RPMs
Measure the RPMs of the current selected drive. It turns the motor on, searches for an invalid Sector ID and it starts the process.

The RPMs will be calculated every 2 seconds but that can be changed with the next option.

Worth noting that you can move and change any of the available parameters during the measurement.

### UpdSec
This is the time in seconds where the RPM measurement takes place.

A good value is between 2-8 seconds.

# A bit of background
The original code and the build pipeline (python 2.7 based) is more than 10 years old but it's been modified to work with Python 3.

Surprisingly it only needed changes on the print statements and string output from the subprocess.Popen commands. The rest did work as it was.

The build script(s) expect the compiler binaries and tools to exist under specific directories and are not taken from the system PATH. The tools are provided for both MacOS and Windows so it should work without issues in any of the systems.

But right at the moment I can only ensure it works out of the box with a MacOS computer with M1/2/3/4/5 processors and using SDCC 4.5.0.

Some of the required tools like hex2bin were compiled years ago and put into the respective directories. Feel free to re-compile and/or source the binaries yourself if you don't trust the executables.

Also mention that the FDC routines where highly influenced from the original FDC Tools code by Julien-nevo (Targhan) *BUT* the main routine that detects the rotational speed of the drive and all the other functionality are entirely mine.

# Collaboration
PLEASE FORK, TEST, MODIFY AND PROPOSE CHANGES TO THIS VERSION SO WE ALL CAN BENEFIT!!!

# How to compile
## Pre-requisites
### MacOS
If you would like to file the required tools by yourself you can follow the following procedure.
Install Homebrew - The missing Package Manager for MacOS (or Linux). In a terminal window with superuser access or skip this step if you don't want to use it (there is a way also to not use brew here described as well):
```shell
sudo su
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Create a link for the just installed SDCC brew package. In a terminal from within the cloned repository type:
```shell
brew install sdcc
ln -Fs /opt/homebrew/Cellar/sdcc/4.5.0 ./build/sdcc/darwin/4.5.0
```
*OR without brew*
```shell
curl -fSL 'https://downloads.sourceforge.net/project/sdcc/sdcc-macos-amd64/4.5.0/sdcc-4.5.0-x86_64-apple-macosx.tar.bz2?ts=gAAAAABpaAkcw2xwyQN-bDp2aq3EVPg9si-ate0GzZyK4LNtNesC7llCVmrUAz8aU3L_Wgn5yNdKvTp_75Sqe4zyORoX0NlfoA%3D%3D&r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fsdcc%2Ffiles%2Fsdcc-macos-amd64%2F4.5.0%2Fsdcc-4.5.0-x86_64-apple-macosx.tar.bz2%2Fdownload' -o sdcc.tar.bz2
mkdir ./build/sdcc/darwin/4.5.0; tar -xvjf sdcc.tar.bz2 -C ./build/sdcc/darwin/4.5.0
mv ./build/sdcc/darwin/4.5.0/sdcc-4.5.0/* ./build/sdcc/darwin/4.5.0
rm -rf ./build/sdcc/darwin/4.5.0/sdcc-4.5.0
```

Install Python from Homebrew package manager OR get it from github and put it in the right directory:
`brew install python`
*OR without brew*
```shell
curl -fsSL https://github.com/bjia56/portable-python/releases/download/cpython-v3.13.9-build.0/python-full-3.13.9-darwin-universal2.zip -o python.zip
unzip python.zip -d ./build/Python3  
mv ./build/Python3/python-full-3.13.9-darwin-universal2/* ./build/Python3
rm -rf ./build/Python3/python-full-3.13.9-darwin-universal2 python.zip
```

Finally install pygal to get nice statistics charts:

If you followed the brew way:
```shell
pip install pygal
```
*OR without brew*
```shell
./build/Python3/bin/python3 -m pip install pygal
```
### Windows
TODO

## Compilation
If you went the brew way you can straight type:
```shell
python3 ./build/build.py ./sources/DskTestFirm
```

*OR without brew*
```shell
./build/Python3/bin/python3 ./build/build.py ./sources/DskTestFirm
```

