#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hangman_char.h"

typedef struct {
    char name[50];
    char path[100];
} WordPack;

WordPack word_pack_paths[] = {
    {"Default", "default.txt"},
    {"Engineering", "engineering.txt"},
    {"Countries", "countries.txt"},
};

typedef struct {
    char** data;
    int size;  // current number of elements
    int cap;   // current max capacity
} StringList;  // to store list of words dynamically

void sl_init(StringList* sl) {
    sl->data = NULL;
    sl->size = 0;
    sl->cap = 0;
}

void sl_push(StringList* sl, const char* s) {
    // if at full capacity 
    if (sl->size == sl->cap) {
        int newCap;
        char** newData;
        if (sl->cap == 0)
            newCap = 2;
        else
            newCap = sl->cap * 2;
        newData = (char**)realloc(sl->data, newCap * sizeof(char*));
        if (newData == NULL) {
            printf("realloc error\n");
            exit(1);
        }
        sl->data = newData;
        sl->cap = newCap;
    }

    size_t len = strlen(s);
    char* copy = (char*)malloc(len + 1);
    if (copy == NULL) {
        printf("malloc error\n");
        exit(1);
    }
    strcpy(copy, s);
    sl->data[sl->size] = copy;
    sl->size = sl->size + 1;
}

void sl_free(StringList* sl) {
    for (size_t i = 0; i < sl->size; ++i)
        free(sl->data[i]);
    free(sl->data);
}

typedef struct GuessNode {
    char ch;
    struct GuessNode* next;
} GuessNode;

static int guess_list_contains(GuessNode* head, char ch) {
    while (head) {
        if (head->ch == ch) return 1;
        head = head->next;
    }
    return 0;
}

static void guess_list_add(GuessNode** head, char ch) {
    GuessNode* node = (GuessNode*)malloc(sizeof(GuessNode));
    if (!node) {
        perror("malloc error");
        exit(1);
    }
    node->ch = ch;
    node->next = *head;
    *head = node;
}

static void guess_list_print(GuessNode* head) {
    printf("Guessed letters: ");
    while (head) {
        printf("%c ", head->ch);
        head = head->next;
    }
    printf("\n");
}

static void guess_list_free(GuessNode* head) {
    while (head) {
        GuessNode* tmp = head;
        head = head->next;
        free(tmp);
    }
}

typedef struct {
    int wins;
    int losses;
} Score;

void score_init(Score* s) { s->wins = s->losses = 0; }
void score_inc_win(Score* s) { s->wins++; }
void score_inc_loss(Score* s) { s->losses++; }

typedef struct {
    char* word;            // owned elsewhere (from words list)
    char* renderedString;  // same length as word, underscores + revealed letters
    int incorrectGuesses;
    int difficulty;  // 0 to 4
    int max_hints;   // maximum hints allowed
    int hints_used;  // number of hints used
} Board;

void board_reset(Board* b, char* word) {
    b->hints_used = 0;  // reset hints used each round

    b->word = word;
    size_t n = strlen(word);
    free(b->renderedString);
    b->renderedString = (char*)malloc(n + 1);
    if (b->renderedString == NULL) {
        printf("malloc error");
        exit(1);
    }
    for (size_t i = 0; i < n; ++i) b->renderedString[i] = '_';
    b->renderedString[n] = '\0';
    b->incorrectGuesses = 0;
}
int board_make_guess(Board* b, char lett, GuessNode** guessList) {
    // repeated letters were not being shown in evil mode. so if letter was already guessed, allow revealing if it matches new word positions
    if (guess_list_contains(*guessList, lett)) {
        bool revealedAny = false;
        int n0 = (int)strlen(b->word);
        for (int i = 0; i < n0; ++i) {
            if (b->word[i] == lett && b->renderedString[i] == '_') {
                b->renderedString[i] = lett;
                revealedAny = true;
            }
        }
        if (revealedAny) {
            return 1; // previously guessed letter now reveals due to dynamic word
        }
        printf("You already guessed '%c'!\n", lett);
        return -1; // already guessed and nothing to reveal
    }
    // Add to guess list
    guess_list_add(guessList, lett);

    // Check if guess is correct
    bool isCorrect = false;
    int n = (int)strlen(b->word);
    for (int i = 0; i < n; ++i) {
        if (b->word[i] == lett) {
            b->renderedString[i] = lett;
            isCorrect = true;
        }
    }
    if (!isCorrect) b->incorrectGuesses++;
    return isCorrect;
}

int board_is_win(const Board* b) { return strcmp(b->word, b->renderedString) == 0; }

int board_is_game_over(const Board* b) {
    return board_is_win(b) || b->incorrectGuesses >= 10;
}

