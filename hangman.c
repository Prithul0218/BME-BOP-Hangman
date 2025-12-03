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
    size_t size;
    size_t cap;
} StringList;  // to store list of words dynamically

void sl_init(StringList* sl) {
    sl->data = NULL;
    sl->size = 0;
    sl->cap = 0;
}

void sl_push(StringList* sl, const char* s) {
    if (sl->size == sl->cap) {
        size_t newcap = sl->cap == 0 ? 2 : sl->cap * 2;
        char** nd = (char**)realloc(sl->data, newcap * sizeof(char*));
        if (nd == NULL) {
            printf("realloc error");
            exit(1);
        }
        sl->data = nd;
        sl->cap = newcap;
    }
    sl->data[sl->size++] = strdup(s);  // allocate and copy string
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
    uint8_t difficulty;  // 0 (short) to 2 (long) words
    uint8_t max_hints;   // maximum hints allowed
    uint8_t hints_used;  // number of hints used
} Board;

void board_reset(Board* b, char* word) {
    b->difficulty = 0;  // default
    b->max_hints = 3;   // default
    b->hints_used = 0;  // reset hints used

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
    /* Check if letter was already guessed */
    if (guess_list_contains(*guessList, lett)) {
        printf("You already guessed '%c'!\n", lett);
        return -1; /* already guessed */
    }
    /* Add to guess list */
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
    printf("Hints: %u/%u used\n", (unsigned)b->hints_used, (unsigned)b->max_hints);
}

/* reveal a helpful letter if hints remain */
static void give_hint(Board* b, GuessNode** guesses) {
    if (b->hints_used >= b->max_hints) {
        printf("No hints left.\n");
        return;
    }

    int len = strlen(b->renderedString);
    /* find all the unrevealed positions */
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

    /* record guess in Linked List*/
    letter = tolower(letter);
    if (!guess_list_contains(*guesses, letter)) {
        guess_list_add(guesses, letter);
    }
    /* reveal all occurrences of this letter */
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
        /* trim newline */
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

int main(void) {
    srand((unsigned)time(NULL)); /* seed random number generator */

    StringList words;
    sl_init(&words);

    /*
    Program Menus
    When the program starts, the user is greeted with a main menu that allows them to begin a game,
    adjust settings, choose a word pack, or exit.All input must be valid
    before the program proceeds.Invalid inputs result in a clear error message
    and a new prompt.Exiting the program displays a short goodbye message and terminates.
    */
   
    printf("Which word pack would you like to use?\n");
    for (size_t i = 0; i < sizeof(word_pack_paths) / sizeof(word_pack_paths[0]); ++i) {
        printf("%zu: %s\n", i + 1, word_pack_paths[i].name);
    }
    size_t choice = 0;
    printf("Enter choice (1-%zu): ", sizeof(word_pack_paths) / sizeof(word_pack_paths[0]));
    if (scanf(" %zu", &choice) != 1) {
        choice = 1;
        printf("\nInvalid input, defaulting to 1.\n");
    }
    if (choice < 1 || choice > (sizeof(word_pack_paths) / sizeof(word_pack_paths[0]))) {
        printf("\nOut of range, defaulting to 1.\n");
        choice = 1;
    }
    load_words(word_pack_paths[choice - 1].path, &words);
    printf("Loaded %zu words from %s.\n", words.size, word_pack_paths[choice - 1].path);

    Score score;
    score_init(&score);

    Board board = {0};
    int playAgain = 1;
    char letter;

    while (playAgain) {
        GuessNode* guesses = NULL; /* linked list of guesses for this round */
        // pick random word
        char* word = words.data[rand() % words.size];
        board_reset(&board, word);

        int aborted = 0;
        while (!board_is_game_over(&board)) {  // keep playing
            printf("\n");
            board_print(&board);
            guess_list_print(guesses);
            printf("Guess a letter or type 'hint': ");
            {
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
                    printf("Enter one letter or 'hint'.\n");
                    continue;
                }
                letter = input[0];
            }
            {
                char guess = (char)tolower((unsigned char)letter);
                if (!isalpha((unsigned char)guess)) {
                    printf("Please enter a letter (A-Z).\n");
                    continue;
                }
                if (!board_make_guess(&board, guess, &guesses)) {
                    printf("Incorrect guess!\n");
                }
            }
        }
        if (aborted) {
            printf("\nGoodbye!\n");
            guess_list_free(guesses);
            break;
        }
        // show result
        if (board_is_win(&board)) {
            printf("Congratulations, you won!\n");
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
