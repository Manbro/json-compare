#ifndef _JSONCOMPARE_H
#define _JSONCOMPARE_H
#include "rapidjson/document.h"


static const char* kTypeNames[] =
{ "Null", "False", "True", "Object", "Array", "String", "Number" };

// The class compares 2 JSON objects parsed by rapidjson
class JSONComparator {
public:
	JSONComparator();

	// Compares 2 JSON objects (already parsed to DOM) and returns the results JSON
	rapidjson::Document compareJSONs(rapidjson::Value& dom1, rapidjson::Value& dom2);

private:
	// Recursive function that compares two JSON values and returns their diffs in a JSON array
	// If no diffs were found, an empty array is returned
	rapidjson::Value compareValues(rapidjson::Value& dom1, rapidjson::Value& dom2, rapidjson::Document::AllocatorType& allocator);

	int diffCount;
};
#endif