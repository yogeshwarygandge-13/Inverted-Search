#include "inverted.h"

void display_database(WordNode *hashtable[])
{
    /* header */
    printf("%-10s%-15s%-15s%-20s%-15s\n",
           "[index]", "[word]", "[filecount]", "[filename]", "[wordcount]");

    for (int i = 0; i < TABLE_SIZE; ++i) {
        WordNode *w = hashtable[i];
        while (w) {
            FileNode *f = w->files;
            int first = 1;
            while (f) {
                if (first) {
                    printf("%-10d%-15s%-15d%-20s%-15d\n",
                           i, w->word, w->file_count, f->filename, f->word_count);
                    first = 0;
                } else {
                    printf("%-40s%-20s%-15d\n", "", f->filename, f->word_count);
                }
                f = f->next;
            }
            w = w->next;
        }
    }
}
