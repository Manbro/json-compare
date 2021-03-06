#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/pointer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "JSONCompareh.h"
using namespace std;
using namespace rapidjson;

	//  Constructor
	JSONComparator::JSONComparator() : diffCount(0) {}

	// Function: compareJSONs
	// Wrapper function that compares 2 JSONs (already parsed) and returns the results in a JSON object
	Document JSONComparator::compareJSONs(Value& dom1, Value& dom2) {

		// Initialize results
		Document results(kObjectType);
		Document::AllocatorType& allocator = results.GetAllocator();
		results.AddMember("JSON1 type", Value::StringRefType(kTypeNames[dom1.GetType()]), allocator);
		results.AddMember("JSON2 type", Value::StringRefType(kTypeNames[dom2.GetType()]), allocator);
		diffCount = 0;

		// Call the recursive compare
		Value diffs = compareValues(dom1, dom2, allocator);

		// Finalize results
		results.AddMember("Number of differences found", diffCount, allocator);
		if (diffCount > 0) 
			results.AddMember("Diffrences", diffs, allocator);

		return results;
	}

	// Function: compareValues
	// Recursive function that compares two JSON values and returns their diffs in a JSON array
	// If no diffs were found, returns an empty array
	Value JSONComparator::compareValues(Value& dom1, Value& dom2, Document::AllocatorType& allocator) {
		Value diffs(kArrayType);

		// Base case comparison: not both are objects and not both are arrays
		if ( !( (dom1.IsObject() && dom2.IsObject()) || (dom1.IsArray() && dom2.IsArray()) ) ) {
			// Same type (primitive):
			if ( dom1.GetType() == dom2.GetType() ) {
				if (dom1.IsString()) {
					if (dom1.GetStringLength() != dom2.GetStringLength() || strcmp(dom1.GetString(), dom2.GetString()) != 0) {
						// Create diff - unequal strings
						Value diff(kObjectType);
						diff.AddMember("Difference Type", "Value mismatch", allocator);
						diff.AddMember("Value_1", dom1, allocator);
						diff.AddMember("Value_2", dom2, allocator);
						diffs.PushBack(diff, allocator);
						++diffCount;
						return diffs;
					}
					else { // identical strings
						assert(diffs.Empty());
						return diffs;
					}
				}
				else if (dom1 != dom2) {
					// Create diff - unequal primitive values
					Value diff(kObjectType);
					diff.AddMember("Difference Type", "Value mismatch", allocator);
					diff.AddMember("Value_1", dom1, allocator);
					diff.AddMember("Value_2", dom2, allocator);
					diffs.PushBack(diff, allocator);
					++diffCount;
					return diffs;
				}
				else { // identical primitive values
					assert(diffs.Empty());
					return diffs;
				}
			}
			// Different types:
			else {
				// Consider nulls and zeros as indentical
				if (! ((dom1.IsNull() && dom2.IsNumber() && dom2.GetInt() == 0) || (dom2.IsNull() && dom1.IsNumber() && dom1.GetInt() == 0)) ) {
					// Create diff - different types
					Value diff(kObjectType);
					diff.AddMember("Difference Type", "Type mismatch", allocator);
					diff.AddMember("Value_1 is of type", Value::StringRefType(kTypeNames[dom1.GetType()]), allocator);
					diff.AddMember("Value_2 is of type", Value::StringRefType(kTypeNames[dom2.GetType()]), allocator);
					diffs.PushBack(diff, allocator);
					++diffCount;
					return diffs;
				}
			}
		}

		// Both are objects:
		else if (dom1.IsObject()) {
			// Compare objects as ordered lists
			for (Value::ConstMemberIterator itr = dom1.MemberBegin(); itr != dom1.MemberEnd(); ++itr) {
				auto currKey = itr->name.GetString();
				auto currIndex = itr - dom1.MemberBegin();
				Value& currValue = dom1[currKey];
				auto valueType = kTypeNames[currValue.GetType()];
				Value::ConstMemberIterator itr2 = dom2.MemberBegin() + currIndex;
				
				// Check if exceeded the size of the second object
				if (currIndex > dom2.MemberCount()  &&  (!currValue.IsNull() && !(currValue.IsNumber() && currValue.GetDouble() == 0.0))) {
					// Create diff - field removed
					Value diff(kObjectType);
					diff.AddMember("Difference Type", "Removed", allocator);
					diff.AddMember("Field name", Value::StringRefType(currKey), allocator);
					diff.AddMember("Field Type", Value::StringRefType(valueType), allocator);
					diff.AddMember("Field Index", currIndex, allocator);
					diffs.PushBack(diff, allocator);
					++diffCount;
					continue;
				}

				// Compare field names
				auto otherKey = itr2->name.GetString();
				if (strcmp(currKey, otherKey) != 0) {
					// Create diff - field name mismatch
					Value diff(kObjectType);
					diff.AddMember("Difference Type", "Field name mismatch", allocator);
					diff.AddMember("Field name", Value::StringRefType(currKey), allocator);
					diff.AddMember("Field Type", Value::StringRefType(valueType), allocator);
					diff.AddMember("Field Index", currIndex, allocator);
					diffs.PushBack(diff, allocator);
					++diffCount;
				}

				// Field names match - compare values recursively
				else {
					Value new_diffs = compareValues(currValue, dom2[currKey], allocator);

					// Wraps the diffs received from recursion
					if (!new_diffs.Empty()) {
						Value diff(kObjectType);
						if (strcmp(currKey, "id") == 0) {
							new_diffs[0]["Difference Type"].SetString("id offset mismatch");
							new_diffs[0].AddMember("Offset_2", new_diffs[0]["Value_2"].GetInt(), allocator);
							new_diffs[0].AddMember("Offset_1", new_diffs[0]["Value_1"].GetInt(), allocator);
							new_diffs[0].RemoveMember("Value_1");
							new_diffs[0].RemoveMember("Value_2");
						}
						diff.AddMember("Field name", Value::StringRefType(currKey), allocator);
						diff.AddMember("Field Type", Value::StringRefType(valueType), allocator);
						diff.AddMember("Field Index", currIndex, allocator);
						diff.AddMember("Differences", new_diffs, allocator);
						
						diffs.PushBack(diff, allocator);
					}
				}
			}

			// Continue to iterate over the second object if it has additional members
			if (dom2.MemberCount() > dom1.MemberCount()) {
				for (Value::ConstMemberIterator itr = dom2.MemberBegin() + dom1.MemberCount(); itr != dom2.MemberEnd(); ++itr) {
					auto currKey = itr->name.GetString();
					auto currIndex = itr - dom2.MemberBegin();
					Value& currValue = dom2[currKey];
					auto valueType = kTypeNames[currValue.GetType()];

					if (!currValue.IsNull() && !(currValue.IsNumber() && currValue.GetDouble() == 0.0)) {
						// Create diff - field added
						Value diff(kObjectType);
						diff.AddMember("Difference Type", "Added", allocator);
						diff.AddMember("Field name", Value::StringRefType(currKey), allocator);
						diff.AddMember("Field Type", Value::StringRefType(valueType), allocator);
						diff.AddMember("Field Index", currIndex, allocator);
						diffs.PushBack(diff, allocator);
						++diffCount;
					}
				}
			}

			return diffs;
		}

		// Both are arrays:
		else if (dom1.IsArray()) { 
			// Before comparing, change all "id" values to be the offset from the previous id
			// this way the offset is compared instead of the value itself
			int curr_id, previous_id;
			for (rapidjson::SizeType i = 0, previous_id = 0; i < dom1.Size(); i++) {
				if (dom1[i].IsObject() && dom1[i].HasMember("id")) {
					curr_id = dom1[i]["id"].GetInt();
					dom1[i]["id"].SetInt(curr_id - previous_id);
					previous_id = curr_id;
				}
			}
			for (rapidjson::SizeType i = 0, previous_id = 0; i < dom2.Size(); i++) {
				if (dom2[i].IsObject() && dom2[i].HasMember("id")) {
					curr_id = dom2[i]["id"].GetInt();
					dom2[i]["id"].SetInt(curr_id - previous_id);
					previous_id = curr_id;
				}
			}

			// Compare arrays
			for (rapidjson::SizeType i = 0; i < dom1.Size(); i++) {
				auto valueType = kTypeNames[dom1[i].GetType()];
				// Check if exceeded the size of the second array
				if (i >= dom2.Size()) {
					// Create diff - element removed
					Value diff(kObjectType);
					diff.AddMember("Difference Type: element was removed from index: ", i, allocator);
					diffs.PushBack(diff, allocator);
					++diffCount;
				}
				else { // Compare values recursively
					Value new_diffs = compareValues(dom1[i], dom2[i], allocator);

					// Wraps the diffs received from recursion
					if (!new_diffs.Empty()) {
						// Create diff that contains the diffs received from recursion
						Value diff(kObjectType);
						diff.AddMember("Value Index", i, allocator);
						diff.AddMember("Value Type", Value::StringRefType(valueType), allocator); 
						diff.AddMember("Differences", new_diffs, allocator);  //TODO : for primitives print differentely
						diffs.PushBack(diff, allocator);
					}
				}
			}
			// Continue to iterate over the second array if it has additional members
			for (rapidjson::SizeType i = dom1.Size(); i < dom2.Size(); i++)
			{
				// Create diff - element added
				Value diff(kObjectType);
				diff.AddMember("Difference Type: element was added to index: ", i, allocator);
				diffs.PushBack(diff, allocator);
				++diffCount;
			}

			return diffs;
		}

		else
		{
			throw std::invalid_argument( "Invalid JSON types" );
		}
	}


