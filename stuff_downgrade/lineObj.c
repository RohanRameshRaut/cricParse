#include <stdio.h>
#include <stdlib.h>

#define ERR (1)

typedef struct LineT{
	unsigned int start_index;
	unsigned char length;
}LineT;

typedef struct ContentT{
	unsigned short cnl; //line number of content line
	unsigned char start;
}ContentT;

typedef struct Players{ // will store the line number and starting index of a each teams 1st player.
	ContentT t1_players;
	ContentT t2_players;
}Players;

typedef struct Metadata{ //match summary
	ContentT team1;
	ContentT team2;
}Metadata;

typedef struct delT{
	unsigned short start;
	//	unsigned char player_index; // p1.t1_players.start
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
	LineT *lines = malloc(nl * (sizeof(*lines))); // try to allocate sufficient space for the lines sequence

	if (!lines) //if allocation fails exit
	{
		fclose(f); return ERR;
	}

	// Now that we have got sufficient memory allocated,
	// we shall continue the construction of the lines object
	// as discussed in the class.

	cnt = 0; // we shall use the cnt again for our computation
	unsigned short lncnt = 0; // why lnsize must be set to 1?? //lnct is the real nl(number of a line).
	LineT line[nl];
	line[lncnt].length = 0;
	line[lncnt].start_index = 0;
	while (cnt <= size)
	{
		if(buf[cnt] == '\n')
		{
			//printf("Line %d: starting_index: %d size %d: \n", lncnt, line[lncnt].start_index, line[lncnt].length);
			lncnt++;
			line[lncnt].start_index = cnt+1;
			line[lncnt].length = 0;
		}
		++line[lncnt].length;
		++cnt;
	}

	// getting the line number(cnl) and start index(start) of team1 and team2.
	Metadata teams;
	ContentT team1, team2;
	lncnt = 0; //reusing the lncnt for line count
	unsigned char sub_str[] = "  teams:"; //pattern to match the '  - teams:'
	unsigned char len = 8; // length of a sub_string is not changing so we counted it
	unsigned short temp;
	unsigned char j;
	while(lncnt < nl){
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			team1.cnl = lncnt + 1;
			team2.cnl = lncnt + 2;

			temp = line[lncnt+1].start_index;
			while(temp < temp+line[lncnt+1].length-2){
				if((('A' <=buf[temp]) && (buf[temp] <= 'Z')) || (('a' <= buf[temp]) && (buf[temp] <= 'z'))){
					team1.start = temp - line[lncnt+1].start_index;
					team2.start = team1.start;
					teams.team1 = team1;
					teams.team2 = team2;
					break;
				}
				temp++;
			}
		}
		lncnt++;
	}
	//printf("team1.cnl: %d\n", team1.cnl);
	//printf("team1.start: %d\n", team1.start);
	//printf("Team1 name: ");
	//	unsigned char c, i;
	//	for(i =0; c != '\n';i++){
	//		c = (buf[line[team1.cnl].start_index+team1.start+i]);
	//		//printf("%c",c);
	//	}
	//printf("team1 name: %c\n", buf[line[team1.cnl].start_index+team1.start]);
	//printf("team2.cnl: %d\n", team2.cnl);
	//printf("team2.start: %d\n", team2.start);

	// geting the players.
	Players p1, p2;
	lncnt = 0; //reusing the lncnt for line count
	unsigned char psub_str[] = "  players:"; //pattern to match the '  - teams:'
	len = 10; // length of a sub_string is not changing so we counted it
	temp = 0;
	j=0;
	while(lncnt < nl){
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != psub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			p1.t1_players.cnl = lncnt + 2; // next line after the team_name
			p2.t2_players.cnl = lncnt + 14;// second team's players starting index
			temp = line[lncnt+2].start_index;
			while(temp < temp+line[lncnt+2].length-2){
				if((('A' <=buf[temp]) && (buf[temp] <= 'Z')) || (('a' <= buf[temp]) && (buf[temp] <= 'z'))){
					p1.t1_players.start = temp - line[lncnt+2].start_index;
					p2.t2_players.start = p1.t1_players.start;
					break;
				}
				temp++;
			}
		}
		lncnt++;
	}
	//	printf("team1_1st_player.cnl: %d\n", p1.t1_players.cnl);
	//	printf("team1_1st_player.start: %d\n", p1.t1_players.start);
	//	printf("Team1_1st_player_name: \n");
	unsigned char c=0, i=0;
	//	for(unsigned char z=0;z<11;z++){ //printing all 11 players
	//		for(i =0; c != '\n';i++){ //printing the 1st player only
	//			c = (buf[line[p1.t1_players.cnl+z].start_index+p1.t1_players.start+i]);
	//			printf("%c",c);
	//		}c=0;
	//	}
	//	printf("team2_1st_player.cnl: %d\n", p2.t2_players.cnl);
	//	printf("team2_1st_player.start: %d\n", p2.t2_players.start);

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//initialising inning objects
	InningT i1, i2;
	// getting the end of 1st innings
	lncnt = 0; //reusing the lncnt
	unsigned char esub_str[] = "- 2nd innings:";
	len = 14; // length of a esub_string is not changing so we counted it
	j=0;
	while(lncnt < nl){
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != esub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.end = lncnt-1;
			i2.start = lncnt;
			i2.end = nl-1;
		}

		lncnt++;
	}
	lncnt = 0; //reusing the lncnt
	unsigned char osub_str[] = "- 1st innings:";
	len = 14; // length of a sub_string is not changing so we counted it
	temp = 0;
	j=0;
	while(lncnt < nl){
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != osub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.start = lncnt;
			temp = line[lncnt+3].start_index;
		}
		lncnt++;
	}
	//________________________________________________________________________________________________________________________________
	// getting i1.novers;
	lncnt = i1.start; //reusing the lncnt for line count
	cnt = 0;
	while(lncnt < i1.end){
		if(((buf[line[lncnt].start_index + line[lncnt].length -4]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -3]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == ':')){
			cnt = cnt + 1;
		}
		lncnt++;
	}

	i1.novers = cnt; // getting total overs
			 //printf("Total number overs in 1st innings are: %d\n", i1.novers);
	overT *o1 = malloc(i1.novers * (sizeof(*o1))); // try to allocate sufficient space for the lines sequence
	i1.overs = o1;

	// getting i2.novers;
	lncnt = i2.start; //reusing the lncnt for line count
	cnt = 0;
	while(lncnt < i2.end){
		if(((buf[line[lncnt].start_index + line[lncnt].length -4]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -3]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == ':')){
			cnt = cnt + 1;
		}
		lncnt++;
	}
	i2.novers = cnt; // getting total overs
			 //printf("Total number overs in 2nd innings are: %d\n", i2.novers);
	overT *o2 = malloc(i2.novers * (sizeof(*o2))); // try to allocate sufficient space for the lines sequence
	i2.overs = o2; 
	//________________________________________________________________________________________________________________________________________________


	// getting i1.over[cnt].start;
	lncnt = i1.start; //reusing the lncnt for line count
	cnt = 0;
	while(lncnt < i1.end){
		if(((buf[line[lncnt].start_index + line[lncnt].length -4]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -3]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == ':')){
			i1.overs[cnt].start = lncnt;
			i1.overs[cnt].over_no = cnt+1;
			cnt = cnt + 1;
		}
		lncnt++;
	}
	//printf("i1.overs[0].start: %d\n", i1.overs[0].start);
	//printf("i1.overs[0].over_no: %d\n", i1.overs[cnt-1].over_no);

	// getting i2.over[cnt].start;
	lncnt = i2.start; //reusing the lncnt for line count
	cnt = 0;
	while(lncnt < i2.end){
		if(((buf[line[lncnt].start_index + line[lncnt].length -4]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -3]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == ':')){
			i2.overs[cnt].start = lncnt;
			i2.overs[cnt].over_no = cnt+1;
			cnt = cnt + 1;
		}
		lncnt++;
	}
	//printf("i2.overs[cnt].start: %d\n", i2.overs[0].start);
	//printf("i2.overs[0].over_no: %d\n", i2.overs[cnt-1].over_no);
	//___________________________________________________________________
	//getting i1.overs.[novers].ndel
	unsigned char i1overs_ndel_sub_str[] = "        bowler:";
	i1.overs[0].ndel = 0;
	cnt = -1;
	len = 15; // length of a sub_string is not changing so we counted it
	j=0;
	unsigned short ii=0;
	for(ii=i1.start;ii<i1.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			i1.overs[cnt].ndel = 0;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1overs_ndel_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.overs[cnt].ndel++;
		}
	}
	//printf("i1.ii: %d\n", ii);
	//printf("i1.overs.[1].ndel: %d\n", i1.overs[1].ndel);
	//printf("i1.cnt: %ld\n", cnt);
	//printf("i1.overs.[cnt].ndel: %d\n", i1.overs[cnt].ndel);
	//allocating the memory to delive
	cnt = 0;
	while(cnt < i1.novers){
		delT *ds = malloc(i1.overs[cnt].ndel * (sizeof(*ds))); // try to allocate sufficient space for the lines sequence
		i1.overs[cnt].ds = ds ;
		cnt++;
	}
	//___________________________________________________________________
	//getting i2.overs.[novers].ndel
	unsigned char i2overs_ndel_sub_str[] = "        bowler:";
	//printf("i2.end: %d\n", i2.end);
	i2.overs[0].ndel = 0;
	cnt = -1;
	len = 15; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i2.start;ii<i2.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			i2.overs[cnt].ndel = 0;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i2overs_ndel_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i2.overs[cnt].ndel++;
		}
	}

	//	printf("i2.overs.[1].ndel: %d\n", i2.overs[1].ndel);
	//	printf("i2.overs.[cnt].ndel: %d\n", i2.overs[cnt].ndel);

	cnt = 0;
	while(cnt < i2.novers){
		delT *ds = malloc(i2.overs[cnt].ndel * (sizeof(*ds))); // try to allocate sufficient space for the lines sequence
		i2.overs[cnt].ds = ds ;
		cnt++;
	}
	// ****************************************************************************************************
	//getting i1.overs.over_no
	cnt = 0;
	while(cnt < i1.novers){
		i1.overs[cnt].over_no = cnt + 1;
		cnt++;
	}
	//getting i2.overs.over_no
	cnt = 0;
	while(cnt < i2.novers){
		i2.overs[cnt].over_no = cnt + 1;
		cnt++;
	}
	//****************************************************************************

	//	getting i1.overs[cnt].bowler_names
	lncnt = i1.start; //reusing the lncnt for line count
	cnt = 0;
	while(lncnt < i1.end){
		if(((buf[line[lncnt].start_index + line[lncnt].length -4]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -3]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == ':')){
			i1.overs[cnt].bowler_name.cnl = lncnt+2;
			i1.overs[cnt].bowler_name.start = line[lncnt+2].start_index+17;
			cnt = cnt + 1;
		}
		lncnt++;
	}
	//printf("i1.overs[cnt].bowler_name.cnl: %d\n", i1.overs[0].bowler_name.cnl);
	//printf("i1.overs[cnt].bowler_name.start: %d\n", i1.overs[0].bowler_name.start);
	//   unsigned char  c=0;
	//   unsigned short i=0;
	//    for(i =0; c != '\n';i++){ //printing the bowler of 9th over
	//            c = (buf[line[i1.overs[8].bowler_name.cnl].start_index+i+16]);
	//            printf("%c",c);
	//    }
	//      getting i2.overs[cnt].bowler_names
	lncnt = i2.start; //reusing the lncnt for line count
	cnt = 0;
	while(lncnt < i2.end){
		if(((buf[line[lncnt].start_index + line[lncnt].length -4]) == '.') && ((buf[line[lncnt].start_index + line[lncnt].length -3]) == '1') && ((buf[line[lncnt].start_index + line[lncnt].length -2]) == ':')){
			i2.overs[cnt].bowler_name.cnl = lncnt+2;
			i2.overs[cnt].bowler_name.start = line[lncnt+2].start_index+17;
			cnt = cnt + 1;
		}
		lncnt++;
	}
	//        printf("i2.overs[cnt].bowler_name.cnl: %d\n", i2.overs[0].bowler_name.cnl);
	//        printf("i2.overs[cnt].bowler_name.start: %d\n", i2.overs[0].bowler_name.start);
	//            unsigned char  c=0;
	//            unsigned short i=0;
	//             for(i =0; c != '\n';i++){ //printing the bowler of 9th over
	//                     c = (buf[line[i2.overs[8].bowler_name.cnl].start_index+i+16]);
	//                     printf("%c",c);
	//             }
	//**************************************************************************************************************
	//      getting i1.overs[0].ds[ndel].del_no
	unsigned char i1_overs_del_no_sub_str[] = "    - ";
	cnt = -1;//over[cnt] counter
	unsigned char cnt0 = 1;//ds[cnt0] counter
	len = 6; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i1.start;ii<i1.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_no_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.overs[cnt].ds[cnt0].del_no.cnl = ii;// line number of a del_no
			i1.overs[cnt].ds[cnt0].del_no.start = 6;//all del_no starts from 6th index 
			cnt0++;
		}
	}
	printf("i1.overs[4].ds[5].del_no.cnl: %d\n",i1.overs[4].ds[5].del_no.cnl);
	//      getting i2.overs[0].ds[ndel].del_no
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	j=0;
	ii=0;
	for(ii=i2.start;ii<i2.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_no_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i2.overs[cnt].ds[cnt0].del_no.cnl = ii;// line number of a del_no
			i2.overs[cnt].ds[cnt0].del_no.start = 6;//all del_no starts from 6th index 
			cnt0++;
		}
	}
	//printf("i2.overs[0].ds[3].del_no.cnl: %d\n",i2.overs[0].ds[3].del_no.cnl);

	//**************************************************************************************************************
	//      getting i1.overs[0].ds[ndel].del_type
	unsigned char i1_overs_del_type_sub_str[] = "        extras:";
	unsigned char i1_overs_del_type_mini_sub_str[] = "        bowler:";
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 15; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i1.start;ii<i1.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_type_mini_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			for(j=0;j<line[ii+1].length-1;j++){
				if((buf[line[ii+1].start_index+j]) != i1_overs_del_type_sub_str[j]){
					break;
				}
			}
			if(j == len){ //if match found, do the below task
				i1.overs[cnt].ds[cnt0].del_type.cnl = ii+2;// line number of a del_type
				i1.overs[cnt].ds[cnt0].del_type.start = 11;//all del_types starts from 11th index(including legbyes, wides)
			}
			cnt0++;
		}
	}
	//	printf("i1.overs[4].ds[5].del_type.cnl: %d\n",i1.overs[4].ds[5].del_type.cnl);

	//********************************************************************************************************************
	//      getting i1.overs[0].ds[ndel].batsaman
	unsigned char i1_overs_del_type_mini_batsman_sub_str[] = "        batsman:";
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 16; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i1.start;ii<i1.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_type_mini_batsman_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.overs[cnt].ds[cnt0].striker.cnl = ii;// line number of a del_type
			i1.overs[cnt].ds[cnt0].non_striker.start = 17;//all del_types starts from 11th index(including legbyes, wides)
			cnt0++;
		}
	}

	//	printf("i1.overs[0].ds[3].striker.cnl: %d\n",i1.overs[0].ds[3].striker.cnl);
	c=0;
	i=0;
	//	for(i =0; c != '\n';i++){ //printing the bowler of 9th over
	//		c = (buf[line[i1.overs[0].ds[3].striker.cnl].start_index+i+17]);
	//		printf("%c",c);
	//	}

	//********************************************************************************************************************
	//      getting i2.overs[0].ds[ndel].batsaman
	unsigned char i2_overs_del_type_mini_batsman_sub_str[] = "        batsman:";
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 16; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i2.start;ii<i2.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i2_overs_del_type_mini_batsman_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i2.overs[cnt].ds[cnt0].striker.cnl = ii;// line number of a del_type
			i2.overs[cnt].ds[cnt0].non_striker.start = 17;//all del_types starts from 11th index(including legbyes, wides)
			cnt0++;
		}
	}

	//	printf("i2.overs[0].ds[3].striker.cnl: %d\n",i2.overs[0].ds[3].striker.cnl);
	//	c=0;
	//	i=0;
	//	for(i =0; c != '\n';i++){ //printing the bowler of 9th over
	//		c = (buf[line[i2.overs[0].ds[3].striker.cnl].start_index+i+17]);
	//		printf("%c",c);
	//	}
	//**********************************************************************************
	//      getting i1.overs[0].ds[ndel].non_striker
	unsigned char i1_overs_del_type_non_striker_sub_str[] = "        non_striker:";
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 20; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i1.start;ii<i1.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_type_non_striker_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.overs[cnt].ds[cnt0].non_striker.cnl = ii;// line number of a del_type
			i1.overs[cnt].ds[cnt0].non_striker.start = 21;//all del_types starts from 11th index(including legbyes, wides)
			cnt0++;
		}
	}

	//        printf("i1.overs[0].ds[3].non_striker.cnl: %d\n",i1.overs[0].ds[3].non_striker.cnl);
	//        c=0;
	//        i=0;
	//        for(i =0; c != '\n';i++){ //printing the bowler of 9th over
	//                c = (buf[line[i1.overs[0].ds[3].non_striker.cnl].start_index+i+21]);
	//                printf("%c",c);
	//        }
	//**********************************************************************************
	//      getting i2.overs[0].ds[ndel].non_striker
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 20; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i2.start;ii<i2.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_type_non_striker_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i2.overs[cnt].ds[cnt0].non_striker.cnl = ii;// line number of a del_type
			i2.overs[cnt].ds[cnt0].non_striker.start = 21;//all del_types starts from 11th index(including legbyes, wides)
			cnt0++;
		}
	}

	printf("i2.overs[0].ds[3].non_striker.cnl: %d\n",i2.overs[0].ds[3].non_striker.cnl);
	c=0;
	i=0;
	for(i =0; c != '\n';i++){ //printing the bowler of 9th over
		c = (buf[line[i2.overs[0].ds[3].non_striker.cnl].start_index+i+21]);
		printf("%c",c);
	}
	//**********************************************************************************
	//      getting i1.overs[0].ds[ndel].runs
	unsigned char i1_overs_del_type_runs_sub_str[] = "          total:";
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 16; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i1.start;ii<i1.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_type_runs_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.overs[cnt].ds[cnt0].runs.cnl = ii;// line number of a del_type
			i1.overs[cnt].ds[cnt0].runs.start = 10;//all del_types starts from 11th index(including legbyes, wides)
			cnt0++;
		}
	}

	//        printf("i1.overs[cnt].ds[cnt0].runs.cnl: %d\n",i1.overs[0].ds[2].runs.cnl);
	//        c=0;
	//        i=0;
	//        for(i =0; c != '\n';i++){ //printing the bowler of 9th over
	//                c = (buf[line[i1.overs[0].ds[2].runs.cnl].start_index+i+10]);
	//                printf("%c",c);
	//        }
	//**********************************************************************************
	//      getting i2.overs[0].ds[ndel].runs
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 16; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i2.start;ii<i2.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_type_runs_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i2.overs[cnt].ds[cnt0].runs.cnl = ii;// line number of a del_type
			i2.overs[cnt].ds[cnt0].runs.start = 10;//all del_types starts from 11th index(including legbyes, wides)
			cnt0++;
		}
	}

	//        printf("i2.overs[0].ds[4].runs.cnl: %d\n",i2.overs[0].ds[4].runs.cnl);
	//        c=0;
	//        i=0;
	//        for(i =0; c != '\n';i++){ //printing the bowler of 9th over
	//                c = (buf[line[i2.overs[0].ds[4].runs.cnl].start_index+i+10]);
	//                printf("%c",c);
	//        }
	//**************************************************************************************************************
	//getting i1.overs[0].ds[ndel].outcome// currently it will only show wicket only(further I'll add more details)
	unsigned char i1_overs_del_outcome_sub_str[] = "          player_out:";
	cnt = -1;//over[cnt] counter
	cnt0 = 1;//ds[cnt0] counter
	len = 21; // length of a sub_string is not changing so we counted it
	j=0;
	ii=0;
	for(ii=i1.start;ii<i1.end;ii++){
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) == '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){
			cnt = cnt + 1;
			cnt0 = 1;
			//printf("ii: %d\n", ii);
		}
		for(j=0;j<line[ii].length-1;j++){
			if((buf[line[ii].start_index+j]) != i1_overs_del_outcome_sub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			i1.overs[cnt].ds[cnt0].outcome.cnl = ii;// line number of a del_type
			printf("i1.overs[cnt].ds[cnt0].outcome.cnl: %d\n", i1.overs[cnt].ds[cnt0].outcome.cnl);
			i1.overs[cnt].ds[cnt0].outcome.start = 10;//all del_types starts from 11th index(including legbyes, wides)
		}
		if(((buf[line[ii].start_index + line[ii].length -4]) == '.') && ((buf[line[ii].start_index + line[ii].length -3]) != '1') && ((buf[line[ii].start_index + line[ii].length -2]) == ':')){ 
			cnt0++;
		}
	}
	printf("i1.overs[4].ds[2].outcome.cnl: %d\n",i1.overs[4].ds[2].outcome.cnl);
	unsigned long ptr=1;
	//  cnt = 0; // we shall use the cnt again for our computation
	//  printf("\n---------Begin-------\n");
	//  while (cnt < 2) 
	//  {
	//     printf("Line %ld: ", (cnt));
	//     fwrite((buf+ptr), 1, line[cnt].length, stdout);
	//     ptr = ptr + line[cnt].length;
	//     ++cnt;
	//  }   
	//  printf("\n---------End---------\n");

	// printing data
	printf("\n---------------------------------------Match Summary-----------------------------------------\n");
	cnt = 0; // we shall use the cnt again for our computation
	ptr=1;
	printf("\nTeam 1: ");
	fwrite((buf+line[teams.team1.cnl].start_index+teams.team1.start), 1, line[teams.team1.cnl].length-teams.team1.start-1, stdout);
	printf("\nTeam 2: ");
	fwrite((buf+line[teams.team2.cnl].start_index+teams.team2.start), 1, line[teams.team2.cnl].length-teams.team2.start, stdout);
	printf("\n------------------------------------Player Names of 1st Team-----------------------------------\n");
	c = 0;
	for(unsigned char z=0;z<11;z++){
		for(i =0; c != '\n';i++){ //printing the 1st player only
			c = (buf[line[p1.t1_players.cnl+z].start_index+p1.t1_players.start+i]);
			printf("%c",c);
		}c=0;
	}
	printf("\n------------------------------------Player Names of 2nd Team-----------------------------------\n");
	c = 0;
	for(unsigned char z=0;z<11;z++){
		for(i =0; c != '\n';i++){ //printing the 1st player only
			c = (buf[line[p2.t2_players.cnl+z].start_index+p2.t2_players.start+i]);
			printf("%c",c);
		}c=0;
	}
	printf("\n------------------------------------Inning 1 Info----------------------------------------------\n");
	printf("Number of overs: %d", i1.novers);
	printf("\nStart line number: %d", i1.start);
	printf("\nEnd line number: %d", i1.end);
	printf("\nOvers starting line: %d\n", i1.overs[0].start);
	


	free(lines); //finally free the allocated memory
	free(buf);
	return 0;
}

