#include<iostream>
#include<fstream>
#include<unistd.h>
#include<string>
#include<string.h>
#include<sstream>
#include<vector>
#include<cstdio>
#include<sys/types.h>
#include<sys/wait.h>
#include<cstdlib>
#include"shell.h"
#include<fcntl.h>

using namespace std;

/**
 * @file     shell.cpp
 * @authors  Harli Bott and Mason Protsman
 * @date     May 3, 2018
 * @version  1.0.0
 */

/**
 * A function that represents main
 *
 * @return  exit status
 */
int main(){


  while(true){ // program loop
    prompt(); // prompts the user 

    char * tokes[1042];  
    char str[1042];
    cin.getline(str, 1042);
    char * token;
    int i = 0;
    int j = 0;
    int compare = 0;

    token = strtok(str, " \r\n");
    
    while(token != NULL){
      tokes[i] = token;
      token = strtok(NULL, " \r\n");
      i++;
    }
    
    tokes[i] = NULL;
    
    if(strcmp(tokes[0], "exit") == 0 || strcmp(tokes[0], "quit") == 0){
      return EXIT_SUCCESS;
    }else if(strcmp(tokes[0], "export") == 0){
      putenv(tokes[1]);
      compare = -1;
    }else if(i > 1){
      while (j < i){
	if(strcmp(tokes[j], "<") == 0 || strcmp(tokes[j], ">") == 0 || strcmp(tokes[j], ">>") == 0){
	  compare = io(tokes[j]);
	  break;
	}
	j++;
      }
    }
    
    if (compare == 0){
      
      pid_t pid = fork();
      
      if (pid < 0){
	perror("fork failed");
	exit(EXIT_FAILURE);
      }
      else if(pid == 0){
	execvp(tokes[0], tokes); 
	cout << "Command not found" << endl;
	exit(0);
	
      }else{
	int status;
	waitpid(pid, &status, 0);
      }
    }
    else if (compare == 1){
      
      pid_t pid = fork();
      
      if(pid < 0){
	cout << "fork failed" << endl;
	exit(EXIT_FAILURE);
      }
      else if(pid == 0){
	
	char * newTokes[1042];
	int k = 0;
	
	while(k < j){
	  newTokes[k] = tokes[k];
	  k++;
	}
	
	fstream file;
	file.open(tokes[j+1], ios::in);
	char line[1042];
	file.getline(line, 1042);

	char * newToken;
	newToken = strtok(line, " \r\n");

	int l = 1;
	while(newToken != NULL){
	  newTokes[l] = newToken;
	  newToken = strtok(NULL, " \r\n");
	  l++;
	}

	newTokes[l] = NULL;

	execvp(newTokes[0], newTokes);
      
      }else{
	int status;
	waitpid(pid, &status, 0);
      }
    }
    else if (compare == 2){
      pid_t pid = fork();
      
      if(pid < 0){
	cout << "fork failed" << endl;
	exit(EXIT_FAILURE);
      }
      else if(pid == 0){
	
	char * newTokes[1042];
	int k = 0;
	
	while(k < j){
	  newTokes[k] = tokes[k];
	  k++;
	}
	newTokes[k] = NULL;
	
	int fd = open(tokes[j+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	dup2(fd, 1);
	close(fd);
	
	execvp(newTokes[0], newTokes);
      
      }else{
	int status;
	waitpid(pid, &status, 0);
      }
    }
    else if (compare == 3){
      
      pid_t pid = fork();
      
      if(pid < 0){
	cout << "fork failed" << endl;
	exit(EXIT_FAILURE);
      }
      else if(pid == 0){
	
	char * newTokes[1042];
	int k = 0;
	
	while(k < j){
	  newTokes[k] = tokes[k];
	  k++;
	}
	newTokes[k] = NULL;
	
	
	int fd = open(tokes[j+1], O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	dup2(fd, 1);
	close(fd);
	
	execvp(newTokes[0], newTokes);
      
      }else{
	int status;
	waitpid(pid, &status, 0);

      }
    }
  }// shell while loop
}// main

/**
 * A function that will support io redirection 
 *
 * @param char *  token
 */
int io(char * token){
  
  if(!strcmp(token, "<")){
    return 1;
  }else if(!strcmp(token, ">")){
    return 2;
  }else if(!strcmp(token, ">>")){
    return 3;
  }else{
    return 0;
  }
}// io redirection method

/**
 * A function that will be the prompt 
 */
void prompt(){
  // gets the cwd and prints it out as a prompt to the screen
  string prompt = "1730sh:";
  char buf[4096];
  getcwd(buf, 4096);

  string current = buf;
  const char * home;
  home = getenv("HOME");

  int length = strlen(home);
  string subs = current.substr(0, length);
  const char * comp = subs.c_str();

  if(strcmp(comp, home) == 0){
    prompt += "~";
    prompt += current.substr(length);
  }else{
    prompt += current;
  }
  prompt += "$ ";
  cout << prompt;
}// prompt

/**
 * A function that will support error handling
 */
void error(int e){
  switch(e){

  case 1:
    cout << "\nYou cannot have more than 10 commands...\nEXIT FAILURE\n" << endl;
    exit(EXIT_FAILURE);

  case 2:
    cout << "\nInvalid arguments...\nEXIT FAILURE\n" << endl;
    exit(EXIT_FAILURE);
   
  case 3:
    cout << "\nFork error\n" << endl;
    exit(EXIT_FAILURE);

  default: 
    cout << "\nThere was an error...\nEXIT FAILURE\n" << endl;
    exit(EXIT_FAILURE);
  }
}// error checking method
