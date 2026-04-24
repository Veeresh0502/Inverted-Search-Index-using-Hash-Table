/*
 * ============================================================================
 * File Name    : display.c
 * Description  : Display functionality for inverted search database
 * ============================================================================
 * 
 * PURPOSE:
 * This file implements the database display functionality. It shows all
 * indexed words along with the files containing them and occurrence counts.
 * 
 * DISPLAY FORMAT:
 * [Index: XX] [word      ] Files: N
 *     ├─ filename1            : M occurrence(s)
 *     ├─ filename2            : K occurrence(s)
 * 
 * OUTPUT INCLUDES:
 * - Hash table index
 * - Word itself
 * - Number of files containing the word
 * - List of files with occurrence counts
 * - Total unique words in database
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: display_database
 * --------------------------
 * Displays the complete inverted index database in a formatted view.
 * 
 * Algorithm:
 * 1. Validate hash_table parameter
 * 2. Iterate through all hash table buckets (0-27)
 * 3. For each bucket:
 *    a. Traverse the linked list of words (MainNodes)
 *    b. For each word:
 *       - Display word and file count
 *       - Traverse file list (SubNodes)
 *       - Display each file and occurrence count
 * 4. Show total unique words count
 * 
 * Parameters:
 *   hash_table - Array of hash table entries to display
 * 
 * Display Format Per Word:
 *   [Index: ##] [word] Files: #
 *       ├─ filename : # occurrence(s)
 * 
 * Side Effects:
 *   - Prints formatted output to stdout
 *   - Does not modify hash_table
 */
void display_database(HashTable_t *hash_table)
{
    /* Validate input parameter */
    if (hash_table == NULL)
    {
        printf("Error: Hash table is NULL.\n");
        return;
    }

    /* Print display header */
    printf("\n==============================================\n");
    printf("     INVERTED SEARCH INDEX DATABASE\n");
    printf("==============================================\n\n");

    int has_data = 0;              /* Flag to track if database has any data */
    int total_unique_words = 0;    /* Counter for total unique words */

    /* Iterate through all hash table buckets */
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        /* Check if this bucket contains any words */
        if (hash_table[i].head != NULL)
        {
            has_data = 1;  /* Database is not empty */
            
            /* Traverse the linked list of words in this bucket */
            MainNode_t *main_current = hash_table[i].head;

            while (main_current != NULL)
            {
                total_unique_words++;  /* Count this unique word */

                /* Display word information: index, word, and file count */
                printf("[Index: %2d] [%-10s] Files: %d\n",
                       i, main_current->word, main_current->file_count);

                /* Traverse the linked list of files for this word */
                SubNode_t *sub_current = main_current->sub_link;
                while (sub_current != NULL)
                {
                    /* Display file name and occurrence count with tree-like formatting */
                    printf("    ├─ %-20s : %d occurrence(s)\n",
                           sub_current->file_name, sub_current->word_count);

                    sub_current = sub_current->sub_link;  /* Move to next file */
                }

                printf("\n");  /* Blank line between words for readability */
                main_current = main_current->main_link;  /* Move to next word */
            }
        }
    }

    /* Handle empty database case */
    if (!has_data)
    {
        printf("Database is empty. No data to display.\n");
    }
    else
    {
        /* Display summary statistics */
        printf("==============================================\n");
        printf("Total unique words in database: %d\n", total_unique_words);
        printf("==============================================\n");
    }
}