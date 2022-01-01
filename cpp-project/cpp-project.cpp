
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <locale>
#include <sstream>
#include <codecvt>
#include <io.h>
#include <fcntl.h>

#include "ngrams.h"
#include "PinyinMoistener.h"
#include "Evaluator.h"

// Regenerate all the N-gram data from scratch.
// This will take several minutes.  
void regenerateData(NGrams& n) { 
    std::vector<std::wstring> testing;
    n.addPairs("../pairs-web.txt", 0.1, testing);
    n.addPairs("../pairs-news.txt", 0.1, testing);
    n.addPairs("../pairs-wiki.txt", 0.1, testing);
    n.save("../ngrams.txt");

    std::wofstream output("testing-pairs.txt");
    output.imbue(std::locale(std::locale::empty(),
        new std::codecvt_utf8<wchar_t>));

    for (std::wstring & line : testing) {
        output << line << L"\n";
    }
}

// load all of the Chinese characters that correspond to each
// unaccented pinyin string. 
std::unordered_map<std::wstring, std::wstring>
loadPinyinToHanzi(const std::string& filename) 
{
    std::unordered_map<std::wstring, std::wstring> pinyinToHanzi;
    std::wifstream file(filename);
    file.imbue(std::locale(std::locale::empty(),
        new std::codecvt_utf8<wchar_t>));

    while (!file.eof()) {
        std::wstring pinyin;
        std::wstring hanzi;
        file >> pinyin >> hanzi;
        pinyinToHanzi.emplace(pinyin, hanzi);
    }
    return pinyinToHanzi;
}



//int doMain()
//{
//    auto pinyinToHanzi = loadPinyinToHanzi("../pinyin-to-hanzi.txt");
//
//    NGrams n {};
//    n.load("../ngrams.txt");
//
//    PinyinMoistener ppMoist(
//        L"wo hen xi huan zhe ben shu",
//        n,
//        pinyinToHanzi);
//
//    return 0;
//}

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);

    std::wcout << L"Welcome to the Pinyin Moistener!" << std::endl; // turbomoist
    std::wcout << std::endl;

    std::wcout << L"Loading pinyin-to-hanzi tables..." << std::endl;
    auto pinyinToHanzi = loadPinyinToHanzi("../pinyin-to-hanzi.txt");

    std::wcout << L"Loading N-gram data: ";
    NGrams n{};
    std::vector<std::wstring> testing;
    n.load("../ngrams.txt");

    std::vector<std::wstring> conversions;

    std::wcout << L"Done!" << std::endl << std::endl;
    while (true) {
        std::wcout << L"Please enter pinyin: ";
        std::wcout.flush();

        std::wstring pinyin;
        std::getline(std::wcin, pinyin);
        PinyinMoistener ppMoist(pinyin, n, pinyinToHanzi, 100);

        std::vector<Path>& poss = ppMoist.getPossibilities();
        for (int i = 0; i < 10; i++) {
            if (poss.size() > i) {
                std::wcout << L"  " << (i + 1) << L". " << poss[i].hanzi << L"  [" << (poss[i].probability * 100.0) << "%]" << std::endl;
            }
        }

        std::wcout << std::endl;
    }
    


    //n.addPairs("../pairs-web.txt", 0.7, testing);
    //n.addPairs("../pairs-news.txt", 0.7, testing);
    //n.addPairs("../pairs-wiki.txt", 0.7, testing);
    //n.addPairs("../pairs-wiki.txt", 0.99, testing);
    //n.setOneGramCount();
    
    //Evaluator eval;
    //std::vector<int> bsws{ 100 };
    //for (int bsw : bsws) {
    //    EvaluatorResult result = eval.evaluate(1000,
    //        [&](const std::wstring& pinyin) {
    //            std::vector<std::wstring> conversions;
    //            PinyinMoistener ppMoist(pinyin, n, pinyinToHanzi, bsw);

    //            std::vector<Path>& poss = ppMoist.getPossibilities();
    //            for (int i = 0; i < 10; i++) {
    //                if (poss.size() > i) {
    //                    conversions.emplace_back(poss[i].hanzi);
    //                }
    //            }

    //            return conversions;
    //        }
    //    );
    //    //std::cout << "Beam search with width: " << bsw << std::endl;
    //    std::cout << result;
    ////}

   

    //doMain();
    //NGrams n {};
    //regenerateData(n);

    return 0;
}