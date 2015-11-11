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
#include<time.h>

#define QUEUE_DIR "/home/user01/queue/"
#define QUEUE_INDEX "/home/user01/queue/index.txt"


/*
	Holds all information needed to print for each file in queue
*/
typedef struct queueEntry {
	char name[512];		//number specifying order in which added to queue
	int owner;
	time_t time;
	char uniqueName[512];	//orignal fileName with "name" prepended and appended
	struct queueEntry *next;
	struct queueEntry *prev;
} queueEntry;

int getNumFiles() {
	DIR *dir;
	struct dirent *f;
	int numFiles = 0;
	
	if((dir = opendir(QUEUE_DIR)) != NULL) {
		while((f = readdir(dir)) != NULL) {
			if(strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0) {
				continue;
			}
			numFiles++;
		}
		closedir(dir);
	}
	return numFiles;
}

//prints each file in queue stored as linked list
void printList(queueEntry *root) {
	if(root == NULL) return;
	while(root != NULL) {
		struct tm *t = localtime(&(root->time));		
		printf("%s %d %d-%d-%d_%d:%d:%d %s\n", root->name, root->owner, (1900+t->tm_year), t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, root->uniqueName);
		root = root->next;
	}
}

//cleanup linked list
void deleteEntries(queueEntry *root) {
	if(root == NULL) return;
	while(root->next != NULL) {
		root = root->next;		
		free(root->prev);
	}
	free(root);
}

/*
	Goes into the index.txt file to find the userid of the user
	who added the file specified by fileName. Each line is of the
	format: <fileName>,<userid>
*/
int getUserId(char *fileName) {
	FILE *f = fopen(QUEUE_INDEX, "r");
	if(f == NULL) return -1;
	char line[1024];
	int size = 1024;
	while(fgets(line, size, f) != NULL) {
		char *token = strtok(line, ",");
		if(token == NULL) continue;
		if(strcmp(token, fileName) == 0) {
			fclose(f);
			return atoi(strtok(NULL, ","));
		}
	}
	
	fclose(f);
	return 0;
}

/*
	Inserts a new queueEntry node into the linked list sorted
	by queueEntry->name which is a unique number specifying the
	order in which a file was added to the queue.
*/
queueEntry *insert(queueEntry *root, queueEntry *newEntry) {
	if(root == NULL) {
		return newEntry;
	}else {
		queueEntry *curEntry = root;
		while(curEntry->next != NULL) {
			//if(newEntry->time < curEntry->time) {
			if(atoi(newEntry->name) < atoi(curEntry->name)) {
				if(curEntry->prev != NULL) {
					curEntry->prev->next = newEntry;
					newEntry->prev = curEntry->prev;
					curEntry->prev = newEntry;
					newEntry->next = curEntry;
					return root;
				}else {
					newEntry->next = curEntry;
					curEntry->prev = newEntry;
					return newEntry;
				}
			}
			curEntry = curEntry->next;
		}
		
		//if(newEntry->time < curEntry->time) {
		if(atoi(newEntry->name) < atoi(curEntry->name)) {
			if(curEntry->prev != NULL) {
				curEntry->prev->next = newEntry;
				newEntry->prev = curEntry->prev;
				curEntry->prev = newEntry;
				newEntry->next = curEntry;
				return root;
			}else {
				newEntry->next = curEntry;
				curEntry->prev = newEntry;
				return newEntry;
			}
		}else {
			curEntry->next = newEntry;
			newEntry->prev = curEntry;
		}
	}
	
	
	return root;
}


/*
	Goes through the spool queue directory and creates a linked list
	of queueEntry nodes, 1 node for each file, sorted by order in
	which files were put into the queue.
*/
queueEntry *getFiles() {
	DIR *dir;
	struct dirent *f;
	int numFiles = getNumFiles();
	if(numFiles == 0) return NULL;
	queueEntry *root = NULL;
	
	//iterate over every file in queue
	if((dir = opendir(QUEUE_DIR)) != NULL) {
		while((f = readdir(dir)) != NULL) {
		
			if(strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0 || strcmp(f->d_name, "index.txt") == 0 ) {
				continue;
			}
			queueEntry *entry = (queueEntry *) malloc(sizeof(queueEntry));
			entry->next = NULL;
			entry->prev = NULL;
			char name[128] = "";
			strncpy(name, f->d_name, strlen(f->d_name)+1);
			char *token = strtok(name, "_");
			
			//create entry->name and entry->unqiueName
			strncpy(entry->name, token, strlen(token)+1);
			strncpy(entry->uniqueName, f->d_name+(strlen(token)+1), (strlen(f->d_name) - strlen(token)));
			
			char path[1024] = "";
			strncpy(path, QUEUE_DIR, strlen(QUEUE_DIR));
			strncat(path, f->d_name, strlen(f->d_name) + 1);
			
			struct stat statbuf;
			if(stat(path, &statbuf) == -1) {
				printf("Error getting file stats for file: %s\n", path);
				return NULL;
			}
			
			int owner = getUserId(f->d_name);
			if(owner <= 0) {
				printf("Error getting owner of file from queue index\n");
				return NULL;
			}
			
			entry->owner = owner;
			entry->time = statbuf.st_mtime;
			
			root = insert(root, entry);
		}
		closedir(dir);
	}	
	 return root;
}


int main(int argc, char *argv[]) {
	
   	queueEntry *root = getFiles();
   	printList(root);
   	deleteEntries(root);	
    return 0;
}
