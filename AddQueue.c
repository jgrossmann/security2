#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>
#include<dirent.h>
#include<libgen.h>
#include<math.h>

#define QUEUE_DIR "/home/user01/queue/"
#define QUEUE_INDEX "/home/user01/queue/index.txt"

int getNextUniqueNumber() {
	DIR *dir;
	struct dirent *f;
	int lastNum = 0;
	
	if((dir = opendir(QUEUE_DIR)) != NULL) {
		while((f = readdir(dir)) != NULL) {
			char *token = strtok(f->d_name, "_");
			if(token == NULL) continue;
			
			int num = atoi(token);
			if(lastNum < num) lastNum = num;
		}
		closedir(dir);
	}
	return lastNum + 1;
}

char *addToIndex(char *fileName, uid_t id) {
	FILE *f = fopen(QUEUE_INDEX, "a+");
	if(f == NULL) {
		return "Could not access index file to append";
	}
	
	fprintf(f, "%s,%d\n", fileName, id);
	fclose(f);
	return NULL;
}


char *copyFileToQueue(char *path, int number) {
	FILE *source = fopen(path, "r");
	if(source == NULL) {
		return "Could not access source file";
	}
	
	char *baseDest = basename(path);
	if(baseDest == NULL) {
		return "Could not get the base file name of source file";
	}
	
	char destPath[1024], strNum[8], fileName[1024];
	int size = log10(number) + 2;
	strncpy(destPath, QUEUE_DIR, strlen(QUEUE_DIR));
	snprintf(strNum, size, "%d", number);
	strncpy(fileName, strNum, size-1);
	strncat(fileName, "_", 1);
	strncat(fileName, baseDest, strlen(baseDest));
	strncat(fileName, "_", 1);
	strncat(fileName, strNum, size);
	strncat(destPath, fileName, strlen(fileName)+1);

	FILE *dest = fopen(destPath, "w+");
	if(dest == NULL) {
		return "Could not create new file in spool queue directory";
	}
	
	
	char ch;
	while( (ch = fgetc(source)) != EOF) {
		fputc(ch, dest);
	}
	
	fclose(source);
	fclose(dest);
	
	return addToIndex(fileName, getuid());
}


int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("\nUSAGE:\n");
        printf("List of Files to add to queue\n");
        return 1;
    }
    
    int uniqueNumber = getNextUniqueNumber();
	if(uniqueNumber == 0) uniqueNumber++;
    
    int i = 1;
    for(;i<argc;i++) {
    	char *error = copyFileToQueue(argv[i], uniqueNumber++);
    	if(error == NULL) {
    		printf("%s: Y\n", argv[i]);
    	}else {
    		printf("%s: X %s\n", argv[i], error);
    	}
    }
    
    return 0;
}
