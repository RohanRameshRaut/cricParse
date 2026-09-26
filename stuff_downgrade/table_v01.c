#include<stdio.h>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

void print_pat(int *arr, unsigned char len, unsigned char a, unsigned char b){
	printf("%c", a);
	for(int i=0;i<len;i++){
		for(int j=0;j<arr[i];j++){
			printf("%c", b);
		}
		printf("%c", a);
	}
}
void print_data(unsigned char row, unsigned char col, unsigned char *value){
	unsigned char flag = 1;
	for(int i=0;i<row;i++){
		for(int j=0;j<col;j++){
			if(value[j] == '\0') flag = 0;//use '\n' for yaml
			if(flag){
				printf("%c", value[j]);
			}
			else{
				printf(" ");
			}
		}
		printf("|");
	}
}

int main(){
	int arr[] = {10, 5, 5, 10};
	unsigned char bname[] = "Batsman";
	unsigned char blname[] = "Bowler";
	unsigned char run[] = "Run";
	unsigned char ball[] = "Ball";
	unsigned char bl[] = "33";
	unsigned char r[] = "100";
	unsigned char len = ARRAY_SIZE(arr);
	unsigned char a = '+', b = '-';
	print_pat(arr, len, a, b); 
	printf("\n");
	printf("|");
	print_data(1, 10, bname);
	print_data(1, 5, run);
	print_data(1, 5, ball);
	print_data(1, 10, blname);
	printf("\n");
	print_pat(arr, len, a, b); 
	printf("\n");

	return 0;
}

