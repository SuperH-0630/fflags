# FFlags 参数解析程序
## 关于FFlags
fflags是基于C语言的命令行参数解析程序，它可以支持解析`unix-like`和`windows`风格的命令行程序。  
支持使用子程序，例如`math sub xxx`，其中`sub`即`math`的子程序，不同的子程序可以使用不同的解析方式。  
fflags是线程安全的，他不是用全局变量来保存数据。  
实际上，fflags不仅可以解析命令行参数，还是以解析任意签名为`int argc, char **agrv`函数的参数。

fflags设计被用于项目`aFun`的命令行参数解析，具体可以参考[aFun on github](https://github.com/aFun-org/aFunlang) 。

## 基本功能
* 针对不同的子程序创建解析语法, 例如`math sub 10 20`中，`sub`即`math`的子程序(`child`)，`10`和`20`为`sub`子程序的参数。
* 有默认子程序，例如设置`sub`为默认子程序，则`math 10 20`和`math sub 10 20`使用相同的方式解析参数。
* 支持`unix-like`参数
  * 短杠`-`即短参数，例如`-a`。
  * 短参数允许多个缩写，例如`-abc`
  * 双短杠`--`即长参数，例如`--point`
  * 短参数和长参数统称为选项，选项后可跟附属参数，例如`--point 21`
  * `--`后链接的参数为野参数，即不属于选项参数又不是选项参数的附属参数的其他参数。
    * 例如`--point 21 22 -- -p 20`中，`21`是`point`长参数的附属参数，`22 -p 20`是野参数
* 支持`windows`的斜杠参数
  * 斜杠后只有一个字符的参数即短参数，例如`/a`。
  * 斜杠后有多于一个字符的参数即长参数，例如`/point`。

## 使用方式
在`fflags.h`文件中，定义了可以简便实用fflags的宏。
### 定义子程序及其参数解析语法
```c
ff_defArg(<子程序名字>, <是否为default true/false>)
    ff_argRule(<短参书 一个字符(无单引号)>, <长参数>, <是否接受参数, not/can/must>, <标识符 int>)
    ...
ff_endArg(<子程序名字>, <是否为default true/false>);
```
**注意：标识符不建议使用`0`，`-1`以及`-2`。具体见下文。**
例如：
```c
ff_defArg(main, true)
    ff_argRule(m, message, not, 1)
    ff_argRule(w, msg2, not, 2)
    ff_argRule(e, msg3, can, 3)
    ff_argRule(r, msg4, must, 4)
ff_endArg(main, true);
```
如果`ff_argRule`时，长参数包含空格符，则需要使用`ff_argRule_`定义：
```c
ff_argRule(<短参书 一个字符(包含单引号)>, <长参数字符串>, <是否接受参数, not/can/must>, <标识符>)
```

### 定义子程序表
将多个子程序汇聚起来，需要定义一个子程序表
```c
ff_childList(<主程序名字>, ff_child(<子程序名字>) ...);
```

### 开始解析参数
#### 解析器的创建与释放
使用`ff_initFFlags(<参数个数>, <参数数组>, <主程序名字>)`创建一个解析器，例如：
```c
ff_FFlags *ff = ff_initFFlags(argc, argv, sys);
```
当该操作返回值为`NULL`时，意味着初始化失败，通常的原因是遇到错误的参数或无适配的子程序。  

参数处理完成后，使用`ff_freeFFlags(<解析器>)`释放解析器，例如：  
```c
ff_freeFFlags(ff);
```

#### 解析参数
使用`ff_getopt(<字符串指针>, <解析器>);`读取一个选项参数。若选项参数有附属参数，则该参数存储到字符串指针中。
```c
char *text = NULL;
int mark = ff_getopt(&text, ff);
```
`mark`即`ff_argRule`时定义的标识符，`test`上的内容即附属参数（若无附属参数则设置为`NULL`）。  
执行`ff_getop`会沿顺序读取选项参数，跳过野参数。  

当`ff_getop`返回值为`0`时，表示参数解析遭遇错误。  
当`ff_getop`返回值为`0`或`-2`时，表示参数解析遭遇错误。  
当`ff_getop`返回`-1`时，表示选项参数已经全部解析完成。  

当`ff_getopt`返回`-1`（即所有参数解析完成时），方可调用`ff_getopt_wild(<字符串指针>, <解析器>)`取出野参数。
```c
char *text = NULL;
bool re = ff_getopt_wild(&text, ff);
```
`test`上的内容即野参数内容。（若读取完成则设置为`NULL`）。  
执行`ff_getopt_wild`会沿顺序读取野参数。  

当`ff_getopt_wild`返回值为`false`代表野参数全部读取完成。  
当`ff_getopt_wild`返回值为`true`代表野参数还未读取完成。  
**只有选项参数解析完成后，才可解析野参数**

## 构建
FFlags使用`cmake`构建项目。
```shell
$ mkdir build
$ cd build
$ cmake ..
```
FFlags无添加`cmake`参数，在`cmake`构建期间可添加`cmake`内置参数。  
通常，使用`make`工具编译和安装项目。  
```shell
$ make
$ make install
```
亦可使用`CTest`运行测试用例。  
经测试，可以为`windows`以及`linux`操作系统中，使用`mingw`, `msvc`, `clang`进行编译。
### 编译目标
编译后可得到`FFlags`动态库，以及`fflags.h`头文件。  
使用该库时，将`FFlags`动态库连接到你的程序，并且在程序在包含`fflags.h`头文件即可。

## 声明
### 免责声明
FFlags为免费开源程序。  
编译、运行该程序过程中造成的任何损失（包括但不限于系统损坏、文件丢失）均由您个人承担，与程序的开发者无关。
### LICENSE
FFlags是在 MIT License 下发布的。  
FFlags代码归属其代码贡献者。