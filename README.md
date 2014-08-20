#log4cplus

基于log4cplus-1.2.0精简（下载地址：http://sourceforge.net/projects/log4cplus/files/log4cplus-stable/）

说明：精简并重构这个log4cplus只是为了满足个人需要。

精简目标：
1) 只支持Windows和Linux平台，去掉其他平台的支持，同时去除相关的代码。
2) 去除unicode的支持
2) 只支持consoleAppender、fileAppender，并添加我们自己使用的ospAppender；去除其它Appender相关的代码。
3) 精简宏，只保留会使用到的宏。
4) 修改代码写法，去掉value等变量命名（在Windows平台下为保留字），成员变量以下划线开头等。
5) 修改目录结构，修改命名空间，修改为自己喜欢的方式。


