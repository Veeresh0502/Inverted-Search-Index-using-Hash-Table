/*
 * ============================================================================
 * File Name    : function.c
 * Description  : Utility and helper functions for inverted search index
 * ============================================================================
 * 
 * PURPOSE:
 * This file contains all utility and helper functions used throughout the
 * inverted search index project. These include hash index calculation,
 * word insertion logic, file validation, and memory management functions.
 * 
 * FUNCTIONS INCLUDED:
 * - is_file_in_database()      : Check if file exists in database
 * - validate_txt_extension()   : Validate .txt file extension
 * - insert_word_to_hashtable() : Core insertion logic
 * - get_hash_index()          : Calculate hash bucket index
 * - free_file_list()          : Deallocate file list
 * - free_database()           : Deallocate entire hash table
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: is_file_in_database
 * ------------------------------
 * Checks if a given filename exists in the database.
 * 
 * Algorithm:
 * 1. Iterate through all hash table buckets
 * 2. For each MainNode in each bucket:
 *    - Traverse its SubNode linked list
 *    - Compare filename with each SubNode's filename
 * 3. Return 1 if found, 0 if not found
 * 
 * Parameters:
 *   hash_table - Array of hash table entries to search
 *   filename   - Name of file to search for
 * 
 * Returns:
 *   1 - File is found in database
 *   0 - File is not found or parameters are NULL
 * 
 * Time Complexity: O(n×m) where n=words, m=files per word
 * 
 * Usage:
 *   if (is_file_in_database(hash, "test.txt")) {
 *       printf("File already indexed\n");
 *   }
 */
int is_file_in_database(HashTable_t *hash_table, const char *filename)
{
    /* Validate input parameters */
    if (hash_table == NULL || filename == NULL)
    {
        return 0;  /* Return "not found" for invalid inputs */
    }

    /* Iterate through all hash table buckets (0 to HASH_TABLE_SIZE-1) */
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        /* Get the first word in this bucket */
        MainNode_t *main_current = hash_table[i].head;
        
        /* Traverse all words in this bucket */
        while (main_current != NULL)
        {
            /* Get the first file for this word */
            SubNode_t *sub_current = main_current->sub_link;
            
            /* Traverse all files for this word */
            while (sub_current != NULL)
            {
                /* Compare filenames */
                if (strcmp(sub_current->file_name, filename) == 0)
                {
                    return 1;  /* Found! */
                }
                sub_current = sub_current->sub_link;
            }
            main_current = main_current->main_link;
        }
    }
    
    return 0;  /* Not found after searching entire database */
}

/*
 * Function: validate_txt_extension
 * ---------------------------------
 * Validates that a filename has the .txt extension.
 * 
 * Algorithm:
 * 1. Find the last occurrence of '.' in filename
 * 2. Compare the extension with ".txt"
 * 
 * Parameters:
 *   filename - String containing the filename to validate
 * 
 * Returns:
 *   SUCCESS - Filename ends with .txt
 *   FAILURE - Filename is NULL, has no extension, or wrong extension
 * 
 * Examples:
 *   validate_txt_extension("file.txt")     → SUCCESS
 *   validate_txt_extension("file.doc")     → FAILURE
 *   validate_txt_extension("file")         → FAILURE
 *   validate_txt_extension("my.file.txt")  → SUCCESS
 */
