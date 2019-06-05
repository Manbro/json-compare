#ifndef _JSONCOMPARE_H
#define _JSONCOMPARE_H
#include "rapidjson/document.h"
using namespace rapidjson;


static const char* kTypeNames[] =
{ "Null", "False", "True", "Object", "Array", "String", "Number" };

class JSONComparator {
public:
	JSONComparator();

	Document compareJSONs(Value& dom1, Value& dom2);

private:
	Value compareValues(Value& dom1, Value& dom2, Document::AllocatorType& allocator);

	int diffCount;
};
#endif