# include<stdio.h>
# include<string.h>
# include "getTeamNames.h"  //used to get the team names.

const char *invalid = "Data is invalid\n";

typedef struct Team {
	char team_name[50];
	char players[11][50];
	int total_runs, total_wickets;
} Team;

void getPlayers01(Team *t1, Team *t2, char playerNames[][50]){
	int flag=0,j=0;
	char dest[50] ="    ", dest1[50] ="    ", buff[100];
	strcat(dest1,t2->team_name);
	strcat(dest,t1->team_name);
	FILE *fp = fopen("yamlFile", "r");
	if(fp == NULL){
		printf("Cannot open file.");
	}
	else{
		while(fgets(buff,sizeof(buff), fp)!=NULL){
			if(strstr(buff, dest)){
				flag = 1;
				continue;
			}
			if(strstr(buff, dest1)){
				flag = 0;
			}
			if(flag){
				sscanf(buff, "   - %[^\n]",playerNames[j++]);
			}
		}
	}
}


void getPlayers02(Team *t2, char playerNames[][50]){
	int flag=0,j=0;
	char dest1[50] ="    ", buff[100];
	strcat(dest1,t2->team_name);
	FILE *fp = fopen("yamlFile", "r");
	if(fp == NULL){
		printf("Cannot open file.");
	}
	else{
		while(fgets(buff,sizeof(buff), fp)!=NULL){
			if(strstr(buff, dest1)){
				flag = 1;
				continue;
			}
			if(strstr(buff, "registry:")){
				flag = 0;
			}
			if(flag){
				sscanf(buff, "   - %[^\n]",playerNames[j++]);
			}
		}
	}
}

void ranger(char tossResult[][50], const char *start_from, const char *last_to){
	int flag = 0, j=0;
	char arr[100];
	FILE *fp = fopen("yamlFile","r");
	if(fp == NULL){
		printf("Cannot open file.");
	}
	else{
		while(fgets(arr, sizeof(arr), fp) != NULL){
			if(strstr(arr, start_from)){
				flag = 1;
				continue;
			}
			if(strstr(arr, last_to)){
				flag = 0;
			}
			if(flag){
				sscanf(arr, "    %*s %[^\n]",tossResult[j++]);
			}
		}
	}
}

//toss validation 
//condition01: winner should belongs to {t1.team_name,t2.team_name}
//condition02: if decision is bat winner will have 1st innings or if decision is bowl winner will have 2nd innings
void tossValidation(Team *t1, Team *t2, char tossResult[2][50], char firstInningsTeamName[1][50], char secondInningsTeamName[1][50], char firstInningsPlayer[1000][1000], char secondInningsPlayer[1000][1000]){
	int flag = 0,j=0;
	char tossWinner[50], tossLoser[50];
	if(strcmp(t1->team_name,tossResult[1]) == 0){
		strcpy(tossWinner, tossResult[1]);
		strcpy(tossLoser, t2->team_name);
		flag = 1;
	}
	else if(strcmp(t2->team_name,tossResult[1]) == 0){
		strcpy(tossWinner, tossResult[1]);
		strcpy(tossLoser, t1->team_name);
		flag = 1;
	}
	else{
		printf("At toss1: %s",invalid);
	}

	if(flag == 1){
		printf("inside the flag = 1 condition\n");

		if(strcmp(tossResult[0], "bat") == 0 && strcmp(tossResult[1], firstInningsTeamName[0]) == 0 && strcmp(secondInningsTeamName[0], tossLoser) == 0){
			printf("Toss is valid\n");
			if(strcmp(t1->team_name, firstInningsTeamName[0]) == 0){
				for(int i=0;i<616;i++){
					int f1 = 0;
					for(int j=0;j<11;j++){
						if(strcmp(firstInningsPlayer[i], t1->players[j]) == 0){
							f1 = 1;
							break;
						}
					}
					if(!f1){
						printf("First Innings players are not valid: %s\n", firstInningsPlayer[i]);
						break;
					}
				}
				for(int i=0;i<616;i++){
					int f1 = 0;
					for(int j=0;j<11;j++){
						if(strcmp(secondInningsPlayer[i], t2->players[j]) == 0){
							f1 = 1;
							break;
						}
					}
					if(!f1){
						printf("Second Innings players are not valid: %s\n", secondInningsPlayer[i]);
						break;
					}
				}
			}
			else if(strcmp(t2->team_name, firstInningsTeamName[0]) == 0){

				for(int i=0;i<616;i++){
					int f1 = 0;
					for(int j=0;j<11;j++){
						if(strcmp(firstInningsPlayer[i], t2->players[j]) == 0){
							f1 = 1;
							break;
						}
					}
					if(!f1){
						printf("First Innings players are not valid: %s\n", firstInningsPlayer[i]);
						break;
					}
				}
				for(int i=0;i<616;i++){
					int f1 = 0;
					for(int j=0;j<11;j++){
						if(strcmp(secondInningsPlayer[i], t1->players[j]) == 0){
							f1 = 1;
							break;
						}
					}
					if(!f1){
						printf("Second Innings players are not valid: %s\n", secondInningsPlayer[i]);
						break;
					}
				}
			}
		}
		else if (strcmp(tossResult[0], "ball") == 0 && strcmp(tossResult[1], secondInningsTeamName[0]) == 0 && strcmp(firstInningsTeamName[0], tossLoser) == 0){
			printf("Toss is valid\n");
			if(strcmp(t1->team_name, secondInningsTeamName[0]) == 0){

				for(int i=0;i<616;i++){
					int f2 = 0;
					for(int j=0;j<11;j++){
						if(strcmp(secondInningsPlayer[i], t1->players[j]) == 0){
							f2 = 1;
							break;
						}
					}
					if(!f2){
						printf("Second Innings players are not valid: %s\n", secondInningsPlayer[j]);
						break;
					}
				}
			}
			else if(strcmp(t2->team_name, secondInningsTeamName[0]) == 0){

				for(int i=0;i<616;i++){
					int f2 = 0;
					for(int j=0;j<11;j++){
						if(strcmp(secondInningsPlayer[i], t2->players[j]) == 0){
							f2 = 1;
							break;
						}
					}
					if(!f2){
						printf("Second Innings players are not valid: %s\n", secondInningsPlayer[j]);
						break;
					}
				}
			}
		}
		else{
			printf("%s", invalid);
		}
	}
}


