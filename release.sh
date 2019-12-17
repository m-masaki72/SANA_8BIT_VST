#!/bin/sh
set -u

# variables
current=$(dirname $0)
macBuildPath=$current/Builds/MacOSX/build
macPreBuildPath=$current/PreBuilds/MacOSX
plugName=SANA_8bit_VST

# copy mac build binaries
if [ -e macBuildPath ]; then
    cp -rf $macBuildPath/Release/$plugName.component $macPreBuildPath/AU/
    cp -rf $macBuildPath/Release/$plugName.vst $macPreBuildPath/VST/
fi

# copy windows build binaries
if [ -e $current/Builds/VisualStudio2017/build ]; then
    echo "now developing"
fi

# remove zip file
if [ -e $current/$plugName.zip ]; then
    rm -rf $current/$plugName.zip
fi

# make zip file
zip -r $current/$plugName.zip $current/PreBuilds/