# 把该文件加的所有文件 用变量DIR_LIB_SRCS表示
AUX_SOURCE_DIRECTORY(. DIR_LIB_SRCS)

# 将mylib文件，生成Mylib.a库文件
ADD_LIBRARY(baseLib STATIC ${DIR_LIB_SRCS})
