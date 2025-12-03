# Developer Documentation

## Overview
This Hangman program is a console-based game implemented in portable C. It supports multiple word packs, difficulty settings (including an Evil mode), hints, and a pattern-aware dynamic word picker. The program emphasizes simple data structures, dynamic memory management, and clear I/O flow.

## Modules and Responsibilities
- `StringList` (dynamic array of strings): load, store, and free word lists.
- Guess linked list: track guessed letters, prevent duplicates, print and free.
- Board: hold current word state, rendered pattern, hint usage, and miss count.
- Word loading: read words from a selected pack into `StringList`.
- Word selection:
  - `pick_random_word`: pick per difficulty ranges.
  - `pick_dynamic_word`: Evil mode filter by revealed pattern and wrong letters; avoid revealing current guess when possible.
- Game loop: menus, input validation, guess handling, hint reveal, round results.

## Data Structures
- `StringList`
  - Fields: `char** data`, `int size`, `int cap`
  - Purpose: dynamically store word strings loaded from files.
- `GuessNode`
  - Fields: `char ch`, `GuessNode* next`
  - Purpose: track guessed letters in a simple singly linked list.
- `Score`
  - Fields: `int wins`, `int losses`
  - Purpose: track session results.
- `Board`
  - Fields: `char* word`, `char* renderedString`, `int incorrectGuesses`, `int difficulty`, `int max_hints`, `int hints_used`
  - Purpose: represent the current round state.

## Algorithms
- Pattern filtering (Evil mode):
  - Build `present[26]` from `renderedString` (revealed letters).
  - Build `wrong[26]` from guessed letters not in `present`.
  - Filter words: same length, match revealed positions, exclude any letter in `wrong`.
  - Prefer candidates without the current guess letter (avoid reveal); otherwise choose among all candidates. Avoid repeating the last word if possible.
- Hint reveal:
  - Collect indices of unrevealed positions, choose a random index, reveal the letter and all its occurrences, track hint usage, and add letter to guess list.
- Guess handling:
  - Reject non-alpha and duplicate guesses.
  - If duplicate but now reveals (due to Evil mode switch), reveal and succeed.
  - Update `renderedString` and `incorrectGuesses` accordingly.

## Functions and Interfaces
- StringList
  - `void sl_init(StringList* sl)`
    - In: pointer to `StringList`
    - Out: initializes `data=NULL`, `size=0`, `cap=0`.
  - `void sl_push(StringList* sl, const char* s)`
    - In: list pointer, C-string
    - Effect: grows array if needed; allocates and copies `s` into list.
  - `void sl_free(StringList* sl)`
    - In: list pointer
    - Effect: frees all owned strings and the array.
- Guess list
  - `static int guess_list_contains(GuessNode* head, char ch)`
    - In: list head, character
    - Out: 1 if present else 0.
  - `static void guess_list_add(GuessNode** head, char ch)`
    - In: address of head pointer, character
    - Effect: pushes a new node at the head.
  - `static void guess_list_print(GuessNode* head)`
    - In: list head
    - Effect: prints guessed letters.
  - `static void guess_list_free(GuessNode* head)`
    - In: list head
    - Effect: frees the list.
- Board
  - `void board_reset(Board* b, char* word)`
    - In: board pointer, word pointer (owned by `StringList`)
    - Effect: resets round state, allocates `renderedString` of underscores.
  - `int board_make_guess(Board* b, char lett, GuessNode** guessList)`
    - In: board pointer, letter, guess list address
    - Out: 1 if correct, 0 if incorrect, -1 if already guessed (no reveal)
    - Effect: updates `renderedString`, may increment `incorrectGuesses`.
  - `int board_is_win(const Board* b)`
    - Out: 1 if `renderedString` equals `word`.
  - `int board_is_game_over(const Board* b)`
    - Out: 1 if win or `incorrectGuesses >= 10`.
  - `void board_print(const Board* b)`
    - Effect: prints ASCII figure, pattern, miss and hint info.
- Words
  - `void load_words(const char* filename, StringList* out)`
    - In: filename, output list
    - Effect: reads lines, trims newline, pushes words; exits on failure.
  - `char* pick_random_word(const StringList* sl, int difficulty)`
    - In: list, difficulty (1–4)
    - Out: pointer to a word from `sl`.
  - `char* pick_dynamic_word(const StringList* sl, const Board* b, char guess, int word_length, GuessNode** guesses)`
    - In: list, current board, current guess, word length, guess list address
    - Out: pointer to chosen candidate.
- Hints
  - `static void give_hint(Board* b, GuessNode** guesses)`
    - In: board pointer, guesses address
    - Effect: reveals a random unrevealed letter, updates guesses and hints.

## Memory Management Notes
- `sl_push` allocates each string; `sl_free` frees them.
- `board_reset` frees the previous `renderedString` before allocating new.
- Guess list nodes are freed each round.
- Temporary arrays in `pick_dynamic_word` are freed before return.

## Error Handling
- All memory allocations checked; program exits with an error message on failure.
- Input parsing validates ranges and formats; invalid input prompts retry.

## Build and Run
- Build: `gcc hangman.c -o hangman`
- Run: `./hangman`

---

# Change Log (Key Decisions)
- Added support to reveal previously guessed letters if word changes to include them (Evil mode fairness).
- Implemented candidate avoidance of current guess to behave like classic Evil Hangman.
