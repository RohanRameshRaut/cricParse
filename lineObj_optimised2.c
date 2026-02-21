#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ERR (1)
#define EMPTY "SEQUENCE_IS_EMPTY"
#define MAXB 1000000 //max bytes allowed
#define MINB 27000 //max bytes allowed
#define MINL 150 //min lines
#define MAXL 25000 //max lines
#define MINLL 3 //min line length
#define MAXLL 75 //max line length
#define IND_MIN 0 //min indentation allowed
#define IND_MAX 12 //max indentation allowed
#define SUFFIX 2 // suffix size after the key ': '
#define MINLENGTH(a, b) ((a) < (b) ? (a): (b))
#define MAXLENGTH(a, b) ((a) > (b) ? (a): (b))

//validation objects
typedef struct sbytesV{
	unsigned long max;//1000000
	unsigned short min;
	unsigned char *wb;// wanted bytes array
}sbytesV;

typedef struct slinesV{
	unsigned char ind_min, ind_max, len_min, len_max, min_l;
	unsigned short max_l;
	unsigned char *sflag, *eflag, *lflag, *siflag; //e=:
}slinesV;

//main objects
typedef struct LineT{
	unsigned int start_index;
	unsigned char length, ind, col;
}LineT;

typedef struct ContentT{
	unsigned short cnl;
	unsigned char start;
}ContentT;

//delT object validation
typedef struct delTV{
	unsigned char *del_upflag, *del_pcflag;
}delTV;

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
//Players object validation
typedef struct PlayersV{
	unsigned char *upflag, *pcflag, *plr_eflag;
}PlayersV;

typedef struct Players{
	unsigned char index;
	ContentT name;
}Players;

//Inning object validation
typedef struct InningV{
	unsigned char *inning_eflag, *inning_sflag, *inning_bflag;//e:empty, s:starting/ending, b:bowler, l:lines, o:overs
	unsigned char *inning_lflag, *inning_oflag;
}InningV;

typedef struct InningT{
	overT *overs;
	_BatsmanT *bat;
	_bowlerT *blr;
	unsigned char nbat, nblr;
	unsigned char novers;
	unsigned short start, end;
}InningT;

//toss object validation
typedef struct TossV{
	unsigned char *toss_dflag;
	unsigned char *toss_wflag;
}TossV;

typedef struct Toss{
	ContentT decision;
	ContentT winner;
}Toss;

//teams object validation
typedef struct TeamsV{
	unsigned char *uflag;
	unsigned char *teams_eflag;
}TeamsV;

