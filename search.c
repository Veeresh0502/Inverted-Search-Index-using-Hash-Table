/*
 * ============================================================================
 * File Name    : search.c
 * Description  : Word search functionality for inverted index database
 * ============================================================================
 * 
 * PURPOSE:
 * This file implements the word search functionality. It allows users to
 * search for a specific word and displays all files containing that word
 * along with the occurrence count in each file.
 * 
 * FEATURES:
 * - Case-insensitive search (converts input to lowercase)
 * - Fast O(1) average-case lookup using hash table
 * - Displays all files containing the searched word
 * - Shows occurrence count for each file
 * - Calculates total occurrences across all files
 * 
 * SEARCH PROCESS:
 * 1. Convert search term to lowercase
 * 2. Calculate hash index from first character
 * 3. Search in the appropriate hash bucket
 * 4. Display results with formatting
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: search_word
 * ---------------------
 * Searches for a word in the inverted index and displays all occurrences.
 * 
 * Algorithm:
 * 1. Validate input parameters (hash_table and word are not NULL)
 * 2. Convert search word to lowercase for case-insensitive search
 * 3. Calculate hash index using first character
 * 4. Traverse the MainNode linked list in the hash bucket
 * 5. When word is found:
 *    - Display file count
 *    - Iterate through all SubNodes (files)
 *    - Display each file name and occurrence count
 *    - Calculate total occurrences
 * 6. If word not found, display appropriate message
 * 
 * Parameters:
 *   hash_table - Array of hash table entries containing the database
 *   word       - The word to search for (case-insensitive)
 * 
 * Time Complexity:
 *   Average Case: O(1) - Direct hash access
 *   Worst Case: O(n) - All words in same bucket (collision)
 * 
 * Output Format:
 *   Search term: 'word'
 *   Hash index: N
 *   ✓ Word found in M file(s):
 *     1. filename1 : X occurrence(s)
 *     2. filename2 : Y occurrence(s)
 *   Total occurrences: X+Y
 * 
 * Example:
 *   search_word(hash_table, "Hello");
 *   // Searches for "hello" (converted to lowercase)
 *   // Output: All files containing "hello" with counts
 */
void search_word(HashTable_t *hash_table, char *word)
{
    /* Validate input parameters */
    if (hash_table == NULL || word == NULL)
    {
        printf("Error: Invalid parameters for search.\n");
        return;
    }

    /* Convert search word to lowercase for case-insensitive search */
    char lowercase_word[MAX_WORD_LENGTH];
    int i;
    for (i = 0; word[i] && i < MAX_WORD_LENGTH - 1; i++)
    {
        lowercase_word[i] = tolower((unsigned char)word[i]);
    }
    lowercase_word[i] = '\0';  /* Null terminate the string */

    /* Calculate hash index based on first character */
    int hash_index = get_hash_index(lowercase_word);

    /* Display search information header */
    printf("\n==============================================\n");
    printf("         SEARCH RESULTS\n");
    printf("==============================================\n");
    printf("Search term: '%s'\n", lowercase_word);
    printf("Hash index: %d\n", hash_index);
    printf("----------------------------------------------\n");

    /* Start searching from the head of the hash bucket */
    MainNode_t *main_current = hash_table[hash_index].head;

    /* Traverse the linked list of words in this bucket */
    while (main_current != NULL)
    {
        /* Check if this is the word we're looking for */
        if (strcmp(main_current->word, lowercase_word) == 0)
        {
            /* Word found! Display the number of files */
            printf("✓ Word found in %d file(s):\n\n", main_current->file_count);

            int file_number = 1;           /* Counter for numbering files */
            int total_occurrences = 0;     /* Sum of all occurrences */

            /* Traverse the linked list of files containing this word */
            SubNode_t *sub_current = main_current->sub_link;
            while (sub_current != NULL)
            {
                /* Display file information with formatting */
                printf("  %d. %-25s : %d occurrence(s)\n",
                       file_number++,
                       sub_current->file_name,
                       sub_current->word_count);

                /* Add to total occurrences */
                total_occurrences += sub_current->word_count;
                
                /* Move to next file */
                sub_current = sub_current->sub_link;
            }

            /* Display summary */
            printf("\n----------------------------------------------\n");
            printf("Total occurrences across all files: %d\n", total_occurrences);
            printf("==============================================\n");
            return;  /* Word found and displayed, exit function */
        }
        
        /* Move to next word in the bucket */
        main_current = main_current->main_link;
    }

    /* If we reach here, word was not found */
    printf("✗ Word '%s' not found in database.\n", lowercase_word);
    printf("==============================================\n");
}