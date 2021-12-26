## Teo-self-presentation demonstration: Installation from Source Code

First install the dependencies:
- [Install CMake 3.12+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-cmake.md)
- [Install YCM 0.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-ycm.md)
- [Install YARP 3.5+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md)
- [Install SPEECH](https://github.com/roboticslab-uc3m/speech/blob/master/doc/speech-install.md) (only required by dialogueManager)

### Install teo-self-presentation demo on Ubuntu

Note that you will be prompted for your password upon using `sudo` a couple of times:

```bash
cd  # go home
mkdir -p repos; cd repos  # create $HOME/repos if it does not exist; then, enter it
git clone https://github.com/roboticslab-uc3m/teo-self-presentation.git  # Download teo-self-presentation demostration software from the repository
cd teo-self-presentation; mkdir build; cd build; cmake ..  # Configure the teo-self-presentation demostration software
make -j$(nproc)  # Compile
sudo make install  # Install :-)
cp ../scripts/gnome/teo-self-presentation.desktop $HOME/Desktop
```

For additional options, use `ccmake` instead of `cmake`.
