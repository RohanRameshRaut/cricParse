#include <stdio.h>
#include <stdlib.h>

#define ERR (1)

/*
 * A simple C program to help you learn rituals we need to perform
 * on a typical Linux/Unix type system using the C standard library
 * routines for IO
 * Study the program. I shall discuss the code in the next session.
 * Note that we shall allocate memory as per the file size.
 * Later, that is, once you become more expert in C coding,
 * we shall see how to do programming with statically allocated memory.
 *
 * For some additional information about these rituals:
 * You may refer the manpages: e.g.
 * $ man ftell
 *
 * To compile and run this code, do the following:
 * $ gcc obj01-read-file.c -o try1
 * $ ./try1 997961.yaml
 *
 * */

int main(int aa, char **ab)
{
    if (aa != 2)
    {
        fprintf(stderr, "Usage: %s <input file name>\n", ab[0]);
        return 2;
    }

    size_t size; // will store the size in bytes
    FILE *f = fopen(ab[1], "rb"); // open file in the so called binary mode
    if (!f) return ERR; // error opening file

    if (fseek(f, 0, SEEK_END) != 0)
    {
      fclose(f); return ERR; 
    }
    size = ftell(f); //get the length of the input file
    // ftell(f): will give you the position of f
    if (size < 0)
    {
      fclose(f); return ERR;
    }
    if (fseek(f, 0, SEEK_SET) != 0)// set back the pointer to the begining
    {
      fclose(f); return ERR;
    }

    char *buf = malloc(size); // try to allocate sufficient space
    if (!buf) //if allocation fails exit
    {
      fclose(f); return ERR;
    }

    size_t read = fread(buf, 1, size, f); //try to read the entire file in one go
    if (read != size)
    {
        //if could not read the complete file exit
        if (ferror(f))
        {
          free(buf); fclose(f); return ERR;
        }
    }
    fclose(f);
    if (!buf)
    {
        perror("Failed to read file");
        return ERR;
    }

    // Now we shall check the contents of the file in a very simple way
    // Just print file size and the entire contents!
    printf("The file you input has:%zu bytes\n", size);
    printf("\n---------Begin-------\n");
    fwrite(buf, 1, size, stdout); // print all file contents using fwrite to the stdout stream
    //fwrite(source_ptr, size_of_each_bytes_of_each_element, count_of_total_number, where_the_data_will_be_written.
    //similar to fread, will return the count of sucessfully written items
    //used to write a block of binary data from memory to a file stream.
   
    printf("\n---------End---------\n");

    free(buf); //finally free the allocated memory
    return 0;
}

