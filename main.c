/*
 * ============================================================================
 * File Name    : main.c
 * Description  : Main program file for Inverted Search Index
 * ============================================================================
 * 
 * PURPOSE:
 * This file contains the main program logic and menu-driven interface for
 * the inverted search index application. Users can create, display, search,
 * save, and update the database through an interactive menu.
 * 
 * PROGRAM FLOW:
 * 1. Initialize hash table
 * 2. Validate command-line arguments
 * 3. Display menu and process user choices
 * 4. Clean up resources on exit
 * 
 * MENU OPTIONS:
 * 1. Create Database    - Process files and build index
 * 2. Display Database   - Show all indexed words
 * 3. Search Word        - Find word occurrences
 * 4. Save Database      - Export to file
 * 5. Update Database    - Load from file
 * 6. Exit              - Clean up and terminate
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: main
 * --------------
 * Entry point of the inverted search index program.
 * 
 * Parameters:
 *   argc - Number of command-line arguments
 *   argv - Array of command-line argument strings (filenames)
 * 
 * Returns:
 *   SUCCESS on normal termination
 *   FAILURE if no valid input files are provided
 * 
 * Description:
 *   Initializes the hash table, validates input files, and provides
 *   an interactive menu for database operations. Ensures proper cleanup
 *   of all allocated resources before exiting.
 */
