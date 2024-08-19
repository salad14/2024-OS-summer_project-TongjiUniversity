# 2024-OS-summer_project-TongjiUniversity<br>
刘继业 2252752 Tongji University, 2024 Summer, Du BoWen 2024/8/26-8/30<br>
源代码：https://github.com/salad14/2024-OS-summer_project-TongjiUniversity<br>
各个实验的代码可切换不同的Branch查看<br>

#Tools
##在Windows上安装wsl<br>
首先下载[Windows Subsystem for Linux ](https://learn.microsoft.com/en-us/windows/wsl/install)和[Ubuntu on Windows](https://apps.microsoft.com/detail/9nblggh4msv6?hl=en-us&gl=US)<br>
在 Windows 中，您可以访问“\\wsl$\”目录下的所有 WSL 文件。例如，Ubuntu 20.04 安装的主目录应该 位于“\\wsl$\Ubuntu-20.04\home\<username>\”。<br>
```bash
C:\Users\17912>wsl --list --verbose
  NAME      STATE           VERSION
* Ubuntu    Stopped         2
