Sonic
=====
## 简介
[Sonic](https://github.com/linyehui/sonic) 是一个跨平台的声波传输库（iOS & Android)，技术上类似于[chirp](http://chirp.io/)和[蛐蛐儿](http://www.xququ.com/index_cn.html)，但声波识别率上和他们还有很大的差距。

声波生成和识别的代码源自[WaveTrans](https://github.com/CloudSide/WaveTrans)，linyehui 对源代码进行了精简，将声波编解码部分的代码进行了独立封装，并做了跨平台移植，从而得到了这个更简练的声波传输库：[Sonic](https://github.com/linyehui/sonic)。

## 关于声波识别率和存在的问题
目前的代码，降噪主要依赖于手机本身的物理降噪（类似双MIC降噪等），嘈杂环境下的声波识别率比较差；
如果你需要产品化这项技术，那么这份代码只能带你入门。

## 开发环境
```bash
OS X 10.9.5
Xcode 6.0.1 (准备开源的时候才做了Xcode6兼容，模拟器下iPhone 6还有问题)
adt-bundle-mac-x86_64-20131030
android-ndk-r9d
```

## 目录说明
```bash
|---sonic
|   |--android           # Android Demo
|   |--ios               # iOS Demo
|   |--sdk               # 跨平台的C++代码
```

## 温馨提示
iOS 和 Android Demo非常简陋，可以在两部设备上相互发送声波并识别，但交互上还有很多已知和未知的Bug :(

### 希望这份代码能给你带来些许帮助。

### Any questions, please feel free to send me an email: 
```bash
echo qwertyuwitotepasdwqifgs | tr qeryuoasdfgipwt lnyhudgmaco.@ie
```