int validate_txt_extension(const char *filename)
{
    /* Check for NULL filename */
    if (filename == NULL)
    {
        return FAILURE;
    }

    /* Find the last occurrence of '.' in the filename */
    const char *dot_position = strrchr(filename, '.');
    
    /* Check if dot exists and extension is ".txt" */
    if (dot_position == NULL || strcmp(dot_position, ".txt") != 0)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Function: insert_word_to_hashtable
 * -----------------------------------
 * Inserts a word occurrence into the hash table, handling all cases:
 * - New word (create MainNode and SubNode)
 * - Existing word, new file (create SubNode)
 * - Existing word, existing file (increment count)
 * 
 * Algorithm:
 * 1. Validate inputs and convert word to lowercase
 * 2. Search for MainNode with this word:
 *    - If not found: Create new MainNode
 * 3. Search for SubNode with this filename:
 *    - If not found: Create new SubNode, increment file_count
 *    - If found: Increment word_count
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 *   word       - Word to insert (will be converted to lowercase)
 *   filename   - File containing the word
 *   index      - Hash table index (must be 0-27)
 * 
 * Data Structure Changes:
 *   Case 1 - New Word:
 *     Creates: MainNode + SubNode
 *     Updates: hash_table[index].head (if first in bucket)
 *   
 *   Case 2 - Existing Word, New File:
 *     Creates: SubNode
 *     Updates: MainNode.file_count++
 *   
 *   Case 3 - Existing Word & File:
 *     Updates: SubNode.word_count++
 * 
 * Memory Allocation:
 *   Allocates memory for MainNode and/or SubNode as needed.
 *   Checks for allocation failures before proceeding.
 * 
 * Side Effects:
 *   - Modifies hash_table structure
 *   - Allocates memory
 *   - Prints error message if allocation fails
 */
void insert_word_to_hashtable(HashTable_t *hash_table, char *word, char *filename, int index)
{
    /* Validate input parameters */
    if (hash_table == NULL || word == NULL || filename == NULL)
    {
        return;
    }

    /* Validate hash index is within bounds */
    if (index < 0 || index >= HASH_TABLE_SIZE)
    {
        return;
    }

    /* Convert word to lowercase for case-insensitive storage */
    char lowercase_word[MAX_WORD_LENGTH];
    int i;
    for (i = 0; word[i] && i < MAX_WORD_LENGTH - 1; i++)
    {
        lowercase_word[i] = tolower((unsigned char)word[i]);
    }
    lowercase_word[i] = '\0';  /* Null terminate */

    /* ==================== SEARCH FOR MAIN NODE (WORD) ==================== */
    
    MainNode_t *main_current = hash_table[index].head;  /* Start of bucket */
    MainNode_t *main_previous = NULL;                   /* Track previous node */

    /* Traverse MainNode linked list to find matching word */
    while (main_current != NULL && strcmp(main_current->word, lowercase_word) != 0)
    {
        main_previous = main_current;
        main_current = main_current->main_link;
    }

    /* If word doesn't exist, create new MainNode */
    if (main_current == NULL)
    {
        MainNode_t *new_main_node = (MainNode_t *)malloc(sizeof(MainNode_t));
        if (new_main_node == NULL)
        {
            printf("Error: Memory allocation failed for MainNode.\n");
            return;
        }

        /* Initialize MainNode fields */
        strncpy(new_main_node->word, lowercase_word, MAX_WORD_LENGTH - 1);
        new_main_node->word[MAX_WORD_LENGTH - 1] = '\0';
        new_main_node->file_count = 0;
        new_main_node->main_link = NULL;
        new_main_node->sub_link = NULL;

        /* Insert into linked list */
        if (main_previous == NULL)
        {
            /* First node in this bucket */
            hash_table[index].head = new_main_node;
        }
        else
        {
            /* Append to end of list */
            main_previous->main_link = new_main_node;
        }

        main_current = new_main_node;  /* Update current pointer */
    }

    /* ==================== SEARCH FOR SUB NODE (FILE) ==================== */
    
    SubNode_t *sub_current = main_current->sub_link;  /* Start of file list */
    SubNode_t *sub_previous = NULL;                   /* Track previous node */

    /* Traverse SubNode linked list to find matching filename */
    while (sub_current != NULL && strcmp(sub_current->file_name, filename) != 0)
    {
        sub_previous = sub_current;
        sub_current = sub_current->sub_link;
    }

    /* If filename doesn't exist for this word, create new SubNode */
    if (sub_current == NULL)
    {
        SubNode_t *new_sub_node = (SubNode_t *)malloc(sizeof(SubNode_t));
        if (new_sub_node == NULL)
        {
            printf("Error: Memory allocation failed for SubNode.\n");
            return;
        }

        /* Initialize SubNode fields */
        strncpy(new_sub_node->file_name, filename, MAX_FILENAME_LENGTH - 1);
        new_sub_node->file_name[MAX_FILENAME_LENGTH - 1] = '\0';
        new_sub_node->word_count = 1;  /* First occurrence */
        new_sub_node->sub_link = NULL;

        /* Insert into linked list */
        if (sub_previous == NULL)
        {
            /* First file for this word */
            main_current->sub_link = new_sub_node;
        }
        else
        {
            /* Append to end of file list */
            sub_previous->sub_link = new_sub_node;
        }

        /* Increment file count for this word */
        main_current->file_count++;
    }
    else
    {
        /* File already exists - just increment occurrence count */
        sub_current->word_count++;
    }
}

/*
 * Function: free_file_list
 * ------------------------
 * Frees all memory allocated for the file list.
 * 
 * Algorithm:
 * 1. Traverse the linked list
 * 2. For each node:
 *    - Save pointer to next node
 *    - Free current node
 *    - Move to next
 * 3. Set head to NULL
 * 
 * Parameters:
 *   head - Pointer to the file list head pointer
 * 
 * Side Effects:
 *   - Frees all FileList_t nodes
 *   - Sets *head to NULL
 *   - Safe to call with NULL or empty list
 * 
 * Usage:
 *   FileList_t *files = ...;
 *   free_file_list(&files);
 *   // files is now NULL
 */
void free_file_list(FileList_t **head)
{
    /* Check for NULL pointer */
    if (head == NULL || *head == NULL)
    {
        return;
    }

    FileList_t *current = *head;
    FileList_t *next_node;

    /* Traverse and free each node */
    while (current != NULL)
    {
        next_node = current->next;  /* Save next pointer */
        free(current);              /* Free current node */
        current = next_node;        /* Move to next */
    }

    *head = NULL;  /* Set head to NULL */
}

/*
 * Function: free_database
 * -----------------------
 * Frees all memory allocated for the entire hash table database.
 * 
 * Algorithm:
 * 1. For each hash table bucket:
 *    a. Traverse MainNode linked list
 *    b. For each MainNode:
 *       - Traverse and free all SubNodes
 *       - Free the MainNode
 *    c. Set bucket head to NULL
 * 
 * Parameters:
 *   hash_table - Array of hash table entries to free
 * 
 * Memory Freed:
 *   - All SubNode_t structures (file entries)
 *   - All MainNode_t structures (word entries)
 *   - Total: All dynamically allocated memory in database
 * 
 * Side Effects:
 *   - Frees all database memory
 *   - Sets all bucket heads to NULL
 *   - Safe to call multiple times
 *   - Database cannot be used after this call
 * 
 * Usage:
 *   HashTable_t hash[HASH_TABLE_SIZE];
 *   // ... use database ...
 *   free_database(hash);
 *   // hash is now empty but can be reused
 */
void free_database(HashTable_t *hash_table)
{
    /* Check for NULL pointer */
    if (hash_table == NULL)
    {
        return;
    }

    /* Iterate through all hash table buckets */
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        MainNode_t *main_current = hash_table[i].head;
        
        /* Free all MainNodes in this bucket */
        while (main_current != NULL)
        {
            MainNode_t *main_next = main_current->main_link;  /* Save next */

            /* Free all SubNodes for this word */
            SubNode_t *sub_current = main_current->sub_link;
            while (sub_current != NULL)
            {
                SubNode_t *sub_next = sub_current->sub_link;  /* Save next */
                free(sub_current);                             /* Free file node */
                sub_current = sub_next;                        /* Move to next */
            }

            free(main_current);        /* Free word node */
            main_current = main_next;  /* Move to next word */
        }
        
        hash_table[i].head = NULL;  /* Set bucket head to NULL */
    }
}

