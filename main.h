/*
 * ============================================================================
 * File Name    : main.h
 * Description  : Header file for Inverted Search Index implementation
 * ============================================================================
 * 
 * OVERVIEW:
 * This header file contains all structure definitions, constants, and function
 * prototypes for the Inverted Search Index project. The inverted index maps
 * words to the files containing them, similar to how search engines work.
 * 
 * DATA STRUCTURE:
 * - Hash Table: 28 buckets (26 letters + 1 digits + 1 symbols)
 * - Main Nodes: Store unique words and their file associations
 * - Sub Nodes: Store file names and word occurrence counts
 * 
 * ============================================================================
 */

#ifndef MAIN_H
#define MAIN_H

/* ======================== STANDARD HEADERS ======================== */
#include <stdio.h>      /* For file I/O operations */
#include <stdlib.h>     /* For memory allocation and utility functions */
#include <string.h>     /* For string manipulation functions */
#include <ctype.h>      /* For character type checking and conversion */

/* ======================== MACRO DEFINITIONS ======================== */

/* Hash table size: 26 (a-z) + 1 (digits) + 1 (symbols) */
#define HASH_TABLE_SIZE 28

/* Maximum length for a word including null terminator */
#define MAX_WORD_LENGTH 30

/* Maximum length for a filename including null terminator */
#define MAX_FILENAME_LENGTH 50

/* Return codes for function success/failure */
#define SUCCESS 1
#define FAILURE 0

/* ======================== STRUCTURE DEFINITIONS ======================== */

/*
 * Structure: SubNode_t
 * --------------------
 * Represents a file entry for a word in the inverted index.
 * This node stores information about a specific file that contains a word.
 * 
 * Members:
 *   file_name   - Name of the file containing the word
 *   word_count  - Number of times the word appears in this file
 *   sub_link    - Pointer to the next file node (linked list)
 * 
 * Usage:
 *   Multiple SubNodes are linked together to represent all files
 *   containing a particular word.
 */
typedef struct SubNode
{
    char file_name[MAX_FILENAME_LENGTH];  /* File containing the word */
    int word_count;                        /* Occurrence count in this file */
    struct SubNode *sub_link;              /* Next file node */
} SubNode_t;

/*
 * Structure: MainNode_t
 * ---------------------
 * Represents a word entry in the inverted index.
 * This is the primary node that stores a unique word and links to
 * all files containing that word.
 * 
 * Members:
 *   word        - The indexed word (stored in lowercase)
 *   file_count  - Number of different files containing this word
 *   sub_link    - Pointer to the first file node (SubNode)
 *   main_link   - Pointer to the next word node in the hash bucket
 * 
 * Usage:
 *   MainNodes are stored in hash table buckets and linked together
 *   to handle hash collisions.
 */
typedef struct MainNode
{
    char word[MAX_WORD_LENGTH];      /* The word itself */
    int file_count;                   /* Number of files containing word */
    struct SubNode *sub_link;         /* Linked list of file nodes */
    struct MainNode *main_link;       /* Next word in hash bucket */
} MainNode_t;

/*
 * Structure: HashTable_t
 * ----------------------
 * Represents a single entry in the hash table.
 * The hash table uses separate chaining for collision resolution.
 * 
 * Members:
 *   index - The hash index (0-27)
 *   head  - Pointer to the first MainNode in this bucket
 * 
 * Hash Index Mapping:
 *   0-25  : Letters a-z
 *   26    : Digits 0-9
 *   27    : Symbols and special characters
 */
typedef struct HashTable
{
    int index;              /* Hash table index */
    MainNode_t *head;       /* First word node in this bucket */
} HashTable_t;

/*
 * Structure: FileList_t
 * ---------------------
 * Represents a node in the file list.
 * Used to maintain the list of input files provided by the user.
 * 
 * Members:
 *   file_name - Name of the text file
 *   next      - Pointer to the next file node
 * 
 * Usage:
 *   Keeps track of all files to be processed during database creation.
 */
typedef struct FileList
{
    char file_name[MAX_FILENAME_LENGTH];  /* Input file name */
    struct FileList *next;                 /* Next file in list */
} FileList_t;

/* ======================== FUNCTION PROTOTYPES ======================== */

/* -------------------- Core Database Functions -------------------- */

/*
 * Function: validate_arguments
 * -----------------------------
 * Validates command-line arguments and builds the file list.
 * 
 * Parameters:
 *   argc - Number of command-line arguments
 *   argv - Array of command-line argument strings
 *   head - Pointer to the file list head pointer
 * 
 * Returns:
 *   SUCCESS if at least one valid file is found
 *   FAILURE if no valid files are found
 * 
 * Description:
 *   Checks each file for .txt extension, existence, and non-empty content.
 *   Valid files are added to a linked list for later processing.
 */
