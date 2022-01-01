#include "FastNGrams.h"

Entry::Entry()
	: lastTwoChars(0), count(0)
{}

FirstCharInfo::FirstCharInfo()
	: startingIndex(0), quantity(0)
{}

FastNGrams::FastNGrams() 
	: emptyKey(), entries(), firstChar()
{
	firstChar.resize(HANZI_HIGH - HANZI_LOW + 1);
}

