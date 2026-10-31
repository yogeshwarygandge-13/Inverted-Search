#include "inverted.h"

void update_database(WordNode *hashtable[])
{
    if (g_db_created) {
        printf("Error: Cannot update after create.\n");
        return;
    }
    if (g_db_updated) {
        printf("Error: Already updated.\n");
        return;
    }

    char backup[MAX_FILENAME];
    printf("Enter backup filename: ");
    if (scanf("%255s", backup) != 1) { printf("Invalid filename.\n"); return; }

    FILE *fp = fopen(backup, "r");
    if (!fp) {
        printf("Error: Could not open backup file '%s'.\n", backup);
        return;
    }

    /* clear existing table */
    for (int i = 0; i < TABLE_SIZE; ++i) hashtable[i] = NULL;

    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] != '#') continue;

        /* trim newline */
        size_t len = strlen(line);
        while (len && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';

        char *p = line + 1; /* skip leading '#' */
        char *endh = strrchr(p, '#');
        if (!endh) continue;
        *endh = '\0';

        char *tok = strtok(p, ";");
        if (!tok) continue;
        int idx = atoi(tok);

        tok = strtok(NULL, ";");
        if (!tok) continue;
        char word[MAX_WORD];
        to_lowercase(tok, word);

        tok = strtok(NULL, ";");
        if (!tok) continue;
        int fcount = atoi(tok);

        if (idx < 0 || idx >= TABLE_SIZE) idx = get_index(word);

        /* create word node and insert at bucket head */
        WordNode *wn = (WordNode*)malloc(sizeof(WordNode));
        if (!wn) { perror("malloc"); fclose(fp); return; }
        strncpy(wn->word, word, MAX_WORD-1);
        wn->word[MAX_WORD-1] = '\0';
        wn->file_count = 0;
        wn->files = NULL;
        wn->next = hashtable[idx];
        hashtable[idx] = wn;

        for (int i = 0; i < fcount; ++i) {
            char *fname = strtok(NULL, ";");
            char *cnt = strtok(NULL, ";");
            if (!fname || !cnt) break;
            FileNode *fn = (FileNode*)malloc(sizeof(FileNode));
            if (!fn) { perror("malloc"); fclose(fp); return; }
            strncpy(fn->filename, fname, MAX_FILENAME-1);
            fn->filename[MAX_FILENAME-1] = '\0';
            fn->word_count = atoi(cnt);
            fn->next = wn->files;
            wn->files = fn;
            wn->file_count++;
        }
    }

    fclose(fp);
    g_db_updated = 1;
    printf("Database updated successfully from '%s'\n", backup);
}
