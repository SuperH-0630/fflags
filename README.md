# FFlags 参数解析程序
## 关于FFlags
fflags是基于C语言的命令行参数解析程序，它可以支持解析`unix-like`和`windows`风格的命令行程序。  
支持使用子程序，例如`hello sub xxx`，其中`sub`即`hello`的子程序，不同的子程序可以使用不同的解析方式。
fflage是线程安全的，他不是用全局变量来保存数据。  
实际上，fflags不仅可以解析命令行参数，还是以解析任意签名为`int argc, char **agrv`函数的参数。

fflags设计被用于项目`aFun`的命令行参数解析，具体可以参考[aFun on github](https://github.com/aFun-org/aFunlang) 。