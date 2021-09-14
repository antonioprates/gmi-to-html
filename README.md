# Gemini GMI to HTML

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme) 
[![license MIT](https://img.shields.io/badge/license-MIT-yellow.svg?style=flat-square)](LICENSE.md)

This is a framework to streamline the process of writing contents as a `Gemini Capsule`, while making it also available as a common website, for a broader audience.

It's a combination of `bash/sugar-c` utilities to assist publishing to both worlds - `Gemini` and `WWW` - from the command line, including a "Ci-like" script for sr.ht. 

At it's core, there's a very simple `text/gemini` to `text/html` conversion tool, written in just about 108 lines of code, using C scripting with [Sugar-C](https://github.com/antonioprates/sugar) (tcc flavour).

See also:
[solderpunk's gemini spec](./gemini_spec.txt), section 1.3.5 for `text/gemini` format reference.

## Table of Contents

- [Install](#install)
- [Features](#features)
- [Conveniences](#conveniences)
- [Usage](#usage)
- [License](#license)

## Install

### With Sugar-C

> `Sugar-C` compiles C code on the fly 'as if' a scripting language out of the box and leans on `<sugar.h>` library as single only import for doing text file operations.

First install `Sugar-C` from [the GitHub repository](https://github.com/antonioprates/sugar), like:

```sh
git clone https://github.com/antonioprates/sugar.git
cd sugar
./install.sh
```

> Note: you might have to `sudo` the `install.sh` depending on your user permissions.

Then, just clone `this` repo:

```sh
git clone https://github.com/antonioprates/gmi-to-html.git
```

Now you should be good to go :)

### Without Sugar-C

Alternatively, you can [get the latest binary](https://github.com/antonioprates/gmi-to-html/releases).

Then, get the bash scripts by cloning `this` repo, as explained in previous section.

Copy the binary into `src` folder and update `gmi-to-html.sh` to use the binary instead:

```diff
< sugar gmi-to-html.c $filePaths
---
> gmi-to-html $filePaths
```

Yet, you can also use a C compiler of your preference, as long as you include  [sugar.h](https://github.com/antonioprates/sugar/blob/master/src/include/sugar.h) file to the project, and compile the tool yourself.

## Features

### Format conversion

These are the markup conversions currently supported by `gmi-to-html.c` (the core tool):
- `=>` internal links, converts path to *.html
- `=>` external links, keeps original URL
- `pre` (```) preformatted text block
- `#` heading 1, also sets the page title
- `##` heading 2
- `###` heading 3
- `italic` words enclosed in backtick (`)
- `bold` words enclosed in asterisks (**)
- `"`, `&`, `<`, `>`, html escaped codes

## Conveniences

### Theming

There is a simple `style.css` included under `src/template` folder. It is provided as a dark-blue themed color scheme, but can be used as a template and changed to tweak the outputted webpage color scheme.

### Favicon

Also, there is a space capsule icon sample, designed by [smalllikeart](https://www.flaticon.com/authors/smalllikeart) and made freely available from [flaticon](https://www.flaticon.com/), converted with [favicon.io](https://favicon.io/favicon-converter/).

## Usage

First, on the command line `cd` into the `gmi-to-html/src` folder, and then you have three options:


### 1. Manual operation

This method is mostly intended for testing purpose. Just provide a list of `file paths` as arguments to `gmi-to-html.c` script - run like:

```sh
sugar gmi-to-html.c ~/gemini/index.gmi ~/gemini/foo/bar.gmi
```

The `HTML` files will be placed in the same path of the `GMI` input files (use with care).

### 2. Semi-automatic

Optionally, you can use the `gmi-to-html.sh` to convert an entire `Gemini Capsule` folder in one go, also copying the `css stylesheet` and `favicon` from `template` folder in the right places for you.

Recommended usage - provide path to `content folder` (without the final slash), like:

```sh
./gmi-to-html.sh ~/gemini
```

Then, you get a pure `HTML` clone under `./website` folder.

> Note: it will overwrite whatever is in `./website`folder.

### 3. Full-automatic (with SourceHut)

Finally, there is a 'CI like' (lol) script to publish to `SourceHut Pages`. Navigate to [gemini://srht.site](gemini://srht.site/) for the information on how to get hosted on [SourceHut](https://sourcehut.org/).

> Configure: edit the `publish-to-srht.sh` to add your `token` and `send_url` configurations.

Provide `content folder` (without the final slash), like so:

```sh
./publish-to-srht.sh ~/gemini
```

> Note: the bash scripts, internally call the C script with the list of files for you, so it assumes you have `Sugar-C` configured in your system.

## License

[MIT License](./LICENSE.md)
