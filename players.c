#include<stdio.h>
#include<string.h>
#include "getTeamNames.h"

int main(){
	char teamNames[2][20];
	team(teamNames);
	for(int i=0;i<2;i++){
		printf("teamNames[%d]=%s",i,teamNames[i]);
	}
	return 0;
}
