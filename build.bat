@echo off

IF NOT EXIST build\ (
    mkdir build
)
pushd build
cmake ..
cmake --build . --config Release %*
popd

IF NOT EXIST dist\ (
    mkdir dist
)

copy build\Release\fivem-translator.dll dist\fivem-translator.dll