#pragma once
#include <vector>
#include <string>
#include <functional>

struct TestingPair
{
	std::wstring chinese;
	std::wstring pinyin;

	TestingPair(
		const std::wstring& chinese, 
		const std::wstring& pinyin
	);
};

struct EvaluatorResult {
	int top1correct; // num of conversions where the
					 // correct answer is the #1 choice
	int top5correct;  // ... within the top 5 choices
	int top10correct;  // ... within the top 10 choices

	int charsCorrect; // characters in the top choice that are correct

	int totalTests;
	int totalChars;

	double averageTime; 

	EvaluatorResult();
};

std::ostream& operator<< (
	std::ostream& out, const EvaluatorResult& result);


typedef std::function<
	std::vector<std::wstring>(const std::wstring&)
> EvaluatorLambda;

class Evaluator
{
private:
	std::vector<TestingPair> testingPairs; 

public:
	Evaluator();

	EvaluatorResult evaluate(int maxPairs, EvaluatorLambda convert);
};

