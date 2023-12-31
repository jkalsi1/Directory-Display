#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>


void parse_dir(char *name, int indent, int *dir_num, int *file_num, int hidden, int size) {
	DIR *dir;
	dir = opendir(name);
    	if (dir == NULL) {
        	return;
    	}
	// use scandir with alphasort to alphabetize dir entries for parsing
    	struct dirent **entries;
	int dir_size;
	dir_size = scandir(name, &entries, NULL, alphasort);
    	
	if (dir_size < 0) {
        	closedir(dir);
        	free(entries);
		return;
   	}
	// parse through entries
    	for (int i = 0; i < dir_size; i++) {
        	struct dirent *cur = entries[i];
	
		// checks if -a switch was included
		if ((strcmp(cur->d_name, ".") == 0 || strcmp(cur->d_name, "..") == 0)
			       	|| (strncmp(cur->d_name, ".", 1) == 0 && !hidden)){
		       	free(cur);
			continue;
		}
		// for formatting to match specified output
		if (indent > 0 ){
	       		printf("|");
		}
        	// check if indent is zero for format purposes	
		for (int j = 0; j < indent; j++) {
            		if (indent > 0 && j == 0) {
		    		printf("   ");
	    		} else {
				printf("|   ");
        		}
		}
		// allocate space for new dir path
		char new_dir[strlen(name) + strlen(cur->d_name) + 2];
        	sprintf(new_dir, "%s/%s", name, cur->d_name);

		//get size of file in case of -s
		struct stat *statbuf = malloc(sizeof(struct stat));
		if (stat(new_dir, statbuf) == -1) {
            		free(cur);
			continue;
        	}
		
		printf("%s-- ", (i == dir_size - 1) ? "`" : "|");
		if (size) {
    			printf("[%10lld] ", (long long) statbuf->st_size);
		}
		printf("%s\n", cur->d_name);	

		free(statbuf);
		// end formatting section
		// increment file or dir num and recurse on entry if its directory
        	if (cur->d_type == DT_DIR) {
            		(*dir_num)++;
            		parse_dir(new_dir, indent + 1, dir_num, file_num, hidden, size);
		} else {
            		(*file_num)++;
		}
		free(cur);
    	}
	// free allocated mem by scandir 
    	closedir(dir);
    	free(entries);
}

int main(int argc, char *argv[]) {
    	if (argc == 1) {
        	printf("No directory specified.\n");
        	return 1;
    	}
        DIR *dir;
       	dir = opendir(argv[argc-1]);
       	if (dir == NULL) {
                printf("Error loading root.\n");
        	return 1;
        }
	closedir(dir);

	int show_hidden = 0;
    	int show_size = 0;
    	char *path = NULL;

	for (int i = 1; i < argc; i++) {
        	if (strcmp(argv[i], "-a") == 0) {
            		show_hidden = 1;
        	} else if (strcmp(argv[i], "-s") == 0) {
            		show_size = 1;
        	}
    	}
	path  = argv[argc-1];
       	int dir_num = 0;
       	int file_num = 0;
       	printf("%s\n", path);
	parse_dir(path, 0, &dir_num, &file_num, show_hidden, show_size);
        printf("\n%d directories, %d files\n", dir_num, file_num);
        return 0;
}
