# Project 4 README

Student: Manik Sethi (920896983)
Student: Saketh Kotapati (919454378)

## Project Status
The project has been completed, and the repo has our code

## Known Issues
Due to some issues with docker crashing on Manik's mac repeatedly, we have to include a gtest and gmock file. 

## Code References
* [How to use XML](https://www.w3schools.com/xml/xml_usedfor.asp)
* [Gtest reference](https://google.github.io/googletest/primer.html#simple-tests)
* [Gmock reference](https://google.github.io/googletest/gmock_for_dummies.html)
## Generative AI Use
I used ChatGPT for Prompt 1 and Prompt 2

### Prompt 1
Hey, I am implementing a CTransportationPlannerCommandLine.cpp. I am getting this error:
```
a reference of type "std::__1::vector<char, std::__1::allocator<char>> &" (not const-qualified) cannot be initialized with a value of type "std::__1::string"C/C++(434)
std::__1::string Command
```

### Response 1
The error you're encountering is because the `Read` function is expecting a `std::vector<char>` type, but you're trying to pass a `std::string` type (`Command`) to it. To resolve this error, you need to convert the `std::string` to a `std::vector<char>` before passing it to the `Read` function.

You can achieve this by using the `std::vector` constructor that takes two iterators:

```cpp
// ProcessCommands implementation
bool CTransportationPlannerCommandLine::ProcessCommands() {
    auto& Implementation = *DImplementation;
    auto& Planner = *Implementation.DPlanner;

    std::string Command;
    while (Implementation.DCmdSource->Read(std::vector<char>(Command.begin(), Command.end()))) {
        std::istringstream ISS(Command);
        std::string FirstWord;
        ISS >> FirstWord;

        // Implement the command processing logic here...

        // Example:
        if (FirstWord == "help") {
            // Print help menu
            Implementation.DOutSink->Write("Help menu\n");
        }
    }

    return true;
}

```
### Changes 1
This prompt helped us realize we were missing the vector that our strings were contained in. Because of this, it was calling the wrong datatype and therefore not working. Here is the code we finally ended up with
```cpp
...
   bool ProcessCommands() {
        
        while (true) { 
            outSink->Write(ConvertStr("> "));
            
            char c = ' ';
            std::string commandstr;
            while (c != '\n') {
                commandSrc->Get(c);
                commandstr += c;
            }

            std::vector<std::string> command = StringUtils::Split(commandstr, " ");

            for (size_t i = 0; i < command.size(); i++) 
                command[i] = StringUtils::Replace(command[i], "\n", "");

            // HELP
            if (command[0] == "help") {
...
```
### Prompt 2
I am getting this reall long error, here is context. Here is CTransportationPlannerCommandLine.cpp:
```cpp
#include "TransportationPlannerCommandLine.h"
#include "TransportationPlanner.h"
#include "DataSink.h"
#include "DataSource.h"
#include "DataFactory.h"
#include <iostream>

// Define the implementation structure
struct CTransportationPlannerCommandLine::SImplementation {
    std::shared_ptr<CDataSource> DCmdSource;
    std::shared_ptr<CDataSink> DOutSink;
    std::shared_ptr<CDataSink> DErrSink;
    std::shared_ptr<CDataFactory> DResultsFactory;
    std::shared_ptr<CTransportationPlanner> DPlanner;

    SImplementation(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner) :
        DCmdSource(cmdsrc), DOutSink(outsink), DErrSink(errsink), DResultsFactory(results), DPlanner(planner) {}
};

// Constructor
CTransportationPlannerCommandLine::CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner) :
    DImplementation(std::make_unique<SImplementation>(cmdsrc, outsink, errsink, results, planner)) {}

// Destructor
CTransportationPlannerCommandLine::~CTransportationPlannerCommandLine() = default;

// ProcessCommands implementation
bool CTransportationPlannerCommandLine::ProcessCommands() {
    // Your implementation here
    std::cout << "Processing commands..." << std::endl;
    return true; // Placeholder return value
}

Here is the GTest error:
./bin/testtpcl
Running main() from /Applications/cshw/ECS34/proj1/include/googletest/googletest/src/gtest_main.cc
[==========] Running 9 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 9 tests from TransporationPlannerCommandLine
[ RUN      ] TransporationPlannerCommandLine.SimpleTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:41: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> "

[  FAILED  ] TransporationPlannerCommandLine.SimpleTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.HelpTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:69: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "------------------------------------------------------------------------\n" "help     Display this help menu\n" "exit     Exit the program\n" "count    Output the number of nodes in the map\n" "node     Syntax \"node [0, count)\" \n" "         Will output node ID and Lat/Lon for node\n" "fastest  Syntax \"fastest start end\" \n" "         Calculates the time for fastest path from start to end\n" "shortest Syntax \"shortest start end\" \n" "         Calculates the distance for the shortest path from start to end\n" "save     Saves the last calculated path to file\n" "print    Prints the steps for the last calculated path\n" "> "
    Which is: "> ------------------------------------------------------------------------\nhelp     Display this help menu\nexit     Exit the program\ncount    Output the number of nodes in the map\nnode     Syntax \"node [0, count)\" \n         Will output node ID and Lat/Lon for node\nfastest  Syntax \"fastest start end\" \n         Calculates the time for fastest path from start to end\nshortest Syntax \"shortest start end\" \n         Calculates the distance for the shortest path from start to end\nsave     Saves the last calculated path to file\nprint    Prints the steps for the last calculated path\n> "
With diff:
@@ -1,1 +1,13 @@
-""
+> ------------------------------------------------------------------------
+help     Display this help menu
+exit     Exit the program
+count    Output the number of nodes in the map
+node     Syntax \"node [0, count)\" 
+         Will output node ID and Lat/Lon for node
+fastest  Syntax \"fastest start end\" 
+         Calculates the time for fastest path from start to end
+shortest Syntax \"shortest start end\" 
+         Calculates the distance for the shortest path from start to end
+save     Saves the last calculated path to file
+print    Prints the steps for the last calculated path
+> 


[  FAILED  ] TransporationPlannerCommandLine.HelpTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.CountTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:90: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "4 nodes\n" "> "
    Which is: "> 4 nodes\n> "
With diff:
@@ -1,1 +1,2 @@
-""
+> 4 nodes
+> 


./testsrc/TPCommandLineTest.cpp:81: Failure
Actual function call count doesn't match EXPECT_CALL(*MockPlanner, NodeCount())...
         Expected: to be called at least once
           Actual: never called - unsatisfied and active

[  FAILED  ] TransporationPlannerCommandLine.CountTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.NodeTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:119: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "Node 0: id = 1234 is at 38d 36' 0\" N, 121d 46' 48\" W\n" "> "
    Which is: "> Node 0: id = 1234 is at 38d 36' 0\" N, 121d 46' 48\" W\n> "
With diff:
@@ -1,1 +1,2 @@
-""
+> Node 0: id = 1234 is at 38d 36' 0\" N, 121d 46' 48\" W
+> 


[  FAILED  ] TransporationPlannerCommandLine.NodeTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.ShortestTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:141: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "Shortest path is 5.2 mi.\n" "> "
    Which is: "> Shortest path is 5.2 mi.\n> "
With diff:
@@ -1,1 +1,2 @@
-""
+> Shortest path is 5.2 mi.
+> 


[  FAILED  ] TransporationPlannerCommandLine.ShortestTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.FastestTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:164: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "Fastest path takes 39 min.\n" "> "
    Which is: "> Fastest path takes 39 min.\n> "
With diff:
@@ -1,1 +1,2 @@
-""
+> Fastest path takes 39 min.
+> 


[  FAILED  ] TransporationPlannerCommandLine.FastestTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.PrintTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:205: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "Fastest path takes 39 min.\n" "> " "Start at 38d 23' 60\" N, 121d 43' 12\" W\n" "Bike N toward Main St. for 6.9 mi\n" "Bike W along Main St. for 4.3 mi\n" "Bike N along B St. for 3.5 mi\n" "End at 38d 32' 60\" N, 121d 47' 60\" W\n" "> "
    Which is: "> Fastest path takes 39 min.\n> Start at 38d 23' 60\" N, 121d 43' 12\" W\nBike N toward Main St. for 6.9 mi\nBike W along Main St. for 4.3 mi\nBike N along B St. for 3.5 mi\nEnd at 38d 32' 60\" N, 121d 47' 60\" W\n> "
With diff:
@@ -1,1 +1,7 @@
-""
+> Fastest path takes 39 min.
+> Start at 38d 23' 60\" N, 121d 43' 12\" W
+Bike N toward Main St. for 6.9 mi
+Bike W along Main St. for 4.3 mi
+Bike N along B St. for 3.5 mi
+End at 38d 32' 60\" N, 121d 47' 60\" W
+> 


[  FAILED  ] TransporationPlannerCommandLine.PrintTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.SaveTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:238: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "Fastest path takes 1 hr 22 min 30 sec.\n" "> " "Path saved to <results>/123_456_1.375000hr.csv\n" "> "
    Which is: "> Fastest path takes 1 hr 22 min 30 sec.\n> Path saved to <results>/123_456_1.375000hr.csv\n> "
With diff:
@@ -1,1 +1,3 @@
-""
+> Fastest path takes 1 hr 22 min 30 sec.
+> Path saved to <results>/123_456_1.375000hr.csv
+> 


./testsrc/TPCommandLineTest.cpp:244: Failure
Expected equality of these values:
  SaveSink->String()
    Which is: ""
  "mode,node_id\n" "Walk,10\n" "Walk,9\n" "Bus,8\n" "Bus,7\n" "Walk,6"
    Which is: "mode,node_id\nWalk,10\nWalk,9\nBus,8\nBus,7\nWalk,6"
With diff:
@@ -1,1 +1,6 @@
-""
+mode,node_id
+Walk,10
+Walk,9
+Bus,8
+Bus,7
+Walk,6


[  FAILED  ] TransporationPlannerCommandLine.SaveTest (0 ms)
[ RUN      ] TransporationPlannerCommandLine.ErrorTest
Processing commands...
./testsrc/TPCommandLineTest.cpp:281: Failure
Expected equality of these values:
  OutputSink->String()
    Which is: ""
  "> " "> " "> " "> " "> " "> " "> " "> " "> " "> "
    Which is: "> > > > > > > > > > "

./testsrc/TPCommandLineTest.cpp:290: Failure
Expected equality of these values:
  ErrorSink->String()
    Which is: ""
  "Unknown command \"foo\" type help for help.\n" "Invalid node command, see help.\n" "Invalid node parameter, see help.\n" "Invalid shortest command, see help.\n" "Invalid shortest parameter, see help.\n" "Invalid fastest command, see help.\n" "Invalid fastest parameter, see help.\n" "No valid path to save, see help.\n" "No valid path to print, see help.\n"
    Which is: "Unknown command \"foo\" type help for help.\nInvalid node command, see help.\nInvalid node parameter, see help.\nInvalid shortest command, see help.\nInvalid shortest parameter, see help.\nInvalid fastest command, see help.\nInvalid fastest parameter, see help.\nNo valid path to save, see help.\nNo valid path to print, see help.\n"
With diff:
@@ -1,1 +1,9 @@
-""
+Unknown command \"foo\" type help for help.
+Invalid node command, see help.
+Invalid node parameter, see help.
+Invalid shortest command, see help.
+Invalid shortest parameter, see help.
+Invalid fastest command, see help.
+Invalid fastest parameter, see help.
+No valid path to save, see help.
+No valid path to print, see help.\n


[  FAILED  ] TransporationPlannerCommandLine.ErrorTest (0 ms)
[----------] 9 tests from TransporationPlannerCommandLine (2 ms total)

[----------] Global test environment tear-down
[==========] 9 tests from 1 test suite ran. (2 ms total)
[  PASSED  ] 0 tests.
[  FAILED  ] 9 tests, listed below:
[  FAILED  ] TransporationPlannerCommandLine.SimpleTest
[  FAILED  ] TransporationPlannerCommandLine.HelpTest
[  FAILED  ] TransporationPlannerCommandLine.CountTest
[  FAILED  ] TransporationPlannerCommandLine.NodeTest
[  FAILED  ] TransporationPlannerCommandLine.ShortestTest
[  FAILED  ] TransporationPlannerCommandLine.FastestTest
[  FAILED  ] TransporationPlannerCommandLine.PrintTest
[  FAILED  ] TransporationPlannerCommandLine.SaveTest
[  FAILED  ] TransporationPlannerCommandLine.ErrorTest

 9 FAILED TESTS
make: [runtests] Error 1 (ignored)
pardan@campus-009-089 docker_setup % 

```
Could you decipher the error messages? Explain why each one is failing.

### Response 2
To fix the failing test cases, we need to ensure that the CTransportationPlannerCommandLine class functions properly according to the expected behavior outlined in the test cases. Let's go through each failing test case and address them:

SimpleTest: The output sink should contain "> " after processing commands.
HelpTest: The output sink should contain the expected help menu after processing commands.
CountTest: The output sink should contain the correct number of nodes.
NodeTest: The output sink should contain information about a specific node.
ShortestTest: The output sink should contain information about the shortest path.
FastestTest: The output sink should contain information about the fastest path.
PrintTest: The output sink should contain detailed information about the steps of a path.
SaveTest: The output sink should contain information about the saved path.
ErrorTest: The error sink should contain messages for various error scenarios.

Your implementation should pass the provided test cases, ensuring that the CTransportationPlannerCommandLine class behaves correctly according to the specified requirements.

### Changes 2
Since we didn't understand the test fully, this only slightly helped us develop implementation to pass the test. Our changes are shown in CTransportationPlannerCommandLine.cpp

### Prompt 3
```
./bin/testxml
Running main() from /Applications/cshw/ECS34/proj1/include/googletest/googletest/src/gtest_main.cc
[==========] Running 9 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 5 tests from XMLReaderTest
[ RUN      ] XMLReaderTest.SimpleTest
[       OK ] XMLReaderTest.SimpleTest (0 ms)
[ RUN      ] XMLReaderTest.ElementTest
./testsrc/XMLTest.cpp:48: Failure
Expected equality of these values:
  Entity.DType
    Which is: 4-byte object <02-00 00-00>
  SXMLEntity::EType::StartElement
    Which is: 4-byte object <00-00 00-00>

./testsrc/XMLTest.cpp:49: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "\n"
  "node"

./testsrc/XMLTest.cpp:50: Failure
Expected equality of these values:
  Entity.DAttributes.size()
    Which is: 0
  3

./testsrc/XMLTest.cpp:61: Failure
Expected equality of these values:
  Entity.DType
    Which is: 4-byte object <02-00 00-00>
  SXMLEntity::EType::EndElement
    Which is: 4-byte object <01-00 00-00>

./testsrc/XMLTest.cpp:62: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "\t"
  "node"

./testsrc/XMLTest.cpp:72: Failure
Expected equality of these values:
  Entity.AttributeValue("id")
    Which is: "62208369"
  "62209104"

./testsrc/XMLTest.cpp:74: Failure
Expected equality of these values:
  Entity.AttributeValue("lat")
    Which is: "38.5178523"
  "38.535052"

./testsrc/XMLTest.cpp:76: Failure
Expected equality of these values:
  Entity.AttributeValue("lon")
    Which is: "-121.7712408"
  "-121.7408606"

./testsrc/XMLTest.cpp:85: Failure
Expected equality of these values:
  Entity.DType
    Which is: 4-byte object <02-00 00-00>
  SXMLEntity::EType::EndElement
    Which is: 4-byte object <01-00 00-00>

./testsrc/XMLTest.cpp:86: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "\n"
  "osm"

./testsrc/XMLTest.cpp:89: Failure
Value of: Reader.End()
  Actual: false
Expected: true

[  FAILED  ] XMLReaderTest.ElementTest (0 ms)
[ RUN      ] XMLReaderTest.CDataTest
[       OK ] XMLReaderTest.CDataTest (0 ms)
[ RUN      ] XMLReaderTest.LongCDataTest
./testsrc/XMLTest.cpp:159: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "0                               01                               12                               23                               34                               45                               56                               67                  "
  "0                               0" "1                               1" "2                               2" "3                               3" "4                               4" "5                               5" "6                               6" "7                               7" "8                               8" "9                               9" "A                               A" "B                               B" "C                               C" "D                               D" "E                               E" "F                               F"
    Which is: "0                               01                               12                               23                               34                               45                               56                               67                               78                               89                               9A                               AB                               BC                               CD                               DE                               EF                               F"

./testsrc/XMLTest.cpp:163: Failure
Expected equality of these values:
  Entity.DType
    Which is: 4-byte object <02-00 00-00>
  SXMLEntity::EType::EndElement
    Which is: 4-byte object <01-00 00-00>

./testsrc/XMLTest.cpp:164: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "             78                               89                               9A                               AB                               BC                               CD                               DE                               EF          "
  "elem"

./testsrc/XMLTest.cpp:167: Failure
Value of: Reader.End()
  Actual: false
Expected: true

[  FAILED  ] XMLReaderTest.LongCDataTest (0 ms)
[ RUN      ] XMLReaderTest.SpecialCharacterTest
./testsrc/XMLTest.cpp:184: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "&"
  "&\"'<>"

./testsrc/XMLTest.cpp:188: Failure
Expected equality of these values:
  Entity.DType
    Which is: 4-byte object <02-00 00-00>
  SXMLEntity::EType::EndElement
    Which is: 4-byte object <01-00 00-00>

./testsrc/XMLTest.cpp:189: Failure
Expected equality of these values:
  Entity.DNameData
    Which is: "\""
  "elem"

./testsrc/XMLTest.cpp:192: Failure
Value of: Reader.End()
  Actual: false
Expected: true

[  FAILED  ] XMLReaderTest.SpecialCharacterTest (0 ms)
[----------] 5 tests from XMLReaderTest (0 ms total)

[----------] 4 tests from XMLWriterTest
[ RUN      ] XMLWriterTest.SimpleTest
[       OK ] XMLWriterTest.SimpleTest (0 ms)
[ RUN      ] XMLWriterTest.ElementTest
[       OK ] XMLWriterTest.ElementTest (0 ms)
[ RUN      ] XMLWriterTest.FlushTest
./testsrc/XMLTest.cpp:244: Failure
Expected equality of these values:
  OutStream->String()
    Which is: "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n\t<node id=\"62208369\" lat=\"38.5178523\" lon=\"-121.7712408\"/>\n\t<node id=\"62209104\" lat=\"38.535052\" lon=\"-121.7408606\"/>\n"
  "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n" "    <node id=\"62208369\" lat=\"38.5178523\" lon=\"-121.7712408\"/>\n" "    <node id=\"62209104\" lat=\"38.535052\" lon=\"-121.7408606\"/>\n" "</osm>"
    Which is: "<osm version=\"0.6\" generator=\"osmconvert 0.8.5\">\n\t<node id=\"62208369\" lat=\"38.5178523\" lon=\"-121.7712408\"/>\n\t<node id=\"62209104\" lat=\"38.535052\" lon=\"-121.7408606\"/>\n</osm>"
With diff:
@@ -1,3 +1,4 @@
 <osm version=\"0.6\" generator=\"osmconvert 0.8.5\">
 \t<node id=\"62208369\" lat=\"38.5178523\" lon=\"-121.7712408\"/>
-\t<node id=\"62209104\" lat=\"38.535052\" lon=\"-121.7408606\"/>\n
+\t<node id=\"62209104\" lat=\"38.535052\" lon=\"-121.7408606\"/>
+</osm>


[  FAILED  ] XMLWriterTest.FlushTest (0 ms)
[ RUN      ] XMLWriterTest.SpecialCharacterTest
./testsrc/XMLTest.cpp:258: Failure
Expected equality of these values:
  OutStream->String()
    Which is: "<elem attr=\"&\"'<>\">&\"'<></elem>"
  "<elem attr=\"&amp;&quot;&apos;&lt;&gt;\">&amp;&quot;&apos;&lt;&gt;</elem>"

[  FAILED  ] XMLWriterTest.SpecialCharacterTest (0 ms)
[----------] 4 tests from XMLWriterTest (0 ms total)

[----------] Global test environment tear-down
[==========] 9 tests from 2 test suites ran. (0 ms total)
[  PASSED  ] 4 tests.
[  FAILED  ] 5 tests, listed below:
[  FAILED  ] XMLReaderTest.ElementTest
[  FAILED  ] XMLReaderTest.LongCDataTest
[  FAILED  ] XMLReaderTest.SpecialCharacterTest
[  FAILED  ] XMLWriterTest.FlushTest
[  FAILED  ] XMLWriterTest.SpecialCharacterTest

 5 FAILED TESTS
make: [runtests] Error 1 (ignored)
pardan@campus-009-089 docker_setup %
```
What are the expected vs actual outputs for the failing tests (Asked this for sake of readibility, super difficult to decypher texts manually)

### Response 3
The failing tests in XMLReaderTest.ElementTest, XMLReaderTest.LongCDataTest, and XMLReaderTest.SpecialCharacterTest are likely due to incorrect parsing or handling of XML elements, character data, and special characters.

For XMLReaderTest.ElementTest, the expected output should be the successful parsing of XML elements with correct types (start and end), names, and attributes. However, the actual output suggests that there are mismatches in the type, name, or attributes of the parsed XML elements.

Similarly, in XMLReaderTest.LongCDataTest, the expected output should be the successful parsing of long CDATA sections. However, the actual output indicates that there are issues with parsing the CDATA section or with the end tag of the element.

Lastly, in XMLReaderTest.SpecialCharacterTest, the expected output should be the correct handling of special characters within XML elements. However, the actual output suggests that there are discrepancies in how special characters are parsed or represented.

To address these issues, further debugging and refinement of the XML parsing logic in the XMLReader.cpp file may be necessary.

### Change 3
Wasn't super helpful, just told us to "go fix it" instead of offering direction. So I asked it again hoping it show something useful.

### Prompt 4
show me the elements that are expected vs actual

### Response 4
Certainly, let's break down the expected vs actual elements for each failing test case:

XMLReaderTest.ElementTest:
Expected Elements:
Start Element:
Type: SXMLEntity::EType::StartElement
Name: "node"
Attributes: Empty (size 0)
End Element:
Type: SXMLEntity::EType::EndElement
Name: "node"
Start Element:
Type: SXMLEntity::EType::StartElement
Name: "osm"
Attributes: Empty (size 0)
End Element:
Type: SXMLEntity::EType::EndElement
Name: "osm"
Actual Elements:
Start Element:
Type: Unknown (different from expected)
Name: "\n" (different from expected)
Attributes: Size is 3 (different from expected)
End Element:
Type: Unknown (different from expected)
Name: "\t" (different from expected)
Attributes:
"id": "62209104" (different from expected)
"lat": "38.535052" (different from expected)
"lon": "-121.7408606" (different from expected)
End Element:
Type: Unknown (different from expected)
Name: "\n" (different from expected)
Reader End:
Actual: false
Expected: true
XMLReaderTest.LongCDataTest:
Expected Elements:
CDATA Section:
Data: "0 01 12 23 34 45 56 67 78 89 9A AB BC CD DE EF F"
End Element:
Name: "elem"
Actual Elements:
CDATA Section:
Data: "0 01 12 23 34 45 56 67 " (different from expected)
End Element:
Name: "78 89 9A AB BC CD DE EF " (different from expected)
Reader End:
Actual: false
Expected: true
XMLReaderTest.SpecialCharacterTest:
Expected Elements:
Start Element:
Name: "elem"
Attribute: "attr" with value "&"'<>"
End Element:
Name: "elem"
Actual Elements:
Start Element:
Name: Unknown (different from expected)
Attribute: Unknown (different from expected)
End Element:
Name: """
Reader End:
Actual: false
Expected: true
These comparisons illustrate the discrepancies between the expected and actual elements parsed by the XML reader.

### Change 4
Realized our XMLReader thinks the "StartElement" and "EndElement" are messed up, so we had to tamper around a lot with that. Was a big pain. Here are the changes.
```cpp
struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DDataSource;
    XML_Parser DXMLParser;
    std::queue<SXMLEntity> DEntityQueue;

    // Handler for start element event
    void StartElementHandler(const std::string &name, const std::vector<std::string> &attrs) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::StartElement;
        for (size_t i = 0; i < attrs.size(); i += 2) {
            TempEntity.SetAttribute(attrs[i], attrs[i + 1]);
        }
        DEntityQueue.push(TempEntity);
    }

    // Handler for end element event
    void EndElementHandler(const std::string &name) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = name;
        TempEntity.DType = SXMLEntity::EType::EndElement;
        DEntityQueue.push(TempEntity);
    }

    // Handler for character data event
    void CharacterDataHandler(const std::string &cdata) {
        SXMLEntity TempEntity;
        TempEntity.DNameData = cdata;
        TempEntity.DType = SXMLEntity::EType::CharData;
        DEntityQueue.push(TempEntity);
    }

    // Callback for start element event
    static void StartElementHandlerCallback(void *context, const XML_Char *name, const XML_Char **atts) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        std::vector<std::string> Attributes;
        auto AttrPtr = atts;
        while (*AttrPtr) {
            Attributes.push_back(*AttrPtr);
            AttrPtr++;
        }
        ReaderObject->StartElementHandler(name, Attributes);
    }

    // Callback for end element event
    static void EndElementHandlerCallBack(void *context, const XML_Char *name) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        ReaderObject->EndElementHandler(name);
    }

    // Callback for character data event
    static void CharacterDataHandlerCallback(void *context, const XML_Char *s, int len) {
        SImplementation *ReaderObject = static_cast<SImplementation *>(context);
        ReaderObject->CharacterDataHandler(std::string(s, len));
    }
... (continues)
```
Corrected our code so that startelementhandler would have startelement as it's DType and vice versa
