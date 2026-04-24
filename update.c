/*
 * ============================================================================
 * File Name    : update.c
 * Description  : Database persistence - load/update functionality
 * ============================================================================
 * 
 * PURPOSE:
 * This file implements database import functionality. It reads a previously
 * saved database file and reconstructs the hash table structure in memory.
 * This allows users to continue working with a previously saved database.
 * 
 * FILE FORMAT (must match save.c format):
 * #<index>;<word>;<file_count>;<file1>;<count1>;<file2>;<count2>;...#
 * 
 * PARSING PROCESS:
 * 1. Read file line by line
 * 2. Parse each line into components
 * 3. Reconstruct MainNodes and SubNodes
 * 4. Insert words into hash table
 * 5. Update file list with discovered files
 * 
 * FEATURES:
 * - Validates file format
 * - Skips malformed lines
 * - Checks hash index bounds
 * - Prevents duplicate file list entries
 * - Reports parsing statistics
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: update_database
 * -------------------------
 * Loads a previously saved database from a file into memory.
 * 
 * Algorithm:
 * 1. Validate input parameters
 * 2. Open saved database file for reading
 * 3. Read file line by line using fgets()
 * 4. For each valid line (starting with '#'):
 *    a. Parse hash index
 *    b. Parse word
 *    c. Parse file count
 *    d. For each file entry:
 *       - Parse filename and occurrence count
 *       - Insert word into hash table (count times)
 *       - Add file to file_list if not already present
 * 5. Close file and report statistics
 * 
 * Parameters:
 *   hash_table - Array of hash table entries (output - will be populated)
 *   filename   - Name of the saved database file to load
 *   file_list  - Pointer to file list head pointer (output - updated with files)
 * 
 * Returns:
 *   SUCCESS - Database loaded successfully
 *   FAILURE - File not found, cannot be read, or parameters are NULL
 * 
 * Expected File Format:
 *   #0;apple;1;fruits.txt;10;#
 *   #7;hello;2;test1.txt;5;test2.txt;3;#
 * 
 * Parsing Details:
 *   - Lines not starting with '#' are ignored
 *   - Invalid hash indices are skipped with warning
 *   - Malformed lines are silently skipped
 *   - Each word occurrence is inserted individually
 * 
 * Side Effects:
 *   - Modifies hash_table by adding words
 *   - Updates file_list with discovered filenames
 *   - Allocates memory for nodes
 *   - Prints progress messages
 * 
 * Example:
 *   HashTable_t hash[HASH_TABLE_SIZE];
 *   FileList_t *files = NULL;
 *   if (update_database(hash, "saved_db.txt", &files) == SUCCESS) {
 *       // Database loaded, ready to use
 *   }
 */
int update_database(HashTable_t *hash_table, char *filename, FileList_t **file_list)
{
    /* Validate input parameters */
    if (hash_table == NULL || filename == NULL)
    {
        printf("Error: Invalid parameters for database update.\n");
        return FAILURE;
    }

    /* Open the saved database file for reading */
    FILE *file_ptr = fopen(filename, "r");
    if (file_ptr == NULL)
    {
        printf("Error: Unable to open file '%s' for reading.\n", filename);
        return FAILURE;
    }

    /* Display update operation header */
    printf("\n==============================================\n");
    printf("          UPDATING DATABASE\n");
    printf("==============================================\n");
    printf("Loading from: %s\n", filename);
    printf("----------------------------------------------\n");

    char line[512];           /* Buffer for reading lines from file */
    int lines_processed = 0;  /* Counter for valid lines processed */
    int words_loaded = 0;     /* Counter for words loaded */

    /* Read file line by line */
    while (fgets(line, sizeof(line), file_ptr) != NULL)
    {
        /* Check if line starts with '#' (valid data line) */
        if (line[0] != '#')
        {
            continue;  /* Skip non-data lines */
        }

        /* Start tokenizing the line using ';' as delimiter */
        char *token = strtok(line, ";");
        if (token == NULL)
        {
            continue;  /* Empty or malformed line */
        }

        /* Parse hash index (skip the '#' character at start) */
        int hash_index = atoi(token + 1);
        
        /* Validate hash index is within bounds */
        if (hash_index < 0 || hash_index >= HASH_TABLE_SIZE)
        {
            printf("⚠ Invalid index %d in file, skipping line.\n", hash_index);
            continue;
        }

        /* Parse the word */
        char *word = strtok(NULL, ";");
        if (word == NULL)
        {
            continue;  /* No word found */
        }

        /* Parse file count (number of files containing this word) */
        char *filecount_str = strtok(NULL, ";");
        if (filecount_str == NULL)
        {
            continue;  /* No file count found */
        }
        int file_count = atoi(filecount_str);

        /* Parse all file entries for this word */
        for (int i = 0; i < file_count; i++)
        {
            /* Get filename */
            char *file_name = strtok(NULL, ";");
            /* Get occurrence count */
            char *word_count_str = strtok(NULL, ";");

            /* Validate both tokens were found */
            if (file_name != NULL && word_count_str != NULL)
            {
                int word_count = atoi(word_count_str);

                /* 
                 * Insert word into hash table multiple times
                 * to reconstruct the original occurrence count.
                 * Each insert increments the count by 1.
                 */
                for (int j = 0; j < word_count; j++)
                {
                    insert_word_to_hashtable(hash_table, word, file_name, hash_index);
                }

                /* 
                 * Add file to file list if not already present
                 * This maintains the list of files in the database
                 */
                if (file_list != NULL && *file_list != NULL)
                {
                    /* Check if file already exists in the list */
                    int file_exists = 0;
                    FileList_t *temp = *file_list;
                    while (temp != NULL)
                    {
                        if (strcmp(temp->file_name, file_name) == 0)
                        {
                            file_exists = 1;
                            break;
                        }
                        temp = temp->next;
                    }

                    /* Add to list if it's a new file */
                    if (!file_exists)
                    {
                        FileList_t *new_node = (FileList_t *)malloc(sizeof(FileList_t));
                        if (new_node != NULL)
                        {
                            /* Copy filename with bounds checking */
                            strncpy(new_node->file_name, file_name, MAX_FILENAME_LENGTH - 1);
                            new_node->file_name[MAX_FILENAME_LENGTH - 1] = '\0';
                            
                            /* Insert at head of list */
                            new_node->next = *file_list;
                            *file_list = new_node;
                        }
                    }
                }
            }
        }

        /* Successfully processed this line */
        lines_processed++;
        words_loaded++;
    }

    /* Close the input file */
    fclose(file_ptr);

    /* Display loading statistics */
    printf("✓ Lines processed: %d\n", lines_processed);
    printf("✓ Words loaded: %d\n", words_loaded);
    printf("==============================================\n");
    printf("✓ Database updated successfully.\n");

    return SUCCESS;
}