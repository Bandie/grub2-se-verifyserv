/*
FILENAME :     grub2-se-verifyserv.c
DESCRIPTION :  Service which touches a file to VRFFILE if the returncode is 0.
AUTHOR :       Bandie
DATE :         2018-03-17T20:31:48+01:00
LICENSE :      GNU-GPLv3
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define VRFFILE "/verified"
#define SLEEP 20
#define PROGRAM "/usr/bin/grub2-verify"
#define STATUS_OK 0

void help(){
  printf("grub2-se-verifyserv [ -v | --verbose ]\n\n"
         "It checks if GRUB2 is verified using %s. If it is \"%s\" will be touched as file. "
         "If not it will be erased.\n\n"
         "The purpose is to integrate it with i3status; "
         "you can then see if could get locked out after a reboot/poweroff.\n\n"
         "i3status integration\n"
         "====================\n"
         "You may want to copy paste the following into your i3status.conf:\n\n"
         "order += \"path_exists GRUB2_signed\"\n"
         "[...]\n[...]\n"
         "path_exist GRUB2_signed {\n"
         "\tpath = \"%s\"\n"
         "}\n\n", PROGRAM, VRFFILE, VRFFILE);
}

void sig_handler(int signo){
	if(signo == SIGTERM || signo == SIGINT){
		// Unverify
		if(access(VRFFILE, F_OK) != -1){ //File exists?
			unlink(VRFFILE);
    }
		exit(0);
	}
}

int main(int argc, char *argv[]){
  const char prog;
	int verbose = 0;
	int statval;


  // Argument handling
  for(int i=0; i<argc; i++){
    if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")){
      help();
      return 0;
    }
    if(!strcmp(argv[i], "--verbose") || !strcmp(argv[i], "-v"))
      verbose = 1;
  }
  
	// Signal handling
	if(signal(SIGTERM, sig_handler) == SIG_ERR || signal(SIGINT, sig_handler) == SIG_ERR)
		printf("ERROR: Can't catch signal!");
  
	while(1){	

		if(fork() == 0){


      // Check if program is even executable
      if(!(!access(PROGRAM, F_OK) && !access(PROGRAM, R_OK) && !access(PROGRAM, X_OK))){
        fprintf(stderr, "ERROR: It is not possible to execute %s: ", PROGRAM);
        if(access(PROGRAM, F_OK)) fprintf(stderr, "It does not exist.\n");
        else fprintf(stderr, "Access denied. (Are you root?)\n");
        return 1;

      }
      else{

  			// Exec program silently by default
        if(!verbose){
    			int fd = open("/dev/null", O_WRONLY | O_CREAT, 0666);		

  	  		dup2(fd, 1);
  		  	dup2(fd, 2);
          execlp(PROGRAM, PROGRAM, NULL);
	  		  close(fd);
        }
        else
          execlp(PROGRAM, PROGRAM, NULL);
       
      }

      
		} 
		else {

			if(verbose) 
				printf("Waiting for %d...\n", getpid());

			wait(&statval);
			if(WIFEXITED(statval)){
				if(verbose) 
					printf("Exit: %d\n", statval);

				if(statval == STATUS_OK){
          // If exit 0, write file with nothing in it
					FILE *f = fopen(VRFFILE, "w");
					if(f == NULL){
						printf("ERROR opening file");
						exit(1);
					}
					fclose(f);
				} 
				else if(statval == 256){
          return 1;
        }
        else
          // If exit != 0
					if(access(VRFFILE, F_OK) != -1) //File exists?
						unlink(VRFFILE); 

				
			} 
		}

  	sleep(SLEEP);
	}
	

  sig_handler(SIGTERM);
  return 0;
}
