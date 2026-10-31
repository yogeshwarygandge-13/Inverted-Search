#include "inverted.h"

void save_database(WordNode *hashtable[])
{
    /* check empty */
    int empty = 1;
    for (int i = 0; i < TABLE_SIZE; ++i) if (hashtable[i]) { empty = 0; break; }
    if (empty) {
        printf("Database is empty. Nothing to save.\n");
        return;
    }

    char backup[MAX_FILENAME];
    printf("Enter the backup filename: ");
    if (scanf("%255s", backup) != 1) { printf("Invalid filename.\n"); return; }

    FILE *fp = fopen(backup, "w");
    if (!fp) { perror("Error opening backup file"); return; }

    for (int i = 0; i < TABLE_SIZE; ++i) {
        WordNode *w = hashtable[i];
        while (w) {
            fprintf(fp, "#%d;%s;%d;", i, w->word, w->file_count);
            FileNode *f = w->files;
            while (f) {
                fprintf(fp, "%s;%d;", f->filename, f->word_count);
                f = f->next;
            }
            fprintf(fp, "#\n");
            w = w->next;
        }
    }

    fclose(fp);
    printf("Database saved successfully in %s\n", backup);
}
