ELECTRON - Electrical Circuit Designer
======================================

OVERVIEW
--------

This project is a graphical application for creating and simulating electrical circuit diagrams. Users can place various electrical components (pieces), connect them via nodes, manipulate their orientation and size, and manage their circuit diagrams by saving, loading, and modifying them.

FEATURES
--------

- Graphical user interface built with C++ graphics.h library.
- Load predefined electrical components from external files.
- Place, rotate, and resize components on a workspace.
- Connect components through interactive nodes.
- Check for circuit completeness.
- Save and load circuit diagrams to and from files.
- Manage component properties such as resistance and current intensity.

GETTING STARTED
---------------

REQUIREMENTS:

- C++ compiler (e.g., Code::Blocks, DevC++).
- graphics.h library support.
- Files containing predefined electrical components must be placed in the correct directory ("piese electrice").

COMPILATION AND EXECUTION:

1. Clone or download the repository.
2. Open the project with a C++ IDE that supports graphics.h.
3. Ensure the graphics.h library is correctly configured.
4. Update the path in the "main()" function to match your directory of electrical components:
   
   string path = R"(your\path\to\piese electrice)";

5. Compile and run the program.

USAGE
-----

MAIN MENU:

- Start: Opens the workspace to create or modify circuits.
- Ajutor (Help): Instructions and user guide.
- Iesire (Exit): Closes the application.

WORKSPACE:

- Introduce: Adds new components to your circuit.
- Sterge (Delete): Removes selected components.
- Instrumente (Tools):
  - Clear workspace.
  - Save current diagram.
  - Save As to a new file.
  - Load existing diagrams.
- Conexitate (Completeness Check): Verifies if the circuit is complete.
- Adjustable Sliders: Control rotation angle and size (zoom) of selected components.
- Connection Management:
  - Left-click on nodes to select and connect components.
  - Right-click to disconnect nodes.
- Component Properties: Edit resistance ("R") and intensity ("I") values of selected components.

FILE STRUCTURE
--------------

- main.cpp: Source code of the application.
- piese electrice/: Directory containing component definition files.
- map.txt or custom-named files: Saved circuit diagrams.

COMPONENT FILES STRUCTURE
-------------------------

Each component file should contain:

[Component Name]  
[Number of Nodes]  
[x and y coordinates for each node...]  
[Number of Drawing Commands]  
[Drawing commands: L (line), R (rectangle), O (circle), and coordinates...]

Example:

Resistor  
2  
0 0  
1 0  
1  
L 0 0 1 0


