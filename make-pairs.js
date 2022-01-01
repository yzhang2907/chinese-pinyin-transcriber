const readline = require('readline');
const fs = require('fs');
const pinyin = require('pinyin');
const OpenCC = require('opencc');
const opencc = new OpenCC('t2s.json');

// returns if the character code is within the range for common hanzi. 
function isHanzi(charCode) {
    return (charCode >= 0x4e00 && charCode <= 0x9fff) ||
        (charCode >= 0x3400 && charCode <= 0x4dbf);
}

// given a string consisting of Chinese characters and other content, 
// extract an array of contiguous Chinese character regions. 
// Example: 
//   "15世紀時，已經成為非洲南部最大的邦國。"
//   -> ["世紀時", "已經成為非洲南部最大的邦國"]
function extractHanziRegions(str) {
    let regions = [];
    let i = 0;
    while (i < str.length) {
        let han = isHanzi(str.charCodeAt(i));
        if (han) {
            let j = i + 1;
            while (j < str.length && isHanzi(str.charCodeAt(j))) {
                j++;
            }
            regions.push(str.substring(i, j));
            i = j;
        } else {
            i++;
        }
    }
    return regions;
}

// Given `filename` containing mixed textual data, extract regions of 
//   contiguous Chinese characters and call `parsedCallback` for each one. 
// When finished, call `closedCallback`. 
function processFile(filename, parsedCallback, closeCallback) {
    // create a `readline` interface, which asynchronously reads the lines from 
    //   the file `filename`. 
    const lineReader = readline.createInterface({
        input: fs.createReadStream(filename),
        terminal: false
    });

    // when each line is received...
    lineReader.on('line', (line) => {
        // the textual content begins after a tab, should contain no spaces
        //   and should be converted to simplified. 
        let content = line.substring(line.indexOf("\t") + 1);
        content = content.replace(/\s/g, "");
        content = opencc.convertSync(content);

        // Extract an array of contiguous Chinese characters
        let regions = extractHanziRegions(content);
        
        // Obtain pinyin for each contiguous region of Chinese characters and 
        //   then invoke `parsedCallback`. 
        regions.forEach((region) => {
            let readings = pinyin(region, {
                heteronym: false,
                segment: true,
                group: false,
                style: pinyin.STYLE_NORMAL
            }).map(x => x[0]);

            if (readings.length !== region.length) {
                console.log("ERROR");
            } else {
                parsedCallback(region, readings);
            }
        });
    });

    // When readline interface is done, call `closeCallback`.
    lineReader.on('close', closeCallback);
}

// Given input raw text in `inputFilename`, extract chinese
// text regions and pair with unaccented pinyin and output
// these regions to `outputFilename`. 
function writePairs(inputFilename, outputFilename) {
    let output = fs.openSync(outputFilename, "w");
    let count = 0;

    processFile(
        inputFilename,
        (line, readings) => {
            count++;
            if (count % 1000 === 0) {
                console.log(count);
            }
            fs.writeSync(output, line + " " + readings.join(" ") + "\n");
        },
        () => {
            fs.closeSync(output);
        }
    );
}

// If you want to add additional data to the dataset,
// do so here.
writePairs("zho_wikipedia_2018_1M-sentences.txt", "pairs-wiki.txt");
writePairs("zho_news_2007-2009_1M-sentences.txt", "pairs-news.txt");
writePairs("zho-cn_web_2015_1M-sentences.txt", "pairs-web.txt");