void getBatsmanFromInnings(const char *start_from, const char *last_to, char allPlayers[][1000]){
	int flag = 0, j=0;
	char arr[100], batsman[50], non_striker[50], player_out[50], buff[100];
	char *pos;
	FILE *fp = fopen("yamlFile","r");
	if(fp == NULL){
		printf("Cannot open file.");
	}
	else{
		while(fgets(arr, sizeof(arr), fp) != NULL){
			if(strstr(arr, start_from)){
				flag = 1;
				continue;
			}
			if(strstr(arr, last_to)){
				flag = 0;
			}
			if(flag){
				if(sscanf(arr, " batsman: %[^\n0-9]",allPlayers[j]) == 1){
					j++;
				}
				if(sscanf(arr, " non_striker: %[^\n0-9]",allPlayers[j]) == 1){
					j++;
				}
				if(sscanf(arr, " player_out: %[^\n]",allPlayers[j]) == 1){
					j++;
				}
			}
		}
	} //printf("Value of j: %d\n", j);
}


int main(){
	Team t1,t2;
	char teamNames[2][20],tossResult[2][50],firstInningsTeamName[1][50], secondInningsTeamName[1][50], firstInningsPlayer[1000][1000], secondInningsPlayer[1000][1000];
	t1.players[11][50];
	team(teamNames);
	sscanf(teamNames[0], "  - %[^\n]",t1.team_name);
	sscanf(teamNames[1], "  - %[^\n]",t2.team_name);

	getPlayers01(&t1,&t2, t1.players);
	printf("Team 1:%s\n",t1.team_name);
	for(int i=0;i<11;i++){
		printf("%s\n",t1.players[i]);
	}

	getPlayers02(&t2, t2.players);
	printf("\nTeam 2:%s\n",t2.team_name);
	for(int i=0;i<11;i++){
		printf("%s\n",t2.players[i]);
	}

	// get the toss
	ranger(tossResult, "toss", "umpires");
	ranger(firstInningsTeamName, "- 1st innings", "deliveries");
	ranger(secondInningsTeamName, "- 2nd innings", "deliveries");

	printf("\nToss Result:\n");
	printf("%s\n",tossResult[0]);
	printf("%s\n",tossResult[1]);

	//batsaManValidation
	getBatsmanFromInnings("1st innings", "2nd innings", firstInningsPlayer);
	getBatsmanFromInnings("2nd innings", "last", secondInningsPlayer);

	//tossValidation and innings players validation
	tossValidation(&t1, &t2, tossResult, firstInningsTeamName, secondInningsTeamName, firstInningsPlayer, secondInningsPlayer); 
	return 0;
}


