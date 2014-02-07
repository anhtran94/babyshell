#include <stdio.h>
#include "parse.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int laststatus=0;

int main()
{
    char buf[1000], **p;
    extern void execute(char **argv);

    while (printf("$ "), fgets(buf, sizeof buf, stdin))
        if ((p = parse(buf)))
            execute(p);
	    	    
    return(laststatus);
}

void execute(char **argv)
{	
	int status;
	extern char **environ;
	char *pter;
	char *path;
	char path0[100]; char path1[200]; char path2[200]; char path3[100];
	struct stat statbuf;
	pid_t pid;
		
	pter = strchr(argv[0], '/');
	if(pter == NULL) {  // no slash, so find command
		
		strcpy(path0, "/bin/");
		strcat(path0, argv[0]);

		if (stat(path0, &statbuf) != -1) { //search in /bin
			path = path0;
		} else {  //search in /usr/bin
			strcpy(path1, "/usr");
			strcat(path1, path0);
			
			if (stat(path1, &statbuf) != -1) {
				path = path1;
			} else { //search in /usr/local/bin
				strcpy(path2, "/usr/local");
				strcat(path2, path0);
				if (stat(path2, &statbuf) != -1) {
					path = path2;
				} else { //search in current dir
					strcpy(path3, "./");
					strcat(path3, argv[0]);
					if (stat(path3, &statbuf) != -1) {
						path = path3;
					} else {
						fprintf(stderr, "%s: Command not found\n", argv[0]);
						return; //end process here
					}
				}
			}
		}
	} else { //there is slash, so execute directly.
		path= argv[0];
	}
	switch((pid = fork())) {
	case -1: //error
		perror("fork");
		break;
	case 0: //child, execute the command
		if (execve(path, argv, environ) == -1) perror (path); 	//execute program				
		break;
	default: //parent
        	if ((pid = wait(&status)) == -1)  perror("wait error");
        	else {
			if (WIFEXITED(status))
			      laststatus= WEXITSTATUS(status);
			else{
			      printf("This process did not end normally or ended via signal\n");
			}
		}	                       
	}
		
}
