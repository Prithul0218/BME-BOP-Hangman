# User Guide

## Goal of the Program
Play classic Hangman in your terminal. Guess letters to reveal a hidden word from a chosen word pack. You have 10 misses before the game is lost. Optionally enable Evil mode, where the computer changes the word to make guessing harder.

## How to Use
1. Build the program
   - macOS/Linux: `gcc hangman.c -o hangman`
2. Run the program
   - `./hangman`
3. Main Menu
   - Begin Game: start a new round.
   - Adjust Settings: set difficulty (Easy/Medium/Hard/Evil) and hints (0–5).
   - Choose Word Pack: pick from available packs (Default, Engineering, Countries).
   - Exit: quit the program.
4. During the Game
   - Type a single letter (A–Z) to guess.
   - Type `hint` to reveal a letter (if you have hints remaining).
   - The screen shows:
     - ASCII hangman drawing
     - Current word pattern (underscores and revealed letters)
     - Misses used and hints used
     - Guessed letters
     - In Evil mode, the program prints the number of possible words left and sometimes debugging info to show word changes.
5. End of Round
   - If you reveal the whole word, you win.
   - If you reach 10 misses, you lose and the correct word is shown.
   - Choose to play again or exit.

## Difficulty
- Easy: short words (length < 5)
- Medium: medium words (length 5–8)
- Hard: long words (length > 8)
- Evil: dynamic words chosen to avoid revealing your guess when possible.

## Hints
- You can set 0–5 hints. Each `hint` reveals a letter at a random unrevealed position and shows all its occurrences.

## Word Packs
- Default pack: everyday words
- Engineering pack: technical terms
- Countries pack: country names

## Where Files Are Saved and Loaded
- Word packs are plain text files in the working directory:
  - `default.txt`, `engineering.txt`, `countries.txt`
- The game reads the selected pack at startup and loads its words into memory.
- The program does not save game progress or scores to files; session scores (wins/losses) are shown on screen only.

## Tips
- Use Evil mode for a challenge; the computer adapts to your guesses.
- Use hints sparingly to reveal helpful letters.
- If input is invalid, the program will prompt you to try again.
