# Othello
 
Team: Haotian Gan, Period 10

## Project Description
A program for playing Othello against a minimax computer player. Supports loading and saving games, and has move-guide functionality. 

### Topics Used
  - Memory Allocation: GTK functions used in the program allocate memory. Calloc and malloc are used for storing game data. 
  - Files: Files are written to and read to in order to load and save games.
  - Signals: GTK event handler signals are used to make the GUI responsive. 
  - Finding information about files: A file save-as GUI, and file load-from GUI presents files in a sorted manner for loading and saving games. 
  
### Project Breakdown
  - A GTK user interface       
  - Human vs. Computer functionality
  - Loading / Creating game files  
  - Order game files by date/last modified
  
### Data Structures/Algorithms
  - The board is represented by a 2D array of ints
  - An array contains all boards generated as the player plays against the computer
  - The computer uses the minimax algorihtm to make moves
  - Files are written to and read from in a specific way for saving/loading games.
  
## Devlog
  - 01/15/2021 Finished learning GTK and Glade
  - 01/17/2021 Finished writing Othello game logic
  - 01/18/2021 Finished making GUI
  - 01/19/2021 Finished writing minimax computer player
  - 01/20/2021 Finished writing move display functionality, moving through past moves functionality, and fixed bugs with the minimax player
  - 01/21/2021 Finished save game functionality
  - 01/22/2021 Finished load game functionality
  - 01/23/2021 Finished move-guide functionality
  - 01/24/2021 GUi improvements, load game functionality and GUI bug fixes
  - 01/25/2021 Submitted project

## Dependencies
  - GTK
  - pkg-config helper tool for compiling 
  
##How to install GTK:
  - apt-get install libgtk-3-dev
  
##How to interact with the program:
  - Upon running the program, you will be presented with an interface. To your left will be an Othello board, and to your right will be a textbox with a few buttons underneath. You play as black. Your opponent, white, will automatically make a move once you make yours.
  - Move-guides will by default be enabled. They show you what valid moves you can make. If you would like to toggle them off, click the "Guides Off" button
  - Moves you and the computer make are recorded in the textbox for you to see. 
  - Use the arrow keys to move to past moves and back. If you are viewing a past move, and would like to jump ahead to your present-most move, simply click anywhere 
    in the grid.
  - If you would like to save your game for future play, press "Save Game" and choose a location to store your file.
  - If you would like to load a previously saved game, press "Load Game" and choose a save file.
  - If you would like to start a new game, press "New Game"
  - A winner will be declared once the game ends. **Games end when *either* you or your opponent have no moves left to play**
  
