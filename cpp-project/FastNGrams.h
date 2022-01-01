#pragma once
#include <string>
#include <vector>

struct Entry {
	uint32_t lastTwoChars;
	uint32_t count;

	Entry();
};

struct FirstCharInfo {
	uint32_t startingIndex;
	uint32_t quantity;
};

const uint32_t 
	HANZI_LOW = 0x3400,
	HANZI_HIGH = 0x9FFF;

class FastNGrams
{
private:
	// If you want to look up "ABC"...
	//   entry = firstChar[A - HANZI_LOW]
	// Then, entries[entry.startingIndex] through entries[entry.startingIndex + entry.quantity - 1]
	//   are the items you need to bsearch.

	Entry emptyKey; // this is the single entry for L""
	std::vector<Entry> entries; // these leave off the first char in the lookup...
	std::vector<FirstCharInfo> firstChar;

	 
	// EPSILON_* are small constants which are added to the numerator
	// and denominator of probabilities coming out of this N-gram 
	// dataset. 
	static constexpr double EPSILON_NUMERATOR = 0.0001;
	static constexpr double EPSILON_DENOMINATOR = 10.0;

public:
	// initialize the N-gram data structure containing no entries. 
	FastNGrams();

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
};

