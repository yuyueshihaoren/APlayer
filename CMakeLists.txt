cmake_minimum_required(VERSION 3.16)
project(aplayer)

set(CMAKE_CXX_STANDARD 17)

add_executable(aplayer main.cpp Logger.cpp Logger.h argument_parsing.cpp argument_parsing.h BlkManager.h  BlkSplitManager.cpp BlkSplitManager.h BlkIOMiddleware.cpp BlkIOMiddleware.h BlkFileManager.cpp BlkFileManager.h BlkNonseekPopenManager.cpp BlkNonseekPopenManager.h BlkSeekPopenManager.cpp BlkSeekPopenManager.h)

target_link_libraries(aplayer -lmpv)
