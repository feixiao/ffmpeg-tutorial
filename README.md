# ffmpeg-tutorial

---

教程实验代码来自[mpenkov/ffmpeg-tutorial](https://github.com/mpenkov/ffmpeg-tutorial)，作者已经停止维护，我在他的基础上，修改了代码学习 FFmpeg。

作者的声明如下：

The [original tutorials](http://dranger.com/ffmpeg/) have now been [updated](https://ffmpeg.org/pipermail/libav-user/2015-February/007896.html).
I won't be maintaining this project anymore, and am keeping it here for historical reasons.

---

#### 环境说明：

- Ubuntu 16.04.1 LTS
- FFmpeg 2.8.7
- SDL 1.2

注：不支持 OSX，不支持 WSL

#### 编译运行

- 首先通过源代码安装[FFmpeg 2.8.7](https://ffmpeg.org/download.html#releases)

```shell
wget https://ffmpeg.org/releases/ffmpeg-4.4.tar.gz
./configure --enable-cross-compile --prefix=../dist/x86_64 --arch=x86_64 --cc='clang -arch x86_64'
make -j8
make install


lipo -info ../dist/x86_64/lib/libavcodec.a

# 清理
make distclean
```

注：/opt/homebrew/opt/目标下面的库文件是通过 arm64 架构编译的 ?

- 编译本教程代码，通过如下命令：

      git clone https://github.com/feixiao/ffmpeg-tutorial.git

  cd ffmpeg-tutorial
  make

- 通过如下命令运行程序：

  bin/tutorial01.out

#### 实验例子：

- tutorial01
  - 将前面的５帧图像分别保存到文件。
- tutorial02
  - 使用 SDL 播放视频帧数据。
- tutorial03
  - 播放音视频数据。
- tutorial04
  - 增加独立线程处理视频数据。
- tutorial05
  - 在 tutorial04 的基础上面增加 pts 进行同步。
- tutorial06
  - 在 tutorial05 的基础上面选择不同的音视频同步策略。
- tutorial07
  - 在 tutorial06 的基础上面增加 seek 处理。
- tutorial08
  - 使用 rtp 打包 aac 和 h264 文件。

#### 参考资料

- [FFMPEG 完美入门资料](http://download.csdn.net/download/leeking1989/7111345)
- [一个广院工科生的视音频技术笔记](http://blog.csdn.net/leixiaohua1020)
