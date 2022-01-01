#include "ngrams.h"
#include <fstream>
#include <sstream>
#include <codecvt>
#include <iostream>

NGrams::NGrams()
	: grams()
	, rng(31415)  // Use a deterministic seed
{}

void NGrams::addGrams(const std::wstring& str, int windowSize) {
	for (size_t i = 0; i < str.size(); i++) {
		// substr for the single ngram entry
		std::wstring sub = str.substr(i, windowSize); 

		if (grams.find(sub) == grams.end()) {
			grams.emplace(sub, 1);
			if (grams.size() % 100000 == 0) {
				std::wcout << grams.size() << L"\n";
			}
		}
		else {
			grams.find(sub)->second++;
		}
	}
}

void NGrams::addString(const std::wstring& str) {
	addGrams(str, 1);
	addGrams(str, 2);
	addGrams(str, 3);
}

void NGrams::addPairs(
	const std::string& filename,
	double testingProportion,
	std::vector<std::wstring>& testingStrings
) {
	std::wifstream file(filename);
	file.imbue(std::locale(std::locale::empty(), 
		new std::codecvt_utf8<wchar_t>));

	std::uniform_real_distribution<double> uniform;

	std::wstring line;
	while (std::getline(file, line)) {
		if (uniform(rng) < testingProportion) {
			testingStrings.emplace_back(line);
		}
		else {
			size_t spaceIndex = line.find(' ');
			if (spaceIndex != std::string::npos) {
				std::wstring hanSegment = line.substr(0, spaceIndex);
				addString(hanSegment);
			}
		}

	}
}

void NGrams::save(const std::string& filename) {
	std::wofstream output(filename);
	output.imbue(std::locale(std::locale::empty(),
		new std::codecvt_utf8<wchar_t>));

	for (auto it = grams.begin(); it != grams.end(); it++) {
		output << it->first << " " << it->second << "\n";
	}
}

void NGrams::load(const std::string& filename) {
	std::wifstream file(filename);
	file.imbue(std::locale(std::locale::empty(),
		new std::codecvt_utf8<wchar_t>));

	int oneGramCount = 0;

	while (!file.eof()) {
		std::wstring word; 
		int count; 
		file >> word >> count;

		if (word.size() == 0)
			continue; // uwu no empty strings lol get out
		
		if (word.size() == 1) {
			//std::cout << word.length() << std::endl;

			oneGramCount += count;
		}

		grams.emplace(word, count);
		if (grams.size() % 1000000 == 0) {
			std::wcout << L".";
			std::wcout.flush();
		}
	}

	grams.emplace(L"", oneGramCount);
	std::wcout << std::endl;
}

void NGrams::setOneGramCount() {
	int oneGramCount = 0;

	for (auto pair : grams) {
		if (pair.first.size() == 1) {
			oneGramCount += pair.second;
		}
	}

	std::wcout << L"1-gram count: " << oneGramCount << std::endl;

	grams.emplace(L"", oneGramCount);
}

double NGrams::probability(const std::wstring& str) {
	if (str.size() == 0) {
		return 0.0;
	}

	// found # of str
	auto itNumerator = grams.find(str); 
	// found # of str[:-1]
	auto itDenominator = grams.find(str.substr(0, str.size() - 1)); 

	if (itNumerator == grams.end() || itDenominator == grams.end()) {
		return probability(str.substr(1)); // fallback to one shorter
	}

	double numerator = itNumerator->second + EPSILON_NUMERATOR;
	double denominator = itDenominator->second + EPSILON_DENOMINATOR;
	return numerator / denominator;
}

void NGrams::clear() {
	grams.clear();
}