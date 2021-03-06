# C++版你画我猜游戏
一个C++编写的，使用C/S架构的、实时、可选房间、多人同房的你画我猜游戏

## 技术点说明
该项目使用MFC进行开发，采用C/S架构，仿照传统你画我猜的游戏玩法制作。本系统中数据通过MySQL数据库存储，用户端不直接操作MySQL数据库，将所有请求以协议包形式发送给服务器，服务器使用协议映射表方式解析协议包，执行相应操作。

其他涉及到的技术点：同步阻塞模型、自定义线程池、线程同步、STL等

## 工作流程说明
### 1.房主创建房间

客户端：给服务器发送房主创建房间消息，同时房主进入房间

服务器端：服务器在房间list中新加入一个房间，设置好房主uid、房间号等信息，创建信号量，给房主发送房间创建返回值消息，若创建成功，则跳转窗口。

### 2.玩家进入房间

客户端：给服务器发送玩家进入房间消息，同时玩家进入房间，房间内人数计数器刷新，客户端在收到人数刷新消息后，如果是房主端，则显示刷新，如果是观众，则跳转窗口并显示人数

服务器端：服务器在房间对应观众map中的观众list列表新加入观众，并给每个观众list中的观众和房主发送人数刷新消息，向线程池投递任务--分配一个线程

### 3.房主点击开始（房间人数大于两人），游戏开始

客户端：房主客户端检测房间人数是否大于两人，若大于，给服务器发送游戏开始消息

服务器：接收到游戏开始消息后，进入游戏流程，见下一步

### 4.服务器选择词条，发给每个玩家

服务器：采用算法从数据库选取一个词条（防止重复），给观众list列表中的每个观众和房主发送游戏词条消息，并开启一个物品词条决定时间+2s计时器（2s考虑到延时时间）

客户端：房主客户端接收到消息后显示词条，观众客户端接收到消息后显示第一个提示，之后每隔物品词条决定时间÷3s显示一个提示（共三个提示，第一个提示直接给出，所以是第二个提示给出后再过一段时间游戏结束），所有客户端在收到游戏词条消息后开启一个物品词条决定时间s计时器，游戏计时（实时）、出题人应该在界面中显示出来。

### 5.服务器开始计时，每个玩家在规定时间内发消息

服务器：接受房主的图片流并转发到每个观众（释放信号量），接受观众的消息并转发到每个观众和房主，如果观众消息答对则转发消息为  [正确答案]  （这里的观众消息结构体中应该有一项答案是否正确的选项）

客户端：房主作画时自己的画板禁止作画，并且接受服务器发来的图片流实时显示，将玩家输入的消息发送到服务器并接受服务器发来的其他玩家消息，对比观众消息结构体中的答案是否正确一项，如果答案正确，则禁止再发送包含答案选项的文本消息。

### 6.时间到，公布答案，一轮结束

服务器：时间计时器到时后，给所有玩家发送一个游戏到时消息（这个消息结构体中应该包含系统结算），信号量相关处理，同时这个时候再接收到答案消息不进行判断

客户端：接收到游戏到时消息以后，显示答案和消息中包含的结算结果

### 7.如退房，则服务器完成相应动作

客户端：检测玩家关闭窗口的判断，如果是房主退房，则发送退房消息，并置结构体中身份为房主，如果是观众退房，则发送退房消息，并置结构体中身份为观众。如果观众接收到房主关闭房间消息，则提示并关闭窗口。

服务器：任何过程中接收到退房消息，如果是房主，则给所有观众发送房主关闭房间消息，依次清空房间列表list和房间对应观众list的map项，如果是观众，则从对应map项中移除观众，信号量相关处理。
