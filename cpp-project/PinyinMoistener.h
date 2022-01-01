#pragma once
#include <vector>
#include <string>
#include "ngrams.h"

// `Path` object stores the probability of `hanzi`
//   being the correct guess during beam search. 
struct Path {
	double probability;
	std::wstring hanzi;

	Path(double probability, std::wstring hanzi);
};

// beam search with each individual pinyin reading in
//  `pinyinToHanzi` as nodes, keeps best `beamSearchSize` 
//   paths in `paths`.  
class PinyinMoistener
{
private:
	std::vector<Path> paths;
	NGrams& grams;
	std::unordered_map<std::wstring, std::wstring>& pinyinToHanzi;
	int beamSearchSize;

	void moisturize(const std::wstring& pinyin);

public:
	PinyinMoistener(
		const std::wstring& pinyin, // a string like "wo de ma ma shi hen pang"
		NGrams& grams,
		std::unordered_map<std::wstring, std::wstring>& pinyinToHanzi,
		int beamSearchSize
	);

	std::vector<Path>& getPossibilities();
	Path getBestPossibility();
};

