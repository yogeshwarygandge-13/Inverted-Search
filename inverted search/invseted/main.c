/*name-yogeshwary
date-16/8/25
project on inverted search*/
#include "inverted.h"

/* define globals */
int g_db_created = 0;
int g_db_updated = 0;

/* helper implementations (small, local) */

int is_txt_file(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot, ".txt") == 0);
}

int file_exists(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    return 0;
    
    fclose(fp);
    return 1;
}

int file_empty(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) return 1; /* treat missing as "empty" for safety - main checks existence first */
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fclose(fp);
    return (sz == 0);
}

int file_in_list(filelist_t *head, const char *filename)
{
    while (head)
     {
        if (strcmp(head->filename, filename) == 0)
         return 1;
        head = head->next;
    }
    return 0;
}

void insert_filelist_end(filelist_t **head, const char *filename)
{
    filelist_t *node = (filelist_t*)malloc(sizeof(filelist_t));
    if (!node)
     { 
        perror("malloc"); 
        exit(1); }
    strncpy(node->filename, filename, MAX_FILENAME-1);
    node->filename[MAX_FILENAME-1] = '\0';
    node->next = NULL;

    if (!*head) 
    { 
        *head = node;
         return;
         }
    filelist_t *t = *head;
    while (t->next) t = t->next;
    t->next = node;
}

void print_filelist(filelist_t *head)
{
    while (head) 
    {
        printf("%s\n", head->filename);
        head = head->next;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) 
    {
        printf("Usage: %s file1.txt file2.txt ...\n", argv[0]);
        return 1;
    }

    /* Build validated file list */
    filelist_t *file_list = NULL;
    for (int i = 1; i < argc; ++i)
     {
        const char *fname = argv[i];

        if (!is_txt_file(fname))
         {
            printf("Error: File '%s' does not have .txt extension.\n", fname);
            continue;
        }

        if (!file_exists(fname)) 
        {
            printf("Error: File '%s' not found.\n", fname);
            continue;
        }

        if (file_empty(fname)) 
        {
            printf("Error: File '%s' is empty.\n", fname);
            continue;
        }

        if (file_in_list(file_list, fname)) 
        {
            printf("Skipping: Duplicate file -> %s\n", fname);
            continue;
        }

        insert_filelist_end(&file_list, fname);
    }

    if (!file_list)
     {
        printf("No valid files to process. Exiting.\n");
        return 1;
    }

    /* hash table */
    WordNode *hashtable[TABLE_SIZE] = { NULL };

    /* menu */
    int choice;
    do {
        printf("\n----- Inverted Search Menu -----\n");
        printf("1. Create Database\n");
        printf("2. Display Database\n");
        printf("3. Search Database\n");
        printf("4. Save Database\n");
        printf("5. Update Database\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
         { 
            /* clear stdin and exit */
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            choice = 6;
        }
        getchar(); /* consume newline */

        switch (choice)
         {
        case 1:
            if (g_db_created) 
            {
                printf("Error: Database already created. Cannot create again.\n");
            }
             else if (g_db_updated)
              {
                printf("Error: Database already updated from backup. Cannot create now.\n");
            }
             else 
             {
                create_database(file_list, hashtable);
                g_db_created = 1;
                printf("Database created successfully.\n");
            }
            break;

        case 2:
            display_database(hashtable);
            break;

        case 3:
            search_database(hashtable);
            break;

        case 4:
            save_database(hashtable);
            break;

        case 5:
            if (g_db_created)
            
            {
                printf("Error: Cannot update after creating database.\n");
            } 
            else if (g_db_updated) 
            {
                printf("Error: Database already updated. Cannot update again.\n");
            } 
            else 
            {
                update_database(hashtable);
                /* update_database sets g_db_updated if success */
            }
            break;

        case 6:
            printf("Exiting...\n");
            break;

        default:
            printf("Invalid choice. Try again.\n");
        }

    } while (choice != 6);

    return 0;
}
