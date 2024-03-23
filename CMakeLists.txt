cmake_minimum_required(VERSION 3.5)
project(FLAC VERSION 1.4.3) # HOMEPAGE_URL "https://www.xiph.org/flac/")

add_compile_options(-Wno-static-in-inline)
add_executable(
    FLAC
    main.c
    flac_scu.c
    )

target_include_directories(FLAC PRIVATE include src src/libFLAC/include)
