SET PROJECT_NAME=SPeMPECMakeGame

SET PROFL_ARGS=--profile:host=hobgoblin-msvc ^
               --profile:build=hobgoblin-msvc

SET BUILD_ARGS=--build=missing --build=outdated --build=cascade

:: libzt won't build with C++20 for some reason
SET EXTRA_ARGS=--settings:host libzt*:compiler.cppstd=17 ^
               --settings:build libzt*:compiler.cppstd=17

conan install . -of _Build/%PROJECT_NAME%-x64/ ^
    %PROFL_ARGS% ^
    %BUILD_ARGS% ^
    %EXTRA_ARGS% ^
    -s:h build_type=Debug ^
    -s:b build_type=Debug
    
conan install . -of _Build/%PROJECT_NAME%-x64/ ^
    %PROFL_ARGS% ^
    %BUILD_ARGS% ^
    %EXTRA_ARGS% ^
    -s:h build_type=Release ^
    -s:b build_type=Release

REM To install RelWithDebInfo profile edit Conan_install_dependencies.bat.

:: conan install . -of _Build/%PROJECT_NAME%-x64/ ^
::    %PROFL_ARGS% ^
::    %BUILD_ARGS% ^
::	  %EXTRA_ARGS% ^
::    -s build_type=RelWithDebInfo

PAUSE
