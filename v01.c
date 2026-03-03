#include<stdio.h>
#include<stdlib.h>

#define ERR (1)
#define MAXB 1000000 //max bytes allowed
#define MINB 27000 //max bytes allowed
#define WBYTES " :-.\'\n_(),"
#define WBYTES_LEN 10
#define KEYS {"1st innings", "2nd innings", "balls_per_over", "batsman", "bowler", "by", "byes", "created", "data_version", "dates", "decision", "deliveries", "extras", "fielders", "gender", "info", "innings", "kind", "legbyes", "match_type", "meta", "noballs", "non_striker", "outcome", "overs", "people", "player_of_match", "player_out", "players", "registry", "revision", "runs", "team", "teams", "toss", "total", "umpires","venue", "wicket", "wides", "winner" };
#define KEYS_LEN 41
#define MINL 150 //min lines
#define MAXL 25000 //max lines
#define MINLL 3 //min line length
#define MAXLL 75 //max line length
#define IND_MIN 0 //min indentation allowed
#define IND_MAX 12 //max indentation allowed
#define SUFFIX 2 // suffix size after the key ': '
#define MAXLENGTH(a, b) ((a) > (b) ? (a): (b))


//ref validation objects
typedef struct sbytesV{
	unsigned long max;//1000000
	unsigned short min;
	unsigned char *wb;// wanted bytes array
}sbytesV;

typedef struct lineV{
	unsigned char ind_min, ind_max, len_min, len_max;
	unsigned char min_lines;
	unsigned short max_lines;
	unsigned char flag_line;
	/* *flag_line:
	   8=starting_of_line_flag
	   7=line coutn _flag
	   6=indentation count err flag
	   5=next line indentation count err
	   4=ending line flag
	   3=empty line flag
	 */
}lineV;

typedef struct inningV{
	unsigned char flag_inning;
	/* *flag_inning: 0=starting of inning flag
	   1= ending
	 */
}inningV;

typedef struct teamsV{
	unsigned char flag_teams;
	/* *flag_teams: 0=empty flag
	   1= count flag
	 */
}teamsV;

typedef struct tossV{
	unsigned char flag_toss;
	/* *flag_teams: 0=decision flag empty
	   1=winner empty
	 */
}tossV;

typedef struct playersV{
	unsigned char flag_player;
	/* *falg_players: 0=empty flag
	 */
}playersV;

typedef struct oversV{
	unsigned char flag_over;
	/* *falg_over: 0=empty flag
	   1= digit falg
	 */
}oversV;

typedef struct delV{
	unsigned char flag_del;
	/* *falg_del: 0=start falg
	   1= del number
	   2= empty
	   3= runs digit
	 */
}delV;

//Normal objects
typedef struct KeyT{
	unsigned char *key;
	unsigned char key_len;
}KeyT;

typedef struct LineT{
	unsigned int start_index;
	unsigned char length, ind, col, key_index;
}LineT;

typedef struct ContentT{
	unsigned short cnl;
	unsigned char start;
}ContentT;

typedef struct PlayersT{
	unsigned char *index;
	unsigned char nplr;
	ContentT *name, teams[2];
}PlayersT;

typedef struct ExtrasT{
	unsigned char extra;
}ExtrasT;

typedef struct DelT{
	unsigned short start;
	unsigned char *plr_index[2], *runs[3];
	ContentT del_no, del_type;
	ExtrasT *extra;
}DelT;

typedef struct OversT{
	unsigned short start;
	DelT *ds;
	unsigned char plr_index;
	unsigned char over_no, ndel;
}OversT;