int validate_arguments(int argc, char *argv[], FileList_t **head);

/*
 * Function: create_database
 * -------------------------
 * Creates the inverted index database from the file list.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 *   head       - Pointer to the file list
 * 
 * Returns:
 *   SUCCESS if database is created successfully
 *   FAILURE if no files could be processed
 * 
 * Description:
 *   Reads each file, extracts words, and builds the inverted index
 *   by inserting words into the hash table structure.
 */
int create_database(HashTable_t *hash_table, FileList_t *head);

/*
 * Function: update_database
 * -------------------------
 * Loads a previously saved database from a file.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 *   filename   - Name of the file containing saved database
 *   file_list  - Pointer to file list head pointer
 * 
 * Returns:
 *   SUCCESS if database is loaded successfully
 *   FAILURE if file cannot be read or is malformed
 * 
 * Description:
 *   Parses the saved database file format and reconstructs the
 *   hash table structure in memory.
 */
int update_database(HashTable_t *hash_table, char *filename, FileList_t **file_list);

/*
 * Function: display_database
 * --------------------------
 * Displays the entire inverted index database.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 * 
 * Description:
 *   Iterates through all hash buckets and displays each word along
 *   with the files containing it and occurrence counts.
 */
void display_database(HashTable_t *hash_table);

/*
 * Function: save_database
 * -----------------------
 * Saves the database to a file for later retrieval.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 *   filename   - Name of the file to save to
 * 
 * Description:
 *   Exports the entire hash table structure to a text file in a
 *   custom format that can be loaded later using update_database.
 * 
 * File Format:
 *   #<index>;<word>;<file_count>;<file1>;<count1>;<file2>;<count2>;...#
 */
void save_database(HashTable_t *hash_table, char *filename);

/*
 * Function: search_word
 * ---------------------
 * Searches for a word in the database and displays results.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 *   word       - Word to search for
 * 
 * Description:
 *   Performs case-insensitive search for the specified word and
 *   displays all files containing it with occurrence counts.
 */
void search_word(HashTable_t *hash_table, char *word);

/* -------------------- Helper and Utility Functions -------------------- */

/*
 * Function: validate_txt_extension
 * --------------------------------
 * Validates that a filename has .txt extension.
 * 
 * Parameters:
 *   filename - Name of the file to validate
 * 
 * Returns:
 *   SUCCESS if filename ends with .txt
 *   FAILURE otherwise
 */
int validate_txt_extension(const char *filename);

/*
 * Function: get_hash_index
 * ------------------------
 * Calculates the hash table index for a word.
 * 
 * Parameters:
 *   word - Word to calculate hash index for
 * 
 * Returns:
 *   Hash index (0-27):
 *     0-25  : For words starting with a-z
 *     26    : For words starting with digits
 *     27    : For words starting with symbols
 * 
 * Description:
 *   Uses the first character of the word to determine the bucket.
 *   Case-insensitive (converts to lowercase first).
 */
int get_hash_index(const char *word);

/*
 * Function: insert_word_to_hashtable
 * ----------------------------------
 * Inserts a word occurrence into the hash table.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 *   word       - Word to insert
 *   filename   - File containing the word
 *   index      - Hash table index for the word
 * 
 * Description:
 *   Creates MainNode if word doesn't exist, creates SubNode if file
 *   doesn't exist for the word, or increments count if both exist.
 *   Words are stored in lowercase for case-insensitive indexing.
 */
void insert_word_to_hashtable(HashTable_t *hash_table, char *word, char *filename, int index);

/*
 * Function: is_file_in_database
 * -----------------------------
 * Checks if a file is already indexed in the database.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 *   filename   - Name of the file to check
 * 
 * Returns:
 *   1 if file is found in database
 *   0 if file is not found
 * 
 * Description:
 *   Searches through all hash buckets and sub-nodes to determine
 *   if the specified file has already been indexed.
 */
int is_file_in_database(HashTable_t *hash_table, const char *filename);

/* -------------------- Memory Management Functions -------------------- */

/*
 * Function: free_file_list
 * ------------------------
 * Frees all memory allocated for the file list.
 * 
 * Parameters:
 *   head - Pointer to the file list head pointer
 * 
 * Description:
 *   Iterates through the file list and frees each node.
 *   Sets the head pointer to NULL after freeing.
 */
void free_file_list(FileList_t **head);

/*
 * Function: free_database
 * -----------------------
 * Frees all memory allocated for the hash table.
 * 
 * Parameters:
 *   hash_table - Array of hash table entries
 * 
 * Description:
 *   Iterates through all hash buckets, freeing all MainNodes and
 *   their associated SubNodes. Ensures no memory leaks.
 */
void free_database(HashTable_t *hash_table);

#endif  /* MAIN_H */