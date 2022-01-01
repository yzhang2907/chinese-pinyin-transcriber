#include "Evaluator.h"
#include <fstream>
#include <locale>
#include <codecvt>
#include <iostream>
#include <functional>
#include <iomanip>
#include <chrono>

TestingPair::TestingPair(
	const std::wstring& chinese,
	const std::wstring& pinyin
) :
	chinese(chinese), pinyin(pinyin)
{ }

EvaluatorResult::EvaluatorResult()
	: top1correct(0), top5correct(0)
	, top10correct(0), charsCorrect(0)
	, totalTests(0), totalChars(0)
	, averageTime(0.0)
{ }

Evaluator::Evaluator()
	: testingPairs()
{
	std::wifstream file("testing-pairs.txt");
	file.imbue(std::locale(std::locale::empty(),
		new std::codecvt_utf8<wchar_t>));

	std::wstring line;
	while (std::getline(file, line)) {
		size_t spaceIndex = line.find(L' ');
		if (spaceIndex != std::string::npos) {
			testingPairs.emplace_back(TestingPair(
				line.substr(0, spaceIndex),
				line.substr(spaceIndex + 1)
			));
		}
	}

	std::cout << "Loaded " << testingPairs.size() 
			  << " testing pairs...\n";
}

EvaluatorResult Evaluator::evaluate(
	int maxPairs, 
	EvaluatorLambda convert
) {
	EvaluatorResult result;
	double totalTime = 0.0;

	for (int k = 0; k < maxPairs && k < testingPairs.size(); k++) {
		const TestingPair& pair = testingPairs[k];

		if (pair.chinese.length() >= 2) {
			result.totalTests++;
			result.totalChars += pair.chinese.length();

			auto start = std::chrono::steady_clock::now();
			std::vector<std::wstring> conversions = convert(pair.pinyin);
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> diff = end - start;
			totalTime += diff.count();

			int found = -1;
			for (int i = 0; i < conversions.size() && i < 10; i++) {
				if (conversions[i] == pair.chinese) {
					found = i;
					break;
				}
			}

			if (found > -1) {
				if (found <= 0) {
					result.top1correct++;
					result.top5correct++;
					result.top10correct++;
				}
				else if (found <= 4) {
					result.top5correct++;
					result.top10correct++;
				}
				else if (found <= 9) {
					result.top10correct++;
				}
			}

			if (conversions.size() >= 1) {
				for (int i = 0; i < pair.chinese.length(); i++) {
					if (pair.chinese[i] == conversions[0][i]) {
						result.charsCorrect++;
					}
				}
			}

		}
	}

	result.averageTime = totalTime / result.totalTests;
	return result;
}

std::ostream& operator<< (
	std::ostream& out, const EvaluatorResult& result
) {
	/*out << "Result: " << std::endl;

	out << "    Top 1 correct: "
		<< (100.0 * (double)result.top1correct / (double)result.totalTests)
		<< std::endl;

	out << "    Top 5 correct: "
		<< (100.0 * (double)result.top5correct / (double)result.totalTests)
		<< std::endl;

	out << "   Top 10 correct: "
		<< (100.0 * (double)result.top10correct / (double)result.totalTests)
		<< std::endl;

	out << "    Chars correct: "
		<< (100.0 * (double)result.charsCorrect / (double)result.totalChars)
		<< std::endl;

	out << "     Average time: "
		<< (result.averageTime * 1000)
		<< " millliseconds"
		<< std::endl;*/

	out << (100.0 * (double)result.top1correct / (double)result.totalTests) << " & "
		<< (100.0 * (double)result.top5correct / (double)result.totalTests) << " & "
		<< (100.0 * (double)result.top10correct / (double)result.totalTests) << " & "
		<< (100.0 * (double)result.charsCorrect / (double)result.totalChars) << " & "
		<< (result.averageTime * 1000) << std::endl;

	return out;
}