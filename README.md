# QuickFPS

这是一个简单的C++/Golang的网络FPS游戏基础框架. 

## 亮点

- 📦**代码至上**. 尽可能使用平台无关的C++代码, 尽可能不添加二进制库. 目前不存在平台有关的代码, 只存在raylib二进制库; 
- 🏠**结构清晰**. 所有的部件均以“模块名/模块.cpp、模块.h”; 
- 🚀**高性能.** 区分不同的模块, 模块之间通信均采用生产/消费模型, 并使用双缓冲加速; 实体的派生全部使用运行时多态, 空间效率高; 
- 🧬**扩展性**. 项目使用C++的派生和多态系统, 轻松扩展新的实体类型; 使用protobuf设计序列化/反序列化过程, 协议扩展简单方便. 

## 组成部件

Client: C++客户端. 使用raylib渲染, 需包含二进制库. 

Server: Golang服务端. 使用与客户端完全相同的演算逻辑. 

## 构建方法

MacOSX / Linux / Windows

```bash
git clone https://github.com/dudujerry/QuickFPS
```

客户端:

```bash
cd QuickFPS/client
mkdir build
cd build
cmake ..
make
```

服务端: 

```bash
cd QuickFPS/server
go mod tidy
go build
```

## 可执行文件

客户端: QuickFPS(.exe)

服务端: qfserver(.exe)

通过在客户端/服务端目录下创建cli_config/ser_config文件的第一行, 第二行指定目标IP, 端口号.

## 第三方库: 

- raylib 5.5
- spdlog
- asio
- concurrentqueue
- nanopb
