#include<stdio.h>
#include<stdlib.h>

#define STRIKER_KEY "  batsman:"
#define STRIKER_KEY_LEN 10
#define NON_STRIKER_KEY "  non_striker:"
#define NON_STRIKER_KEY_LEN 14
#define ARR {"Rohan", "Sahil", "Mahadev"};
#define ARR_LEN 3

int main(){
	unsigned char *sub_str = STRIKER_KEY, len = STRIKER_KEY_LEN;
	unsigned char *sub_str2[] = ARR;
	unsigned char cnt=0, val = 1;
	unsigned char *arr_len = (char*) malloc(3*sizeof(char));
	//	int i =0;
	//	for(i=0;i<len;i++){
	//		printf("%c", sub_str[i]);
	//	}
	//	printf("\n");
	//	sub_str = NON_STRIKER_KEY, len = NON_STRIKER_KEY_LEN;
	//	i =0;
	//	for(i=0;i<len;i++){
	//		printf("%c", sub_str[i]);
	//	}
	//	printf("\n");
	//
	//	//testing the ascii total run counter
	//	unsigned char *buf = "532", total = 0;
	//	printf("buf[0] ASCII: %d, VALUE: %c\n", buf[0], buf[0]);
	//	printf("buf[1] ASCII: %d, VALUE: %c\n", buf[1], buf[1]);
	//	printf("buf[2] ASCII: %d, VALUE: %c\n", buf[2], buf[2]);
	//
	//	total += (char)buf[0] - 48;
	//	total += (char)buf[1] - 48;
	//	total += (char)buf[2] - 48;
	//

	for(int i=0;i<ARR_LEN;i++){
		for(char j=0;sub_str2[i][j]!='\0';j++){
			cnt += 1;
		}
		arr_len[i] = cnt;
		printf("arr_len[%d]: %d\n", i, arr_len[i]);
		cnt = 0;
	}
	cnt = 0;
	cnt = cnt | (1 << 7);
	cnt = cnt | (1 << 6);
	printf("cnt: %d\n", cnt);

	val = 28;
	if(val == 28) printf("it's present\n");

	return 0;
}
