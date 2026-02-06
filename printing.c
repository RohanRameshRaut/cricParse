#include <stdio.h>
#include <stdlib.h>

#define ERR (1)

/*
 * Here we shall construct the lines object as discussed in the class.
 * Exercise: try constructing some other objects in a similar manner.
 * */

int main(int aa, char **ab)
{
    if (aa != 2)
    {
        fprintf(stderr, "Usage: %s <input file name>\n", ab[0]);
        return 2;
    }

    size_t size;
    FILE *f = fopen(ab[1], "rb"); // open file in the so called binary mode
    if (!f) return ERR; // error opening file

    if (fseek(f, 0, SEEK_END) != 0)
    {
      fclose(f); return ERR; 
    }
    size = ftell(f); //get the length of the input file
    if (size < 0)
    {
      fclose(f); return ERR;
    }
    if (fseek(f, 0, SEEK_SET) != 0)
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

    // Here we shall construct the lines object as discussed in the class.
    // first we shall find the number of lines present in the input
    // file.

    unsigned long nl = 0, cnt = 0;
    while (cnt <= size)
    {
      if(buf[cnt] == '\n') nl++;
      ++cnt;
    }

    printf("Total number of lines is: %ld\n", nl);
    unsigned long *lines = malloc(nl * (sizeof(unsigned long))); // try to allocate sufficient space for the lines sequence

    if (!lines) //if allocation fails exit
    {
      fclose(f); return ERR;
    }

    // Now that we have got sufficient memory allocated,
    // we shall continue the construction of the lines object
    // as discussed in the class.

    cnt = 0; // we shall use the cnt again for our computation
    unsigned long lnsize = 1, lncnt = 0; // why lnsize must be set to 1??
    while (cnt <= size)
    {
      if(buf[cnt] == '\n')
      {
         lines[lncnt] = lnsize-1;
         lncnt++;
         lnsize = 0;
      }
      ++lnsize;
      ++cnt;
    }

    // Now we should have the lines object constructed properly!
    // we shall check the lines object
    // Just print some lines!
    // Note: such checks are mostly useless and very less reliable
    // but these are okay in our first few babysteps!
    //
    printf("The first 10 lines from your input file are:\n");
    cnt = 0; // we shall use the cnt again for our computation
    unsigned long ptr=0;
    printf("lines[cnt]: %ld\n", lines[1]);
    printf("(buf+ptr): %ld\n", *buf+ptr);
    printf("\n---------Begin-------\n");
    while (cnt < 10)
    {
       printf("Line %ld: ", (cnt));
       fwrite((buf+ptr), 1, lines[cnt], stdout);
       ptr = ptr + lines[cnt]+1;
       ++cnt;
       printf("\n"); //print a newline on our own!
    }
    printf("\n---------End---------\n");

    free(lines); //finally free the allocated memory
    free(buf);
    return 0;
}

