const fs = require('fs');

let count = 0;
fs.readFileSync('essay.txt', 'utf-8')
    .split(/\n|\r\n/)
    .map(x => x.split("\t"))
    .forEach(x => {
        if (x[1] !== undefined) count += parseInt(x[1]);
    });
console.log(count);

// 204370001 freqeuncy count total