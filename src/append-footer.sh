#!/usr/bin/env bash

# Appends template/footer.gmi to Gemini Capsule gmi files
# 2021, by Antonio Prates <antonioprates at gmail dot com>

# get path to content folder as first argument
gmipath=$1

# get footer file as second argument
footer=$2

# prevent running the script with no defined path
[[ -z $gmipath ]] && echo "Provide content folder (without final slash) and footer file, like:" \
&& echo "./append-footer.sh ~/gemini ./template/footer.gmi" && exit 1

# prevent running the script with no defined footer
[[ -z $footer ]] && echo "Provide content folder (without final slash) and footer file, like:" \
&& echo "./append-footer.sh ~/gemini ./template/footer.gmi" && exit 1

function appendGeminiFiles() {
    for item in $1/*; do
        if [[ -d $item ]];
        then
            # if it's a directory, search for files in it
            appendGeminiFiles $item
        elif [[ ${item: -4} == ".gmi" ]];
        then
            # else if it's .gmi a file, append footer
            cat $footer >> $item
            echo "'$item' ++ '$footer'"
        fi
    done
}

echo "Appending footer:"
appendGeminiFiles $gmipath