int main(int argc, char *argv[])
{
    /* ==================== INITIALIZATION ==================== */
    
    /* Initialize hash table with HASH_TABLE_SIZE (28) buckets */
    HashTable_t hash_table[HASH_TABLE_SIZE];
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        hash_table[i].index = i;      /* Set bucket index */
        hash_table[i].head = NULL;     /* No words initially */
    }

    /* Initialize file list and database status flags */
    FileList_t *file_list = NULL;          /* List of input files */
    int is_database_created = 0;           /* Flag: database created from files */
    int is_database_updated = 0;           /* Flag: database loaded from save file */

    /* ==================== ARGUMENT VALIDATION ==================== */
    
    /* Validate command-line arguments and build file list */
    if (validate_arguments(argc, argv, &file_list) == FAILURE)
    {
        printf("\n✗ No valid input files found. Exiting program.\n");
        return FAILURE;
    }

    /* ==================== MAIN MENU LOOP ==================== */
    
    int choice;  /* User's menu selection */

    while (1)  /* Infinite loop until user chooses to exit */
    {
        /* Display menu options */
        printf("\n=========================================\n");
        printf("       INVERTED SEARCH INDEX MENU\n");
        printf("=========================================\n");
        printf("1. Create Database\n");
        printf("2. Display Database\n");
        printf("3. Search Word\n");
        printf("4. Save Database\n");
        printf("5. Update Database\n");
        printf("6. Exit\n");
        printf("-----------------------------------------\n");
        printf("Enter your choice (1-6): ");

        /* Read user's choice with input validation */
        if (scanf("%d", &choice) != 1)
        {
            /* Clear invalid input from buffer */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("✗ Invalid input. Please enter a number between 1 and 6.\n");
            continue;
        }
        getchar(); /* Consume the newline character left by scanf */

        /* ==================== MENU OPTION PROCESSING ==================== */
        
        switch (choice)
        {
        case 1: /* CREATE DATABASE */
        {
            if (is_database_created)
            {
                printf("\n⚠ Database already created.\n");
                printf("To add new files, use 'Update Database' option after saving.\n");
            }
            else if (is_database_updated)
            {
                printf("\n⚠ Database was loaded from file.\n");
                printf("Adding only new files not already in database...\n\n");

                /* Filter out files already in database */
                FileList_t *filtered_list = NULL;
                FileList_t *current = file_list;

                while (current != NULL)
                {
                    if (is_file_in_database(hash_table, current->file_name) == 0)
                    {
                        FileList_t *new_node = (FileList_t *)malloc(sizeof(FileList_t));
                        if (new_node != NULL)
                        {
                            strcpy(new_node->file_name, current->file_name);
                            new_node->next = NULL;

                            if (filtered_list == NULL)
                            {
                                filtered_list = new_node;
                            }
                            else
                            {
                                FileList_t *temp = filtered_list;
                                while (temp->next != NULL)
                                {
                                    temp = temp->next;
                                }
                                temp->next = new_node;
                            }
                        }
                    }
                    else
                    {
                        printf("⚠ Skipping existing file: %s\n", current->file_name);
                    }
                    current = current->next;
                }

                if (filtered_list == NULL)
                {
                    printf("\n⚠ No new files to add.\n");
                }
                else if (create_database(hash_table, filtered_list) == SUCCESS)
                {
                    is_database_created = 1;
                }

                free_file_list(&filtered_list);
            }
            else
            {
                if (create_database(hash_table, file_list) == SUCCESS)
                {
                    is_database_created = 1;
                }
            }
            break;
        }

        case 2: /* DISPLAY DATABASE */
        {
            if (!is_database_created && !is_database_updated)
            {
                printf("\n⚠ Database not created or loaded yet.\n");
                printf("Please create or update the database first.\n");
            }
            else
            {
                display_database(hash_table);
            }
            break;
        }

        case 3: /* SEARCH WORD */
        {
            if (!is_database_created && !is_database_updated)
            {
                printf("\n⚠ Database not created or loaded yet.\n");
                printf("Please create or update the database first.\n");
                break;
            }

            char word_to_search[MAX_WORD_LENGTH];
            printf("Enter word to search: ");
            if (scanf("%s", word_to_search) == 1)
            {
                search_word(hash_table, word_to_search);
            }
            else
            {
                printf("✗ Invalid input.\n");
            }
            getchar(); /* Consume newline */
            break;
        }

        case 4: /* SAVE DATABASE */
        {
            if (!is_database_created && !is_database_updated)
            {
                printf("\n⚠ No database to save.\n");
                printf("Please create or update the database first.\n");
                break;
            }

            char save_filename[MAX_FILENAME_LENGTH];
            printf("Enter filename to save database (.txt): ");
            if (scanf("%s", save_filename) != 1)
            {
                printf("✗ Invalid input.\n");
                getchar();
                break;
            }

            if (validate_txt_extension(save_filename) == FAILURE)
            {
                printf("✗ Invalid file extension. Please use .txt extension.\n");
                break;
            }

            save_database(hash_table, save_filename);
            break;
        }

        case 5: /* UPDATE DATABASE */
        {
            if (is_database_created)
            {
                printf("\n⚠ Database already created.\n");
                printf("Update is only available before creating the database.\n");
                printf("If you want to load a saved database, restart the program.\n");
                break;
            }

            if (is_database_updated)
            {
                printf("\n⚠ Database already loaded from file.\n");
                printf("Cannot update again. Use 'Create Database' to add new files.\n");
                break;
            }

            char update_filename[MAX_FILENAME_LENGTH];
            printf("Enter filename to load database from (.txt): ");
            if (scanf("%s", update_filename) != 1)
            {
                printf("✗ Invalid input.\n");
                getchar();
                break;
            }

            if (validate_txt_extension(update_filename) == FAILURE)
            {
                printf("✗ Invalid file extension. Please use .txt extension.\n");
                break;
            }

            if (update_database(hash_table, update_filename, &file_list) == SUCCESS)
            {
                is_database_updated = 1;
            }
            break;
        }

        case 6: /* EXIT */
        {
            printf("\n=========================================\n");
            printf("         EXITING PROGRAM\n");
            printf("=========================================\n");
            printf("Cleaning up resources...\n");

            free_database(hash_table);
            free_file_list(&file_list);

            printf("✓ Resources freed successfully.\n");
            printf("Thank you for using Inverted Search Index!\n");
            printf("=========================================\n");

            return SUCCESS;
        }

        default:
        {
            printf("\n✗ Invalid choice. Please enter a number between 1 and 6.\n");
            break;
        }
        }
    }

    return SUCCESS;
}