void board_print(const Board* b) {
    printf("%s\n\n%s\n", HANGMAN_ASCII[b->incorrectGuesses], b->renderedString);
    printf("Misses: %d/10\n", b->incorrectGuesses);
    printf("Hints: %u/%u used\n", (unsigned)b->hints_used, (unsigned)b->max_hints);
}

// reveal a helpful letter if hints remain
static void give_hint(Board* b, GuessNode** guesses) {
    if (b->hints_used >= b->max_hints) {
        printf("No hints left.\n");
        return;
    }

    int len = strlen(b->renderedString);
    // find all the unrevealed positions
    int indexes[256];
    int count = 0;
    for (int i = 0; i < len; ++i) {
        if (b->renderedString[i] == '_') indexes[count++] = i;
    }
    if (count == 0) {
        printf("idk how you got here but all letters are already revealed.\n");
        return;
    }
    int pick = rand() % count;
    int indexReveal = indexes[pick];
    char letter = b->word[indexReveal];

    // record guess in Linked List
    letter = tolower(letter);
    if (!guess_list_contains(*guesses, letter)) {
        guess_list_add(guesses, letter);
    }
    // reveal all occurrences of this letter
    for (int j = 0; j < (int)strlen(b->word); ++j) {
        if (b->word[j] == letter) b->renderedString[j] = letter;
    }
    b->hints_used++;
    printf("Hint: revealed letter '%c'\n", letter);
}

void load_words(const char* filename, StringList* out) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("fopen error");
        exit(1);
    }
    char buf[256];  // 256 max line length for now
    while (fgets(buf, sizeof buf, f)) {
        // trim newline terminator
        size_t len = strlen(buf);
        while (len && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
            buf[--len] = '\0';
        if (len == 0) continue;
        sl_push(out, buf);
    }
    fclose(f);
    if (out->size == 0) {
        perror("no words loaded");
        exit(1);
    }
}

char* pick_random_word(const StringList* sl, int difficulty) {
    // Filter words based on difficulty
    StringList filtered;
    sl_init(&filtered);

    if (difficulty == 4) {  // Evil Hangman mode, pick any word
        for (size_t i = 0; i < sl->size; ++i) {
            sl_push(&filtered, sl->data[i]);
        }
    } else {
        for (size_t i = 0; i < sl->size; ++i) {
            size_t len = strlen(sl->data[i]);
            if ((difficulty == 1 && len < 5) ||               // easy
                (difficulty == 2 && len >= 5 && len <= 8) ||  // medium
                (difficulty == 3 && len > 8)) {               // hard
                sl_push(&filtered, sl->data[i]);
            }
        }
    }
    if (filtered.size == 0) {
        printf("No words available for the selected difficulty. Using all words.\n");
        return sl->data[rand() % sl->size];
    } else {
        return filtered.data[rand() % filtered.size];
    }
    sl_free(&filtered);
}

