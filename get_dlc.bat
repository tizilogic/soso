@pushd "%~dp0"
@git submodule update --init Tools/windows_x64
@git submodule update --init ext/sht
@popd