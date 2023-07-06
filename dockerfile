# Building stage (1 stage)
# Here we install all build deps and assemble tests with main
# execuatble.
# tag 20230329 is an edge build
FROM alpine:20230329 AS build_env

RUN apk update
RUN apk add build-base cmake zip unzip curl git ninja pkgconfig
ARG VCPKG_FORCE_SYSTEM_BINARIES=1


# Building vcpkg
ADD https://github.com/microsoft/vcpkg/archive/master.tar.gz \
    vcpkg.tar.gz
RUN mkdir /vcpkg && \
    tar xf vcpkg.tar.gz --strip-components=1 -C /vcpkg
RUN /vcpkg/bootstrap-vcpkg.sh
RUN ln -s /vcpkg/vcpkg /bin/vcpkg

# Installing dependencies
RUN vcpkg install zlib catch2 rang --triplet=x64-linux && \
    vcpkg integrate install
# FIX: rang is header-only, this step is required
RUN touch /vcpkg/installed/x64-linux/share/rang/rang-targets.cmake

# Actual building
COPY . /ggpeg/
RUN mkdir /ggpeg/build
WORKDIR /ggpeg/build/
RUN cmake ..                                                       \ 
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=Debug
RUN cmake --build .

# Testing stage (2 stage)
# Here we run all tests to ensure that all modules function
# correctly.
FROM alpine:20230329
# This is a depedancy of GGPEG
RUN apk update && apk add libc++
COPY --from=build_env /ggpeg/build/*test .
RUN for test in *test; do "./$test"; done

# Packaging into smaller image (3 stage)
# Here we package app in small container.
FROM alpine:20230329
# This is a depedancy of GGPEG
RUN apk update && apk add libc++
COPY --from=build_env /ggpeg/build/ggpeg .
ENTRYPOINT [ "./ggpeg" ]