char* pick_dynamic_word(const StringList* sl, const Board* b, char guess, int word_length, GuessNode** guesses) {
    if (sl->size <= 0) return NULL;

    // Build a set of wrong-letters and revealed (present) letters from guesses in the revealed pattern
    int wrong[26] = {0};
    int present[26] = {0};
    // mark letters that are already revealed in the current board pattern
    for (int i = 0; i < word_length; ++i) {
        char pat = b->renderedString[i];
        if (pat != '_') {
            char lc = (char)tolower((unsigned char)pat);
            if (lc >= 'a' && lc <= 'z') present[lc - 'a'] = 1;
        }
    }
    // mark guessed letters that do NOT appear in the revealed pattern as wrong
    for (GuessNode* g = *guesses; g != NULL; g = g->next) {
        char lc = g->ch;
        if (lc >= 'a' && lc <= 'z' && !present[lc - 'a']) wrong[lc - 'a'] = 1;
    }
    // Build a list of candidate words that match the current pattern and do not contain any wrong letters
    int cap = sl->size;
    char** current = (char**)malloc((size_t)cap * sizeof(char*));
    if (!current) {
        perror("malloc error");
        exit(1);
    }
    int number_of_candidates = 0;
    for (int i = 0; i < sl->size; ++i) {
        const char* w = sl->data[i];
        // length must match the current word length
        if ((int)strlen(w) != word_length) continue;
        int ok = 1;
        // pattern matcher. every revealed position must match the candidate
        for (int j = 0; j < word_length; ++j) {
            char pat = b->renderedString[j];
            if (pat != '_' && w[j] != pat) {
                ok = 0;
                break;
            }
        }
        if (!ok) continue;
        // exclude words containing any letter we know is surely wrong
        for (const char* p = w; ok && *p; ++p) {
            char lc = (char)tolower((unsigned char)*p);
            if (lc >= 'a' && lc <= 'z' && wrong[lc - 'a']) ok = 0;
        }
        if (ok) current[number_of_candidates++] = (char*)w;
    }

    if (number_of_candidates == 0) {
        free(current);
        printf("No words of the required length found. Using any word.\n");
        // so now pick any word from the pack. If possible, avoid choosing the exact same word as last time to keep gameplay varied.
         
        if (sl->size == 1) {
            return sl->data[0];
        } else {
            int idx = rand() % sl->size;
            if (sl->data[idx] == b->word) {
                idx = (idx + 1) % sl->size;
            }
            return sl->data[idx];
        }
    }

    printf("Possible words left: %d\n", number_of_candidates);

    // Prefer candidates that avoid revealing the current guess letter, if possible
    int avoidCount = 0;
    for (int i = 0; i < number_of_candidates; ++i) {
        if (strchr(current[i], guess) == NULL) avoidCount++;
    }
    char* candidate = NULL;
    if (avoidCount > 0) {
        // build a list of words that avoid the current guess letters and pick from them
        char** avoider_words = (char**)malloc((size_t)avoidCount * sizeof(char*));
        int ai = 0;
        for (int i = 0; i < number_of_candidates; ++i) {
            if (strchr(current[i], guess) == NULL) avoider_words[ai++] = current[i];
        }
        int pick = rand() % avoidCount;
        candidate = avoider_words[pick];
        // pick another word if avoid count > 1 and we picked same word again */
        if (avoidCount > 1 && candidate == b->word) {
            pick = (pick + 1) % avoidCount;
            candidate = avoider_words[pick];
        }

        printf("Candidates: ");
        for (int i = 0; i < avoidCount; ++i) {
            printf("%s ", avoider_words[i]);
        }
        printf("\nAvoiding letter '%c'\n", guess);
        free(avoider_words);
    } else {
        // no way to avoid revealing; pick among all, prefer different word
        int pick = rand() % number_of_candidates;
        candidate = current[pick];
        if (number_of_candidates > 1 && candidate == b->word) {
            pick = (pick + 1) % number_of_candidates;
            candidate = current[pick];
        }

        printf("Candidates: ");
        for (int i = 0; i < number_of_candidates; ++i) {
            printf("%s ", current[i]);
        }
        printf("\nCouldn't avoid letter '%c'. Letting it pass.\n", guess);
    }

    printf("\n");
    free(current);
    return candidate;
}

