#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ERR (1)
#define PLAYERS "players"
#define PLAYERS_LENGTH 7
#define SUFFIX ": "
#define SUFFIX_LENGTH 2
#define DECISION "decision"
#define DECISION_LENGTH 8
#define WINNER "winner"
#define WINNER_LENGTH 6
#define BATSMAN "batsman"
#define BATSMAN_LENGTH 7
#define BOWLER "bowler"
#define BOWLER_LENGTH 6
#define NON_STRIKER "non_striker"
#define NON_STRIKER_LENGTH 10


typedef struct LineT{
	unsigned int start_index;
	unsigned char length, ind, col;
}LineT;

typedef struct ContentT{
	unsigned short cnl;
	unsigned char start;
}ContentT;

typedef struct delT{
	unsigned short start;
	ContentT del_no;
	ContentT del_type;
	ContentT runs;
	ContentT outcome;
	unsigned char player_index;
	unsigned char player_index2;
}delT;

typedef struct overT{
	unsigned short start;
	delT *ds;
	unsigned char player_index;
	unsigned char over_no, ndel;
}overT;

typedef struct _BatsmanT{
	ContentT name;
	unsigned short runs;
	unsigned char balls;
	ContentT out;
}_BatsmanT;

typedef struct _bowlerT{
	ContentT name;
	unsigned char runs;
	unsigned char overs;
	ContentT wickets;
}_bowlerT;

typedef struct Players{
	unsigned char index;
	ContentT name;
	struct Players *next;
}Players;

typedef struct InningT{
	overT *overs;
	unsigned char novers;
	unsigned short start, end;
}InningT;

typedef struct Toss{
	ContentT decision;
	ContentT winner;
}Toss;

