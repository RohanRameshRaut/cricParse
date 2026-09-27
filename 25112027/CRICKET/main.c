#include<stdio.h>
#include<stdlib.h>
#include<ncurses.h>

//#define ERR(1)
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define MAXB 1000000 //max bytes allowed
#define MINB 27000 //max bytes allowed
#define WBYTES " :-.\'\n_(),"
#define WBYTES_LEN 10
#define KEYS {"1st innings", "2nd innings", "balls_per_over", "batsman", "bowler", "by", "byes", "created", "data_version", "dates", "decision", "deliveries", "extras", "fielders", "gender", "info", "innings", "kind", "legbyes", "match_type", "meta", "noballs", "non_striker", "outcome", "overs", "people", "player_of_match", "player_out", "players", "registry", "revision", "runs", "team", "teams", "toss", "total", "umpires","venue", "wicket", "wides", "winner" };
#define KEYS_LEN 41
#define EXTRAS {6, 18, 21, 39}//6:byes, 18:legbyes, 21:noballs, 39:wides
#define EXTRAS_LEN 4
#define RUNS {3, 12, 31}
#define RUNS_LEN 3
#define MINL 150 //min lines
#define MAXL 25000 //max lines
#define MINLL 3 //min line length
#define MAXLL 75 //max line length
#define IND_MIN 0 //min indentation allowed
#define IND_MAX 12 //max indentation allowed
#define SUFFIX 2 // suffix size after the key ': '
#define MAXLENGTH(a, b) ((a) > (b) ? (a): (b))
#define MINLENGTH(a, b) ((a) < (b) ? (a): (b))

void h_line(int y, int x, int width, WINDOW *win);
void v_line(int y, int x, int height, WINDOW *win);
void print_t(int y, int x, char **arr, int len, WINDOW *win, int col_w);

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
}lineV;

typedef struct inningV{
	unsigned char flag_inning;
}inningV;

typedef struct teamsV{
	unsigned char flag_teams;
}teamsV;

typedef struct tossV{
	unsigned char flag_toss;
}tossV;

typedef struct playersV{
	unsigned char flag_player;
}playersV;

typedef struct oversV{
	unsigned char flag_over;
}oversV;

