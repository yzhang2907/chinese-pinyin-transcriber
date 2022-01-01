#include "PinyinMoistener.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <codecvt>
#include <fstream>

Path::Path(double probability, std::wstring hanzi)
	: probability(probability), hanzi(hanzi)
{}

PinyinMoistener::PinyinMoistener(
	const std::wstring& pinyin,
	NGrams& grams,
	std::unordered_map<std::wstring, std::wstring>& pinyinToHanzi,
	int beamSearchSize
) :
	paths {},
	grams(grams),
	pinyinToHanzi(pinyinToHanzi), 
	beamSearchSize(beamSearchSize)
{
	moisturize(pinyin);
}

std::vector<Path>& PinyinMoistener::getPossibilities() {
	return paths;
}

Path PinyinMoistener::getBestPossibility() {
	return paths[0];
}

std::vector<std::wstring>
tokenize(std::wstring const& str, const wchar_t delim) {
	std::wstringstream ss(str);   // construct a stream from the string 
	std::vector<std::wstring> out;

	std::wstring s;
	while (std::getline(ss, s, delim)) {
		out.push_back(s);
	}

	return out;
}

void PinyinMoistener::moisturize(const std::wstring& pinyin) {
	std::vector<std::wstring> pinyins = tokenize(pinyin, L' ');
	std::vector<std::wstring> allPossibleHanzi;

	// std::wofstream log("log.txt");
	// log.imbue(std::locale(std::locale::empty(),
	// 	new std::codecvt_utf8<wchar_t>));

	for (size_t i = 0; i < pinyins.size(); i++) {
		allPossibleHanzi.emplace_back(pinyinToHanzi[pinyins[i]]);
	}

	paths.emplace_back(Path(1.0, L""));    // starting with empty string

	for (size_t i = 0; i < allPossibleHanzi.size(); i++) {
		std::wstring possibleHanzi = allPossibleHanzi[i];
		size_t totalPaths = paths.size();

		// TODO: How to avoid bias with a really common 1st char?  L"hei yo"

		for (size_t pi = 0; pi < totalPaths; pi++) {
			for (size_t hi = 0; hi < possibleHanzi.size(); hi++) {
				std::wstring newHanzi(paths[pi].hanzi);
				newHanzi.append(possibleHanzi.substr(hi, 1));
				
				double newProb = paths[pi].probability;

				int startIdx = newHanzi.size() - 3;
				int chars = 3;

				if (startIdx < 0) {
					startIdx = 0; 
					chars = newHanzi.size();
				}

				std::wstring searchPhrase = chars == 0 ? L"" : newHanzi.substr(startIdx, chars);
				double prob = grams.probability(searchPhrase);
				newProb *= prob;

				//log << searchPhrase << " " << prob << "\n";

				paths.push_back(Path(newProb, newHanzi));
			}
		}

		// TODO: Sort paths by probability and only keep best N
		paths.erase(paths.begin(), paths.begin() + totalPaths);
		std::sort(paths.begin(), paths.end(), [](const Path& p1, const Path& p2) {
			return p1.probability > p2.probability;
		});
		if (paths.size() > beamSearchSize) {
			paths.erase(paths.begin() + beamSearchSize, paths.end());
		}

		// Normalize adding up to one and being normal and shit
		// EvilCat.jpg
		double totalProb = 0.0;
		for (const Path& p : paths) {
			totalProb += p.probability;
		}
		for (Path& p : paths) {
			p.probability /= totalProb;
		}
	}

	// for (const Path& p : paths) {
	//  	log << p.hanzi << " " << p.probability << "\n";
	// }
}