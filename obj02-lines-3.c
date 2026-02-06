#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ERR (1)

int main(void)
{
    const char *linesfile = "linesobj";

    // object to write - an array of unsigned int
    // Note that our lines object is very similar
    // The object shall be written to a file named linesobj
    //
    unsigned int object[] = {0u, 1u, 2u, 4294967295u}; // let us construct an object manually
    // note that unsigned int is 32 bit wide
    // so the smallest integer that can be stored is 0 and
    // the largest integer that can be stored is 4294967295
    // the suffix u is part of the C rituals.
    //
    size_t cnt = 4;
     

    FILE *outfile = fopen(yamlFile, "wb");
    if (!outfile)
    {
        perror("Error: fopen for write"); // perror may potentially help us locate errors, if any
        // read this code carefully to see how we may use perror more effectively.
        return ERR;
    }

    // Let's first write the size of our object [array] first
    // The size shall be the count of the elements in the array
    if (fwrite(&cnt, sizeof(cnt), 1, outfile) != 1)
    {
        perror("Error: fwrite cnt");
        fclose(outfile);
        return ERR;
    }

    // Write the array now
    if (fwrite(object, sizeof(unsigned int), cnt, outfile) != cnt)
    {
        perror("Error: fwrite object");
        fclose(outfile);
        return ERR;
    }

    fclose(outfile);

    // Let's now read the object from the file
    FILE *infile = fopen(linesfile, "rb");
    if (!infile)
    {
        perror("Error: fopen for read");
        return ERR;
    }

    cnt = 0; //reuse cnt!

    // We shall read the object size
    // i.e. the count of elements in the array first
    if (fread(&cnt, sizeof(cnt), 1, infile) != 1)
    {
        perror("Error: fread cnt");
        fclose(infile);
        return ERR;
    }

    unsigned int *newobj = malloc(cnt * sizeof(unsigned int));
    if (!newobj)
    {
        perror("Error: malloc newobj");
        fclose(infile);
        return ERR;
    }

    // Now we shall read the actual object completely i.e. the array itself
    if (fread(newobj, sizeof(unsigned int), cnt, infile) != cnt)
    {
        perror("Error: fread newobj");
        free(newobj);
        fclose(infile);
        return ERR;
    }

    fclose(infile);

    // Let's check - just print the stuff!
    printf ("\nThe values are as under:\n");
    for (size_t i = 0; i < cnt; ++i)
        printf("lines[%zu] = %u\n", i, newobj[i]);

    // Let's check - print the bytes directly!
    // C generally doesn't allow us to access bytes within an integer
    // directly, but we can do so using the so called `union` type
    // Do not worry too much now if you don't understand what is
    // union type etc
    // I shall explain it duly in the sessions.
    //
    // This example is just to give you an idea of the facilities
    // provided by C as a coding tool.
    typedef union _ubytes
    {
      unsigned int a;
      unsigned char b[4];
    }ubytes;

    printf ("\nThe bytes are as under:\n");
    for (size_t i = 0; i < cnt; ++i)
    {
        ubytes ub;
        ub.a = newobj[i];
        printf("bytes in the %zuth integer are: 0x%02x 0x%02x 0x%02x 0x%02x\n", i, ub.b[0], ub.b[1], ub.b[2], ub.b[3]);
    }

    free(newobj);
    return 0;
}