int main(void) {
    srand((unsigned)time(NULL)); // seed random number generator

    StringList words;
    sl_init(&words);

    Score score;
    score_init(&score);

    Board board = {0};
    board.renderedString = NULL;
    board.difficulty = 1;     // Default difficulty: Medium
    board.max_hints = 3;      // Default maximum hints
    board.hints_used = 0;     // Initialize hints used
    int wordpack_choice = 1;  // Default word pack choice

    printf("\nWelcome to Hangman!\n");
    printf("-------------------\n");

    bool inMenu = true;
    while (inMenu) {
        printf("\nMain Menu:\n");
        printf("1. Begin Game\n");
        printf("2. Adjust Settings\n");
        printf("3. Choose Word Pack\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        char menu_choice;
        while (true) {
            if (scanf(" %c", &menu_choice) != 1) {
                printf("Invalid input. Please enter a number between 1 and 4: ");
                continue;
            }
            if (menu_choice < '1' || menu_choice > '4') {
                printf("Invalid choice. Please enter a number between 1 and 4: ");
                continue;
            }
            break;
        }
        if (menu_choice == '1') {
            inMenu = false;  // Exit menu to start game
            break;
        }
        if (menu_choice == '2') {
            printf("\n1. Set Difficulty\n");
            printf("2. Set Maximum Hints\n");
            printf("Enter your choice: ");
            char settings_choice;
            while (true) {
                if (scanf(" %c", &settings_choice) != 1) {
                    printf("Invalid input. Please enter 1 or 2: ");
                    continue;
                }
                if (settings_choice != '1' && settings_choice != '2') {
                    printf("Invalid choice. Please enter 1 or 2: ");
                    continue;
                }
                break;
            }
            if (settings_choice == '1') {
                printf("\nSelect Difficulty Level:\n");
                printf("1. Easy (Short words)\n");
                printf("2. Medium (Medium-length words)\n");
                printf("3. Hard (Long words)\n");
                printf("4. Evil Hangman (Dynamic words)\n");
                printf("Enter your choice: ");
                int difficulty_choice;
                while (true) {
                    if (scanf(" %d", &difficulty_choice) != 1) {
                        printf("Invalid input. Please enter 1, 2, 3, or 4: ");
                        continue;
                    }
                    if (difficulty_choice < 1 || difficulty_choice > 4) {
                        printf("Invalid choice. Please enter 1, 2, 3, or 4: ");
                        continue;
                    }
                    break;
                }
                board.difficulty = difficulty_choice;  // Set difficulty directly
                printf("Difficulty set to %d.\n", board.difficulty);

            } else if (settings_choice == '2') {
                int max_hints;
                printf("Enter maximum number of hints allowed (0-5): ");
                while (true) {
                    if (scanf(" %d", &max_hints) != 1 || max_hints < 0 || max_hints > 5) {
                        printf("Invalid input. Please enter a number between 0 and 5: ");
                        continue;
                    }
                    break;
                }
                board.max_hints = max_hints;
                printf("Maximum hints set to %d.\n", max_hints);
            }
        }
        if (menu_choice == '3') {
            printf("\nWhich word pack would you like to use?\n");
            int number_of_packs = (int)(sizeof(word_pack_paths) / sizeof(word_pack_paths[0]));
            for (int i = 0; i < number_of_packs; ++i) {
                printf("%d: %s\n", i + 1, word_pack_paths[i].name);
            }
            printf("Enter choice (1-%d): ", number_of_packs);
            while (true) {
                if (scanf(" %d", &wordpack_choice) != 1) {
                    printf("\nInvalid input, try again: \n");
                    while (getchar() != '\n');  // clear input buffer
                    continue;
                }
                if (wordpack_choice < 1 || wordpack_choice > (int)(sizeof(word_pack_paths) / sizeof(word_pack_paths[0]))) {
                    printf("\nOut of range, try again: \n");
                    continue;
                }
                break;
            }
        }
        if (menu_choice == '4') {
            printf("Goodbye!\n");
            return 0;
        }
    }

    load_words(word_pack_paths[wordpack_choice - 1].path, &words);
    printf("Loaded %d words from %s.\n", words.size, word_pack_paths[wordpack_choice - 1].path);
    printf("Starting game with difficulty %d and max hints %d.\n", board.difficulty, board.max_hints);

    int playAgain = 1;
    char letter;

    while (playAgain) {
        GuessNode* guesses = NULL; /* linked list of guesses for this round */

        char* word = pick_random_word(&words, board.difficulty);
        board_reset(&board, word);

        int aborted = 0;
        while (!board_is_game_over(&board)) {  // keep playing
            printf("\n");
            board_print(&board);
            guess_list_print(guesses);
            printf("Guess a letter or type 'hint': ");

            char input[5];
            int sr = scanf(" %4s", input);
            if (sr == EOF) {
                aborted = 1;
                break;
            }
            if (sr != 1) continue;
            if (strcmp(input, "hint") == 0) {
                give_hint(&board, &guesses);
                continue;
            }
            if (strlen(input) != 1) {
                printf("Invalid input. Enter one letter or 'hint'.\n");
                continue;
            }
            letter = input[0];

            char guess = (char)tolower((unsigned char)letter);
            if (!isalpha((unsigned char)guess)) {
                printf("Please enter a letter (A-Z).\n");
                continue;
            }
            // Evil Hangman mode - pick a new word if possible
            if (board.difficulty == 4) {
                printf("\n-------------Evil Hangman debug info:-------------\n");
                printf("Your original word was: %s\n", board.word);
                word = pick_dynamic_word(&words, &board, guess, (int)strlen(board.word), &guesses);
                board.word = word;
                printf("The new word is: %s\n", board.word);
                printf("--------------------------------------------------\n");
            }
            if (!board_make_guess(&board, guess, &guesses)) {
                printf("Incorrect guess!\n");
            }
        }
        if (aborted) {
            printf("\nGoodbye!\n");
            guess_list_free(guesses);
            break;
        }
        // show result
        if (board_is_win(&board)) {
            printf("Congratulations, you won! The word was: %s\n", board.word);
            score_inc_win(&score);
        } else {
            board_print(&board);
            printf("Ohno, you lost. The word was: %s\n", board.word);
            score_inc_loss(&score);
        }
        printf("Wins: %d, Losses: %d\n",
               score.wins, score.losses);

        printf("Play again? (y/n): ");
        while (scanf(" %c", &letter) != 1) {
            printf("\nInvalid input, try again.\n");
        }  // read input string
        playAgain = tolower(letter) == 'y';
        guess_list_free(guesses);
    }

    free(board.renderedString);
    sl_free(&words);
    return 0;
}
