Project : 3D Procedural Dungeon Generator in DirectX 11 - Cass Bennett 2014


Project Page: https://cassbennett.com/portfolio/procedural-dungeon/
(Github was not used as source control for this project, but is used to host the code)
---------------------------------------------------------------------------


Project Description:
This project is an application I developed for the Procedural Generation module at Abertay University in the Computer Games Technology Course.
The application recursively generates a 3D dungeon each time it is ran, with rooms at random sizes and adjustable dungeon depth. 
Project files I specifically worked on are found in '.../SourceCode/Engine/DungeonSourceFiles' 
Files used from previous projects or given as foundation from a lecturer and adapted to the project are found in '.../SourceCode/Engine/FoundationSourceFiles' 

---------------------------------------------------------------------------

Features:

- Executable folder as well as Source folder
- 3D Procedural Dungeon Generation
- Instructions
- Player Movement
- Collision with generated walls (Can be toggled on/off)
- Distortion post processing shader (Can be toggled on/off as well as the intensity changed in real-time)
- Wireframe toggle

---------------------------------------------------------------------------

Installation:

- Select the 'Clone or Download' button on the applications GitHub page - https://github.com/CassBennett/Procedural-Dungeon
- Download the Project Zip
- Extract All and open the folder 

To run the program:
	- Open the 'Build' folder
	- Double Click the 'Run Dungeon.exe' executable file. 
	* This will procedurally generate a dungeon with a depth of 5 (General size) * 

To access the source or Visual Studio Solution:
	- Open the 'SourceCode' folder
	
	* Source files * 
	- All source files of the project can be found in the 'Engine' folder within the 'SourceCode' folder. 
	- The 'DungeonSourceFiles' folder holds the source files that I worked significantly on, specifically for the dungeon project
	- The 'FoundationSourceFiles' folder holds the source files that I adapted to this project using previously used code, or code provided by the lecturers at Abertay University
	- The Shader files can be found in the 'Engine' folder.

	* Visual Studio Solution * 
	- To open the project in Visual Studio, open the 'Engine.sln' solution file that can be found in the 'SourceCode' folder.
	- You will need Visual Studio 12 or higher to compile the project as it is, otherwise you may need to change the Platform Toolset in the Solution Properties

---------------------------------------------------------------------------

The controls of the application are: 

Camera Controls:

	UP ARROW KEY 	- 	Move Forward
	DOWN ARROW KEY 	- 	Move Backward
	LEFT ARROW KEY	- 	Rotate Left
	RIGHT ARROW KEY - 	Rotate Right
	A KEY		- 	Move Up
	Z KEY		- 	Move Down

Distortion Controls:
	D KEY		- 	Toggle Distortion Effect
	F KEY 		- 	Decrease Distortion
	G KEY 		- 	Increase Distortion

Application Controls:
	W KEY 		- 	Toggle Wireframe
	C KEY 		- 	Toggle Collision
	ESCAPE 		- 	Close application
	I KEY 		- 	View instructions on screen

---------------------------------------------------------------------------

Author: Cassie Bennett
Website: https://cassbennett.com/
