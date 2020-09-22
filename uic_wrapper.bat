@echo off
SetLocal EnableDelayedExpansion
(set PATH=C:\Qt\qt5-mingw64\qtbase\bin;!PATH!)
if defined QT_PLUGIN_PATH (
    set QT_PLUGIN_PATH=C:\Qt\qt5-mingw64\qtbase\plugins;!QT_PLUGIN_PATH!
) else (
    set QT_PLUGIN_PATH=C:\Qt\qt5-mingw64\qtbase\plugins
)
C:\Qt\qt5-mingw64\qtbase\bin\uic.exe %*
EndLocal
