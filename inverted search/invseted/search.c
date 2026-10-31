#include "inverted.h"

void search_database(WordNode *hashtable[])
{
    char raw[MAX_WORD];
    printf("Enter the word to search: ");
    if (scanf("%99s", raw) != 1) {
        printf("Input error.\n");
        return;
    }

    char token[MAX_WORD];
    if (!normalize_token(raw, token, sizeof(token))) {
        printf("Invalid search token.\n");
        return;
    }

    int idx = get_index(token);
    WordNode *w = hashtable[idx];
    while (w) {
        if (strcmp(w->word, token) == 0) {
            printf("the word %s is present %d file(s)\n", token, w->file_count);
            FileNode *f = w->files;
            while (f) {
                printf("in file %s for %d times\n", f->filename, f->word_count);
                f = f->next;
            }
            return;
        }
        w = w->next;
    }

    printf("The word '%s' is not found in any file.\n", token);
}
