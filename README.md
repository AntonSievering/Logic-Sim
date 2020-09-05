# Logic Simulator

## Setup
### Download and install (on Windows)
To download this project, you can clone the source code in this repository. After that you can compile it e. g. using with a Windows C++ compiler. You will receive an
executable that you need to place into the folder, where the folders "sprites" and "circuits" are located. When finished, you can start the program and there you go.
### Download and install (on other Platforms)
If you dont have Windows, cant use this software. But a linux version is in development.

## Mainscreen
Once started you will find yourself with a blurred background picture and four buttons.
### New button
The first one, "New", moves you into a new empty workspace.
### Load button
The second button, "Load", will show you a gui that prompts you to choose a custom IC to load. When you chosed one, you will be moved into a workspace containing
the contents of the chosen IC. If you dont want to load an IC, you can simply click on the top right "X" button, that moves you back to the mainscreen.
### Information button
The third button, "Information", opens a gui showing the type of the binary (using Python or the olc::PixelGameEngine Sprite saving method), the current version
and a text with somewhat useful information.
### Exit button
If you press this button, the program will be stoped.

## During Working
If you decided to start a new project or to load one, you will be moved into the workspace. There are four modes: viewing, adding, wiring and deleting.
You can choose these modes using the buttons on the top. Below the Buttons you will see your current mode written.
### Basic Control
You can pan the scene while pressing the right mouse button. Also, when pressing "q", you can zoom in and when pressing "e", you can zoom out again.
When you want to save your IC, you can press "ctrl" adn "s" and the first time you will see a gui where you can give it a name. When you want a different name
you also can press "shift", "ctrl" and "s" and the gui will reapear.
### Viewing Mode
In the Viewing Mode you can zoom and pan without beeing able to move gates. Furthermore, you can press Buttons (the red ones) to toggle them.
### Adding Mode
In the Adding Mode, you will see two more buttons appear: "Add Preset IC" and "Add Custom IC" on the bottom. When pressed, they will show a gui, where you can
select premade ICs and your custom ones. In the scene, it will add them in the center of your screen. You can close the guis with the "X"s in the top right corners.
Moreover, in the scene, you can select ICs (left mouse click) and move them around (moving the mouse). While you are dragging an IC, you can press "R" to show a
window, that allowes you to rename your ICs. More to that later.
### Wiring Mode
In the Wiring Mode, you can connect ICs. To do that, you first must click (left mouse button) on an output node of an IC. If done correctly, it will light up in green.
Then you can select an input node of another (or the same) IC. This will create a new connection from the output to the input. It is not allowed the other way
around to prevent e. g. shortings. Your ouput node will be selected until you select another output node or click anywhere (but not onto a node) on the screen.
### Deleting Mode
In the Deleting Mode, you can delete nodes and ICs. When you click on an IC or a node, they will instantly disapear and wont be recoverable. So, be careful!

## After Working
When you are done working on your circuit, you can save it and press "esc" and you will be moved to the mainscreen, where you can stop the program or start a new project.

# HAVE FUN AND BE CREATIVE!
