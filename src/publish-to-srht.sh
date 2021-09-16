#!/usr/bin/env bash

# Publish Capsule and Webpage to [SourceHut Pages](gemini://srht.site/)
# 2021, by Antonio Prates <antonioprates at gmail dot com>

# Note: this script assumes you have Sugar C  in yor system
# (see provided README.md for more information)

# SOURCEHUT CONFIGS ################################################################
token=""
send_url="https://pages.sr.ht/publish/<USERNAME>.srht.site"
######################################################### END OF SOURCEHUT CONFIGS #

# get path to content folder as first argument
gmipath=$1
capsule=$PWD/capsule
website=$PWD/website # if you change here also change capsule-to-website.sh

# prevent running the script with no defined path
[[ -z $gmipath ]] && echo "Please, provide path to content folder (without final slash), like:" \
&& echo "./gmi-to-html.sh ~/gemini" && exit 1

# take a clean copy of gmi folder (delete old copy if exists)
if [[ -d $capsule ]];
then
    rm -r $capsule
fi
cp -r $gmipath $capsule

# append the footer before conversion
./append-footer.sh $capsule ./template/footer.gmi

# clone convert capsule to website
./capsule-to-website.sh $capsule

echo "Publishing GEMINI Capsule:"
tar -czvf capsule.tar.gz -C $capsule .
curl --oauth2-bearer $token -Fcontent=@capsule.tar.gz -Fprotocol=GEMINI $send_url

echo "Publishing HTTP Website:"
tar -czvf website.tar.gz -C $website .
curl --oauth2-bearer $token -Fcontent=@website.tar.gz -Fprotocol=HTTPS $send_url

echo "Cleaning up..."
rm -r $capsule
rm capsule.tar.gz
rm -r $website
rm website.tar.gz
sync

echo "Done"
