#include <stdio.h>
#include <stdlib.h>

typedef struct _tableT {
	unsigned char nr, nc, cnr, cnc; // number of rows and cols , c:content
	unsigned char cc, rc, tc; // colum, row and cross characters e.g. -, |, + but can be different
}tableT;

int main(int argc, char **argv)
{
	if(argc<3){
		printf("err argc\n");
		return 2;
	}
	tableT t;
	t.nr = atoi(argv[1]);
	t.cnr = t.nr+2;
	t.nc = atoi(argv[2]);
	t.cnc = t.nc+2;
	t.rc = '-';
	t.cc = '|';
	t.tc = '+';
	size_t i, j, k, size=0;
	size = (t.cnr*t.cnc);
	unsigned char *comp = malloc (size*(sizeof(unsigned char))); //allocate memory for all 50 rows
	comp[0] = t.tc;//+
	comp[t.cnc-1] = t.tc;//+
	comp[size-t.cnc] = t.tc;//+
	comp[size-1] = t.tc;//+

	for(i=1;i<t.cnc-1;i++){
		comp[i] = t.rc;//'-'
		comp[(size-t.cnc)+i] = t.rc;
	}

	for(i=t.cnc;i<(size-t.cnc);i+=t.cnc){
		comp[i] = t.cc;
		for(j=1;j<t.cnc-1;j++){
			comp[i+j] = ' ';
		}
		comp[i+j] = t.cc;
	}
	// now let's print the table
	printf ("\nThe table should look as under:\n");
	printf("comp[size-t.cnc], index: %ld\n", size-t.cnc);
	printf("size-1, index: %ld\n", size-1);
	fwrite((comp), 1, t.cnc,stdout);
	printf("\n");
	for (i = t.cnc; i < (size-t.cnc); i+= t.cnc)
	{
		fwrite((comp+i), 1, t.cnc,stdout);
		printf("\n");
	}
	fwrite((comp+(size-t.cnc)), 1, t.cnc,stdout);
	printf("\n");

	return 0;
}

