/*
 * ============================================================================
 * File Name    : save.c
 * Description  : Database persistence - save functionality
 * ============================================================================
 * 
 * PURPOSE:
 * This file implements database export functionality. It serializes the
 * entire hash table structure to a text file in a custom format that can
 * be loaded later using the update_database function.
 * 
 * FILE FORMAT:
 * Each line represents one word entry:
 * #<index>;<word>;<file_count>;<file1>;<count1>;<file2>;<count2>;...#
 * 
 * FORMAT BREAKDOWN:
 * - # : Start marker
 * - index : Hash table index (0-27)
 * - word : The indexed word
 * - file_count : Number of files containing this word
 * - file1, count1 : Filename and occurrence count (repeated for each file)
 * - # : End marker
 * 
 * EXAMPLE:
 * #7;hello;2;test1.txt;5;test2.txt;3;#
 * This means: word "hello" at index 7, in 2 files
 *             test1.txt (5 times), test2.txt (3 times)
 * 
 * PROCESS:
 * 1. Open output file for writing
 * 2. Iterate through all hash buckets
 * 3. For each word, write formatted line
 * 4. Close file and report statistics
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: save_database
 * -----------------------
 * Exports the entire inverted index database to a text file.
 * 
 * Algorithm:
 * 1. Validate input parameters
 * 2. Open file for writing
 * 3. Iterate through all hash table buckets (0 to HASH_TABLE_SIZE-1)
 * 4. For each MainNode in each bucket:
 *    a. Write hash index, word, and file count
 *    b. Write all SubNodes (filename and word count pairs)
 *    c. Write end marker
 * 5. Close file and display statistics
 * 
 * Parameters:
 *   hash_table - Array of hash table entries to save
 *   filename   - Name of the output file (should end with .txt)
 * 
 * Output File Format:
 *   #<index>;<word>;<file_count>;<file1>;<count1>;<file2>;<count2>;...#
 * 
 * Example Output:
 *   #0;apple;1;fruits.txt;10;#
 *   #7;hello;2;test1.txt;5;test2.txt;3;#
 *   #26;123abc;1;data.txt;2;#
 * 
 * Side Effects:
 *   - Creates or overwrites the specified file
 *   - Prints status messages to stdout
 * 
 * Error Handling:
 *   - Returns if hash_table or filename is NULL
 *   - Returns if file cannot be opened for writing
 *   - Handles empty database case
 */
void save_database(HashTable_t *hash_table, char *filename)
{
    /* Validate input parameters */
    if (hash_table == NULL || filename == NULL)
    {
        printf("Error: Invalid parameters for saving database.\n");
        return;
    }

    /* Open file for writing (creates new file or overwrites existing) */
    FILE *file_ptr = fopen(filename, "w");
    if (file_ptr == NULL)
    {
        printf("Error: Unable to open file '%s' for writing.\n", filename);
        return;
    }

    /* Display save operation header */
    printf("\n==============================================\n");
    printf("          SAVING DATABASE\n");
    printf("==============================================\n");

    int data_found = 0;      /* Flag to check if database has data */
    int words_saved = 0;     /* Counter for total words saved */

    /* 
     * FILE FORMAT SPECIFICATION:
     * #<index>;<word>;<file_count>;<filename1>;<count1>;<filename2>;<count2>;...#
     * 
     * Example: #7;hello;2;test1.txt;5;test2.txt;3;#
     * Means: "hello" at index 7, appears in 2 files:
     *        - test1.txt: 5 times
     *        - test2.txt: 3 times
     */

    /* Iterate through all hash table buckets */
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        /* Get the first word in this bucket */
        MainNode_t *main_current = hash_table[i].head;

        /* Traverse all words in this bucket */
        while (main_current != NULL)
        {
            data_found = 1;      /* Database is not empty */
            words_saved++;       /* Count this word */

            /* Write the word entry header: #<index>;<word>;<file_count>; */
            fprintf(file_ptr, "#%d;%s;%d;",
                    i,                          /* Hash index */
                    main_current->word,         /* The word */
                    main_current->file_count);  /* Number of files */

            /* Write all file entries for this word */
            SubNode_t *sub_current = main_current->sub_link;
            while (sub_current != NULL)
            {
                /* Write filename and occurrence count */
                fprintf(file_ptr, "%s;%d;",
                        sub_current->file_name,    /* Filename */
                        sub_current->word_count);  /* Count in this file */

                /* Move to next file */
                sub_current = sub_current->sub_link;
            }

            /* Write end marker and newline */
            fprintf(file_ptr, "#\n");
            
            /* Move to next word in this bucket */
            main_current = main_current->main_link;
        }
    }

    /* Close the output file */
    fclose(file_ptr);

    /* Display save results */
    if (data_found)
    {
        printf("✓ Database saved successfully to '%s'\n", filename);
        printf("  Total words saved: %d\n", words_saved);
    }
    else
    {
        printf("⚠ Database is empty. No data saved.\n");
    }

    printf("==============================================\n");
}