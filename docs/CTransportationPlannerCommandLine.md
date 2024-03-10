# CTransportationPlannerCommandLine
This class allows for handlign transportation planning tasks through CLI

## Constructor
```cpp
CTransportationPlannerCommandLine(std::shared_ptr<CDataSource> cmdsrc, std::shared_ptr<CDataSink> outsink, std::shared_ptr<CDataSink> errsink, std::shared_ptr<CDataFactory> results, std::shared_ptr<CTransportationPlanner> planner);
```
## Destructor
```cpp
~CTransportationPlannerCommandLine();
```
## Member Functions
### ProcessCommands
```cpp
bool ProcessCommands();
```
This is the core function. It reads command from data source, processes them, and then writes to a data sink. Uses Data factory for formatting. If processed succesfully, returns true, if not then it returns false.
