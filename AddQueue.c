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


/*
	Each file in the queue has a unique number to append to
	the file name. This ensures the filenames are unique. The
	number is the first character with an underscore: "_" after it
*/
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

/*
	Add the filename and corresponding user id to the
	index file in the form: <filename>,<userid>
*/
char *addToIndex(char *fileName, uid_t id) {
	FILE *f = fopen(QUEUE_INDEX, "a");
	if(f == NULL) {
		return "Could not access index file to append";
	}
	fprintf(f, "%s,%d\n", fileName, id);
	fclose(f);
	return NULL;
}


/*
	Copies a file into the protected spool queue directory.
	To open the source file, setuid to the calling user id.
	Switch back to setuid of the spool user id: user01 after
	to create a file in the spool queue directory.
*/
char *copyFileToQueue(char *path, int number) {
	
	//setuid to calling userid to open source file
	uid_t spoolId = geteuid();
	setuid(getuid());
	FILE *source = fopen(path, "r");
	if(source == NULL) {
		setuid(spoolId);
		return "Could not access source file";
	}
	
	char *baseDest = basename(path);
	if(baseDest == NULL) {
		setuid(spoolId);
		return "Could not get the base file name of source file";
	}
	
	//create file name of file in spool queue
	char destPath[1024], strNum[8], fileName[1024];
	int size = log10(number) + 2;
	strncpy(destPath, QUEUE_DIR, strlen(QUEUE_DIR)+1);
	snprintf(strNum, size, "%d", number);
	strncpy(fileName, strNum, size);
	strncat(fileName, "_", 2);
	strncat(fileName, baseDest, strlen(baseDest)+1);
	strncat(fileName, "_", 2);
	strncat(fileName, strNum, size);
	strncat(destPath, fileName, strlen(fileName)+1);

	//setuid back to spool user to create file in queue directory
	setuid(spoolId);
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
        printf("addqueue <file1> [<file2> <file3>...] etc");
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
		uniqueNumber--;
    	}
    }
    
    return 0;
}
