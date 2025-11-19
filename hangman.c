#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hangman_char.h"

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

typedef struct {
    int wins;
    int losses;
    int incorrectGuesses;
} Score;

void score_init(Score* s) { s->wins = s->losses = s->incorrectGuesses = 0; }
void score_inc_win(Score* s) { s->wins++; }
void score_inc_loss(Score* s) { s->losses++; }
void score_inc_incorrect(Score* s) { s->incorrectGuesses++; }

typedef struct {
    char* word;    // owned elsewhere (from words list)
    char* guessed; // same length as word, underscores + revealed letters
    int incorrectGuesses;
} Board;

void board_reset(Board* b, char* word) {
    b->word = word;
    size_t n = strlen(word);
    free(b->guessed);
    b->guessed = (char*)malloc(n + 1);
    if (b->guessed == NULL) {
        printf("malloc error");
        exit(1);
    }
    for (size_t i = 0; i < n; ++i) b->guessed[i] = '_';
    b->guessed[n] = '\0';
    b->incorrectGuesses = 0;
}

int board_make_guess(Board* b, char ch) {
    int isCorrect = 0;
    size_t n = strlen(b->word);
    for (size_t i = 0; i < n; ++i) {
        if (b->word[i] == ch) {
            b->guessed[i] = ch;
            isCorrect = 1;
        }
    }
    if (!isCorrect) b->incorrectGuesses++;
    return isCorrect;
}

int board_is_win(const Board* b) {
    return strcmp(b->word, b->guessed) == 0;
}

int board_is_game_over(const Board* b) {
    return board_is_win(b) || b->incorrectGuesses >= 10;
}

void board_print(const Board* b) {
    printf("%s\n\n%s\n", HANGMAN_ASCII[b->incorrectGuesses], b->guessed);
}

void load_words(const char* filename, StringList* out) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open %s\n", filename);
        exit(1);
    }
    char buf[256];  // random max line for now
    while (fgets(buf, sizeof buf, f)) {
        /* trim newline */
        size_t len = strlen(buf);
        while (len && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) buf[--len] = '\0';
        if (len == 0) continue;
        sl_push(out, buf);
    }
    fclose(f);
    if (out->size == 0) {
        fprintf(stderr, "Error: no words loaded from %s\n", filename);
        exit(1);
    }
}

int main(void) {
    srand((unsigned)time(NULL));        // seed random number generator

    StringList words;
    sl_init(&words);
    load_words("default.txt", &words);

    Score score;
    score_init(&score);

    Board board = {0};
    int playAgain = 1;
    char letter;

    while (playAgain) {
        score.incorrectGuesses = 0;
        // pick random word 
        char* word = words.data[rand() % words.size];
        board_reset(&board, word);

        while (!board_is_game_over(&board)) {       // keep playing 
            board_print(&board);
            printf("Guess a letter: ");
            scanf(" %c", &letter);  // read input string
            char guess = tolower(letter);

            if (!board_make_guess(&board, guess)) {
                printf("Incorrect guess!\n");
                score_inc_incorrect(&score);
            }
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
        printf("Wins: %d, Losses: %d, Incorrect Guesses: %d\n",
               score.wins, score.losses, score.incorrectGuesses);

        printf("Play again? (y/n): ");
            scanf(" %c", &letter);  // read input string
        playAgain = tolower(letter) == 'y';
    }

    free(board.guessed);
    sl_free(&words);
    return 0;
}
