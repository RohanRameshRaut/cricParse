#include <stdio.h>
#include <stdlib.h>

int main()
{
    // This program contains some errors!
    // Study the code and remove the errors.
    //
    // object to construct - a table of
    // given number of rows and colums
    //
    // type for the table
    typedef struct _tableT {
      unsigned char nr, nc; // number of rows and cols
      unsigned char cw, rh; // column width, row height
      unsigned char cc, rc, tc; // colum, row and cross characters e.g. -, |, + but can be different
    }tableT;

    tableT t;
    t.nr = 6;
    t.nc = 4;
    t.cw = 6;
    t.rh = 1;
    t.rc = '-';
    t.cc = '|';
    t.tc = '+';
    // let's create the table components
    // e.g. for this case
    // com1 (t.cw - 1)
    // +----
    // com2
    // |    
    // com3
    // +----+----+----+----+\n
    // com4
    // |    |    |    |    |\n
    //
    // com5
    // +----+----+----+----+\n
    // |    |    |    |    |\n
    //
    // now the table can be constructed as under:
    // +----+----+----+----+\n
    // |    |    |    |    |\n
    // +----+----+----+----+\n
    // |    |    |    |    |\n
    // +----+----+----+----+\n
    // |    |    |    |    |\n
    // +----+----+----+----+\n
    // |    |    |    |    |\n
    // +----+----+----+----+\n
    // |    |    |    |    |\n
    // +----+----+----+----+\n
    // |    |    |    |    |\n
    // +----+----+----+----+\n
    //
    unsigned char *comp[4]; //to hold the 5 components needed to construct the table object
    //let's allocate space needed to hold these components
    comp[0] = malloc (t.cw - 1);
    comp[1] = malloc (t.cw - 1);
    comp[2] = malloc ((t.cw - 1) * t.nc + 2);
    comp[3] = malloc ((t.cw - 1) * t.nc + 2);
    // components comp[3] and comp[2] are sufficient 
    // to create the table so we shall not create comp [4]
    // now finish creating the components
    comp[0][0] = '+';
    comp[1][0] = '|';
    size_t i, j, k;
    for (j = 1; j < t.cw-1; ++j)
       {
        // +----
        // |    
         comp[0][j] = '-';
         comp[1][j] = ' ';
       }

    for (i = 0; i < t.nc; ++i)
    {
      for (j = 0; j < t.cw-1; ++j)
      {
        comp[2][i*(t.cw-1)+j] = comp[0][j];
        comp[3][i*(t.cw-1)+j] = comp[1][j];
        //
        // +----+----+----+----   +\n
        // |    |    |    |       |\n
      }
      comp[2][t.nc*(t.cw-1)] = '+';
      comp[3][t.nc*(t.cw-1)] = '|';
    }
    // now let's print the table
    printf ("\nThe table should look as under:\n");
    for (i = 0; i < t.nr; ++i)
    {
      // comp[2][0], 1, (t.nc*(t.rw-1)+1),
      fwrite((comp[2]), 1, (t.nc*(t.cw-1)+1),stdout);
      printf("\n");
      fwrite((comp[3]), 1, (t.nc*(t.cw-1)+1),stdout);
      printf("\n");
    }
    fwrite((comp[2]+0), 1, (t.nc*(t.cw-1)+1),stdout);
    printf("\n");
    
    return 0;
}

