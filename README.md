# Hangman Game
Class project for basics of programming 1 at my university (BME).

## DETAILED SPECIFICATIONS
**This is AI generated for now, will improve later to match the real game**

FEATURE LIST
• Always 10 misses allowed
• Difficulty options: short, medium, long words
• Adjustable hint count: 0–5 hints
• Multiple selectable word packs
• Random word selection based on difficulty + pack
• 10-stage ASCII hangman drawing
• Pattern analyser showing how many words still match the current pattern
• Evil Mode at game start
• Full validation of all user input
• Play-again and main-menu navigation
• Word packs loaded only once at startup

---

INTRODUCTION
This Hangman program is an interactive command-line game where the player attempts to guess a hidden word by entering letters or using optional hints. Words are selected from the currently chosen word pack and filtered through the selected difficulty category, which represents the length range of the target word. The player has 10 misses before losing the round, with an ASCII hangman drawing that updates after each incorrect guess. The game is designed to behave consistently for all users by strictly validating input and clearly defining all menus, settings, and gameplay behaviour.

---

PROGRAM MENUS
At startup, the program greets the user and shows the main menu, where the player may start a new game, adjust settings, choose a word pack, or exit. All menu selections must be numeric and valid. Invalid entries produce an error message and a reprompt. Exiting the program displays a short goodbye message and immediately terminates execution.

---

WORD PACKS
The program supports multiple word packs, such as a default list or themed collections. All packs are read once at launch and stored for fast use. Each pack contains words of various lengths. When the user selects a pack, that pack becomes the active source for all future games until changed. If a selected difficulty level cannot be satisfied within the pack (for example, no words in the required length range), the program informs the user when attempting to start a game and returns to the main menu.

---

SETTINGS (DIFFICULTY AND HINTS)
In the settings menu, the player may choose a difficulty level—short, medium, or long words—which defines the approximate length of words selected for the game. The player may also set how many hints are available, from 0 to 5. Invalid entries, such as out-of-range values or non-numeric input, generate clear error messages and require a new input. All settings remain active until the player changes them again.

---

EVIL MODE
At the start of each new game, before selecting a word, the player is asked to choose between Classic Mode and Evil Mode.

In Classic Mode, the program selects one random word from the chosen pack and difficulty range, and the player attempts to guess that specific word.

In Evil Mode, the program does not fix a word at the beginning. Instead, it maintains a dynamic list of all words matching the chosen difficulty. After each guess, the program groups all possible words according to how the guessed letter would appear (e.g., positions revealed or not). It then selects the group that reveals the least information to the player, typically the largest group that avoids exposing new letters when possible. This adaptive behaviour continues until only one word remains, at which point the game proceeds like Classic Mode. All other elements—ASCII drawing, misses, and guess validation—operate the same way in Evil Mode.

---

GAMEPLAY
During the game, the player may guess a letter or type “hint”. A valid letter guess consists of exactly one alphabetic character; uppercase letters are accepted but converted to lowercase. If a letter has already been guessed, the program notifies the user and asks for a new input. Any invalid input (multiple characters, numbers, symbols, or unrelated commands) results in an error message and a reprompt.

Correct guesses reveal all instances of the letter in the word pattern. Incorrect guesses reduce the remaining misses and advance the ASCII hangman drawing. After every action, the program displays the updated word pattern, the remaining hints, remaining misses, the list of guessed letters, the ASCII figure, and the current number of words still matching the pattern. The pattern analyser updates this number by filtering the active word list according to known correct and incorrect letters; in Evil Mode, this list changes dynamically according to the adaptive rules.

---

WIN AND LOSS
The player wins when all letters in the word are revealed. The program displays the completed word and offers the choice to play again or return to the main menu.
The player loses when all 10 misses are used. The final ASCII hangman drawing is shown, the real word is revealed, and the same choices (play again or main menu) are presented. Invalid inputs in these menus are handled with error messages and reprompts.

---

PLAY AGAIN
Choosing to play again immediately begins a new round using the same settings and the same word pack. Returning to the main menu restores the standard navigation options.

---

ERROR HANDLING AND ENDING GAME
If the word packs cannot be read during startup, the program prints an error message and exits. Throughout (menus, gameplay, settings), all invalid user input is handled gracefully. The program always reprompts the user without crashing, ensuring stable and predictable behaviour.

---

## END OF SPECIFICATION