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

	//testing the ascii total run counter
	unsigned char *buf = "532", total = 0;
	printf("buf[0] ASCII: %d, VALUE: %c\n", buf[0], buf[0]);
	printf("buf[1] ASCII: %d, VALUE: %c\n", buf[1], buf[1]);
	printf("buf[2] ASCII: %d, VALUE: %c\n", buf[2], buf[2]);

	total += (char)buf[0] - 48;
	total += (char)buf[1] - 48;
	total += (char)buf[2] - 48;

	printf("total: %d\n", total);

	return 0;
}
