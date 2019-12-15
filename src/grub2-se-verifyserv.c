/*
FILENAME :     grub2-se-verifyserv.c
DESCRIPTION :  Service which touches a file to VRFFILE if the return code of PROGRAM is STATUS_OK.
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
#define PROGRAM "/usr/sbin/grub-verify"
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

void exec_failed(){
  fprintf(stderr, "ERROR: It is not possible to execute %s: ", PROGRAM);
  if(access(PROGRAM, F_OK)) fprintf(stderr, "It does not exist (in your PATH variable).\n");
  else fprintf(stderr, "Access denied. (Are you root?)\n");
  if(access(VRFFILE, F_OK) != -1)
    unlink(VRFFILE);
  exit(EXIT_FAILURE);
}

void sig_handler(int signo){
  if(signo == SIGTERM || signo == SIGINT){
    // Unverify
    if(access(VRFFILE, F_OK) != -1) //File exists?
      unlink(VRFFILE);
    
    exit(0);
  }
}

int main(int argc, char *argv[]){
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
    fprintf(stderr, "ERROR: Can't catch signal!\n");
  
  while(1){  

    if(fork() == 0){
      // Exec program silently by default
      if(!verbose){
        int fd = open("/dev/null", O_WRONLY | O_CREAT, 0666);    

        dup2(fd, 1);
        dup2(fd, 2);
        if(execlp(PROGRAM, PROGRAM, NULL) < 0)
          exec_failed();
        
        close(fd);
      }
      else{
        if(execlp(PROGRAM, PROGRAM, NULL) < 0)
          exec_failed();
      } 

    }
    else {

      if(verbose) 
        printf("Waiting for %d...\n", getpid());

      wait(&statval);
      if(verbose) 
        printf("Exit: %d\n", WEXITSTATUS(statval));

      if(WEXITSTATUS(statval) == STATUS_OK){
        // If exit is STATUS_OK, write file with nothing in it
        if(access(VRFFILE, F_OK) == -1){
          FILE *f = fopen(VRFFILE, "w");
          if(f == NULL){
            fprintf(stderr, "ERROR opening file\n");
            return 1;
          }
          fclose(f);
        }
      } 
      else
        // If exit != 0
        if(access(VRFFILE, F_OK) != -1) //File exists?
          unlink(VRFFILE);  
    }
    sleep(SLEEP);
  }
  sig_handler(SIGTERM);
  return 0;
}
