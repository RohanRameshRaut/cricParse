#include<stdio.h>

#define STRIKER_KEY "  batsman:"
#define STRIKER_KEY_LEN 10
#define NON_STRIKER_KEY "  non_striker:"
#define NON_STRIKER_KEY_LEN 14

int main(){
	unsigned char *sub_str = STRIKER_KEY, len = STRIKER_KEY_LEN;
	int i =0;
	for(i=0;i<len;i++){
		printf("%c", sub_str[i]);
	}
	printf("\n");
	sub_str = NON_STRIKER_KEY, len = NON_STRIKER_KEY_LEN;
	i =0;
	for(i=0;i<len;i++){
		printf("%c", sub_str[i]);
	}
	printf("\n");
	return 0;
}