// Main function:
// Reads two files: json1.json and json2.json from current directory
// Parses the files using rapidjson
// Compares the JSON objects using JSONComparator
// Writes the results to a JSON file: result.json
int main()
{
	FILE* json_1;
	FILE* json_2;
	errno_t err;

	// Open json1.json file
	if ((err = fopen_s(&json_1, "json1.json", "rb")) != 0) {
		fprintf(stderr, "Could not open file '%s': %s\n", "json1.json", strerror(err));
		return EXIT_FAILURE;
	}

	// Open json2.json file
	if ((err = fopen_s(&json_2, "json2.json", "rb")) != 0) {
		fprintf(stderr, "Could not open file '%s': %s\n", "json2.json", strerror(err));
		return EXIT_FAILURE;
	}

	// Parse jsons using rapidjson
	char json_1_readBuffer[65536];
	char json_2_readBuffer[65536];
	FileReadStream is1(json_1, json_1_readBuffer, sizeof(json_1_readBuffer));
	FileReadStream is2(json_2, json_2_readBuffer, sizeof(json_2_readBuffer));

	Document dom1;
	dom1.ParseStream(is1);
	if (dom1.HasParseError())
	{
		std::cout << "Failed to parse json1.json, Error code : " << dom1.GetParseError() << '\n'
			<< "Error offset : " << dom1.GetErrorOffset() << '\n';
		return EXIT_FAILURE;
	}

	Document dom2;
	dom2.ParseStream(is2);
	if (dom2.HasParseError())
	{
		std::cout << "Failed to parse json2.json, Error code : " << dom2.GetParseError() << '\n'
			<< "Error offset : " << dom2.GetErrorOffset() << '\n';
		return EXIT_FAILURE;
	}

	
	// Compare jsons
	Document results;
	JSONComparator comparator;
	cout << "Comparing JSONs..." << endl;
	results = comparator.compareJSONs(dom1, dom2);

	// Write results to a file
	cout << "Writing results to result.json" << endl;
	FILE* result = fopen("result.json", "wb");
	char writeBuffer[65536];
	FileWriteStream os(result, writeBuffer, sizeof(writeBuffer));
	PrettyWriter<FileWriteStream> writer(os);
	results.Accept(writer);
	
	cout << "Done" << endl;

	fclose(json_1);
	fclose(json_2);
	fclose(result);

	return EXIT_SUCCESS;

}