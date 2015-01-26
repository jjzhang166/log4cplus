#log4cplus

基于[log4cplus](http://sourceforge.net/projects/log4cplus/files/log4cplus-stable/)修改

修改内容：

1) 只支持Windows和Linux平台，去掉其他平台的支持（vs2010和gcc4.4测试通过）。

2) 去除unicode的支持

3) 只保留consoleAppender、fileAppender；并添加customAppender，以方便我们通过回调函数调用本地的方法（如windows平台的OutputDebugString）。

4) 精简宏，只保留目前会使用到的宏。

5) 修改目录结构，修改命名空间，修改代码写法，修改为我熟悉的方式。

6) 去掉配置文件看门狗线程

7) 去掉原有锁，替换简单的锁

8）添加log4cplus封装类，封装类头文件不依赖于log4cplus，这样便于使用其他的日志组件（如glog、log4cpp等）。





