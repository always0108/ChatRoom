##myshell 项目文档

####一、项目需求
1. 支持输入输出重定向(< > >>) 
2. 支持管道(|) 
3. 支持后台运行程序(&) *
4. 支持shell内置命令(实现cd即可)
5. 支持tab补全目录文件以及历史记录上下翻(提示:使用readline库)*
6. ctrl+c不能中断程序(提示:使用信号) *
7. 设置环境变量，使程序可以像bash，zsh这种shell一样运行 *
      	注:带*号的为附加功能，其他为基本功能
		

####二、运行环境

####linux操作系统
#####对电脑配置没有要求


####三、项目设计

整体思路：
![这里写图片描述](http://img.blog.csdn.net/20170730203811263?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbGR4MTk5ODAxMDg=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)


####四、重点难点
1.环境变量的添加要写入~/.bash_profile,才能永久保存，不要重新添加
2.ctrl+c 的问题，导致输入有时为空，容易出现段错误，要判断一下
3.对重定向符号和管道符号的判断问题，是否符合规范

####五、用户使用
1.在终端输入myshell即可启动程序（已经设置好了环境变量）

2.输入命令即可执行，若没有问题则显示出结果，有问题就会报错；等待等待输入下一个命令。ctrl + c 不能退出（由于忽略信号），要按exit或者logout退出。

3.支持命令记录功能，按上下键可以实现上下翻功能，并且支持目录的自动补全（TAB键）

4.<,>,>>，|只支持单个出现,不能混合出现

5.输入&表示在后台运行



####六、部分功能运行演示：

![这里写图片描述](http://img.blog.csdn.net/20170730204647121?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbGR4MTk5ODAxMDg=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

![这里写图片描述](http://img.blog.csdn.net/20170730204532449?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbGR4MTk5ODAxMDg=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

![这里写图片描述](http://img.blog.csdn.net/20170730205526402?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbGR4MTk5ODAxMDg=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)



