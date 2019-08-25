cmake_minimum_required(VERSION 3.5.1)

project(pmem)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PMEMOBJ REQUIRED libpmemobj)
pkg_check_modules(PMEMBLK REQUIRED libpmemblk)
pkg_check_modules(PMEMLOG REQUIRED libpmemlog)
pkg_check_modules(PMEMPOOL REQUIRED libpmempool)
pkg_check_modules(PMEM REQUIRED libpmem)
pkg_check_modules(VMEM REQUIRED libvmem)
pkg_check_modules(VMALLOC REQUIRED libvmmalloc)

add_executable(main main.cpp)

target_include_directories(
    main
    PUBLIC
    ${PMEMOBJ_INCLUDE_DIRECTORIES} 
    ${PMEMBLK_INCLUDE_DIRECTORIES}
    ${PMEMLOG_INCLUDE_DIRECTORIES} )

target_link_libraries(main
    ${PMEMOBJ_LIBRARIES}
    ${PMEMBLK_LIBRARIES}
    ${PMEMLOG_LIBRARIES})
