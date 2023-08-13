

# 把该文件加的所有文件 用变量DIR_LIB_SRCS表示
AUX_SOURCE_DIRECTORY(. SRC_LIST)
ADD_EXECUTABLE(Benchmark ${SRC_LIST})
# 链接库文件到可执行文件中
TARGET_LINK_LIBRARIES(Benchmark baseLib)
TARGET_LINK_LIBRARIES(Benchmark pthread)




