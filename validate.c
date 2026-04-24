/*
 * ============================================================================
 * File Name    : validate.c
 * Description  : Command-line argument validation and file list creation
 * ============================================================================
 * 
 * PURPOSE:
 * This file handles validation of command-line arguments. It checks each
 * provided filename for:
 * - Valid .txt extension
 * - File existence and readability
 * - Non-empty file content
 * - No duplicate files
 * 
 * Valid files are added to a linked list for later processing.
 * 
 * VALIDATION CRITERIA:
 * 1. Must have .txt extension
 * 2. File must exist and be readable
 * 3. File must not be empty
 * 4. File must not be a duplicate
 * 
 * ============================================================================
 */

#include "main.h"

/*
 * Function: validate_arguments
 * -----------------------------
 * Validates all command-line arguments and creates a file list.
 * 
 * Process:
 * 1. Check if at least one filename is provided
 * 2. For each filename:
 *    - Validate .txt extension
 *    - Check for duplicates in the list
 *    - Verify file exists and is readable
 *    - Confirm file is not empty
 *    - Add valid files to linked list
 * 3. Display final validated file list
 * 
 * Parameters:
 *   argc - Number of command-line arguments (including program name)
 *   argv - Array of argument strings (argv[0] is program name)
 *   head - Pointer to the file list head pointer (output parameter)
 * 
 * Returns:
 *   SUCCESS - At least one valid file was found
 *   FAILURE - No valid files found or no arguments provided
 * 
 * Side Effects:
 *   - Allocates memory for file list nodes
 *   - Modifies *head to point to the created list
 *   - Prints validation status for each file
 */
int validate_arguments(int argc, char *argv[], FileList_t **head)
{
    /* Check if any filenames were provided */
    if (argc < 2)
    {
        printf("Error: No input files provided.\n");
        printf("Usage: %s <file1.txt> <file2.txt> ...\n", argv[0]);
        return FAILURE;
    }

    /* Print validation header */
    printf("\n============================================\n");
    printf("        Validating and Storing Files\n");
    printf("============================================\n");

    int valid_file_count = 0;  /* Counter for successfully added files */

    /* Iterate through all command-line arguments (skip argv[0] which is program name) */
    for (int i = 1; i < argc; i++)
    {
        /* STEP 1: Validate .txt extension */
        if (validate_txt_extension(argv[i]) == FAILURE)
        {
            printf("✗ Skipping invalid file (not .txt): %s\n", argv[i]);
            continue;
        }

        /* Check if file already exists in list (avoid duplicates) */
        FileList_t *current = *head;
        int is_duplicate = 0;
        while (current != NULL)
        {
            if (strcmp(current->file_name, argv[i]) == 0)
            {
                is_duplicate = 1;
                break;
            }
            current = current->next;
        }

        if (is_duplicate)
        {
            printf("✗ Duplicate file skipped: %s\n", argv[i]);
            continue;
        }

        /* Try opening file */
        FILE *file_ptr = fopen(argv[i], "r");
        if (file_ptr == NULL)
        {
            printf("✗ File not found or cannot be opened: %s\n", argv[i]);
            continue;
        }

        /* Check for empty file */
        fseek(file_ptr, 0, SEEK_END);
        long file_size = ftell(file_ptr);
        rewind(file_ptr);

        if (file_size == 0)
        {
            printf("✗ Empty file skipped: %s\n", argv[i]);
            fclose(file_ptr);
            continue;
        }

        fclose(file_ptr);

        /* Create new node */
        FileList_t *new_node = (FileList_t *)malloc(sizeof(FileList_t));
        if (new_node == NULL)
        {
            printf("✗ Memory allocation failed for: %s\n", argv[i]);
            continue;
        }

        strncpy(new_node->file_name, argv[i], MAX_FILENAME_LENGTH - 1);
        new_node->file_name[MAX_FILENAME_LENGTH - 1] = '\0';
        new_node->next = NULL;

        /* Add to linked list */
        if (*head == NULL)
        {
            *head = new_node;
        }
        else
        {
            FileList_t *temp = *head;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = new_node;
        }

        printf("✓ File added successfully: %s\n", argv[i]);
        valid_file_count++;
    }

    /* Print final file list */
    if (*head == NULL)
    {
        printf("\n✗ No valid files found.\n");
        printf("============================================\n");
        return FAILURE;
    }

    printf("\n============================================\n");
    printf("           Files Stored in List\n");
    printf("============================================\n");

    FileList_t *temp = *head;
    while (temp != NULL)
    {
        printf("[%s]", temp->file_name);
        if (temp->next != NULL)
        {
            printf(" -> ");
        }
        else
        {
            printf(" -> NULL\n");
        }
        temp = temp->next;
    }

    printf("============================================\n");
    printf("Total valid files: %d\n", valid_file_count);
    printf("============================================\n");

    return SUCCESS;
}