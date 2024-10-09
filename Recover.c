#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK_SIZE 512

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Ensure correct usage
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    // Open forensic image
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    // Buffer to store a block of 512 bytes
    BYTE buffer[BLOCK_SIZE];

    // File pointer for recovered JPEGs
    FILE *img = NULL;

    // Counter to name the JPEG files
    int file_count = 0;
    char filename[8];  // "###.jpg\0"

    // Boolean to track if we are currently writing a JPEG
    int is_writing = 0;

    // Read blocks of 512 bytes from the forensic image
    while (fread(buffer, BLOCK_SIZE, 1, file) == 1)
    {
        // Check if the block is the start of a new JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // If we are already writing a JPEG, close the current file
            if (is_writing)
            {
                fclose(img);
            }

            // Create a new JPEG file
            sprintf(filename, "%03i.jpg", file_count);
            img = fopen(filename, "w");
            if (img == NULL)
            {
                printf("Could not create output file.\n");
                fclose(file);
                return 1;
            }

            // Increment the file counter
            file_count++;

            // We are now writing a new JPEG
            is_writing = 1;
        }

        // If we are already writing a JPEG, write the current block to the file
        if (is_writing)
        {
            fwrite(buffer, BLOCK_SIZE, 1, img);
        }
    }

    // Close any remaining open files
    if (img != NULL)
    {
        fclose(img);
    }

    fclose(file);

    return 0;
}
