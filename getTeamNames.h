# include<stdio.h>
# include<string.h>
# include<ctype.h>

void team(char teamNames[][20]){
	FILE *fp;
	int flag = 0, j=0;
	char arr[100];
	fp = fopen("yamlFile","r+");
	if(fp == NULL){
		printf("The file is not present");
	}
	else{
		while(fgets(arr, sizeof(arr), fp) != NULL){
			if(strstr(arr, "teams")){
				flag = 1;
				continue;
			}
			if(strstr(arr, "toss")){
				flag = 0;
			}
			if(flag){
				strcpy(teamNames[j], arr);
				j++;
			}
		}
		fclose(fp);
	}
}

