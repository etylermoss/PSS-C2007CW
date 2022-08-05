cmake_minimum_required(VERSION 3.0.0)
project(psyet3_coursework VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
add_compile_options(-std=gnu99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
link_libraries(Threads::Threads)
find_package(Threads)

set(LIB_SHARED util.c log.c print_list.c process_queue.c mutex_counter.c hash_table.c coursework.c linkedlist.c)

set(LIB_CURR generator.c simulator.c terminator.c paging_daemon.c)

add_executable(simulator7 simulator7.c ${LIB_SHARED} ${LIB_CURR})