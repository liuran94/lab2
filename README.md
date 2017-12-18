# lab2
首先进入存放.o文件的文件夹

输入以下命令生成可执行文件（文件夹中已经放入了生成好的可执行文件，本步可省略）：
```
g++ main.o queue.o acc.o MurmurHash2.o bloom.o url.o matrix.o -o lab2
```

输入以下命令运行爬虫和pr计算
```
./lab2 http://news.sohu.com 10.108.86.80 80 ./url.txt ./top.txt ./download
```
