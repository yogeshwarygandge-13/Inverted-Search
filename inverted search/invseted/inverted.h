#ifndef INVERTED_H 
#define INVERTED_H
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILENAME 256
#define MAX_WORD 100
#define TABLE_SIZE 27 /* 26 letters + 1 for non-alpha */

typedef struct filelist
{ 
char filename[MAX_FILENAME];
struct filelist *next;
} filelist_t; /* file occurrence node inside a word (keeps per-word file and count) */

typedef struct FileNode 
{ 
char filename[MAX_FILENAME];
int word_count;
struct FileNode *next;
} FileNode; /* word node used in hash table */

typedef struct WordNode
{ 
char word[MAX_WORD]; 
int file_count; /* number of files this word appears in */
FileNode *files; /* linked list of FileNode */ 
struct WordNode *next;
} WordNode;
 /* global session flags (defined in main.c) */ 

extern int g_db_created; 
extern int g_db_updated; 
/* ---------- helper prototypes ---------- */ 
int is_txt_file(const char *filename); 
int file_exists(const char *filename);
int file_empty(const char *filename); 
int file_in_list(filelist_t *head, const char *filename);
void insert_filelist_end(filelist_t **head, const char *filename); 
void print_filelist(filelist_t *head);
   /* ---------- normalization / hashing ---------- */ 
int normalize_token(const char *in, char *out, size_t out_size); /* returns 1 if token valid */ 
int get_index(const char *word); 
void to_lowercase(const char *src, char *dest); 

   /* ---------- core operations ---------- */ 
void create_database(filelist_t *file_list, WordNode *hashtable[]); 
void display_database(WordNode *hashtable[]); 
void search_database(WordNode *hashtable[]);
void save_database(WordNode *hashtable[]);
void update_database(WordNode *hashtable[]);

#endif