typedef struct Teams{
	ContentT name;
	Players *plr;
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
	sbytesV b;
	b.min = MINB, b.max = MAXB;
	if (size < b.min || b.max < size)
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
	unsigned char wnb[] = {' ',':','-','.','\'','\n','_', '(', ')', ','}, i=0, j=0;
	b.wb = wnb;
	while (cnt < size)
	{	
		for(i=0;i<10;i++){ //10 is the size of wnb array
			if(buf[cnt] == b.wb[i]){
				break;
			}
		}
		if((i==10) && !(('A'<=buf[cnt] && buf[cnt]<='Z') || ('a'<=buf[cnt] && buf[cnt]<='z') || ('0'<=buf[cnt] && buf[cnt]<='9'))){
			printf("invalid char: %c\n", buf[cnt]);
		}
		if(buf[cnt] == '\n') nl++;
		++cnt;
	}
	LineT *line = malloc((nl) * (sizeof(LineT))); // try to allocate sufficient space for the lines sequence
	slinesV lsV; //slinesV object 
	TeamsV tsV;
	PlayersV psV;
	TossV ttsV;
	InningV inV;
	lsV.min_l = MINL, lsV.max_l = MAXL; //min, max lines validaton
	if(!(lsV.min_l <= nl && nl <= lsV.max_l)) return ERR;
	lsV.sflag = (char*) calloc(nl, (sizeof(char))); //flags memory allocation for all lines
	lsV.eflag = (char*) calloc(nl, (sizeof(char)));
	lsV.lflag = (char*) calloc(nl, (sizeof(char)));
	lsV.siflag = (char*) calloc(nl, (sizeof(char)));
	tsV.uflag = (char*) calloc(nl, (sizeof(char)));
	tsV.teams_eflag = (char*) calloc(nl, (sizeof(char)));
	psV.upflag = (char*) calloc(nl, (sizeof(char)));
	psV.pcflag = (char*) calloc(nl, (sizeof(char)));
	psV.plr_eflag = (char*) calloc(nl, (sizeof(char)));
	ttsV.toss_dflag = (char*) calloc(nl, (sizeof(char)));
	ttsV.toss_wflag = (char*) calloc(nl, (sizeof(char)));
	inV.inning_eflag = (char*) calloc(nl, (sizeof(char)));
	inV.inning_lflag = (char*) calloc(nl, (sizeof(char)));
	inV.inning_sflag = (char*) calloc(nl, (sizeof(char)));
	inV.inning_oflag = (char*) calloc(nl, (sizeof(char)));
	inV.inning_bflag = (char*) calloc(nl, (sizeof(char)));
	if (!line || !lsV.sflag || !lsV.eflag || !lsV.lflag || !lsV.siflag) //if allocation fails exit
	{
		fclose(f); return ERR;
	}
	cnt = 0; // we shall use the cnt again for our computation
	unsigned short lncnt = 0;
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
			//				printf("Line %d: starting_index: %d size %d: \n", lncnt, line[lncnt].start_index, line[lncnt].length);
			line[lncnt].col = 0;
			if(buf[cnt-1] == ':'){ //check for the colon at the end and set the flag 1 else 0
				line[lncnt].col = 1;
			}
			if(!((buf[cnt-1] == ':' || buf[cnt-1] == '\'' || buf[cnt-1] == '-')||('a' <= buf[cnt-1] && buf[cnt-1] <= 'z')||('0' <= buf[cnt-1] && buf[cnt-1] <= '9')||('A'<=buf[cnt-1]&&buf[cnt-1]<='Z'))){
				lsV.eflag[lncnt] = 1;// line ending validation
				printf("error at line: %d\n", lncnt);
				printf("cnt-1: %c\n", buf[cnt-1]);
			} else{lsV.eflag[lncnt] = 0;}
			lncnt += 1;
			line[lncnt].ind = 0, flag = 1;
			sizem = 0;
			line[lncnt].start_index = cnt+1;
		}
		else{
			if(('A' <= buf[cnt] && buf[cnt] <= 'Z') || ('a' <= buf[cnt] && buf[cnt] <= 'z') || ('0' <= buf[cnt] && buf[cnt] <= '9')){
				flag = 0;
			}
			else if(flag==1 && (buf[cnt] == ' ' || buf[cnt] == '-')){
				line[lncnt].ind += 1; // counting the indentation
			}
		}
		sizem = sizem + 1;
		cnt +=1;
	}
	line[nl-1].length = sizem-1;
	//		printf("Line %d: starting_index: %d size %d: \n", lncnt, line[nl-1].start_index, line[nl-1].length);

	//--------------------------------------------------------------------------------------------------------------------
	// ind_max, ind_min validation

	// 	indentation validation for line after the ':'
	lncnt = 0;
	lsV.len_min = MINLL, lsV.len_max = MAXLL;
	lsV.ind_min = IND_MIN, lsV.ind_max = IND_MAX;
	flag = 1;
	Players *p[2];
	Teams teams[2];
	unsigned char sub_str[] = "    - ", len = 6, c=0, flag2=0, flag3 = 1, flag4 = 0, flag5 = 1, NP=0, NPP=0, reg_sub_str[] = "  registry", reg_len = 10, team_sub_str[] = "  - ", team_len = 4;
	while(lncnt < nl){
		if(flag){
			for(j=0;j<line[lncnt].length-1;j++){
				if((buf[line[lncnt].start_index+j]) != sub_str[j]){
					break;
				}
			}
			if(j == len){
				NP += 1;	
			}
			if(1 <= NP && NP < 11 && (j != len)){
				psV.pcflag[lncnt] = 1;
				printf("player count err at: %d\n", lncnt);
				return ERR;
				NP = 0;
			}
			if(11 <= NP && (j != len)){//reset the NP for next team
				NPP = NP;	
				p[c] = malloc(NPP * (sizeof(Players)));
				NP = 0, c=1;
			}
		}
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != reg_sub_str[j]){
				break;
			}
		}
		if(j == reg_len){
			flag = 0;
			flag2 = 1;
		}
		if(flag2){
			for(j=0;j<line[lncnt].length-1;j++){
				if((buf[line[lncnt].start_index+j]) != team_sub_str[j]){
					break;
				}
			}
			if(j == team_len){
				teams[0].name.cnl = lncnt, teams[0].name.start = line[lncnt].ind, teams[1].name.cnl = lncnt+1, teams[1].name.start = line[lncnt].ind;
				if(! (0 < (line[teams[0].name.cnl].length-teams[0].name.start) && 0 < (line[teams[1].name.cnl].length-teams[1].name.start))){ //checking for the input size of decision
					tsV.teams_eflag[teams[0].name.cnl] = 1;
					printf("err at team name: %d\n", teams[0].name.cnl);
					return ERR;
				}
				//	printf("line[teams[c].name.cnl]: %d\n", line[teams[0].name.cnl].length-teams[0].name.start);
				//	printf("line[teams[c].name.cnl]: %d\n", line[teams[1].name.cnl].length-teams[1].name.start);
				//	fwrite(buf+line[teams[0].name.cnl].start_index+teams[0].name.start, 1, line[teams[0].name.cnl].length-teams[0].name.start, stdout); printf("\n");
				//	fwrite(buf+line[teams[1].name.cnl].start_index+teams[0].name.start, 1, line[teams[1].name.cnl].length-teams[1].name.start, stdout);
				flag2 = 0;
			}
		}
		if(!((buf[line[lncnt].start_index] == ' ')||(buf[line[lncnt].start_index] == '-')||('a'<= buf[line[lncnt].start_index] && buf[line[lncnt].start_index]<='z'))){
			lsV.sflag[lncnt] = 1;
			printf("error at starting char: %c\n", buf[line[lncnt].start_index]);
		}
		if(!(lsV.len_min <= line[lncnt].length && line[lncnt].length <= lsV.len_max)){
			printf("line length count error at line: %d\n", lncnt);
		}
		if(!(lsV.ind_min <= line[lncnt].ind && line[lncnt].ind <= lsV.ind_max)){
			printf("indentation count error at line: %d\n", lncnt);
		}
		if(line[lncnt].col == 1){
			if(line[lncnt].ind+SUFFIX != line[lncnt+1].ind){
				lsV.siflag[lncnt] = 1;
				printf("ind err: %d\n", lncnt);
				return ERR;
			}
		}
		lncnt += 1;
	}
	Toss Toss;
	flag = 1, c=0, flag2=0;
	unsigned char tsub_str[] = "    decision:", tlen=13, counter = 0;
	InningT inning[2];
	teams[0].plr = p[0];
	teams[1].plr = p[1];
	inning[0].novers = 0;
	inning[1].novers = 0;
	lncnt = 0; //reusing the lncnt
	while(lncnt < nl){
		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')){
			counter += 1;//validating the start of inning
		}
		if(flag){
			for(j=0;j<line[lncnt].length-1;j++){
				if((buf[line[lncnt].start_index+j]) != sub_str[j]){
					break;
				}
			}
			if(j == len){
				//team name validation
				for(j=0; j<(MINLENGTH((line[teams[c].name.cnl].length-teams[c].name.start),(line[teams[c+1].name.cnl].length-teams[c+1].name.start)));j++){
					if(buf[line[teams[c].name.cnl].start_index+teams[c].name.start+j] != buf[line[teams[c+1].name.cnl].start_index+teams[c+1].name.start+j]){
						break;
					}
				} if(j==MINLENGTH((line[teams[c].name.cnl].length-teams[c].name.start),(line[teams[c+1].name.cnl].length-teams[c+1].name.start))){
					tsV.uflag[teams[c].name.cnl] = 1;
					printf("team names are same at: %d\n", teams[c].name.cnl);
				}
				for(j=0; j<NPP; j++){
					teams[c].plr[j].name.cnl = lncnt+j, teams[c].plr[j].name.start = line[lncnt+j].ind, teams[c].plr[j].index = j;
				}
				for(j=0; j<NPP; j++){
					teams[c+1].plr[j].name.cnl = lncnt+NPP+1+j, teams[c+1].plr[j].name.start = line[lncnt+j].ind, teams[c+1].plr[j].index = j;
				}
				for(c=0;c<2;c++){//uniqe player validation
					for(i=0; i<NPP-1; i++){
						if (!(0 < line[teams[c].plr[i].name.cnl].length-teams[c].plr[i].name.start)){
							psV.plr_eflag[teams[c].plr[i].name.cnl] = 1;
							printf("player is empty: %d\n", teams[c].plr[i].name.cnl);
						}
						for(j=0; j<(MINLENGTH((line[teams[c].plr[i].name.cnl].length-teams[c].plr[i].name.start),(line[teams[c].plr[i+1].name.cnl].length-teams[c].plr[i+1].name.start)));j++){
							if(buf[line[teams[c].plr[i].name.cnl].start_index+teams[c].plr[i].name.start+j] != buf[line[teams[c].plr[i+1].name.cnl].start_index+teams[c].plr[i+1].name.start+j]){
								break;
							}
						} if(j==MINLENGTH((line[teams[c].plr[i].name.cnl].length-teams[c].plr[i].name.start),(line[teams[c].plr[i+1].name.cnl].length-teams[c].plr[i].name.start))){
							psV.upflag[teams[c].plr[i].name.cnl] = 1;
							printf("player names are same");
						}
					}
				}
				flag = 0;
			}
		}
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != tsub_str[j]){
				break;
			}
		}
		if(j == tlen){
			Toss.decision.cnl = lncnt, Toss.decision.start = tlen+1;
			Toss.winner.cnl = lncnt+1, Toss.winner.start = tlen-1;
		}
		if(flag5){
			if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')){
				inning[0].start = lncnt;
				inning[1].start = lncnt+2;
				flag2 = 1;
				flag5 = 0;
			}
		}
		if(flag2){
			for(j=0;j<line[lncnt].length-1;j++){
				if((buf[line[lncnt].start_index+j]) != sub_str[j]){
					break;
				}
			}
			if(j == len){
				if(((buf[line[lncnt].start_index + line[lncnt].length -3]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -1]) == ':')){
					inning[0].novers++;
				}

			}
		}
		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ') && inning[1].start < lncnt){
			inning[1].start = lncnt;
			inning[0].end = lncnt-1;
			inning[1].end = nl-1;
			flag4 = 1;
			flag2 = 0;
		}
		if(flag4){
			for(j=0;j<line[lncnt].length-1;j++){
				if((buf[line[lncnt].start_index+j]) != sub_str[j]){
					break;
				}
			}
			if(j == len){
				if(((buf[line[lncnt].start_index + line[lncnt].length -3]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -1]) == ':')){
					inning[1].novers++;
				}
			}
		}
		lncnt++;
	}
	if(counter < 2){// validate the starting of inning
		inV.inning_eflag[lncnt] = 1;
		printf("inning indentation err at: %d\n", lncnt);
		return 0;
	}

	overT *o1 = malloc(inning[0].novers * (sizeof(overT))); // try to allocate sufficient space for the lines sequence
	inning[0].overs = o1; // will allocate 0 to 49 overs and we'll access it from 0
	overT *o2 = malloc(inning[1].novers * (sizeof(overT))); // try to allocate sufficient space for the lines sequence
	inning[1].overs = o2;
	printf("inning[0].start: %d\n",  inning[0].start);
	printf("inning[1].start: %d\n",  inning[1].start);

	//	________________________________________________________________________________________________________________________________
	c=101, i=0, cnt= 0;
	unsigned short it = line[Toss.decision.cnl].start_index+Toss.decision.start;
	unsigned short iit = line[Toss.winner.cnl].start_index+Toss.winner.start;
	if(! ((3 == (line[Toss.decision.cnl].length-Toss.decision.start) || (line[Toss.decision.cnl].length-Toss.decision.start) == 5) && (0 < line[Toss.winner.cnl].length-Toss.winner.start))){ //checking for the input size of decision
		ttsV.toss_dflag[Toss.decision.cnl] = 1;
		printf("err at decision: %d\n", Toss.decision.cnl);
	} else{
		if(buf[it] == 'b' && buf[it+1] == 'a' && buf[it+2] == 't'){
			while(i<2){
				for(j=0;j<MAXLENGTH((line[teams[i].name.cnl].length-teams[i].name.start),(line[Toss.winner.cnl].length-Toss.winner.start));j++){
					if((buf[iit+j]) != buf[line[teams[i].name.cnl].start_index+teams[i].name.start+j]){
						break;
					}
				}
				if(j == line[teams[i].name.cnl].length-teams[i].name.start){
					c = !i;
				}
				i += 1;
			}
			if((j < line[teams[i-1].name.cnl].length-teams[i-1].name.start) && i>1){
				ttsV.toss_wflag[Toss.winner.cnl] = 1;
				printf("err at toss winner: %d\n", Toss.winner.cnl);
				return ERR;
			}
		}
		else if(buf[it] == 'f' && buf[it+1] == 'i' && buf[it+2] == 'e' && buf[it+3] == 'l' && buf[it+4] == 'd'){
			while(i<2){
				for(j=0;j<line[Toss.winner.cnl].length-Toss.winner.start;j++){
					if((buf[iit+j]) != buf[line[teams[i].name.cnl].start_index+teams[i].name.start+j]){
						break;
					}
				}
				if(j == line[Toss.winner.cnl].length-Toss.winner.start){
					c = i;
				}
				i += 1;
			}
			if((j != line[Toss.winner.cnl].length-Toss.winner.start) && i>1){
				ttsV.toss_wflag[Toss.winner.cnl] = 1;
				printf("err at toss winner: %d\n", Toss.winner.cnl);
				return ERR;
			}
		} else{ ttsV.toss_dflag[Toss.decision.cnl] = 1;} }
	//getting overs start, ndel, over_no and bowler index for both the innings
	lncnt = inning[0].start; //reusing the lncnt
	it = c, i = 0, c = 0, cnt = 0;
	unsigned char bow_sub_str[] = "        bowler:", bowlen=15;
	while(lncnt < nl){
		if(((buf[line[lncnt].start_index+line[lncnt].length-2]) == '1')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
			inning[i].overs[cnt].start = lncnt;
			inning[i].overs[cnt].ndel = 0;
			inning[i].overs[cnt].over_no = cnt+1, c=0;
			cnt = cnt + 1;
		}
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != bow_sub_str[j]){
				break;
			}
		}
		if(j == bowlen){
			c = 0;
			while( c < NPP){
				for(j=0;j<line[lncnt].length-17;j++){
					if(buf[line[teams[it].plr[c].name.cnl].start_index+teams[it].plr[c].name.start+j] != (buf[line[lncnt].start_index+16+j])){
						break;
					}
				}
				if(j == line[lncnt].length-17){
					inning[i].overs[cnt-1].player_index = teams[it].plr[c].index;
					//					printf("bowler player_index: %d at: %d\n", inning[i].overs[cnt-1].player_index, inning[i].overs[cnt-1].over_no);
				}
				c += 1;
			}
		}
		if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '.')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
			inning[i].overs[cnt-1].ndel++;
		}
		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
			i = 1, cnt = 0, c=0, it = !it;
		}
		lncnt++;
	}
	for(i=0;i<2;i++){
		for(j=0;j<inning[i].novers;j++){
			delT *ds = malloc(inning[i].overs[j].ndel * (sizeof(delT))); // try to allocate sufficient space for the lines sequence
			inning[i].overs[j].ds = ds ;
		}
	}
	//______________________________________________________________________________________________________________________
	c=0, i=0, cnt=0;
	unsigned char cnt0=0;
	unsigned char stri_sub_str[] = "        batsman:", strilen=16;
	unsigned char nsub_str[] = "        non_striker:";
	unsigned char osub_str[] = "        extras:", olen=14;
	unsigned char rsub_str[] = "        runs:", rlen=12;
	unsigned char wsub_str[] = "          player_out:", wlen=21;
	len = 20;
	lncnt = inning[0].start; //reusing the lncnt
	while(lncnt < nl){
		if(((buf[line[lncnt].start_index+line[lncnt].length-2]) == '1')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
			cnt++, cnt0 = 0, c=0;
		}
		if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '.')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
			inning[i].overs[cnt-1].ds[cnt0].del_no.cnl = lncnt, inning[i].overs[cnt-1].ds[cnt0].del_no.start = 6;
			inning[i].overs[cnt-1].ds[cnt0].start = lncnt, c = 0, cnt0++;
		}
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != stri_sub_str[j]){
				break;
			}
		}
		if(j == strilen){
			c = 0;
			while( c < NPP){
				for(j=0;j<line[lncnt].length-18;j++){
					if(buf[line[teams[it].plr[c].name.cnl].start_index+teams[it].plr[c].name.start+j] != (buf[line[lncnt].start_index+17+j])){
						break;
					}
				}
				if(j == line[lncnt].length-18){
					inning[i].overs[cnt-1].ds[cnt0-1].player_index = teams[it].plr[c].index;
					//					printf("striker player_index: %d\n", inning[i].overs[cnt-1].ds[cnt0-1].player_index);
				}
				c += 1;
			}
		}
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != nsub_str[j]){
				break;
			}
		}
		if(j == len){
			c = 0;
			while( c < NPP){
				for(j=0;j<line[lncnt].length-22;j++){
					if(buf[line[teams[it].plr[c].name.cnl].start_index+teams[it].plr[c].name.start+j] != (buf[line[lncnt].start_index+len+1+j])){
						break;
					}
				}
				if(j == line[lncnt].length-22){
					inning[i].overs[cnt-1].ds[cnt0-1].player_index2 = teams[it].plr[c].index;
					//	printf("non_striker player_index2: %d\n", inning[i].overs[cnt-1].ds[cnt0-1].player_index2);
				}
				c += 1;
			}
		}

		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != osub_str[j]){
				break;
			}
		}
		if(j == olen){
			inning[i].overs[cnt-1].ds[cnt0-1].del_type.cnl = lncnt;
			inning[i].overs[cnt-1].ds[cnt0-1].del_type.start = olen+1;
			//			printf("inning[%d].overs[%ld].ds[%d].del_type.cnl: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].del_type.cnl);
		}
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != wsub_str[j]){
				break;
			}
		}
		if(j == wlen){
			inning[i].overs[cnt-1].ds[cnt0-1].outcome.cnl = lncnt;
			inning[i].overs[cnt-1].ds[cnt0-1].outcome.start = wlen+1;
		}
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != rsub_str[j]){
				break;
			}
		}
		if(j == rlen){
			inning[i].overs[cnt-1].ds[cnt0-1].runs.cnl = lncnt;
			inning[i].overs[cnt-1].ds[cnt0-1].runs.start = rlen+1;
			//	printf("inning[%d].overs[%ld].ds[%d].runs.cnl: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].runs.cnl);
		}
		if(((buf[line[lncnt].start_index]) == '-') && ((buf[line[lncnt].start_index+1]) == ' ')&& (buf[line[lncnt].start_index+2]) == '2'){
			i = 1, cnt = 0,c=0, it = !it;
			printf("\n\n2nd inning\n");
		}
		lncnt++;
	}
	free(buf);
	free(line);
	return 0;
}
