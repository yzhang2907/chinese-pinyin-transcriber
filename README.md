# Chinese-Pinyin Transcriber with N-gram Language Model

A non-interactive pinyin-to-hanzi converter for Chinese with a 93% character 
accuracy. 

## What does this do?

Chinese typists use pinyin pronunciation ("ni hao") to type Chinese sentences
("你好"). This project converts from pinyin to Chinese without requiring
user intervention. 

## Installation and usage

1. Run `node gen-hanzi.js` to consume `dict-zi.js` and output
   `pinyin-to-hanzi.js`, a mapping from pinyin w/o accents to the
   characters they might be. The file has entries like this:

    neng 㲌㴰䏻䘅能螚

2. Run `node make-pairs.js` to consume `zho*`, the source text,
   and output `pairs-*.txt`, an easy-to-consume collection of 
   contiguous chinese characters with their correct pinyin readings.
   Basically, this acts as the train/test dataset.
   Output looks like:

    蓦然回首 mo ran hui shou
    我们已经走了这么远 wo men yi jing zou le zhe me yuan

3. Use MS Visual Studio 2019 or later to open `cpp-project` and run
   in release mode (x64). You will be presented with a prompt
   that will allow you to convert pinyin to Chinese characters. 

NOTE: The data files are 500+ MB, so they are NOT in this Github repo.
   Please contact the author, Lux Zhang, at yz862@cornell.edu
   to arrange for access to these files.

