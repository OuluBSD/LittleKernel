#include "AgentDemo.h"


#if 0


- 3D-network
	- X-axis: TIME-DOMAIN
	- Y-axis: KEYPOINT-LAYER
	- Z-axis: PARALLEL

- Y-axis action planner: KEYPOINT-LAYER(1.), PARALLEL(text="hello world")
		- point A: TIME-DOMAIN(program-main-phase)
		- point B: TIME-DOMAIN(human-viewer-phase)
- X-axis action planner: TIME-DOMAIN(program-main-phase), KEYPOINT-LAYER(5.)
		- point A: PARALLEL(class1)
		- point B: PARALLEL()
- Z-axis action planner: TIME-DOMAIN(program-main-phase), PARALLEL(MainProgram)
		- point A: KEYPOINT-LAYER(1.)
		- point B: KEYPOINT-LAYER(2.)
	


---->

- KINDA:		multiple domains of atoms
- KINDA:		multiple paths -> add details (earlier will be kept valid)
- ACTUALLY:		try every axis & move freely in Y-axis KEYPOINT-LAYER (with own problems though)
- ACTUALLY:		find route forward in TIME-DOMAIN using TIME/VALUE-SUBSET
- ACTUALLY:		find route backward in TIME-DOMAIN using (VALUE/CONSTRAINT)*(CONSTRAINT/TIME)-SUBSET
					(convert a need to assertions and create assertions to earlier phases)
- REQUIRE:		vertical visit in KEYPOINT-LAYER in initial point and in goal point
- WTF:			moving in vertical domain... doesnt make much sense
				e.g. init ctor: X(0)Y(4)Z(0) -> x(0)Y(5)z(1)
					 add field: X(main:6)Y(6)Z(mainclass:1) -> X(init:0)Y(4)Z(mainclass:2)
- KINDA:		move freely sometimes just to prepare an action
- KINDA:		only some points are valid: add fields only in some "times"
- IN THE END:	make the program from all the visited paths
- MAIN POINT:	you have multiple paths, which are visited in the end to make the AST of program
- KINDA:		- you move in 3D-world as a "processor with registers" or a "last known cache + priority focus"
				- when you visit a node: your register gets a value set
				- pre & post conditions requires a value in the processor
				- the route network what processor visits forms the program AST
				- you kinda enable different domains of that processor in different Y-axis points
					- you need easy context translation thought
					
<----
We have a 3D world where the X-axis is TIME.
The Y-axis is a dependency-relationship hierarchy, where the high-level goals are at the top and the implementation details are at the bottom.
The Z-axis is the data, and when the data is processed, its Z-axis position changes.

Incomplete X-axis atoms:
- X(program-developement)
- X(program-main-phase)
- X(human-viewer-phase)

Incomplete Y-axis atoms:
- Y(user-interaction)
- Y(user-expectations)
- Y(asset-data)
- Y(program-procedural-architecture)
- Y(sub-procedures)
- Y(data-structure-architecture)
- Y(data-interation-architecture)
- Y(data-interaction-functions)
	
Incomplete Z-axis atoms:
- Z(text:String="hello world")
- Z(main:MainProgram)

Actions inside 'X(program-main-phase)':
- print_text(text)

Actions inside 'X(human-viewer-phase)':
- user_reads(text)

Actions inside 'X(program-main-phase) & Y(data-structure-architecture)':
- create(data_structure)
- modify(data_structure)
- delete(data_structure)

Actions inside 'X(program-main-phase) & Y(data-structure-architecture) & Z(text "hello world")':
- add(text) to data_structure
- concat(data_structure, text)

Actions inside 'X(human-viewer-phase) & Y(user-interaction)':
- click(button)
- scroll(text_field)
- type(text, text_field)
- select(option, select_menu)

Actions inside 'X(program-developement) & Y(main-function) & Z(first-program-statement)':
- declare(variable)
- assign(value, variable)
- call(function)
- return(value)

Actions inside 'X(program-developement) & Y(main-function) & Z(internal-function)':
- create(function)
- modify(function)
- delete(function)

List of related XYZ coordinates:
- X(program-main-phase) & Y(asset-data) & Z(main_scene)
- X(human-viewer-phase) & Y(user-interaction) & Z(option_menu)
- X(program-developement) & Y(data-interaction-functions) & Z(parse)
- X(program-developement) & Y(main-function) & Z(if_statement)
- X(program-developement) & Y(program-procedural-architecture) & Z(function_call)
- X(human-viewer-phase) & Y(user-expectations) & Z(consistent_interface)
- X(program-developement) & Y(data-structure-architecture) & Z(hash_map)

List of actual actions in the result plan at 'X(program-developement) & Y(main-function) & Z(first-program-statement)':
- declare("number_of_students" variable)
- assign(20, "number_of_students")
- call(calculate_average_grade function)
- return(average_grade)

List of actual actions in the result plan at 'X(program-main-phase) & Y(user-interaction) & Z(option_menu_clicked)':
- save_changes()
- close_menu()
- refresh_UI()

----->
1. In one XYZ point
	- LLM generates
		- next XYZ points
		- actions
			- for staying in XYZ
			- for moving in X,y or Z
			- for moving to completely new XYZ
		- pre & post conditions for actions
	- heuristics
		- LLM is given topic or goal in mind to focus better next XYZ points
2. Route searching phase
	- LLM is given goal and worldstate and XYZ point and next XYZ points
	- there is few meta-phases before the classic actionplanner action
		- want to move XYZ? or just X or Y or Z
		- what is missing in the worldstate?
		- can you guess the path to goal?
		- which actions are unrelated?



#endif
