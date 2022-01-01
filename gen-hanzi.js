// generate mapping from pinyin to hanzi 

const fs = require('fs');
const dict = require('./dict-zi');   // dictionary of pronounciations
const OpenCC = require('opencc');
const opencc = new OpenCC('t2s.json');  // traditional to simplified converter

// remove accent marks from pinyin
function removeTone(pinyin) {
    return pinyin.normalize("NFD").replace(/[\u0300-\u036f]/g, "");
}

let pinyinToHanzi = new Map();

/*
const hanzi = fs.readFileSync('hanzi.json', 'utf-8')
    .split('\n')
    .map((x) => JSON.parse(x))
    .forEach((x) => {
        const pinyin = x.pinyin.map((p) => removeTone(p));
        const hanzi = x.character;
        pinyin.forEach((p) => {
            if (!pinyinToHanzi.has(p)) {
                pinyinToHanzi.set(p, [hanzi]);
            } else {
                pinyinToHanzi.get(p).push(hanzi);
            }
        });
    });
*/

// return whether a character code is within the range for (common) hanzi
function isHanzi(charCode) {
    return (charCode >= 0x4e00 && charCode <= 0x9fff) ||
        (charCode >= 0x3400 && charCode <= 0x4dbf);
}

// populate `pinyinToHanzi` with mappings from unaccented pinyin to arrays of 
// characters with that reading. 
for (let charCode in dict) {
    if (!isHanzi(charCode)) {
        continue;
    }

    // convert to simplified
    let hanzi = opencc.convertSync(String.fromCharCode(charCode)); 

    // some characters have multiple readings separated by commas
    // split readings into an array and remove tone markers. 
    let pinyin = dict[charCode].split(",").map(x => removeTone(x));
    
    // add this character to the list of characters for each reading
    pinyin.forEach((p) => {
        if (!pinyinToHanzi.has(p)) {
            pinyinToHanzi.set(p, [hanzi]);
        } else if (!pinyinToHanzi.get(p).includes(hanzi)) {
            pinyinToHanzi.get(p).push(hanzi);
        }
    });
}

// write the result to pinyin-to-hanzi.txt
// pinyin #hanzi A B C D ... Z
const out = fs.openSync("pinyin-to-hanzi.txt", "w");

pinyinToHanzi.forEach((v, k) => {  // this is a creation of satanism
    fs.writeSync(out, `${k} ${v.join("")}\n`);
});

fs.closeSync(out);