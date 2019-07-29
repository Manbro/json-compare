# JSON Compare application

## IDE & Tools I used:

IDE: **Visual Studio 2017**  

Compiler:  **MSVC** (default VS2017 compiler)

Platform: **Windows 10**.

External libraries: **rapidjson**.

## Compiling & Running the app

### Prerequisites

1. The input files for JSONCompare must be located in the same directory where the **JSONCompare.cpp** is located and have the names: **"json1.json"** and **"json2.json"**.
2. The header-only library **"rapidjson"** (<http://rapidjson.org/>) headers must be added to your compiler's include search path. One way to do so is to download the **"include/rapidjson"** directory from <https://github.com/Tencent/rapidjson/> and add the directory path to your include path.

### Running on Windows

1. If you have Visual Studio IDE installed:  Open the JSONCompare.sln solution file with Visual Studio, add rapidjson headers to your include path by going to: **Project -> Properties -> VCC++ Directories -> General -> Include Directories** and add the path to where you downloaded the "include/rapidjson" directory, screenshot:

   ![1560264294926](C:\Users\Manbro\AppData\Roaming\Typora\typora-user-images\1560264294926.png)

   

   Run the program from the IDE by choosing **Debug** -> **Start Without Debugging** on the menu bar.

2. Alternative way using the command line: depends on what C++ compiler you are using, see more info: <https://docs.microsoft.com/en-us/cpp/build/walkthrough-compiling-a-native-cpp-program-on-the-command-line?view=vs-2019>

### Reviewing the results

When the program is executed, a console window should indicate its completion status.

The results will be in "result.json" file in the same directory where you ran the program.



## Algorithm

The algorithm is recursive and runs in linear time. It produces an array of "diff objects" - JSON objects that describe each difference found, it works as follows:

1. If values are primitives and equal, return an empty array.

2. If values are 1 difference away (i.e. different types or primitives) return [{diff_object}].

3. If values are both objects, initialize an empty result array, and iterate through fields, comparing them sequentially:

   ​	3.1 If current index exceeds the other object's size, add the corresponding {diff_object} to the result array.

   ​	3.2 If 2 fields with current index have different keys, add the corresponding {diff_object} to the result array.

   ​	3.3 If 2 fields with current index have the same keys, compare their values recursively, and add the return value to the result array.

   Return the result array.

4. If values are both arrays, iterate through both arrays and change all of the "id" values to be their offset from the previous id. Then, compare the arrays the same way as objects (without the key checking).

   #### Diff objects format:

   ```
   1. Base Cases:
   	Primitive equal types and equal values:
   		[]
   	Primitive equal types with different values:
   		[{
   			"Difference Type": "Value mismatch",
   			"Value_1": <value_1>,
   			"Value_2": <value_2>
   		}]
   	Different types:
   		[{
   			"Difference Type": "Type mismatch",
   			"Value_1 is of type": <type_1>,
   			"Value_2 is of type": <type_2>
   		}]
   2. If both are objects:
   	Field removed:
   		[{
   			"Difference Type": "Removed",
   			"Field Name": <field_name>,
   			"Field Type": <field_type>,
   			"Field index": <field_index>
   		}]
   	Field added:
   		[{
   			"Difference Type": "Added",
   			"Field Name": <field_name>,
   			"Field Type": <field_type>,
   			"Field index": <field_index>
   		}]
   	Two fields with same index but different names:
   		[{
   			"Difference Type": "Field name mismatch",
   			"Field Name": <field_name>,
   			"Field Type": <field_type>,
   			"Field index": <field_index>
   		}]
   	Two fields with same index and equal names but different values: (compare values recursively)
   		[{
   			"Field Name": <field_name>,
   			"Field Type": <field_type>,
   			"Field index": <field_index>
   			"Differences": [<return value of comapreValues(value1,value2)]
   		}]
   3. If both are arrays:
   	Element removed:
   		[{
   			"Difference Type": "element was removed from index i"
   		}]
   	Element added:
   		[{
   			"Difference Type": "element was added to index i"
   		}]
   	"id" offset mismatch:
   		[{
   			"Difference Type": "id offset mismatch",
   			"Offset_1": <offset_1>,
   			"Offset_2": <offset_2>
   		}]
   	Two elements with same index but different values: (compare values recursively)
   		[{
   			"Value index": <value_index>
   			"Value Type": <value_type>,
   			"Differences": [<return value of compareValues(value1,value2)]
   		}]
   ```

   

## Result Example: Screenshot

![1560267322798](C:\Users\Manbro\AppData\Roaming\Typora\typora-user-images\1560267322798.png)
