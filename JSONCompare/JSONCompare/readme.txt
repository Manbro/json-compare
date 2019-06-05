- External library required: http://rapidjson.org/

- The main function reads "json1.json" and "json2.json" files from current directory outputs results to "result.json".

- Output format:
		1. Base cases:
			Primitive equal types and equal values:
				[]
			Primitive equal types with different values:
				[{
					"Difference Type": "Value mismatch",
					"Value_1" : <value_1>,
					"Value_2" : <value_2>
				}]
			Different types:
				[{
					"Difference Type": "Type mismatch",
					"Value_1 is of type" : <type_1>,
					"Value_2 is of type" : <type_2>
				}]
	  	2. If both are objects:
		Important note: Objects are compared as ordered lists
			 Field removed:
				[{
					"Difference Type": "Removed",
					"Field Name" : <field_name>,
					"Field Type" : <field_type>,
					"Field index" : <field_index>
				}]
			 Field added:
				[{
					"Difference Type": "Added",
					"Field Name" : <field_name>,
					"Field Type" : <field_type>,
					"Field index" : <field_index>
				}]
			 Two fields with same index but different names:
				[{
					"Difference Type": "Field name mismatch",
					"Field Name" : <field_name>,
					"Field Type" : <field_type>,
					"Field index" : <field_index>
				}]
			 Two fields with same index and equal names but different values: (compares values recursively)
				[{
					"Field Name" : <field_name>,
					"Field Type" : <field_type>,
					"Field index" : <field_index>,
					"Differences": [<return value of compareValues(value1,value2)>]
				}]
		3. If both are arrays :
			 Element removed:
				[{
					Difference Type: element was removed from index: i
				}]
			 Field added:
				[{
					Difference Type: element was removed to index: i
				}]
			 "id" offset mismatch:
				[{
					"Difference Type": "id offset mismatch",
					"Offset_1" : <offset_1>,
					"Offset_2" : <offset_2>
				}]
			 Two fields with same index but different values: (compares values recursively)
				[{
					"Value index" : <field_index>,
					"Value Type" : <field_type>,
					"Differences": [<return value of compareValues(value1,value2)>]
				}]
	