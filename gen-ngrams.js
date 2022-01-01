// generates ngram data from files.
//
// THIS ONLY WORKS UP TO 16,777,216 N-GRAM KEYS due to built-in
// limitations of the Map datatype in Javascript
//
// The current data set results in about 20M 1-, 2-, and 3-grams,
// so use the NGram reader in cpp-project instead!

const readline = require('readline');
const fs = require('fs');

// NGrams represents our in-memory data structure for N-gram data. 
class NGrams {
    // construct with empty data
    constructor() {
        this.grams = new Map();
    }

    // Given `str`, a string of contiguous chinese characters, and 
    // `windowSize`, the value of N (for instance, if `windowSize` is 3,
    // we look for triplets), add all `windowSize`-grams to the data structure.
    addGrams(str, windowSize) {

        // Check argument type
        if (typeof str !== "string") {
            console.error(`NOT A STRING: ${str}`);
            return;
        }

        // For each starting index `i` from which we can grab `windowSize`
        // characters, extract the N-gram and increment its count in the
        // data structure.
        for (let i = 0; i <= str.length - windowSize; i++) {
            let sub = str.substring(i, i + windowSize); 
            if (sub === undefined) {
                console.error(`undefined sub: i = ${i}, windowSize = ${windowSize}, str = ${str}`);
                return;
            }
            if (!this.grams.has(sub)) {
                this.grams.set(sub, 1);
                if (this.grams.size % 100000 == 0) {
                    console.log(`NGrams: ${this.grams.size}`);
                }
            } else {
                let temp = this.grams.get(sub);
                this.grams.set(sub, temp + 1);
            }
        }
    }

    // Add 1-, 2-, and 3-grams for the string `str`.
    addString(str) {
        this.addGrams(str, 1);
        this.addGrams(str, 2);
        this.addGrams(str, 3);
    }

    // Read `filename`, a pairs file in the format
    //     部手机 bu shou ji
    //     液晶显示屏 ye jing xian shi ping
    // by extracting the chinese text before the first space
    // of each line. Then add 1-, 2-, and 3-grams for the
    // chinese text to our data structure.
    addPairs(filename) {
        return new Promise((resolve, reject) => {
            const lineReader = readline.createInterface({
                input: fs.createReadStream(filename),
                terminal: false
            });

            lineReader.on('line', (line) => {
                let index = line.indexOf(" ");
                if (index >= 0) {       // ignore empty lines
                    let hanSegment = line.substring(0, index);
                    this.addString(hanSegment);
                }
            });

            lineReader.on('close', () => {
                resolve(); 
            });
        });
    }

    // simultaneously process all of the pairs files in `filenames`. 
    addMultiplePairsFiles(filenames) {
        Promise.all(filenames.map(name => this.addPairs(name)));
    }

    // output the N-grams to `filename` in the following format:
    //    受理办 18
    //    族企业 266
    //    接危 30
    //    少想将 1
    save(filename) {
        let output = fs.openSync(filename, "w");
        fs.writeSync(
            output, 
            Array.from(this.grams)
                .map(x => `${x[0]} ${x[1]}`)
                .join("\n")
        );
        fs.closeSync(output);
    }
}


// For each pairs file we output, add its content to the N-Gram
// data structure, then write the result to "all-grams.txt".
let n = new NGrams();

n.addMultiplePairsFiles([
    "pairs-news.txt", 
    "pairs-web.txt", 
    "pairs-wiki.txt"
]);

n.save("all-grams.txt");

