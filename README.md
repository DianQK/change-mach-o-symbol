# change-mach-o-symbol

通过修改 Mach-O 文件的符号表信息，解决符号冲突问题，参见：[通过修改 Mach-O 文件解决闭源组件符号冲突问题](https://blog.indigo.codes/2021/08/15/change-mach-o-symbol/)。

使用说明：

1. 运行根目录的 `build.sh` 得到 `change-mach-o-symbol` 可执行二进制
2. 切换到 `example` 目录
3. 运行 `build.sh`，得到 `build/example` 可执行二进制
4. 运行 `build/example` 得到不预期的输出
5. 运行 `fix_symbol_build.sh` 修改重复符号，重新链接
5. 再次运行 `build/example` 得到预期输出


```sh
$ ./build.sh
$ cd example
$ ./build.sh
$ ./build/example
foo in main
foo in main
$ ./fix_symbol_build.sh
$ ./build/example
foo in main
foo in lib
```
