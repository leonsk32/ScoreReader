# Score Reader

自主プロジェクトの一環で作成。

画像として用意した楽譜を読み取ります。
以下の方法で演奏することができます。

- 7dof
    - 7 自由度アームによるキーボード演奏
- arduino
    - arduino オルゴール
- unity
    - バーチャルピアノ（Unity）


## How to install OpenCV2

``` bash
brew install pkg-config
brew install opencv@2
```

## How to compile

``` bash
# For pkg-config to find opencv@2 you may need to set:
export PKG_CONFIG_PATH="/usr/local/opt/opencv@2/lib/pkgconfig"

make all
```

options:
``` bash
# If you need to have opencv@2 first in your PATH run:
echo 'export PATH="/usr/local/opt/opencv@2/bin:$PATH"' >> ~/.zshrc

# For compilers to find opencv@2 you may need to set:
export LDFLAGS="-L/usr/local/opt/opencv@2/lib"
export CPPFLAGS="-I/usr/local/opt/opencv@2/include"
```

## How to run
``` bash
# ./score_reder <image file location without extension> <extension>
./score_reader scores/kirakira jpg

# 記号読み取り
./score_reader scores/kirakira jpg 1 G_clef
```

# 7dof arm
``` bash
cd ./7dof
roslaunch dynamixel_7dof_arm dynamixel_7dof_arm_bringup.launch
roseus
load "calib.l"
load "mechanical_pianist.l"
```

# arduino
``` bash
sudo chmod a+rw /dev/ttyACM0
cd ./arduino/mechanical_pianist
mechanical_pianist.ino
```