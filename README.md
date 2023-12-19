# NetDisks
基于自己实现的多线程网络服务器IO模型，实现了类百度网盘功能，用户可以在服务器提供的虚拟目录下进行上传下载文件。

介绍：
多线程网络服务器I/O模型：
全局对象server初始化时创建了两个资源：监听套接字，交由主线程服务。工厂对象，创建线程池。
主线程：1.接收来自客户端的连接并分发给线程池中的工作线程 2.记录日志 3.检测定时器状态，超时则下一轮工作周期，否则做一些其他事情
工作线程：1.接收epoll_wait 2.处理一些客户端的请求 3.检测定时器状态，超时则下一轮工作周期，否则做一些其他事情（如去公共队列中获取新的连接）
优化点：
在主线程1时，从现实场景考虑，接收的连接数要有一定数量限制的，无限或者一个。
在定时器的设置时候，通过系统调用clock_gettime获取时间，相对于gettimeofday有更精确的粒度。
工作线程1时，获得主线程分发的新连接之后，为其注册的时间为读时间，并为水平触发模式。从现实场景考虑，客户端并发量不大，总是可读的，因此写事件在有需时直接处理或者延迟注册。其次为什么为水平触发模式，从现实场景考虑，不论是水平触发模式或者边缘触发模式，数据还是要一次性读入缓冲区，所以自己为每个新连接关联一个对象，并且设置了动态增长的收发缓冲区。
工作线程2，模型与业务模块设定好了接口，及调用recv发送数据，send接收数据，handle进行处理接收缓冲区数据
工作线程3，通过nptl线程库提供的超时获取锁接口对公共连接队列获取新的连接。

百度网盘功能：
服务器和客户端通过大火车协议进行连接
服务器通过状态机记录客户端的状态，若客户端挂了，若处于下载上传状态则进行数据的存储，并记录位置。