typedef struct delV{
	unsigned char flag_del;
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
	unsigned char extra, runs;//key index(wides, legbyes, byes etc);
	char plr_index[3];//plr_index[3] will not have for each del so, it will be waste. 
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

//summary objects
typedef struct BatsmanS{
	unsigned char run, ball;
	char index, w_index;
}BatsmanS;

typedef struct BowlerS{
	unsigned char run, ball;
	char index;
}BowlerS;

typedef struct TeamS{
	unsigned char nplr, wicket, over, nbatsman, nbowler, *index;
	BatsmanS *batsman;
	BowlerS *bowler;
	ContentT *name;
	unsigned short run;
}TeamS;

TeamS team[2];
InningT inning[2];

int main(int aa, char **ab){
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
	unsigned char *sub_str = WBYTES, len=0, sizem = 1, flag = 1, i=0, j=0, cnt0, k=0;
	unsigned char *dsub_str[] = KEYS;
	int isub_str[] = EXTRAS;
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
				//		printf("key: %s, index: %d\n", dsub_str[i], i);
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
			line[lncnt].col = 0;
			if(buf[cnt-1] == ':'){ //check for the colon at the end and set the flag 1 else 0
				line[lncnt].col = 1;
			}
			if(!((buf[cnt-1] == ':'|| buf[cnt-1] == ')' || buf[cnt-1] == '\'' || buf[cnt-1] == '-')||('a' <= buf[cnt-1] && buf[cnt-1] <= 'z')||('0' <= buf[cnt-1] && buf[cnt-1] <= '9')||('A'<=buf[cnt-1]&&buf[cnt-1]<='Z'))){ 
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
		for(j=0; j<line[lncnt].length; j++){
			if(buf[line[lncnt].start_index+line[lncnt].ind+j] == ':')
				break;
			cnt += 1;//count the length of key of a particular line
		}
		/* key_index is only an internal index for known cricket keys.
		 * YAML also permits arbitrary mapping keys, so -1 does NOT mean
		 * invalid YAML. */
		if(j < line[lncnt].length){
			for(i=0;i<KEYS_LEN;i++){
				for(j=0;j<MAXLENGTH(cnt, KEYS_ARR[i]);j++){
					if(buf[line[lncnt].start_index+line[lncnt].ind+j] != dsub_str[i][j])
						break;
				}
				if(j == MAXLENGTH(cnt, KEYS_ARR[i]))
					line[lncnt].key_index = i;//assign the matched key index
			}
		}
		lncnt += 1;
		cnt = 0;
	}
	PlayersT players;
	char a = -1;
	lncnt = 0, len=0, cnt=0, i=0,j=0, flag=0, players.nplr=0;
	ContentT teams[2];
	team[0].nplr = 0, team[1].nplr = 0;
	teamsV tsV;
	tsV.flag_teams = 0;
	ContentT toss[2];//toss[0]:decision, toss[1]:winner
	tossV tssV;
	tssV.flag_toss = 0;
	while(lncnt < nl){//get the PlayersT object initialised
		if(line[lncnt].key_index == 29) flag = 0;//match the <registry> key at 29th index
		if(flag){
			if((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':'){
				players.teams[i].cnl = lncnt;
				players.teams[i].start = line[lncnt].ind;
				i = 1, a++;
			}
			if((buf[line[lncnt].start_index+line[lncnt].ind-2]) == '-'){ //match the pattern from player name lines "ind-2 contains '-'"
				players.nplr += 1;
				team[a].nplr += 1;
			}
		}
		if(line[lncnt].key_index == 28) flag = 1;//match the <players> key at 28th index
		if(line[lncnt].key_index == 33){//match the <teams> key at 33 index
			/* Count the actual YAML sequence entries under teams:.
			 * line[].ind also counts the '-' sequence marker, so use the
			 * real leading-space count here. */
			unsigned char team_count = 0;
			unsigned char team_parent_spaces = 0;
			unsigned long ti = lncnt + 1;
			unsigned int tp;

			tp = line[lncnt].start_index;
			while (tp < line[lncnt].start_index + line[lncnt].length && buf[tp] == ' ') {
				team_parent_spaces++;
				tp++;
			}

			while (ti < nl) {
				unsigned int ts = line[ti].start_index;
				unsigned char spaces = 0;
				unsigned int te = line[ti].start_index + line[ti].length;

				while (ts < te && buf[ts] == ' ') {
					spaces++;
					ts++;
				}
				if (spaces != team_parent_spaces || ts >= te || buf[ts] != '-')
					break;

				if (team_count < 2) {
					teams[team_count].cnl = ti;
					teams[team_count].start = spaces + 1;
					while (teams[team_count].start < line[ti].length &&
					       buf[line[ti].start_index + teams[team_count].start] == ' ')
						teams[team_count].start++;
					if (teams[team_count].start >= line[ti].length)
						tsV.flag_teams |= (team_count == 0) ? (1 << 7) : (1 << 6);
				}
				team_count++;
				ti++;
			}

			if (team_count != 2)
				tsV.flag_teams |= (1 << 5);
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
	players.index = (char*) calloc(players.nplr, (sizeof(char)));
	team[0].index = (char*) calloc(team[0].nplr, (sizeof(char)));
	if(!team[0].index) return ERR;
	team[1].index = (char*) calloc(team[1].nplr, (sizeof(char)));
	if(!team[1].index) return ERR;
	a = 0, j = 0;
	for(i=0;i<team[0].nplr;i++){
		team[0].index[i] = i;
	}
	for(i=team[0].nplr;i<players.nplr;i++){
		team[1].index[j] = i;
		j++;
	}
	playersV psV;
	psV.flag_player = 0;
	if(!players.index) return ERR;
	for(i=0;i<players.nplr;i++) players.index[i] = i;
	players.name = (ContentT*) calloc(players.nplr, (sizeof(ContentT)));
	if(!players.name) return ERR;
	team[0].name = (ContentT*) calloc(team[0].nplr, (sizeof(ContentT)));
	if(!team[0].name) return ERR;
	team[1].name = (ContentT*) calloc(team[1].nplr, (sizeof(ContentT)));
	if(!team[1].name) return ERR;
	lncnt = 0, i=0, flag=0, j=0;
	inningV isV;
	a = -1;
	unsigned char m = 0;
	isV.flag_inning = 0;
	inning[0].novers = 0, inning[1].novers = 0;
	while(lncnt < nl){//get the Players.name initialised
		if(line[lncnt].key_index == 29) flag = 0;//match the <registry> key at 29th index
		if(flag){
			if((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':'){
				a++;
				m = 0;
			}
			if((buf[line[lncnt].start_index+line[lncnt].ind-2]) == '-'){ //match the pattern from player name lines "ind-2 contains '-'"
				players.name[i].cnl = lncnt;
				players.name[i].start = line[lncnt].ind;
				team[a].name[m].cnl = lncnt;
				team[a].name[m].start = line[lncnt].ind;
				if(! (0 < (line[players.name[i].cnl].length-players.name[i].start))) psV.flag_player = psV.flag_player | (1<<7);//empty player name flag
																		//	fwrite(buf+line[team[a].name[i].cnl].start_index+team[a].name[i].start, 1, line[team[a].name[i].cnl].length-team[a].name[i].start, stdout);
																		//	printf("\n");
				i += 1;
				m += 1;
			}
		}
		if(line[lncnt].key_index == 28) flag = 1;//match the <players> key at 28th index

		if(line[lncnt].key_index == 0){
			inning[0].start = lncnt;
			if(!inning[0].start) isV.flag_inning = isV.flag_inning | (1<<7);//isEmpty start of 1st inning, validation
		}
		if(line[lncnt].key_index == 32){
			inning[j].teams.cnl = lncnt;
			inning[j].teams.start = line[lncnt].ind+KEYS_ARR[line[lncnt].key_index]+SUFFIX;
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
	OversT *o1 = calloc(inning[0].novers, (sizeof(OversT)));
	inning[0].overs = o1;
	OversT *o2 = calloc(inning[1].novers, (sizeof(OversT)));
	inning[1].overs = o2;
	oversV osV;//over_no and del_no digit validation
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
	lncnt = inning[0].start, i=0, flag=0, j=0, cnt=0, len=0;
	team[0].nbowler = 0;
	team[1].nbowler = 0;
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
			DelT *ds = calloc(inning[i].overs[j].ndel, (sizeof(DelT))); // try to allocate sufficient space for the lines sequence
			if(!ds) return ERR;
			inning[i].overs[j].ds = ds ;
		}
	}
	lncnt = inning[0].start, i=0, flag=0, j=0, cnt=0, len=0, cnt0=0, sizem=0;
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
			inning[i].overs[cnt-1].ds[cnt0].runs = 0;
			inning[i].overs[cnt-1].ds[cnt0].plr_index[0] = -1,inning[i].overs[cnt-1].ds[cnt0].plr_index[1] = -1, cnt0++;
		}

		if(line[lncnt].key_index == 3 || line[lncnt].key_index == 22 || line[lncnt].key_index == 27){//3:batsman key, 22:non_striker key, 27:player_out
			inning[i].overs[cnt-1].ds[cnt0-1].extra = 0b11110000;
			//			inning[i].overs[cnt-1].ds[cnt0-1].runs = 0;//init for all
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
					if(line[lncnt].key_index == 3) inning[i].overs[cnt-1].ds[cnt0-1].plr_index[0] = flag;//printf("inning[%d].overs[%d].ds[%d].plr_index[0]: %d, lncnt: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].plr_index[0], lncnt);
					if(line[lncnt].key_index == 22) inning[i].overs[cnt-1].ds[cnt0-1].plr_index[1] = flag;//printf("inning[%d].overs[%d].ds[%d].plr_index[1]: %d lncnt: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].plr_index[1], lncnt);
					if(line[lncnt].key_index == 27) inning[i].overs[cnt-1].ds[cnt0-1].plr_index[2] = flag;//printf("inning[%d].overs[%d].ds[%d].plr_index[2]: %d lncnt: %d\n", i, cnt-1, cnt0-1, inning[i].overs[cnt-1].ds[cnt0-1].plr_index[2], lncnt);
				}
				flag += 1;
			}
		}
		if(line[lncnt].key_index == 12){//extras key index
			if((buf[line[lncnt].start_index+line[lncnt].length-1]) == ':'){//line's last index should have colon, or line[lncnt].col
				for(j=0;j<EXTRAS_LEN;j++){
					if(isub_str[j] == line[lncnt+1].key_index){
						inning[i].overs[cnt-1].ds[cnt0-1].extra = (j << 4) | buf[line[lncnt+1].start_index+line[lncnt+1].length-1] ;//override the -1(8) with key index at left side nibble and //get the extras run at right side nibble
					}
				}
				if(!('0' <= (buf[line[lncnt+1].start_index+line[lncnt+1].ind+KEYS_ARR[line[lncnt+1].key_index]+SUFFIX]) && (buf[line[lncnt+1].start_index+line[lncnt+1].ind+KEYS_ARR[line[lncnt+1].key_index]+SUFFIX]) <= '9' )) dsV.flag_del = dsV.flag_del | (1<<7);
			}
		}
		if(line[lncnt].key_index == 31) sizem = line[lncnt].ind;//31:runs key index
		if(line[lncnt].ind == sizem+2){//runs section digit validation
			if(line[lncnt].key_index == 12){//extras key index
				if(!('0' <= (buf[line[lncnt].start_index+line[lncnt].length-1]) && (buf[line[lncnt].start_index+line[lncnt].length-1] <= '9'))) dsV.flag_del = dsV.flag_del | (1<<6); //extras: 0 digit validation
			}
			if(line[lncnt].key_index == 3){//batsman key index
				if(!('0' <= (buf[line[lncnt].start_index+line[lncnt].length-1]) && (buf[line[lncnt].start_index+line[lncnt].length-1] <= '9'))) dsV.flag_del = dsV.flag_del | (1<<6); //batsman: 0 digit validation
				inning[i].overs[cnt-1].ds[cnt0-1].runs = inning[i].overs[cnt-1].ds[cnt0-1].runs | (buf[line[lncnt].start_index+line[lncnt].length-1]-48) << 4;
				//	printf("inning[%d].overs[%d].ds[%d].runs: %c\n", i, cnt-1, cnt0-1,buf[line[lncnt].start_index+line[lncnt].length-1]);
			}
			if(line[lncnt].key_index == 35){//total key index
				if(!('0' <= (buf[line[lncnt].start_index+line[lncnt].length-1]) && (buf[line[lncnt].start_index+line[lncnt].length-1] <= '9'))) dsV.flag_del = dsV.flag_del | (1<<6); //total: 0 digit validation
				inning[i].overs[cnt-1].ds[cnt0-1].runs = inning[i].overs[cnt-1].ds[cnt0-1].runs | (buf[line[lncnt].start_index+line[lncnt].length-1]-48);
				//				printf("inning[%d].overs[%d].ds[%d].runs: %c | buf[line[%d].start_index+line[%d].length-1]: %c\n", i, cnt-1, cnt0-1,inning[i].overs[cnt-1].ds[cnt0-1].runs, lncnt, lncnt, buf[line[lncnt].start_index+line[lncnt].length-1]);
			}
		}
		if(line[lncnt].key_index == 1){//1: 2nd inning, to reset the values
			i = 1, cnt = 0, flag = 0;
		}
		lncnt += 1;
	}
	//---------------------------------------------------------------------------------------------------------------
	unsigned char c = 101;//dummy initialisation
	i=0, a = 101;
	while(i<2){//players.teams[i].cnl].length-1, -1 to ignore the ':' at the end of players team name
		for(j=0;j<MAXLENGTH(((line[players.teams[i].cnl].length-1)-players.teams[i].start),(line[inning[0].teams.cnl].length-inning[0].teams.start));j++){
			if((buf[line[inning[0].teams.cnl].start_index+inning[0].teams.start+j]) != buf[line[players.teams[i].cnl].start_index+players.teams[i].start+j]){
				break;
			}
		}
		if(j == (line[players.teams[i].cnl].length-1)-players.teams[i].start){
			c = i;//it will set(0 or 1) according to 1st innings team index
		}
		i += 1;
	}
	flag=0, cnt=0, len=0, cnt0=0;//count the number of batsman
	for(i=0;i<2;i++){
		for(j=0;j<team[c].nplr;j++){
			for(cnt=0;cnt<inning[i].novers;cnt++){
				for(cnt0=0;cnt0<inning[i].overs[cnt].ndel;cnt0++){
					flag = 0;
					if(team[c].index[j] == inning[i].overs[cnt].ds[cnt0].plr_index[0] || team[c].index[j] == inning[i].overs[cnt].ds[cnt0].plr_index[1]){
						team[c].nbatsman += 1;
						flag = 1;
						break;
					}
				}
				if(flag) break;
			}
		}
		c = !c;
	}
	BatsmanS *bt1 = calloc(team[0].nbatsman, sizeof(BatsmanS));//allocate memory to batsman
	if(!bt1) return ERR;
	team[0].batsman = bt1;
	BatsmanS *bt2 = calloc(team[1].nbatsman, sizeof(BatsmanS));
	if(!bt2) return ERR;
	team[1].batsman = bt2;
	//---------------------------------------------------------------------------------------------------------------
	i=0, flag=0, j=0, cnt=0, len=0, cnt0=0, a=0, c = !c;//count the number of bowler, (c = !c, flip the c to get opposite teams bowler info)
	for(i=0;i<2;i++){
		for(j=0;j<team[c].nplr;j++){
			for(cnt=0;cnt<inning[i].novers;cnt++){
				flag = 0;
				if(team[c].index[j] == inning[i].overs[cnt].plr_index){
					team[c].nbowler += 1;
					flag = 1;
					break;
				}
			}
		}
		c = !c;
	}
	BowlerS *bs1 = calloc(team[0].nbowler, sizeof(BowlerS));//allocate memory to bowlers
	if(!bs1) return ERR;
	team[0].bowler = bs1;
	BowlerS *bs2 = calloc(team[1].nbowler, sizeof(BowlerS));
	if(!bs2) return ERR;
	team[1].bowler = bs2;
	i=0, flag=0, j=0, cnt=0, len=0, cnt0=0, a=0, c = !c, lncnt = 0;//team score count
	for(i=0;i<2;i++){
		for(cnt=0;cnt<inning[i].novers;cnt++){
			for(cnt0=0;cnt0<inning[i].overs[cnt].ndel;cnt0++){
				lncnt += inning[i].overs[cnt].ds[cnt0].runs & 0b00001111;
			}
		}
		team[c].run = lncnt;
		lncnt = 0;
		c = !c;
	}
	cnt=0, cnt0=0, a=0, lncnt = 0;//individual batsman runs
	unsigned short ball = 0;
	for(i=0;i<2;i++){
		for(j=0;j<team[c].nbatsman;j++){
			for(cnt=0;cnt<inning[i].novers;cnt++){
				for(cnt0=0;cnt0<inning[i].overs[cnt].ndel;cnt0++){
					if(team[c].index[j] == inning[i].overs[cnt].ds[cnt0].plr_index[0]){
						lncnt = lncnt + (inning[i].overs[cnt].ds[cnt0].runs>>4);
						ball += 1;
					}
				}
			}
			team[c].batsman[j].run = lncnt;
			team[c].batsman[j].ball = ball;
			lncnt = 0, ball = 0;
		}
		c = !c;
	}
	cnt=0, cnt0=0, a=0, lncnt = 0, c = !c;//individual bowler ball and run count
	ball = 0;
	for(i=0;i<2;i++){
		for(j=0;j<team[c].nbowler;j++){
			for(cnt=0;cnt<inning[i].novers;cnt++){
				for(cnt0=0;cnt0<inning[i].overs[cnt].ndel;cnt0++){
					if(team[c].index[j] == inning[i].overs[cnt].plr_index){
						lncnt = lncnt + (inning[i].overs[cnt].ds[cnt0].runs& 0b00001111);
						ball += 1;
					}
				}
			}
			team[c].bowler[j].run = lncnt;
			team[c].bowler[j].ball= ball;
			lncnt = 0, ball = 0;
		}
		c = !c;
	}
	cnt=0, cnt0=0, c = !c;//get the bowler_index for wicket of batsman
	char wicket_index = -1;
	for(i=0;i<2;i++){
		for(j=0;j<team[c].nbatsman;j++){
			for(cnt=0;cnt<inning[i].novers;cnt++){
				for(cnt0=0;cnt0<inning[i].overs[cnt].ndel;cnt0++){
					flag = 0;
					if(team[c].index[j] == inning[i].overs[cnt].ds[cnt0].plr_index[2]){ 
						wicket_index = inning[i].overs[cnt].plr_index;//bowler_index of that over
						flag = 1;
						break;
					}
				}
				if(flag) break;
			}
			team[c].batsman[j].w_index = wicket_index;
			wicket_index = -1;
		}
		c = !c;
	}
	cnt=0, cnt0=0;//get the batsman index
	wicket_index = -1;
	for(i=0;i<2;i++){
		a = 0;
		for(j=0;j<team[c].nplr;j++){
			wicket_index = -1;
			flag = 0;
			for(cnt=0;cnt<inning[i].novers;cnt++){
				for(cnt0=0;cnt0<inning[i].overs[cnt].ndel;cnt0++){
					if((team[c].index[j] == inning[i].overs[cnt].ds[cnt0].plr_index[0]) || (team[c].index[j] == inning[i].overs[cnt].ds[cnt0].plr_index[1])){ 
						wicket_index = team[c].index[j];
						flag = 1;
						break;
					}
				}
				if(flag) break;
			}
			if(wicket_index != -1){
				team[c].batsman[a].index = wicket_index;
				a += 1;
			}
		}
		c = !c;
	}
	cnt=0, c = !c;//get the bowler index
	wicket_index = -1;
	for(i=0;i<2;i++){
		a = 0;
		for(j=0;j<team[c].nplr;j++){
			wicket_index = -1;
			for(cnt=0;cnt<inning[i].novers;cnt++){
				if(team[c].index[j] == inning[i].overs[cnt].plr_index){
					wicket_index = team[c].index[j];
					break;
				}
			}
			if(wicket_index != -1){
				team[c].bowler[a].index = wicket_index;
				a += 1;
			}
		}
		c = !c;
	}
	//-------------------------------------------------------------------------------
	// Validation + ncurses visualisation.
	// All validation below works directly on the existing objects/buffer.

	/* Additional cricket-domain validation.  The existing bit flags are reused. */
	if (players.nplr == 0 || team[0].nplr == 0 || team[1].nplr == 0)
		psV.flag_player |= (1 << 6);

	if (inning[0].novers == 0 || inning[1].novers == 0)
		isV.flag_inning |= (1 << 3);

	for (i = 0; i < 2; i++) {
		for (j = 0; j < inning[i].novers; j++) {
			if (inning[i].overs[j].ndel == 0)
				osV.flag_over |= (1 << 5);
			if (inning[i].overs[j].ndel > 12)
				osV.flag_over |= (1 << 4);
			if (j > 0 && inning[i].overs[j].over_no != (unsigned char)(inning[i].overs[j-1].over_no + 1))
				osV.flag_over |= (1 << 3);

			for (cnt0 = 0; cnt0 < inning[i].overs[j].ndel; cnt0++) {
				DelT *d = &inning[i].overs[j].ds[cnt0];
				if (d->plr_index[0] < 0 || d->plr_index[0] >= (char)players.nplr)
					dsV.flag_del |= (1 << 6);
				if (d->plr_index[1] < 0 || d->plr_index[1] >= (char)players.nplr)
					dsV.flag_del |= (1 << 5);
				if (d->plr_index[2] >= (char)players.nplr)
					dsV.flag_del |= (1 << 4);
				if ((d->runs & 0x0f) > 15 || ((d->runs >> 4) & 0x0f) > 15)
					dsV.flag_del |= (1 << 3);
				/* extra is packed as (extra-type << 4) | extra-runs. */
				if (d->extra != 0xf0 && ((d->extra >> 4) > 3 || (d->extra & 0x0f) > 9))
					dsV.flag_del |= (1 << 2);
				if (d->plr_index[0] >= 0 && d->plr_index[0] == d->plr_index[1])
					dsV.flag_del |= (1 << 1);
				/* total = batsman runs + extras for a delivery. */
				if (d->extra != 0xf0) {
					unsigned char extra_runs = d->extra & 0x0f;
					unsigned char batsman_runs = (d->runs >> 4) & 0x0f;
					unsigned char total_runs = d->runs & 0x0f;
					if (total_runs != (unsigned char)(batsman_runs + extra_runs))
						dsV.flag_del |= (1 << 0);
				}
			}
		}
	}

	/* ncurses starts here.  Use stdscr directly for the top-level screen so
	 * there is always a refreshed window visible to the user. */
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	int my, mx, running = 1, x = 2, y = 1, z = 0;
	getmaxyx(stdscr, my, mx);

	while (running) {
		clear();
		box(stdscr, 0, 0);
		y = 1;
		x = 2;
		mvprintw(y++, x, "cricParse -- YAML Visualiser & Data Validator");
		mvprintw(y++, x, "Choose index");
		mvprintw(y++, x, "-------------------------------------------------");
		mvprintw(y++, x, "0. Data / Domain Validation");
		mvprintw(y++, x, "1. Cricket Summary");
		mvprintw(y++, x, "q. Exit");
		refresh();

		int ch = getch();

		if (ch == 'q' || ch == 'Q' || ch == 27) {
			running = 0;
		}
		else if (ch == '0') {
			int vrunning = 1;
			while (vrunning) {
				clear();
				box(stdscr, 0, 0);
				y = 1;
				x = 2;
				mvprintw(y++, x, "Data / Domain Validation");
				mvprintw(y++, x, "(Press 'q' to return)");
				mvprintw(y++, x, "-------------------------------------------------");


				/* YAML syntax / structural validation. */
				mvprintw(y++, x, "YAML SYNTAX / STRUCTURE");
				int syntax_errors = 0;
				for (unsigned long li = 0; li < nl; li++) {
					int bad = 0;
					/* key_index == -1 is valid: YAML mapping keys are not limited
					 * to the application's cricket-key table. */
					if (line[li].ind > IND_MAX) bad = 1;
					if (line[li].length < MINLL || line[li].length > MAXLL) bad = 1;
					if (bad) {
						mvprintw(y++, x, "line %lu: invalid YAML syntax/indentation/key", li + 1);
						syntax_errors++;
						if (y >= my - 4) break;
					}
				}
				if (lsV.flag_line & (1 << 2)) {
					for (unsigned long li = 0; li < nl; li++) {
						unsigned int p = line[li].start_index, q = p + line[li].length;
						unsigned int spaces = 0;
						for (unsigned int t = p; t < q; t++) if (buf[t] == ' ') spaces++;
						if (spaces == line[li].length) { mvprintw(y++, x, "line %lu: empty/whitespace-only line", li + 1); break; }
					}
					syntax_errors++;
				}
				if (lsV.flag_line & (1 << 3)) {
					for (unsigned long li = 0; li < nl; li++) {
						unsigned int p = line[li].start_index + line[li].length - 1;
						if (buf[p] != ':' && buf[p] != ')' && buf[p] != '\'' && buf[p] != '-' &&
						    !((buf[p] >= 'a' && buf[p] <= 'z') || (buf[p] >= 'A' && buf[p] <= 'Z') || (buf[p] >= '0' && buf[p] <= '9'))) {
							mvprintw(y++, x, "line %lu: invalid line ending/content", li + 1); break;
						}
					}
					syntax_errors++;
				}
				if (lsV.flag_line & (1 << 4)) {
					for (unsigned long li = 0; li + 1 < nl; li++) {
						if (line[li].col == 1 && line[li].ind + SUFFIX != line[li + 1].ind) {
							mvprintw(y++, x, "line %lu: child indentation does not match parent", li + 1); break;
						}
					}
					syntax_errors++;
				}
				if (lsV.flag_line & (1 << 5)) {
					for (unsigned long li = 0; li < nl; li++) if (line[li].ind > IND_MAX) { mvprintw(y++, x, "line %lu: indentation outside 0..%d", li + 1, IND_MAX); break; }
					syntax_errors++;
				}
				if (lsV.flag_line & (1 << 6)) {
					for (unsigned long li = 0; li < nl; li++) if (line[li].length < MINLL || line[li].length > MAXLL) { mvprintw(y++, x, "line %lu: line length outside %d..%d", li + 1, MINLL, MAXLL); break; }
					syntax_errors++;
				}
				if (!syntax_errors)
					mvprintw(y++, x, "OK - YAML syntax/structure checks passed");

				if (y < my - 10) y++;
				mvprintw(y++, x, "CRICKET DOMAIN / SEMANTIC VALIDATION");
				int domain_errors = 0;

				if (tsV.flag_teams) {
					if (tsV.flag_teams & (1 << 5)) mvprintw(y++, x, "line ?: invalid team count");
					if (tsV.flag_teams & (1 << 7)) mvprintw(y++, x, "line %hu: first team name is empty", teams[0].cnl + 1);
					if (tsV.flag_teams & (1 << 6)) mvprintw(y++, x, "line %hu: second team name is empty", teams[1].cnl + 1);
					domain_errors++;
				}
				if (tssV.flag_toss) {
					if (tssV.flag_toss & (1 << 7)) mvprintw(y++, x, "line %hu: toss decision is empty", toss[0].cnl + 1);
					if (tssV.flag_toss & (1 << 6)) mvprintw(y++, x, "line %hu: toss winner is empty", toss[1].cnl + 1);
					domain_errors++;
				}
				if (isV.flag_inning) {
					if (isV.flag_inning & (1 << 7)) mvprintw(y++, x, "line 1: first innings section is empty/missing");
					if (isV.flag_inning & (1 << 6)) mvprintw(y++, x, "line 1: second innings section is empty/missing");
					if (isV.flag_inning & (1 << 5)) mvprintw(y++, x, "line %hu: first innings has no valid end", inning[0].end + 1);
					if (isV.flag_inning & (1 << 4)) mvprintw(y++, x, "line %hu: second innings has no valid end", inning[1].end + 1);
					if (isV.flag_inning & (1 << 3)) mvprintw(y++, x, "line ?: one or both innings contain no overs");
					domain_errors++;
				}
				if (psV.flag_player) {
					if (psV.flag_player & (1 << 7)) mvprintw(y++, x, "line ?: player name is empty");
					if (psV.flag_player & (1 << 6)) mvprintw(y++, x, "line ?: one or both teams have no players");
					domain_errors++;
				}
				if (osV.flag_over) {
					for (i = 0; i < 2; i++) {
						for (j = 0; j < inning[i].novers; j++) {
							if ((osV.flag_over & (1 << 6) || osV.flag_over & (1 << 5)) && inning[i].overs[j].ndel == 0) { mvprintw(y++, x, "line %hu: over has no deliveries", inning[i].overs[j].start + 1); break; }
							if ((osV.flag_over & (1 << 4)) && inning[i].overs[j].ndel > 12) { mvprintw(y++, x, "line %hu: over has more than 12 deliveries", inning[i].overs[j].start + 1); break; }
							if ((osV.flag_over & (1 << 3)) && j > 0 && inning[i].overs[j].over_no != (unsigned char)(inning[i].overs[j-1].over_no + 1)) { mvprintw(y++, x, "line %hu: over numbers are not sequential", inning[i].overs[j].start + 1); break; }
						}
					}
					if (osV.flag_over & (1 << 7)) mvprintw(y++, x, "line ?: invalid over number syntax");
					domain_errors++;
				}
				if (dsV.flag_del) {
					for (i = 0; i < 2; i++) {
						for (j = 0; j < inning[i].novers; j++) {
							for (cnt0 = 0; cnt0 < inning[i].overs[j].ndel; cnt0++) {
								DelT *d = &inning[i].overs[j].ds[cnt0];
								if ((dsV.flag_del & (1 << 7)) && d->del_no.start == 0) mvprintw(y++, x, "line %hu: delivery value is empty", d->del_no.cnl + 1);
								if ((dsV.flag_del & (1 << 6)) && (d->plr_index[0] < 0 || d->plr_index[0] >= (char)players.nplr)) mvprintw(y++, x, "line %hu: invalid batsman reference", d->del_no.cnl + 1);
								if ((dsV.flag_del & (1 << 5)) && (d->plr_index[1] < 0 || d->plr_index[1] >= (char)players.nplr)) mvprintw(y++, x, "line %hu: invalid non-striker reference", d->del_no.cnl + 1);
								if ((dsV.flag_del & (1 << 4)) && d->plr_index[2] >= (char)players.nplr) mvprintw(y++, x, "line %hu: invalid player_out reference", d->del_no.cnl + 1);
								if ((dsV.flag_del & (1 << 1)) && d->plr_index[0] >= 0 && d->plr_index[0] == d->plr_index[1]) mvprintw(y++, x, "line %hu: batsman and non-striker are identical", d->del_no.cnl + 1);
								if ((dsV.flag_del & (1 << 0)) && d->extra != 0xf0) { unsigned char e = d->extra & 0x0f, b = (d->runs >> 4) & 0x0f, t = d->runs & 0x0f; if (t != (unsigned char)(b + e)) mvprintw(y++, x, "line %hu: total != batsman + extras", d->del_no.cnl + 1); }
							}
						}
					}
					if (dsV.flag_del & (1 << 3)) mvprintw(y++, x, "line ?: packed run value is invalid");
					if (dsV.flag_del & (1 << 2)) mvprintw(y++, x, "line ?: invalid extra encoding");
					domain_errors++;
				}

				if (!domain_errors)
					mvprintw(y++, x, "OK - cricket domain checks passed");

				if (!syntax_errors && !domain_errors) {
					//mvprintw(y++, x, "");
					y++;
					mvprintw(y++, x, "VALIDATION PASSED");
				}
				else {
					//mvprintw(y++, x, "");
					y++;
					mvprintw(y++, x, "VALIDATION FAILED");
				}

				refresh();
				int vc = getch();
				if (vc == 'q' || vc == 'Q' || vc == 27)
					vrunning = 0;
			}
		}
		else if (ch == '1') {
			/* Original cricket-summary navigation, retained. */
			int summary_running = 1;
			while (summary_running) {
				clear();
				box(stdscr, 0, 0);
				y = 1;
				x = 2;
				mvprintw(y++, x, "Cricket Summary");
				mvprintw(y++, x, "Choose team");
				mvprintw(y++, x, "-------------------------------------------------");
				for (z = 0; z < 2; z++) {
					mvprintw(y + z, x, "%d.", z);
					for (i = 0; i < line[players.teams[z].cnl].length - players.teams[z].start; i++)
						mvprintw(y + z, x + i + 2, "%c", buf[line[players.teams[z].cnl].start_index + players.teams[z].start + i]);
				}
				mvprintw(y + 2, x, "q. Back");
				refresh();

				int ch = getch();
				if (ch == 'q' || ch == 'Q' || ch == 27) {
					summary_running = 0;
					continue;
				}
				if (ch != '0' && ch != '1') continue;
				z = ch - '0';

				int team_running = 1;
				while (team_running) {
					clear();
					box(stdscr, 0, 0);
					y = 1;
					x = 2;
					mvprintw(y++, x, "Team Summary");
					mvprintw(y++, x, "Team: ");
					for (i = 0; i < line[players.teams[z].cnl].length - players.teams[z].start; i++)
						mvprintw(y - 1, x + 6 + i, "%c", buf[line[players.teams[z].cnl].start_index + players.teams[z].start + i]);
					mvprintw(y++, x, "-------------------------------------------------");
					mvprintw(y++, x, "0. Players List");
					mvprintw(y++, x, "1. Score Card");
					mvprintw(y++, x, "q. Back");
					refresh();

					int ch2 = getch();
					if (ch2 == 'q' || ch2 == 'Q' || ch2 == 27) {
						team_running = 0;
					}
					else if (ch2 == '0') {
						clear();
						box(stdscr, 0, 0);
						y = 1;
						x = 2;
						mvprintw(y++, x, "Player List");
						mvprintw(y++, x, "(Press any key to return)");
						mvprintw(y++, x, "-------------------------------------------------");
						for (j = 0; j < team[z].nplr && y < my - 2; j++) {
							for (i = 0; i < line[team[z].name[j].cnl].length - team[z].name[j].start; i++)
								mvprintw(y, x + i, "%c", buf[line[team[z].name[j].cnl].start_index + team[z].name[j].start + i]);
							y++;
						}
						refresh();
						getch();
					}
					else if (ch2 == '1') {
						int max_x = 0, col_w = 0, box_h = 0, box_w = 0, ncols = 4;
						char *title[] = {"Batsman", "Run", "Ball", "Wicket"};
						for (j = 0; j < team[z].nbatsman; j++) {
							int name_len = line[team[z].name[j].cnl].length - team[z].name[j].start;
							if (name_len > max_x) max_x = name_len;
						}
						col_w = max_x + 8;
						box_w = col_w * ncols;
						box_h = team[z].nbatsman + 2;

						clear();
						box(stdscr, 0, 0);
						y = 1;
						x = 2;
						mvprintw(y++, x, "Score Card");
						mvprintw(y++, x, "(Press any key to return)");
						h_line(y, x, box_w, stdscr);
						y++;
						print_t(y, x, title, ncols, stdscr, col_w);
						y++;
						h_line(y, x, box_w, stdscr);
						y++;

						for (j = 0; j < team[z].nbatsman && y < my - 3; j++) {
							for (i = 0; i < line[team[z].name[j].cnl].length - team[z].name[j].start; i++)
								mvprintw(y, x + i, "%c", buf[line[team[z].name[j].cnl].start_index + team[z].name[j].start + i]);
							mvprintw(y, x + col_w + 2, "%d", team[z].batsman[j].run);
							mvprintw(y, x + 2 * col_w + 2, "%d", team[z].batsman[j].ball);
							mvprintw(y, x + 3 * col_w + 2, "%s", team[z].batsman[j].w_index >= 0 ? "OUT" : "NOT-OUT");
							y++;
						}
						h_line(y, x, box_w, stdscr);
						refresh();
						getch();
					}
				}
			}
		}
	}
	endwin();

	//free the memory allocated
	for(i=0;i<2;i++){
		for(j=0;j<inning[i].novers;j++) free(inning[i].overs[j].ds);//free allocated memory to deliveries
	}
	free(KEYS_ARR), free(players.index), free(players.name), free(o1), free(o2), free(line), free(buf);
	return 0;
}

void h_line(int y, int x, int width, WINDOW *win){
	for(int i=0;i<width;i++){
		mvwprintw(win, y, x+i, "-");
	}
}

void v_line(int y, int x, int height, WINDOW *win){
	for(int i=0;i<height;i++){
		mvwprintw(win, y+i, x, "|");
	}
}

void print_t(int y, int x, char *arr[], int len, WINDOW *win, int col_w){
	char i, j;
	for(i=0;i<len;i++){
		mvwprintw(win, y, x, "%s", arr[i]);
		x += col_w+1;//1 for vertical col line
	}
}