typedef struct InningT{
	ContentT *teams[2];
	OversT *overs;
	unsigned char novers, start, end;
}InningT;

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

	unsigned long cnt = 0, nl = 0;
	unsigned short lncnt = 0;
	unsigned char *sub_str = WBYTES, len=0, sizem = 1, flag = 1, i=0, j=0;
	unsigned char *dsub_str[] = KEYS;
	b.wb = sub_str;
	while (cnt < size)
	{
		for(i=0;i<WBYTES_LEN;i++){
			if(buf[cnt] == b.wb[i]){
				break;
			}
		}
		if((i==WBYTES_LEN) && !(('A'<=buf[cnt] && buf[cnt]<='Z') || ('a'<=buf[cnt] && buf[cnt]<='z') || ('0'<=buf[cnt] && buf[cnt]<='9'))){
			printf("invalid char: %c\n", buf[cnt]);
		}
		if(buf[cnt] == '\n') nl++;
		++cnt;
	}
	char *KEYS_ARR = (char*) calloc(KEYS_LEN, sizeof(char)); //allocate the memory to keys index array
	if(!KEYS_ARR) return ERR; 
	for(i=0;i<KEYS_LEN;i++){//calculating the length of each key
		printf("key: %s, index: %d\n", dsub_str[i], i);
		for(j=0;dsub_str[i][j] != '\0';j++){
			len += 1;
		}
		KEYS_ARR[i] = len;//store the length of each key
		len = 0;
	}
	LineT *line = (LineT*) calloc((nl), (sizeof(LineT))); // try to allocate sufficient space for the lines sequence
	if(!line) return ERR;
	cnt = 0; // we shall use the cnt again for our computation
	lineV lsV;
	lsV.flag_line = 0;
	while (cnt < size-1)
	{
		if(buf[cnt] == '\n')
		{
			line[lncnt].length = sizem-1;
			//			printf("Line %d: starting_index: %d size %d: \n", lncnt, line[lncnt].start_index, line[lncnt].length);
			line[lncnt].col = 0;
			if(buf[cnt-1] == ':'){ //check for the colon at the end and set the flag 1 else 0
				line[lncnt].col = 1;
			}
			if(!((buf[cnt-1] == ':' || buf[cnt-1] == '\'' || buf[cnt-1] == '-')||('a' <= buf[cnt-1] && buf[cnt-1] <= 'z')||('0' <= buf[cnt-1] && buf[cnt-1] <= '9')||('A'<=buf[cnt-1]&&buf[cnt-1]<='Z'))){ 
				lsV.flag_line = lsV.flag_line | (1<<3);//line ending validation
				printf("error at line: %d\n", lncnt);
				printf("cnt-1: %c\n", buf[cnt-1]);
			};
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
	//	printf("Line %d: starting_index: %d size %d: \n", lncnt, line[nl-1].start_index, line[nl-1].length);
	lncnt = 0, len=0, cnt=0;
	lsV.len_min = MINLL, lsV.len_max = MAXLL, lsV.ind_min = IND_MIN, lsV.ind_max = IND_MAX;
	while(lncnt < nl){
		if(!((buf[line[lncnt].start_index] == ' ')||(buf[line[lncnt].start_index] == '-')||('a'<= buf[line[lncnt].start_index] && buf[line[lncnt].start_index]<='z'))){
			lsV.flag_line = lsV.flag_line | (1<<7);//starting of a line err
			printf("error at starting char: %c\n", buf[line[lncnt].start_index]);
		}
		if(!(lsV.len_min <= line[lncnt].length && line[lncnt].length <= lsV.len_max)){
			lsV.flag_line = lsV.flag_line | (1<<6);//line count err
			printf("line length count error at line: %d\n", lncnt);
		}
		if(!(lsV.ind_min <= line[lncnt].ind && line[lncnt].ind <= lsV.ind_max)){
			lsV.flag_line = lsV.flag_line | (1<<5);//indentation count err
			printf("indentation count error at line: %d\n", lncnt);
		}
		if(line[lncnt].col == 1){
			if(line[lncnt].ind+SUFFIX != line[lncnt+1].ind){
				lsV.flag_line = lsV.flag_line | (1<<4);//next line indentation count err
				printf("ind err: %d\n", lncnt);
				return ERR;
			}
		}
		for(i=0;i<line[lncnt].length;i++){//check for empty line
			if(buf[line[lncnt].start_index+i] == ' '){
				len += 1;
			}
		} 
		if(len == line[lncnt].length){
			lsV.flag_line = lsV.flag_line | (1<<2);//empty line err
			printf("empty line err at: %d\n", lncnt);
		}
		lncnt += 1;
		len = 0;
	}

	lncnt = 0, len=0, cnt=0, i=0,j=0;
	while(lncnt < nl){//replace all the keys with index
		for(j=0; buf[line[lncnt].start_index+line[lncnt].ind+j] != ':'&&j<line[lncnt].length; j++){
			cnt += 1;//count the length of key of a perticular line
		}
		for(i=0;i<KEYS_LEN;i++){
			for(j=0;j<MAXLENGTH(cnt, KEYS_ARR[i]);j++){
				if(buf[line[lncnt].start_index+line[lncnt].ind+j] != dsub_str[i][j]){
					break;
				}
			}
			if(j == MAXLENGTH(cnt, KEYS_ARR[i])){
				line[lncnt].key_index = i;
				//printf("line[%d].key_index: %d\n", lncnt, line[lncnt].key_index);
			}
		}
		lncnt += 1;
		cnt = 0;
	}
	PlayersT players;
	lncnt = 0, len=0, cnt=0, i=0,j=0, flag=0, players.nplr=0;
	ContentT teams[2];
	ContentT toss[2];//toss[0]:decision, toss[1]:winner
	while(lncnt < nl){//get the PlayersT object initialised
		if(line[lncnt].key_index == 29) flag = 0;//match the <registry> key at 29th index
		if(flag){
			if((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':'){
				players.teams[i].cnl = lncnt;
				players.teams[i].start = line[lncnt].ind;
				//	fwrite(buf+line[players.teams[i].cnl].start_index+players.teams[i].start, 1, line[players.teams[i].cnl].length-players.teams[i].start, stdout);
				//	printf("\n");
				i = 1;
			}
			if((buf[line[lncnt].start_index+line[lncnt].ind-2]) == '-'){ //match the pattern from player name lines "ind-2 contains '-'"
				players.nplr += 1;
			}
		}
		if(line[lncnt].key_index == 28) flag = 1;//match the <players> key at 28th index
		if(line[lncnt].key_index == 33){//match the <teams> key at 33 index
			teams[0].cnl = lncnt+1;
			teams[0].start = line[lncnt+1].ind;
			teams[1].cnl = lncnt+2;
			teams[1].start = line[lncnt+2].ind;
			//	fwrite(buf+line[teams[0].cnl].start_index+teams[0].start, 1, line[teams[0].cnl].length-teams[0].start, stdout);
			//	printf("\n");
			//	fwrite(buf+line[teams[1].cnl].start_index+teams[1].start, 1, line[teams[1].cnl].length-teams[1].start, stdout);
			//	printf("\n");
		}
		if(line[lncnt].key_index == 34) j = 1;//match the <toss> key at 34 index
		if(j){
			if(line[lncnt].key_index == 10){//match the <decision> key at 10 index
				toss[0].cnl = lncnt;
				toss[0].start = line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX;//add indentation+key_length+SUFFIX
			}
			if(line[lncnt].key_index == 40){//match the <winner> key at 40 index
				toss[1].cnl = lncnt;
				toss[1].start = line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX;//add indentation+key_length+SUFFIX
			}
		}
		if(line[lncnt].key_index == 36) j = 0;//match the <umpires> key at 36 index
		lncnt += 1;
	}
//	fwrite(buf+line[toss[0].cnl].start_index+toss[0].start, 1, line[toss[0].cnl].length-toss[0].start, stdout);
//	printf("\n");
//	fwrite(buf+line[toss[1].cnl].start_index+toss[1].start, 1, line[toss[1].cnl].length-toss[1].start, stdout);
//	printf("\n");
	players.index = (char*) calloc(players.nplr, (sizeof(char)));
	if(!players.index) return ERR;
	for(i=0;i<players.nplr;i++) players.index[i] = i;
	players.name = (ContentT*) calloc(players.nplr, (sizeof(ContentT)));
	if(!players.name) return ERR;
	lncnt = 0, i=0, flag=0;
	while(lncnt < nl){//get the Players.name initialised
		if(line[lncnt].key_index == 29) flag = 0;//match the <registry> key at 29th index
		if(flag){
			if((buf[line[lncnt].start_index+line[lncnt].ind-2]) == '-'){ //match the pattern from player name lines "ind-2 contains '-'"
				players.name[i].cnl = lncnt;
				players.name[i].start = line[lncnt].ind;
				//	fwrite(buf+line[players.name[i].cnl].start_index+players.name[i].start, 1, line[players.name[i].cnl].length-players.name[i].start, stdout);
				//	printf("\n");
			}
		}
		if(line[lncnt].key_index == 28) flag = 1;//match the <players> key at 28th index
		lncnt += 1;
		i += 1;
	}


	return 0;
}
