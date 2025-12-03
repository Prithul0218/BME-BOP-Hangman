Hangman Game - Detailed Specifications 
Feature List 
• Always 10 misses allowed • Difficulty options: short, medium, long words • Adjustable hint count: 0–5 hints • Multiple selectable word packs • Random word selection based on difficulty + pack • 10-stage ASCII hangman drawing • Pattern analyser showing how many words still match the current pattern • Full validation of all user input • Play again and main-menu navigation • Word packs loaded only once at startup • Evil mode - dynamically set words. 
 
 
Introduction 
This Hangman program is an interactive command-line game where the player tries to guess a 
randomly selected word by entering letters or using limited hints. The word is chosen from a 
selected word pack according to a chosen difficulty category. The player has 10 misses before 
the round is lost, and an ASCII hangman drawing visually progresses with each mistake. The 
program ensures predictable and consistent behaviour through precise handling of menus, 
settings, game flow, and user input. 
Program Menus 
When the program starts, the user is greeted with a main menu that allows them to begin a 
game, adjust settings, choose a word pack, or exit. All input must be valid before the program 
proceeds. Invalid inputs result in a clear error message and a new prompt. Exiting the program 
displays a short goodbye message and terminates. 
Word Packs 
The game supports multiple word packs such as a default list or themed collections. All packs 
are loaded once at launch and stored for fast lookup. Each pack contains words of varying 
lengths. When the user selects a pack, the choice remains active until the game ends. 
Settings (Difficulty) 
In the settings menu, the player can adjust two parameters: the difficulty category (short, 
medium, long words, or evil mode) and the number of hints allowed (between 0 and 5). Difficulty 
determines the general length range of the word to be selected. Invalid values, such as 
non-numeric or out-of-range inputs for hints, result in an error message and a repeat prompt. 
Settings remain in effect for all future games until changed. 
Evil mode 
The computer keeps changing the word to be extra difficult and only locks in on a word when it 
can not change it any more 
Gameplay 
During the game, the user may guess a letter or type “hint”. Letter guesses must consist of 
exactly one alphabetic character. The system converts uppercase letters to lowercase and 
checks whether the letter has been guessed before. Repeated letters produce a warning and a 
reprompt. Invalid input (multiple characters, numbers, random text) also generates an error 
message. 
Correct guesses reveal all instances of the letter in the word. Incorrect guesses reduce the 
remaining misses and update the ASCII hangman drawing. After each action, the game shows 
the updated word pattern, remaining hints, remaining misses, guessed letters, the ASCII figure, 
and the number of words that still match the pattern. The pattern analyser determines this by 
filtering the current word pack against the known correct and incorrect letters. 
Win and losses 
The player wins the game as soon as the entire word is revealed. The program displays the 
completed word and offers the options to play again or return to the main menu. The player loses when all 10 misses are used. The final hangman ASCII drawing is shown, the 
correct word is revealed, and the same menu of options is offered. Invalid choices are handled 
with error messages and reprompts. 
Play again 
Choosing to play again immediately starts a new round using the same difficulty, hint count, and 
word pack. Returning to the main menu restores the original navigation options. 
Error handling and ending game 
If word packs cannot be loaded at startup, the game prints an error and exits. Throughout the 
game, all invalid input is handled gracefully without crashing. The program always reprompts 
the user after an invalid choice, ensuring stable operation and predictable behaviour. 