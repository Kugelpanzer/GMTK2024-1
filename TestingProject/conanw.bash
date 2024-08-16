#! /bin/bash

# Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
# See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

echo "Looking for conan..."
if ! command -v git &> /dev/null
then
    echo "Error: conan could not be found; exiting."
    exit 1
fi
echo "conan found"

BUILD_PARAMS="--build=missing --build=outdated --build=cascade"

if [[ "$1" == "clean" ]];
then
    # No extra arguments required

    rm -rf build_

    exit 0
fi

if [[ "$1" == "install" ]];
then
    # 2nd argument: compiler (gcc|clang)
    # 3rd argument: profile  (Debug|Release)

    conan install . -of build_/ \
        -pr:h=hobgoblin-$2      \
        -pr:b=hobgoblin-$2      \
        -s build_type=$3        \
        ${BUILD_PARAMS}

    exit 0
fi

if [[ "$1" == "build" ]];
then
    # 2nd argument: compiler (gcc|clang|apple-clang)
    # 3rd argument: profile  (Debug|Release)

    conan build . -of build_/ \
        -pr:h=hobgoblin-$2    \
        -pr:b=hobgoblin-$2    \
        -s build_type=$3      \
        ${BUILD_PARAMS}

    exit 0
fi

if [[ "$1" == "create" ]];
then
    # 2nd argument: compiler (gcc|clang|apple-clang)
    # 3rd argument: profile  (Debug|Release)

    conan create .         \
        --user jbatnozic   \
        --channel stable   \
        -pr:h=hobgoblin-$2 \
        -pr:b=hobgoblin-$2 \
        -s build_type=$3   \
        ${BUILD_PARAMS}

    exit 0
fi

if [[ "$1" == "export" ]];
then
    # No extra arguments required

    conan export . --user jbatnozic --channel stable

    exit 0
fi

echo "Error: unknown command '$1' provided; exiting."
exit 1
