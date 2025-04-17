## Teo-self-presentation demonstration: Installation from Source Code

First install the dependencies:
- [Install CMake 3.19+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-cmake.md)
- [Install YCM 0.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-ycm.md)
- [Install YARP 3.10+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-yarp.md)
- [Install SPEECH](https://github.com/roboticslab-uc3m/speech/blob/master/doc/speech-install.md) (only required by dialogueManager)

### Install teo-self-presentation demo on Ubuntu

Note that you will be prompted for your password upon using `sudo` a couple of times:

```bash
cd  # go home
mkdir -p repos; cd repos  # create $HOME/repos if it does not exist; then, enter it
git clone https://github.com/roboticslab-uc3m/teo-self-presentation.git  # download sources
cd teo-self-presentation && mkdir build && cd build && cmake ..  # configure via CMake
make -j$(nproc)  # compile
sudo make install  # install
cp ../scripts/gnome/teo-self-presentation.desktop $HOME/Desktop
```

For additional options, use `ccmake` instead of `cmake`.
