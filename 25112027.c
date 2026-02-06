// Rohan Ramesh Raut
// MCA-I
//*************************************************************
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
	unsigned char player_index; // p1.t1_players.start
	ContentT del_no;
	ContentT del_type;
	ContentT runs;
	ContentT outcome;
}delT;

typedef struct overT{
	unsigned short start;
	delT *ds;
	unsigned char over_no, ndel, player_index;
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
					break;
				}
				temp++;
			}
		}
		lncnt++;
	}
	printf("team1.cnl: %d\n", team1.cnl);
	printf("team1.start: %d\n", team1.start);
	printf("Team1 name: ");
	unsigned char c, i;
	for(i =0; c != '\n';i++){
		c = (buf[line[team1.cnl].start_index+team1.start+i]);
		printf("%c",c);
	}
	//printf("team1 name: %c\n", buf[line[team1.cnl].start_index+team1.start]);
	printf("team2.cnl: %d\n", team2.cnl);
	printf("team2.start: %d\n", team2.start);

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
	printf("team1_1st_player.cnl: %d\n", p1.t1_players.cnl);
	printf("team1_1st_player.start: %d\n", p1.t1_players.start);
	printf("Team1_1st_player_name: \n");
	c=0, i=0;
	for(unsigned char z=0;z<11;z++){ //printing all 11 players
		for(i =0; c != '\n';i++){ //printing the 1st player only
			c = (buf[line[p1.t1_players.cnl+z].start_index+p1.t1_players.start+i]);
			//printf("%c",c);
		}c=0;}
	printf("team2_1st_player.cnl: %d\n", p2.t2_players.cnl);
	printf("team2_1st_player.start: %d\n", p2.t2_players.start);

	// getting delivery object 
	delT d1;
	lncnt = 0; //reusing the lncnt for line count
	unsigned char dsub_str[] = "- 1st innings:";
	len = 14; // length of a sub_string is not changing so we counted it
	unsigned char wides_sub_str[] = "          wides:";
	unsigned char wicket_sub_str[] = "        wicket:";
	unsigned char wide_sub_str_len = 16, wicket_sub_str_len = 15;
	temp = 0;
	j=0;
	while(lncnt < nl){
		for(j=0;j<line[lncnt].length-1;j++){
			if((buf[line[lncnt].start_index+j]) != dsub_str[j]){
				break;
			}
		}
		if(j == len){ //if match found, do the below task
			d1.del_no.cnl  = lncnt + 3; // next lines after the pattern
			d1.start = lncnt + 3;// line number of starting of deliveries
			temp = line[lncnt+3].start_index;
			while(temp < temp+line[lncnt+3].length-2){
				if((('0' <=buf[temp]) && (buf[temp] <= '9')) || buf[temp] == '.'){ // get only 0 to 9 and .
					d1.del_no.start = temp - line[lncnt+3].start_index; //get the actual content index(0.1)
					break;
				}
				temp++;
			}
		}
		lncnt++;
	} 
	//printf("d1.start: %d\n", d1.start);
	//printf("d1.del_no.cnl: %d\n", d1.del_no.cnl);
	//printf("d1.del_no.start: %d\n", d1.del_no.start);
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//		we'll get the player index from <playerIndexTable> for 1: batsman(immidiately after the del_no) 2: bowler(immidiately after the batsman)
	//		for 3: non_striker is after the check of <wideBall>
	//		<wideBall> check after the bowler(within extras:), if yes then del_type.cnl and del_start else -1 or null.
	//		<runs>(we'll go with totals only, though we can adjust according to our need) (we can get the run from the non_strikers line number and starting index)
	//		<outcome> after 3 lines of runs: there could be a "wicket:" section if yes then outcome.cnl and outcome.start of content(right after the wicket) else -1 or null
//	while(p2.t2_players.cnl < p2.t2_players.cnl + 11){
//		c = 0;
//		for(i=0; c != '\n';i++){
//			c = p2.t2_players.cnl+p2.t2_players.start+i;
//			for(j=0;j<line[d1.del_no.cnl+1].length-1;j++){
//				if((buf[c]+j) != (buf[line[d1.del_no.cnl+1].start_index+17+j])){
//					break;
//				}
//			} 
//			if(j == (line[d1.del_no.cnl+1].length)){
//				d1.player_index = p2.t2_players.cnl;
//				break;
//			}
//		}
//		p2.t2_players.cnl ++;
//	}
//	printf("d1.player_index: %d\n", d1.player_index);
	free(lines); //finally free the allocated memory
	free(buf);
	return 0;
}