/*
 * Function: get_hash_index
 * ------------------------
 * Calculates the hash table index for a given word based on its first character.
 * 
 * Hash Function:
 *   - Letters (a-z or A-Z): Maps to indices 0-25
 *   - Digits (0-9): Maps to index 26
 *   - Symbols/Others: Maps to index 27
 * 
 * Algorithm:
 * 1. Get first character of word
 * 2. Convert to lowercase if uppercase
 * 3. Determine character type and return appropriate index
 * 
 * Parameters:
 *   word - String to calculate hash index for
 * 
 * Returns:
 *   0-25  : Word starts with letter (a-z)
 *   26    : Word starts with digit (0-9)
 *   27    : Word starts with symbol or NULL/empty
 * 
 * Examples:
 *   get_hash_index("apple")  → 0  (a)
 *   get_hash_index("Zebra")  → 25 (z, converted from Z)
 *   get_hash_index("123")    → 26 (digit)
 *   get_hash_index("#tag")   → 27 (symbol)
 *   get_hash_index("")       → 27 (empty)
 * 
 * Properties:
 *   - Case-insensitive: "Apple" and "apple" hash to same index
 *   - Deterministic: Same input always produces same output
 *   - Fast: O(1) time complexity
 */
int get_hash_index(const char *word)
{
    /* Handle NULL or empty string */
    if (word == NULL || word[0] == '\0')
    {
        return HASH_TABLE_SIZE - 1;  /* Return symbols bucket (27) */
    }

    char first_char = word[0];

    /* Convert to lowercase if uppercase */
    if (isupper((unsigned char)first_char))
    {
        first_char = tolower((unsigned char)first_char);
    }

    /* Determine character type and return appropriate index */
    if (first_char >= 'a' && first_char <= 'z')
    {
        /* Letters: 'a'=0, 'b'=1, ..., 'z'=25 */
        return first_char - 'a';
    }
    else if (first_char >= '0' && first_char <= '9')
    {
        /* Digits: all map to index 26 */
        return 26;
    }
    else
    {
        /* Symbols and special characters: all map to index 27 */
        return 27;
    }
}