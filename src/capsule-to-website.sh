#!/usr/bin/env bash

# Converts Gemini Capsule folder to a HTML Website folder
# 2021, by Antonio Prates <antonioprates at gmail dot com>

# Note: this script assumes you have Sugar C  in yor system
# (see provided README.md for more information)

# get path to content folder as first argument
gmipath=$1
website=$PWD/website # if you change here also change publish-to-srht.sh
filePaths=""

# prevent running the script with no defined path
[[ -z $gmipath ]] && echo "Please, provide path to content folder (without final slash), like:" \
&& echo "./capsule-to-website.sh ~/gemini" && exit 1

function getFilePaths() {
    for item in $1/*; do
        if [[ -d $item ]];
        then
            # if it's a directory
            # then copy favicon.ico and styles.css to it as well
            cp -uvx $PWD/template/favicon.ico $item
            cp -uvx $PWD/template/styles.css $item
            # and get list of files in it
            getFilePaths $item
        elif [[ ${item: -4} == ".gmi" ]];
        then
            # else if it's .gmi a file, append to the conversion list
            filePaths="$filePaths $item"
        fi
    done
}

echo "Convering Gemini Capsule from '$gmipath' to './website':"

# take a clean copy of gmi folder (delete old copy if exists)
if [[ -d $website ]];
then
    rm -r $website
fi
cp -r $gmipath $website

# copy/update favicon.ico and styles.css to root folder
cp -uvx $PWD/template/favicon.ico $website
cp -uvx $PWD/template/styles.css $website

# colect all file paths in contents and run the conversion tool
getFilePaths $website
sugar gmi-to-html.c $filePaths

# clean up
rm $filePaths
