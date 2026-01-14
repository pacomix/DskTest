# A bit of background
The original code and the build pipeline (python 2.7 based) is more than 10 years old but it's been modified to work with Python 3.

Surprisingly it only needed changes on the print statements and string output from the subprocess.Popen commands. The rest did work as it was.

The build script(s) expect the compiler binaries and tools to exist under specific directories and are not taken from the system PATH. The tools are provided for both MacOS and Windows so it should work without issues in any of the systems.

But right at the moment I can only ensure it works out of the box with a MacOS computer with M1/2/3/4/5 processors and using SDCC 4.5.0.

Some of the required tools like hex2bin were compiled years ago and put into the respective directories. Feel free to re-compile and/or source the binaries yourself if you don't trust the executables.

Also mention that part of the FDC routines where highly influenced/adapted/integrated from the FDC Tools by Julien-nevo (Targhan) *BUT* the main routine that detects the rotational speed of the drive is entirely mine.

This version of the DskTest has never been released since the main intention was to bring the size down to 2KiB or less. It's a new rework of the tool using the Amstrad CPC Firmware (although it was writen 4 years ago) and I don't remember if I finally managed to correctly use Firmware routines that deals with floats and real number in the right way. In any case, I didn't have the possibility to use a real disc drive to test that part so it's not ensure the tool, as it is provided, is reliable.

All the unuseful stuff was scrapped from the code like graphic drawing routines and unuseful dependencies leaving the visual aspect like a "professional" tool :D

PLEASE FORK, TEST AND MODIFY THIS VERSION SO WE ALL CAN BENEFIT!!!

# Setup environment
## MacOS
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

# How to compile
If you went the brew way you can straight type:
```shell
python3 ./build/build.py ./sources/DskTestFirm
```

*OR without brew*
```shell
./build/Python3/bin/python3 ./build/build.py ./sources/DskTestFirm
```

