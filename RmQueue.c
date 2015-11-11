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
#define QUEUE_TEMP "/home/user01/queue/temp.txt"
#define QUEUE_INDEX "/home/user01/queue/index.txt"


int checkOwnership(char *name, int id) {
	FILE *f = fopen(QUEUE_INDEX, "r");
	if(f == NULL) return 0;
	char line[1024] = "";
	int size = 1024;
	while(fgets(line, size, f) != NULL) {
		char *token = strtok(line, ",");
		if(token == NULL) continue;
		if(strcmp(token, name) == 0) {
			fclose(f);
			int fileId = atoi(strtok(NULL, ","));
			return (fileId == id);
		}
	}
		
	fclose(f);
	return 0;
}

int removeFile(char *name) {
	FILE *f = fopen(QUEUE_INDEX, "r");
	if(f == NULL) return -1;
	
	FILE *temp = fopen(QUEUE_TEMP, "w+");
	if(temp == NULL) return -1;
	
	char line[1024] = "";
	int size = 1024;
	while(fgets(line, size, f) != NULL) {
		char lineCpy[1024];
		strncpy(lineCpy, line, strlen(line)+1);
		char *token = strtok(line, ",");
		if(token == NULL) continue;
		if(strcmp(token, name) != 0) {
			fprintf(temp, "%s", lineCpy);
		}
	}
		
	
	fclose(f);
	remove(QUEUE_INDEX);
	
	FILE *newF = fopen(QUEUE_INDEX, "w");
	
	rewind(temp);	
	while(fgets(line, size, temp) != NULL) {
		fprintf(newF, "%s", line);
	}
	
	
	fclose(temp);
	remove(QUEUE_TEMP);
	fclose(newF);
	
	char path[1024];
	strncpy(path, QUEUE_DIR, strlen(QUEUE_DIR)+1);
	strncat(path, name, strlen(name)+1);
	
	return remove(path);
}

char *removeFromQueue(char *rmName, uid_t id) {
	DIR *dir;
	struct dirent *f;
	
	if((dir = opendir(QUEUE_DIR)) != NULL) {
		while((f = readdir(dir)) != NULL) {
			if(strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0 || strcmp(f->d_name, "index.txt") == 0) {
				continue;
			}
			char name[128] = "", uniqueName[128];
			strncpy(name, f->d_name, strlen(f->d_name)+1);
			char *token = strtok(name, "_");
			//strncpy(uniqueName, f->d_name, strlen(f->d_name)+1);
			strncpy(uniqueName, f->d_name+(strlen(token)+1), (strlen(f->d_name) - strlen(token)));
			if(strcmp(uniqueName, rmName) == 0) {
				if(checkOwnership(f->d_name, id)) {
					if(removeFile(f->d_name) == -1) {
						return "Could not remove file from spool queue directory";
					}else {
						return NULL;
					}
				}else {
					return "You Can't remove a file that you do not own";
				}
			}
		}
		closedir(dir);
	}else {
		printf("Could not open queue directory\n");
	}
	return "File does not exist in queue";
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
        printf("\nUSAGE:\n");
        printf("List of Files to remove from queue\n");
        return 1;
    }
	
	uid_t id = getuid();
	
  	int i = 1;
    for(;i<argc;i++) {
    	char *error = removeFromQueue(argv[i], id);
    	if(error == NULL) {
    		printf("%s: Y\n", argv[i]);
    	}else {
    		printf("%s: X %s\n", argv[i], error);
    	}
    }
    
    return 0;
}
