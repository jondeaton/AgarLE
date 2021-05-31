cmake_minimum_required(VERSION 2.8...3.20)

set(UTIL_SOURCE
        thread-pool.h       thread-pool.cpp
        ostreamlock.h       ostreamlock.cpp
        semaphore.h)

add_library(util ${UTIL_SOURCE})
