<h3 align="center">一款可运行的基于C++11 实现的HTTP服务器</h3>



### 1、简介

个人修改的c++版本，与原版功能一致

### 2、编译

请严格按照下面步骤进行编译

#### 1、编译准备工作

文件夹中的 post.html 以及 post.cgi 文件需要一定的权限才可以执行。

所以需要先进行权限给予。

第一步、克隆项目并给予足够权限，打开命令行依次输入下述命令：

~~~c
git clone  https://github.com/forthespada/MyPoorWebServer

cd  httpdocs

sudo chmod 600 test.html

sudo chmod 600 post.html

sudo chmod +X post.cgi
~~~

在进行sudo命令时，需要输入Linux下的sudo命令。

如下图所示：

![](./image/1.png)


#### 2、编译执行

输入下述命令即可。

sh run.sh

#### 3、整体过程图



1、项目默认端口号是6379，如像下图地址栏所示，默认显示的界面是是test.html界面。

2、运行成功后默认显示的为test.html界面，同时同一路径下还有 post.html界面，可以将地址栏的“**test.html**”改成“**post.html**”来进行查看。



![](./image/2.png)

![](./image/3.png)

![](./image/4.png)

![](./image/5.png)

#### 4、整体框架图

![](./image/myhttp.png)


