#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define ERR (1)



int main(int argc, char **argv){
	if(argc <2){
		fprintf(stderr, "Usage: %s <input file name>\n", argv[0]);
		return 2;
	}
	printf("argc: %d\n", argc);
	for(int l=1;l<argc;l++){

		size_t size;
		FILE *f = fopen(argv[l], "rb");
		if(!f) return ERR;

		if(fseek(f, 0, SEEK_END) != 0){
			fclose(f); return ERR;
		}
		size = ftell(f);
		if(size < 0){
			fclose(f); return ERR;
		}
		if(fseek(f, 0, SEEK_SET) != 0){
			fclose(f); return ERR;
		}

		char *buf = malloc(size);
		if(!buf){
			fclose(f); return ERR;
		}
		size_t read = fread(buf, 1, size, f);
		if(read != size){
			if(ferror(f)){
				free(buf); fclose(f); return ERR;
			}
		}
		fclose(f);
		if(!buf){
			perror("Failed to read file");
			return ERR;
		}

		const char *match = buf;
		int flag=0;
		int i, j, line=1;
		const char sub_str[] = "  venue: ";
		int len = strlen(sub_str);
		for(i=0;i<=(size-len);i++){
			int k = i;
			if(buf[k] == '\n'){
				line = line + 1;
			}
			for(j=0;j<len;j++){
				if(buf[k] != sub_str[j]){
					break;
				}
				k++;
			}
			if(j == len){
				flag = 1;
				break;
			}
		}
		if(!flag){
			printf("Not found at: %s\n", argv[l]);
		}else{
			printf("In file <%s> Found at line: %d\n", argv[l], line);
		}
	}

	return 0;
}
