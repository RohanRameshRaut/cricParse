#include <stdio.h>
#include <stdlib.h>
#define ERR (1)

typedef struct LineT{
	unsigned int start_index;
	unsigned char length;
}LineT;

typedef struct ContentT{
	unsigned short cnl;
	unsigned char start;
}ContentT;

typedef struct Players{
	ContentT t1_players;
	ContentT t2_players;
}Players;

typedef struct Metadata{ //match summary
	ContentT team1;
	ContentT team2;
}Metadata;

typedef struct delT{
	unsigned short start;
	ContentT del_no;
	ContentT del_type;
	ContentT runs;
	ContentT outcome;
	ContentT striker;
	ContentT non_striker;
}delT;

typedef struct overT{
	unsigned short start;
	delT *ds;
	ContentT bowler_name;
	unsigned char over_no, ndel;
}overT;

typedef struct InningT{
	overT *overs;
	unsigned char novers;
	unsigned short start, end;
}InningT;

typedef struct Match_Innings{
	InningT in1;
	InningT in2;
}Match_Innings;

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
		if (ferror(f)) //if could not read the complete file exit
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

	unsigned long nl = 0, cnt = 0;
	while (cnt <= size)
	{
		if(buf[cnt] == '\n') nl++;
		++cnt;
	}
	LineT *line = malloc(nl * (sizeof(*line))); // try to allocate sufficient space for the lines sequence
	if (!line) //if allocation fails exit
	{
		fclose(f); return ERR;
	}
	cnt = 0; // we shall use the cnt again for our computation
	unsigned short lncnt = 0; // why lnsize must be set to 1?? //lnct is the real nl(number of a line).
	line[0].length = 0;
	line[0].start_index = 0;
	while (cnt <= size)
	{
		if(buf[cnt] == '\n')
		{
			lncnt++;
			line[lncnt].start_index = cnt+1;
			line[lncnt].length = 0;
		}
		++line[lncnt].length;
		++cnt;
	}
	Metadata teams;
	Players p1;
	unsigned char sub_str[] = "  players:", len = 10, j=0, c=0, i=0;
	InningT inning[2];
	inning[0].novers = 0;
	lncnt = 0; //reusing the lncnt
	while(lncnt < nl){
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != sub_str[j]){
				break;
			}
		}
		if(j == len){
			teams.team1.cnl = lncnt + 1, teams.team1.start = 4, teams.team2.cnl = lncnt + 13, teams.team2.start = 4;
			p1.t1_players.cnl = lncnt + 2, p1.t1_players.start = 6, p1.t2_players.cnl = lncnt + 14, p1.t2_players.start = 6;
		}
		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '1'){
			inning[0].start = lncnt;
		}
		if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '1')&&((buf[line[lncnt].start_index+line[lncnt].length-2]) == ':')){
			inning[i].novers++;
		}
		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
			inning[1].start = lncnt, inning[0].end = lncnt-1, inning[1].end = nl-1, i = 1, inning[i].novers = 0;
		}
		lncnt++;
	}
	overT *o1 = malloc(inning[0].novers * (sizeof(*o1))); // try to allocate sufficient space for the lines sequence
	inning[0].overs = o1;
	overT *o2 = malloc(inning[1].novers * (sizeof(*o2))); // try to allocate sufficient space for the lines sequence
	inning[1].overs = o2;
	//	________________________________________________________________________________________________________________________________
	c=0, i=0, cnt=0;
	inning[0].overs[0].start= 0;
	lncnt = inning[0].start; //reusing the lncnt
	while(lncnt < nl){
		if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '1')&&((buf[line[lncnt].start_index+line[lncnt].length-2]) == ':')){
			inning[i].overs[cnt].start = lncnt, inning[i].overs[cnt].over_no = cnt+1, cnt++;
			inning[i].overs[cnt].ndel = 0;
		}
		if(((buf[line[lncnt].start_index+line[lncnt].length-4]) != '.')&&((buf[line[lncnt].start_index+line[lncnt].length-2]) == ':')){
			printf("I am in\n");
			inning[i].overs[cnt].ndel++;
		}
		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
			i = 1, cnt = 0;
		}
		lncnt++;
	}
	printf("Inning 1 over 1 start: %d\n", inning[0].overs[0].start);
//	printf("Inning 2 over 1 start: %d\n", inning[1].overs[0].start);
//	printf("Inning 1 over 3: %d\n", inning[0].overs[2].over_no);
//	printf("Inning 2 over 3: %d\n", inning[1].overs[2].over_no);
	printf("Number of deliveries in Inning 1 over 4: %d\n", inning[0].overs[3].ndel);
	printf("Number of deliveries in Inning 2 over 1: %d\n", inning[1].overs[0].ndel);

	return 0;
}

