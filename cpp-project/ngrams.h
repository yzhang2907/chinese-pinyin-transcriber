#pragma once

#include <unordered_map>
#include <string>
#include <random>

// in-memory data structure for storing N-grams. 
class NGrams {
private:
	// we used an unordered_map to store the N-grams because it is
	// about 6 times more efficient than a standard map. 
	// wstrings are necessary because data is in the 16-bit unicode
	// range. We did not use utf-8 because data sanitization ensures
	// that all characters are below code 0xFFFF.
	std::unordered_map<std::wstring, int> grams;

	// a random number generator for determining the train-test split
	std::mt19937_64 rng;

	// EPSILON_* are small constants which are added to the numerator
	// and denominator of probabilities coming out of this N-gram 
	// dataset. 
	static constexpr double EPSILON_NUMERATOR = 0.0001;
	static constexpr double EPSILON_DENOMINATOR = 10.0;

public:
	// initialize the N-gram data structure containing no entries. 
	NGrams();
	
	// add all `windowSize`-grams in `str` to our data structure.
	void addGrams(const std::wstring& str, int windowSize);

	// add all 1-, 2-, and 3-grams in `str` to our data structure. 
	void addString(const std::wstring& str);

	// Read `filename`, a pairs file in the format
	//     部手机 bu shou ji
	//     液晶显示屏 ye jing xian shi ping
	// by extracting the chinese text before the first space
	// of each line. Then add 1-, 2-, and 3-grams for the
	// chinese text to our data structure.
	//
	// `testingProportion` determines what fraction of the 
	// data is used for testing; this data is written to
	// `testingStrings`. 
	void addPairs(
		const std::string& filename,
		double testingProportion, 
		std::vector<std::wstring>& testingStrings
	);

	void setOneGramCount();

	// output the N-grams to `filename` in the following format:
	//    受理办 18
	//    族企业 266
	//    接危 30
	//    少想将 1
	void save(const std::string& filename);

	// load the N-grams from `filename`, appending to whatever
	// data is already present
	void load(const std::string& filename);

	// Gives the probability that the last character in `str`
	// will appear *given* that all but the last character in `str`
	// have already appeared. In short,
	//
	//   str   |  probability(str)
	//   --------------------------
	//   "A"   |  P("A" | "")   (probability that "A" occurs with
	//                           no priors)
	//   "AB"  |  P("B" | "A")  (prob. that "B" occurs after "A")
	//   "ABC" |  P("C" | "AB") (prob. that "C" occurs after "AB")
	double probability(const std::wstring& str);

	void clear();
};