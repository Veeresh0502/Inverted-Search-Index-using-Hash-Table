/*
 * ============================================================================
 * File Name    : create.c
 * Description  : Database creation functionality for inverted search index
 * ============================================================================
 * 
 * PURPOSE:
 * This file implements the core database creation logic. It reads words from
 * text files and builds the inverted index hash table structure.
 * 
 * PROCESS:
 * 1. Open each file in the file list
 * 2. Read words one by one
 * 3. Clean words (remove punctuation)
 * 4. Calculate hash index based on first character
 * 5. Insert word into hash table
 * 6. Track statistics (files processed, words indexed)
 * 
 * DATA FLOW:
 * Input Files → Read Words → Clean Words → Hash Index → Insert to Table
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: create_database
 * -------------------------
 * Creates the inverted index database from a list of text files.
 * 
 * Algorithm:
 * 1. Validate inputs (hash_table and file_list)
 * 2. For each file in the list:
 *    a. Open the file
 *    b. Read each word using fscanf
 *    c. Remove trailing punctuation from word
 *    d. Calculate hash index for the word
 *    e. Insert word into hash table
 *    f. Track words added from this file
 * 3. Display processing statistics
 * 
 * Parameters:
 *   hash_table - Array of hash table entries (output)
 *   file_list  - Linked list of files to process (input)
 * 
 * Returns:
 *   SUCCESS - At least one file was successfully processed
 *   FAILURE - No files could be processed or inputs are NULL
 * 
 * Side Effects:
 *   - Modifies hash_table by adding words
 *   - Allocates memory for MainNodes and SubNodes
 *   - Prints progress messages for each file
 */
int create_database(HashTable_t *hash_table, FileList_t *file_list)
{
    /* Validate input parameters */
    if (hash_table == NULL)
    {
        printf("Error: Hash table is NULL.\n");
        return FAILURE;
    }

    if (file_list == NULL)
    {
        printf("Error: No valid files available to create database.\n");
        return FAILURE;
    }

    /* Display processing header */
    printf("\n==============================================\n");
    printf("          CREATING DATABASE\n");
    printf("==============================================\n");

    /* Initialize counters for statistics */
    int files_processed = 0;      /* Number of files successfully processed */
    int total_words_added = 0;    /* Total number of word occurrences indexed */

    FileList_t *current_file = file_list;  /* Pointer to traverse file list */

    /* Process each file in the list */
    while (current_file != NULL)
    {
        /* Attempt to open the file for reading */
        FILE *file_ptr = fopen(current_file->file_name, "r");
        if (file_ptr == NULL)
        {
            /* File could not be opened - skip it */
            printf("✗ Unable to open file: %-25s (Skipped)\n", current_file->file_name);
            current_file = current_file->next;
            continue;
        }

        int words_from_file = 0;     /* Counter for words from current file */
        char word[MAX_WORD_LENGTH];  /* Buffer to store each word */

        /* Read words from the file until end-of-file */
        while (fscanf(file_ptr, "%s", word) != EOF)
        {
            /* Clean the word by removing trailing punctuation */
            int len = strlen(word);
            while (len > 0 && ispunct((unsigned char)word[len - 1]))
            {
                word[len - 1] = '\0';  /* Remove punctuation character */
                len--;
            }

            /* Skip if word is empty after cleaning */
            if (strlen(word) == 0)
            {
                continue;
            }

            /* Calculate hash index based on first character */
            int hash_index = get_hash_index(word);
            if (hash_index == -1)
            {
                continue;  /* Invalid index - skip word */
            }

            /* Insert the word into the hash table */
            insert_word_to_hashtable(hash_table, word, current_file->file_name, hash_index);
            words_from_file++;
            total_words_added++;
        }

        fclose(file_ptr);  /* Close the file after processing */

        /* Report status for this file */
        if (words_from_file > 0)
        {
            printf("✓ Successfully processed: %-25s (%d words)\n", 
                   current_file->file_name, words_from_file);
            files_processed++;
        }
        else
        {
            printf("⚠ No valid words found in: %-25s\n", current_file->file_name);
        }

        current_file = current_file->next;  /* Move to next file */
    }

    /* Display final statistics */
    printf("==============================================\n");
    printf("  Files processed: %d\n", files_processed);
    printf("  Total words indexed: %d\n", total_words_added);
    printf("==============================================\n");

    /* Check if any files were successfully processed */
    if (files_processed == 0)
    {
        printf("✗ Database creation failed: No valid files processed.\n");
        return FAILURE;
    }

    printf("✓ Database created successfully.\n");
    return SUCCESS;
}