typedef struct Teams{
	ContentT name;
	_BatsmanT *bat;
	_bowlerT *blr;
	Players *plr;
	unsigned char nbat, nblr;
}Teams;


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
	while (cnt < size)
	{	

		if(buf[cnt] == '\n') nl++;
		++cnt;
	}
	LineT *line = malloc((nl) * (sizeof(LineT))); // try to allocate sufficient space for the lines sequence
	if (!line) //if allocation fails exit
	{
		fclose(f); return ERR;
	}
	cnt = 0; // we shall use the cnt again for our computation
	unsigned short lncnt = 0; // why lnsize must be set to 1?? //lncnt is the real nl(number of a line).
	line[0].length = 0;
	unsigned long sizem=1;
	line[0].start_index = 0;
	line[0].ind = 0, line[0].col = 0;
	unsigned char flag = 1;
	while (cnt < size-1)
	{
		if(buf[cnt] == '\n')
		{
			line[lncnt].length = sizem-1;
			//			printf("Line %d: starting_index: %d size %d: \n", lncnt, line[lncnt].start_index, line[lncnt].length);
			lncnt += 1;
			line[lncnt].col = 0;
			if(buf[cnt-1] == ':'){
				line[lncnt].col = 1;
			}
			line[lncnt].ind = 0, flag = 1;
			sizem = 0;
			line[lncnt].start_index = cnt+1;
		}
		else{
			if(('A' <= buf[cnt] && buf[cnt] <= 'Z') || ('a' <= buf[cnt] && buf[cnt] <= 'z') || ('0' <= buf[cnt] && buf[cnt] <= '9')){
				flag = 0;
			}
			else if(flag==1 && (buf[cnt] == ' ' || buf[cnt] == '-')){
				line[lncnt].ind += 1;
			}
		}
		sizem = sizem + 1;
		cnt +=1;
	}
	line[nl-1].length = sizem-1;
	//	printf("Line %d: starting_index: %d size %d: \n", lncnt, line[lncnt].start_index, line[nl-lncnt].length);

	//--------------------------------------------------------------------------------------------------------------------
	// 	indentation validation for line after the ':'
	lncnt = 0;
	while(lncnt < nl){
		if(line[lncnt].col){
			if(line[lncnt].ind+2 != line[lncnt+1].ind){
				return ERR;
			}
		}
		lncnt += 1;
	}
	Teams teams[2];
	lncnt = 0; //reusing the lncnt
	while(lncnt < 50){
			for(j=0;j<line[lncnt].length-1;j++){
				if((buf[line[lncnt].start_index+line[lncnt].ind+j]) != PLAYERS[j]){
					break;
				}
			}
			if(j == PLAYERS_LENGTH){
				for(int i=0;i<2;i++){
					j = 0;
					teams[i].name.cnl = lncnt + 1;
					teams[i].name.start = line[lncnt+1].start_index+line[lncnt+1].ind;
					do{
						if(buf[line[lncnt].ind-2] == '-'){
							struct Players *head = NULL, newPlayer = NULL;
							static struct Players *temp = NULL;
							newPlayer = (struct Players*) malloc (sizeof(struct Players));
							newPlayer->index = j;
							newPlayer->name.cnl = lncnt;
							newPlayer->name.start = line[lncnt].start_index+line[lncnt].ind;
							newPlayer->next = NULL;

							if(head == NULL){
								head = newPlayer;
								temp = newPlayer;
								teams[i].plr = newPlayer;
							}
							else{
								temp->next = newPlayer;
								temp = newPlayer;
							}
						}
						lncnt += 1;
					}while(line[lncnt].ind != 2);
				}
			}
	//	________________________________________________________________________________________________________________________________
	//	c=0, i=0, cnt= 0;
	unsigned char it = buf[line[Toss.decision.cnl].start_index+Toss.decision.start];
	unsigned char iit = buf[line[Toss.winner.cnl].start_index+Toss.winner.start];
	//		if(it == 'b' && it+1 == 'a' && it+2 == 't')&&
	//	  (iit  //use the if-else flags to trigger the value of teams[i], i=0 or i=1)
	for(int j=0;j<line[lncnt].length-1;j++){
		if((buf[line[lncnt].start_index+j]) != sub_str[j]){
			break;
		}
	}
	if(j == len){

	}

	//	lncnt = inning[0].start; //reusing the lncnt
	//	while(lncnt < nl){
	//		if(((buf[line[lncnt].start_index+line[lncnt].length-2]) == '1')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
	//			inning[i].overs[cnt].start = lncnt;
	//			inning[i].overs[cnt].ndel = 0;
	//			inning[i].overs[cnt].over_no = cnt+1, c=0;
	//			cnt = cnt + 1;
	//			while( c < 11){
	//				for(j=0;j<line[lncnt+2].length-17;j++){
	//					if(buf[line[inning[i].plr[c].name.cnl].start_index+inning[i].plr[c].name.start+j] != (buf[line[lncnt+2].start_index+16+j])){
	//						break;
	//					}
	//				}
	////				printf("j: %d, line[lncnt+2].length-16: %d\n", j, line[lncnt+2].length-17);
	//				if(j == line[lncnt+2].length-17){
	//					inning[i].overs[cnt-1].player_index = inning[i].plr[c].index;
	////					printf("bowler player_index: %d\n", inning[i].overs[cnt-1].player_index);
	//				}
	//				c += 1;
	//			}
	//		}
	//		if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '.')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
	//			inning[i].overs[cnt-1].ndel++;
	//		}
	//		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
	//			i = 1, cnt = 0, c=0;
	//		}
	//		lncnt++;
	//	}
	//	for(i=0;i<2;i++){
	//		for(j=0;j<inning[i].novers;j++){
	//			//			printf("inning[%d].overs[%d].ndel: %d\n", i, j, inning[i].overs[j].ndel );
	//			delT *ds = malloc(inning[i].overs[j].ndel * (sizeof(delT))); // try to allocate sufficient space for the lines sequence
	//			inning[i].overs[j].ds = ds ;
	//		}
	//	}
	//	//______________________________________________________________________________________________________________________
	//	//			for(i=0; i<2; i++){
	//	//				for(cnt=0; cnt < inning[i].novers; cnt++){
	//	//					fwrite((buf+line[inning[i].overs[cnt].bowler_name.cnl].start_index+inning[i].overs[cnt].bowler_name.start), 1, line[inning[i].overs[cnt].bowler_name.cnl].length-inning[i].overs[cnt].bowler_name.start, stdout);
	//	//					printf("\n");
	//	//				}
	//	//			}
	//	c=0, i=0, cnt=0;
	//	unsigned char cnt0=0;
	//	unsigned char nsub_str[] = "        non_striker:";
	//	unsigned char osub_str[] = "        extras:", olen=14;
	//	unsigned char rsub_str[] = "        runs:", rlen=12;
	//	unsigned char wsub_str[] = "          player_out:", wlen=21;
	//	len = 20;
	//	lncnt = inning[0].start; //reusing the lncnt
	//	inning[0].nbat = 2;
	//	while(lncnt < nl){
	//		if(((buf[line[lncnt].start_index+line[lncnt].length-2]) == '1')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
	//			cnt++, cnt0 = 0, c=0;
	//		}
	//		if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '.')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
	//			inning[i].overs[cnt-1].ds[cnt0].del_no.cnl = lncnt, inning[i].overs[cnt-1].ds[cnt0].del_no.start = 6;
	//			inning[i].overs[cnt-1].ds[cnt0].start = lncnt, c = 0, cnt0++;
	//			//	inning[i].overs[cnt-1].ds[cnt0].striker.cnl = lncnt + 1, inning[i].overs[cnt-1].ds[cnt0].striker.start = 17;
	//			while( c < 11){
	//				for(j=0;j<line[lncnt+1].length-18;j++){
	//					if(buf[line[inning[i].plr[c].name.cnl].start_index+inning[i].plr[c].name.start+j] != (buf[line[lncnt+1].start_index+17+j])){
	//						break;
	//					}
	//				}
	//				printf("j: %d, line[lncnt+1].length-18: %d\n", j, line[lncnt+1].length-18);
	//				if(j == line[lncnt+1].length-18){
	//					inning[i].overs[cnt-1].ds[cnt0-1].player_index = inning[i].plr[c].index;
	//					printf("striker player_index: %d\n", inning[i].overs[cnt-1].ds[cnt0-1].player_index);
	//				}
	//				c += 1;
	//			}
	//		}
	//		for(j=0;j<line[lncnt].length-1;j++){
	//			if((buf[line[lncnt].start_index+j]) != nsub_str[j]){
	//				break;
	//			}
	//		}
	//		if(j == len){
	//			c = 0;
	//			while( c < 11){
	//				for(j=0;j<line[lncnt].length-21;j++){
	//					if(buf[line[inning[i].plr[c].name.cnl].start_index+inning[i].plr[c].name.start+j] != (buf[line[lncnt].start_index+len+1+j])){
	//						break;
	//					}
	//				}
	//				if(j == line[lncnt].length-21){
	//					inning[i].overs[cnt-1].ds[cnt0-1].player_index2 = inning[i].plr[c].index;
	//					printf("non_striker player_index2: %d\n", inning[i].overs[cnt-1].ds[cnt0-1].player_index2);
	//				}
	//				c += 1;
	//			}
	//		}
	//
	//		for(j=0;j<line[lncnt].length-1;j++){
	//			if((buf[line[lncnt].start_index+j]) != osub_str[j]){
	//				break;
	//			}
	//		}
	//		if(j == olen){
	//			inning[i].overs[cnt-1].ds[cnt0-1].del_type.cnl = lncnt;
	//			inning[i].overs[cnt-1].ds[cnt0-1].del_type.start = olen+1;
	//			//			printf("inning[%d].overs[%ld].ds[%d].del_type.cnl: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].del_type.cnl);
	//		}
	//		for(j=0;j<line[lncnt].length-1;j++){
	//			if((buf[line[lncnt].start_index+j]) != wsub_str[j]){
	//				break;
	//			}
	//		}
	//		if(j == wlen){
	//			inning[i].overs[cnt-1].ds[cnt0-1].outcome.cnl = lncnt;
	//			inning[i].overs[cnt-1].ds[cnt0-1].outcome.start = wlen+1;
	//			//getting the total number of _Batsman
	//			if((cnt0-1) < (inning[i].overs[cnt-1].ndel)){
	//				inning[i].nbat++;
	//			}
	//			//			printf("inning[%d].overs[%ld].ds[%d].outcome.cnl: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].outcome.cnl);
	//			//			printf("inning[%d].overs[%ld].ds[%d].outcome.start: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].outcome.start);
	//		//	fwrite((buf+line[inning[i].overs[cnt-1].ds[cnt0-1].outcome.cnl].start_index+inning[i].overs[cnt-1].ds[cnt0-1].outcome.start), 1, line[inning[i].overs[cnt-1].ds[cnt0-1].outcome.cnl].length-inning[i].overs[cnt-1].ds[cnt0-1].outcome.start, stdout);
	//			//printf("inning [%d]: \n", i);
	//		}
	//		for(j=0;j<line[lncnt].length-1;j++){
	//			if((buf[line[lncnt].start_index+j]) != rsub_str[j]){
	//				break;
	//			}
	//		}
	//		if(j == rlen){
	//			inning[i].overs[cnt-1].ds[cnt0-1].runs.cnl = lncnt;
	//			inning[i].overs[cnt-1].ds[cnt0-1].runs.start = rlen+1;
	//			//	printf("inning[%d].overs[%ld].ds[%d].runs.cnl: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].runs.cnl);
	//		}
	//		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
	//			i = 1, cnt = 0, inning[i].nbat = 2, c=0;
	//		}
	//		lncnt++;
	//	}
	//	printf("inning 1 .nbat: %d\n", inning[0].nbat);
	//	printf("inning 2 .nbat: %d\n", inning[1].nbat);
	//	_BatsmanT *b1 = malloc(inning[0].nbat * (sizeof(_BatsmanT)));
	//	inning[0].bat = b1;
	//	_BatsmanT *b2 = malloc(inning[1].nbat * (sizeof(_BatsmanT)));
	//	inning[1].bat = b2;
	//	printf("inning[0].overs[0].ds[2].non_striker.cnl: %d\n", inning[0].overs[0].ds[0].non_striker.cnl);
	//	for(i=0; i<1; i++){
	//		for(cnt=0; cnt < inning[i].novers; cnt++){
	//			for(j=0; j<inning[i].overs[cnt].ndel; j++){
	//						fwrite((buf+line[inning[i].overs[cnt].ds[j].striker.cnl].start_index+inning[i].overs[cnt].ds[j].striker.start), 1, line[inning[i].overs[cnt].ds[j].striker.cnl].length-inning[i].overs[cnt].ds[j].striker.start, stdout);
	//						printf("\n");
	//						fwrite((buf+line[inning[i].overs[cnt].ds[j].non_striker.cnl].start_index+inning[i].overs[cnt].ds[j].non_striker.start), 1, line[inning[i].overs[cnt].ds[j].non_striker.cnl].length-inning[i].overs[cnt].ds[j].non_striker.start, stdout);
	//						printf("\n");
	//			}
	//		}
	//	}

	free(buf);
	free(line);

	return 0;
}

