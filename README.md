# billing

本项目为[billing_go](https://github.com/liuguangw/billing_go)的移植版本，使用 C++ 编写(版本>=c++11)，基本功能和go语言版本的保持一致。

因为此项目使用了Linux的[GNU C Library (glibc)](https://www.gnu.org/software/libc/libc.html)函数，所以只能编译出Linux版本。

编译此项目，需要安装C++编译器、`make`、`cmake`、`git`才能进行编译。

clone此项目，并初始化deps目录下的子模块，再执行`build.sh`即可进行编译，默认采用静态链接。

