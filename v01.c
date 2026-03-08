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
#define MINLENGTH(a, b) ((a) < (b) ? (a): (b))

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

typedef struct extrasV{
	unsigned char flag_extras;
	/* *falg_extras: 0=empty flag or not digit flag
	 */
}extrasV;

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
	unsigned char length, ind, col;
	char key_index;
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

typedef struct DelT{
	unsigned short start;
	char extra;//key index(wides, legbyes, byes etc);
	char plr_index[3], runs[3];//plr_index[3] will not have for each del so, it will be waste. 
	ContentT del_no;//no del_type needed as ContentT type, we'll handle it at extras
}DelT;

typedef struct OversT{
	unsigned short start;
	DelT *ds;
	unsigned char plr_index;
	unsigned char over_no, ndel;
}OversT;

typedef struct InningT{
	ContentT teams;
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
	unsigned char *sub_str = WBYTES, len=0, sizem = 1, flag = 1, i=0, j=0, cnt0;
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
		line[lncnt].key_index = -1;//initialise key_index with -1 for all
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
				line[lncnt].key_index = i;//assign the matched key index
			}
		}
		lncnt += 1;
		cnt = 0;
	}
	PlayersT players;
	lncnt = 0, len=0, cnt=0, i=0,j=0, flag=0, players.nplr=0;
	ContentT teams[2];
	teamsV tsV;
	tsV.flag_teams = 0;
	ContentT toss[2];//toss[0]:decision, toss[1]:winner
	tossV tssV;
	tssV.flag_toss = 0;
	while(lncnt < nl){//get the PlayersT object initialised
			  //		printf("line[%d].key_index: %d\n", lncnt, line[lncnt].key_index);
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
		if(!((KEYS_ARR[34]-KEYS_ARR[33]) == 3)) tsV.flag_teams = tsV.flag_teams | (1<<5);//team count err flag
		if(line[lncnt].key_index == 33){//match the <teams> key at 33 index
			teams[0].cnl = lncnt+1;
			teams[0].start = line[lncnt+1].ind;
			if(! (0 < (line[teams[0].cnl].length-teams[0].start))) tsV.flag_teams = tsV.flag_teams | (1<<7);//empty 1st team name flag
			teams[1].cnl = lncnt+2;
			teams[1].start = line[lncnt+2].ind;
			if(! (0 < (line[teams[1].cnl].length-teams[1].start))) tsV.flag_teams = tsV.flag_teams | (1<<6);//empty 2nd team name flag
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
				if(! (0 < (line[toss[0].cnl].length-toss[0].start))) tssV.flag_toss = tssV.flag_toss | (1<<7);//empty decision flag
			}
			if(line[lncnt].key_index == 40){//match the <winner> key at 40 index
				toss[1].cnl = lncnt;
				toss[1].start = line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX;//add indentation+key_length+SUFFIX
				if(! (0 < (line[toss[1].cnl].length-toss[1].start))) tssV.flag_toss = tssV.flag_toss | (1<<6);//empty winner flag
			}
		}
		if(line[lncnt].key_index == 36) j = 0;//match the <umpires> key at 36 index
		lncnt += 1;
	}
	//	fwrite(buf+line[toss[0].cnl].start_index+toss[0].start, 1, line[toss[0].cnl].length-toss[0].start, stdout);
	//	printf("\n");
	//	fwrite(buf+line[toss[1].cnl].start_index+toss[1].start, 1, line[toss[1].cnl].length-toss[1].start, stdout);
	//	printf("\n");
	//	printf("nplr: %d\n", players.nplr);
	players.index = (char*) calloc(players.nplr, (sizeof(char)));
	playersV psV;
	psV.flag_player = 0;
	if(!players.index) return ERR;
	for(i=0;i<players.nplr;i++) players.index[i] = i;
	players.name = (ContentT*) calloc(players.nplr, (sizeof(ContentT)));
	if(!players.name) return ERR;
	lncnt = 0, i=0, flag=0, j=0;
	InningT inning[2];
	inningV isV;
	isV.flag_inning = 0;
	inning[0].novers = 0;
	while(lncnt < nl){//get the Players.name initialised
		if(line[lncnt].key_index == 29) flag = 0;//match the <registry> key at 29th index
		if(flag){
			if((buf[line[lncnt].start_index+line[lncnt].ind-2]) == '-'){ //match the pattern from player name lines "ind-2 contains '-'"
				players.name[i].cnl = lncnt;
				players.name[i].start = line[lncnt].ind;
				if(! (0 < (line[players.name[i].cnl].length-players.name[i].start))) psV.flag_player = psV.flag_player | (1<<7);//empty player name flag
																		//fwrite(buf+line[players.name[i].cnl].start_index+players.name[i].start, 1, line[players.name[i].cnl].length-players.name[i].start, stdout);
																		//printf("\n");
				i += 1;
			}
		}
		if(line[lncnt].key_index == 28) flag = 1;//match the <players> key at 28th index

		if(line[lncnt].key_index == 0){
			inning[0].start = lncnt;
			if(!inning[0].start) isV.flag_inning = isV.flag_inning | (1<<7);//isEmpty start of 1st inning, validation
											//printf("inning[0].start: %d\n", inning[0].start);
		}
		if(line[lncnt].key_index == 32){
			inning[j].teams.cnl = lncnt;
			inning[j].teams.start = line[lncnt].ind;
		}
		if(buf[line[lncnt].start_index+line[lncnt].length-1] == ':' &&
				buf[line[lncnt].start_index+line[lncnt].length-2] == '1' &&
				buf[line[lncnt].start_index+line[lncnt].length-3] == '.'){
			inning[j].novers += 1;
		}
		if(line[lncnt].key_index == 1){
			inning[1].start = lncnt;
			if(!inning[1].start) isV.flag_inning = isV.flag_inning | (1<<6);//isEmpty start of 2nd inning, validation
			inning[0].end = lncnt-1;
			if(!inning[0].end) isV.flag_inning = isV.flag_inning | (1<<5);//isEmpty end of 1st inning, validation
			inning[1].end = nl-1;
			if(!inning[1].end) isV.flag_inning = isV.flag_inning | (1<<4);//isEmpty end of 2nd inning, validation
			j = 1;
		}
		lncnt += 1;
	}
	OversT *o1 = (OversT*) calloc(inning[0].novers, (sizeof(OversT)));
	inning[0].overs = o1;
	OversT *o2 = (OversT*) calloc(inning[1].novers, (sizeof(OversT)));
	inning[1].overs = o2;
	//over_no and del_no digit validation
	oversV osV;
	osV.flag_over = 0;
	lncnt = inning[0].start;
	while(lncnt < nl){
		if(line[lncnt].ind == 6 && buf[line[lncnt].ind-1] == '-'){
			cnt = line[lncnt].start_index+line[lncnt].ind;
			if(!('0' <= buf[cnt] && buf[cnt] <= '9')) osV.flag_over = osV.flag_over | (1<<7);
			if('0' <= buf[cnt+1] && buf[cnt+1] <= '9'){
				if(buf[cnt+2] != '.') osV.flag_over = osV.flag_over | (1<<7);
				if(!('0' <= buf[cnt+3] && buf[cnt+3] <= '9')) osV.flag_over = osV.flag_over | (1<<7);
			}
			if(buf[cnt+1] == '.'){
				if(!('0' <= buf[cnt+2] && buf[cnt+2] <= '9')) return ERR;//reture ERR as we capture the over as '.1:', and if its not present then we shall return
			}
		}
		lncnt += 1;
	}
	//-------------------------------------------------------------------------------------------------------
	lncnt = 0, i=0, flag=0, j=0, cnt=0, len=0;
	while(lncnt < nl){//get the Players.name initialised
		if(buf[line[lncnt].start_index+line[lncnt].length-1] == ':' &&
				buf[line[lncnt].start_index+line[lncnt].length-2] == '1' &&
				buf[line[lncnt].start_index+line[lncnt].length-3] == '.'){
			inning[i].overs[cnt].start = lncnt;
			if(! (0 < (inning[i].overs[cnt].start))) osV.flag_over = osV.flag_over | (1<<6);//empty over start flag
			inning[i].overs[cnt].ndel = 0;
			inning[i].overs[cnt].over_no = cnt+1;
			cnt += 1;
		}
		if(line[lncnt].key_index == 4){//4: bowler key
			len = 0;
			for(j=0;buf[line[lncnt].start_index+line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX+j] != '\n';j++){
				len += 1;
			}
			flag = 0;
			while(flag < players.nplr){
				for(j=0;j<MINLENGTH(len,line[players.name[flag].cnl].length-players.name[flag].start);j++){
					if(buf[line[lncnt].start_index+line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX+j] !=
							buf[line[players.name[flag].cnl].start_index+players.name[flag].start+j]){
						break;
					}
				}
				if(j == MAXLENGTH(len, (line[players.name[flag].cnl].length-players.name[flag].start))){
					inning[i].overs[cnt-1].plr_index = flag;//bowler index , flag/player.index[flag]
				}
				flag += 1;
			}
		}
		if(buf[line[lncnt].start_index+line[lncnt].length-1] == ':' &&
				buf[line[lncnt].start_index+line[lncnt].length-3] == '.'){
			inning[i].overs[cnt-1].ndel += 1;
		}
		if(line[lncnt].key_index == 1){//1: 2nd inning, to reset the values
			i = 1, cnt = 0, flag = 0;
		}
		lncnt += 1;
	}
	for(i=0;i<2;i++){//allocating memory to deliveries
		for(j=0;j<inning[i].novers;j++){
			//			printf("inning[%d].overs[%d].ndel: %d\n", i, j, inning[i].overs[j].ndel);
			DelT *ds = malloc(inning[i].overs[j].ndel * (sizeof(DelT))); // try to allocate sufficient space for the lines sequence
			if(!ds) return ERR;
			inning[i].overs[j].ds = ds ;
		}
	}
	lncnt = 0, i=0, flag=0, j=0, cnt=0, len=0, cnt0=0;
	delV dsV;
	dsV.flag_del = 0;
	while(lncnt < nl){
		if(buf[line[lncnt].start_index+line[lncnt].length-1] == ':' &&
				buf[line[lncnt].start_index+line[lncnt].length-2] == '1' &&
				buf[line[lncnt].start_index+line[lncnt].length-3] == '.'){
			cnt += 1, cnt0=0, flag=0;
		}
		if(((buf[line[lncnt].start_index+line[lncnt].length-3]) == '.')&&((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':')){
			inning[i].overs[cnt-1].ds[cnt0].del_no.cnl = lncnt, inning[i].overs[cnt-1].ds[cnt0].del_no.start = line[lncnt].ind;
			if(! (0 < (inning[i].overs[cnt-1].ds[cnt0].del_no.start))) dsV.flag_del = dsV.flag_del | (1<<7);//empty delivery start flag
			inning[i].overs[cnt-1].ds[cnt0].start = lncnt, flag = 0, inning[i].overs[cnt-1].ds[cnt0].plr_index[2] = -1; 
			inning[i].overs[cnt-1].ds[cnt0].plr_index[0] = -1,inning[i].overs[cnt-1].ds[cnt0].plr_index[1] = -1, cnt0++;
		}

		if(line[lncnt].key_index == 3 || line[lncnt].key_index == 22 || line[lncnt].key_index == 27){//3:batsman key, 22:non_striker key, 27:player_out
			len = 0;
			for(j=0;buf[line[lncnt].start_index+line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX+j] != '\n';j++){
				len += 1;
			}
			flag = 0;
			while(flag < players.nplr){
				for(j=0;j<MINLENGTH(len,line[players.name[flag].cnl].length-players.name[flag].start);j++){
					if(buf[line[lncnt].start_index+line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX+j] !=
							buf[line[players.name[flag].cnl].start_index+players.name[flag].start+j]){
						break;
					}
				}
				if(j == MAXLENGTH(len, (line[players.name[flag].cnl].length-players.name[flag].start))){
					if(line[lncnt].key_index == 3) inning[i].overs[cnt-1].ds[cnt0-1].plr_index[0] = flag;// printf("inning[%d].overs[%d].ds[%d].plr_index[0]: %d, lncnt: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].plr_index[0], lncnt);
					if(line[lncnt].key_index == 22) inning[i].overs[cnt-1].ds[cnt0-1].plr_index[1] = flag;// printf("inning[%d].overs[%d].ds[%d].plr_index[1]: %d lncnt: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].plr_index[1], lncnt);
					if(line[lncnt].key_index == 27) inning[i].overs[cnt-1].ds[cnt0-1].plr_index[2] = flag;//printf("inning[%d].overs[%d].ds[%d].plr_index[2]: %d lncnt: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].plr_index[2], lncnt);
				}
				flag += 1;
			}
		}
		if((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':'){//line's last index should have colon, or line[lncnt].col
			if(line[lncnt].key_index == 12){//extras key index
				inning[i].overs[cnt-1].ds[cnt0-1].extra = line[lncnt+1].key_index;
				if(!('0' <= (buf[line[lncnt+1].start_index+line[lncnt+1].ind+KEYS_ARR[line[lncnt+1].key_index]+SUFFIX]) && (buf[line[lncnt+1].start_index+line[lncnt+1].ind+KEYS_ARR[line[lncnt+1].key_index]+SUFFIX]) <= '9' )) dsV.flag_del = dsV.flag_del | (1<<7);
			}
		}
		if(line[lncnt].key_index == 1){//1: 2nd inning, to reset the values
			i = 1, cnt = 0, flag = 0;
		}
		lncnt += 1;
	}
	return 0;
}
