#log4cplus

基于log4cplus-1.2.0精简（下载地址：http://sourceforge.net/projects/log4cplus/files/log4cplus-stable/）

精简内容：

1) 只支持Windows和Linux平台，去掉其他平台的支持。

2) 去除unicode的支持

3) 只保留consoleAppender、fileAppender；并添加customAppender，以方便我们通过回调函数调用本地的方法（如windows平台的OutputDebugString）。

4) 精简宏，只保留目前会使用到的宏。

5) 修改目录结构，修改命名空间，修改代码写法，修改为我熟悉的方式。

6) 去掉配置文件看门狗线程

7) 去掉原有锁，替换简单的锁

注意：静态类log4cplusS暂有问题，待修复



