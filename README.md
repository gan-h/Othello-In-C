# ~~Chess~~ Othello
 
Team: Haotian Gan, Period 10

## Project Description
A program for playing Othello. The minimum viable product will allow for you to play Othello against yourself, store and load game data, and mark up each game with your own analysis. Additional features will include a computer player, and automatic computer analysis of each game. 

## Technical Design

### Using Topics Covered in Class
  - Memory will be dynamically allocated to account for boards positions generated as the Othello game progresses. 
  - Files will be used to store game data, and parsed to load game data. 
  - A computer player will be forked from the main process so that any computation it does will not block the main process.
  - The computer player will communicate with the main process through a named pipe.
  - File stats <sys/stat.h>, will be used to order player game data by date created, and date last modified.  
  
### Project Breakdown and Responsibilities
  - User interface design        : Haotian Gan
  - Human vs. Human functionality: Haotian Gan
  - Loading / Creating game file : Haotian Gan
  - Order game files by date/last modified/other: Haotian Gan
  - Self analysis functionality  : Haotian Gan
  - Computer player (minimax)    : Haotian Gan
  - Computer analysis            : Haotian Gan
  
  See DEVLOG for more specific tasks and contributions.
  
### Data Structures/Algorithms
  - The board will be represented by a 2D array of integers, where each number will correspond to a piece (i.e: 2 = white)
  - Boards will be linked together with a linked list
  - The computer player will use the minimax algorithm
  - Game files will be sorted using merge sort
  
## Project Timeline
  - 01/13/2021 Initial interface, human vs. human functionality
  - 01/15/2021 Loading / creating game file, ordering game files
  - 01/18/2021 Self analysis functionality
  - 01/21/2021 Computer player, computer analysis
  - 01/23/2021 Extra time for more features

## List of Required Libraries
  - <gtk/gtk.h> 
  - Standard C files 
  
##How to install GTK:
  - apt-get install libgtk-3-dev
  - I will try to include gtk into the project directly so that it is easier to compile. 

##Interface Description:
  - Main page: 
    - Left Side : 8x8 Othello board
    - Right Side: Moves made and a two buttons to "travel to" past moves
  - Analysis page:
    - Left Side : 8x8 Othello board
    - Left Side : Click board squares to add a highlight markup
    - Right Side: Moves made and a two buttons to "travel to" past moves
    - Right Side Top: Buttons to markup moves as "inaccurate", "mistake", or a "blunder". 
  - Game files page:
    - Scroll through rows of games. Each row has a game number, if white won or black won, time the game took, date played
    - "Open game from file"
    - "Open game file folder"
  
  
## Instructions on How to Use Project
