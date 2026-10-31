#include "inverted.h"

/* Normalize token: strip leading/trailing non-alnum and lowercase inner . 
   Returns 1 if token valid (non-empty after stripping), else 0 */
int normalize_token(const char *in, char *out, size_t out_size)
{
    if (!in || !out || out_size == 0) return 0;
    int s = 0;
    int e = (int)strlen(in) - 1;
    while (s <= e && !isalnum((unsigned char)in[s])) s++;
    while (e >= s && !isalnum((unsigned char)in[e])) e--;
    if (s > e) return 0;

    size_t k = 0;
    for (int i = s; i <= e && k + 1 < out_size; ++i)
     {
        out[k++] = (char)tolower((unsigned char)in[i]);
    }
    out[k] = '\0';
    return (k > 0);
}

int get_index(const char *word)
{
    if (!word || !word[0]) return TABLE_SIZE - 1;
    char c = (char)tolower((unsigned char)word[0]);
    if (c >= 'a' && c <= 'z') return c - 'a';
    return TABLE_SIZE - 1;
}

void to_lowercase(const char *src, char *dest)
{
    while (*src)
     { 
        *dest++ = (char)tolower((unsigned char)*src++); 
    }
    *dest = '\0';
}

/* Append a FileNode at end to preserve file ordering */
static void append_filenode(FileNode **head, const char *filename, int count)
{
    FileNode *node = (FileNode*)malloc(sizeof(FileNode));
    if (!node)
     {
         perror("malloc"); exit(1);
     }
    strncpy(node->filename, filename, MAX_FILENAME-1);
    node->filename[MAX_FILENAME-1] = '\0';
    node->word_count = count;
    node->next = NULL;

    if (!*head) 
    { 
        *head = node; return;
    }
    FileNode *t = *head;
    while (t->next) t = t->next;
    t->next = node;
}

/* create_database: read words from each file (in order of file_list) and build hashtable.
   For each new word, insert WordNode at head of corresponding bucket.*/

void create_database(filelist_t *file_list, WordNode *hashtable[])
{
    /* defensive: if already created or updated, do not proceed */
    if (g_db_created)
    {
         printf("Error: Database already created.\n"); 
         return; 
    }
    if (g_db_updated) 
    { 
         printf("Error: Cannot create after update.\n");
         return; 
    }

    /* initialize buckets */
    for (int i = 0; i < TABLE_SIZE; ++i)
    {
         hashtable[i] = NULL;
    }

    for (filelist_t *f = file_list; f != NULL; f = f->next)
     {
        FILE *fp = fopen(f->filename, "r");
        if (!fp)
         {
             fprintf(stderr, "Warning: cannot open '%s'. Skipping.\n", f->filename);
              continue; 
        }

        char raw[MAX_WORD];
        while (fscanf(fp, "%99s", raw) == 1) {
            char token[MAX_WORD];
            if (!normalize_token(raw, token, sizeof(token))) continue;

            int idx = get_index(token);

            /* find the word in bucket */
            WordNode *wn = hashtable[idx], *found = NULL;
            while (wn) {
                if (strcmp(wn->word, token) == 0) { found = wn; break; }
                wn = wn->next;
            }

            if (!found) {
                /* new word */
                WordNode *neww = (WordNode*)malloc(sizeof(WordNode));
                if (!neww) { perror("malloc"); fclose(fp); return; }
                strncpy(neww->word, token, MAX_WORD-1);
                neww->word[MAX_WORD-1] = '\0';
                neww->file_count = 0;
                neww->files = NULL;
                neww->next = hashtable[idx];
                hashtable[idx] = neww;

                /* add first file node */
                append_filenode(&neww->files, f->filename, 1);
                neww->file_count = 1;
            } else {
                /* check if file already present in this word's file list */
                FileNode *fn = found->files, *fprev = NULL;
                FileNode *ffound = NULL;
                while (fn) {
                    if (strcmp(fn->filename, f->filename) == 0) { ffound = fn; break; }
                    fprev = fn;
                    fn = fn->next;
                }
                if (!ffound) {
                    /* append file node to preserve file order */
                    append_filenode(&found->files, f->filename, 1);
                    found->file_count++;
                } else {
                    /* increment existing file's count */
                    ffound->word_count++;
                }
            }
        } /* end of reading words from file */
        fclose(fp);
    } /* next file */
}
