#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ntsid.h>

#define MAX 100

//Function Declarations
int getcommand();
void runcommand();
void checkForTime();
void reportTime();



//Variables
char input[MAX];
char *commandWithArgs[20];
char delim[] = "\t\n<>()|; ";
char buffer[MAX];
char  *getdir;
char  *dir;
char  *gotodir;
char *appendRedirectPath;
int redirect = 0;
int timeFlag =0;
time_t afterInputTimeStamp;
time_t beforSyscallTimsStamp;
time_t afterSyscallTimeStamp;




int main()

{

    char prompt[] = "acposeyShell:";
    int flag = 1;
    int argc = 0;


    while(flag){

        printf("%s$ ", prompt);
        argc = getcommand();
        checkForTime();
        runcommand(argc);   //run command function will have the wait(&status) in it
        printf("\n");
    }

}




void checkForTime(){

    int k = 0;

    if(!strcmp(commandWithArgs[0], "time")){  //check if time was part of the user command.  if it is it will be the first command

        timeFlag = 1;       //set timeFlag so we can time everything later when a program is executed


        //loop to remove time from command with args after time flag has been triggered.
        for(k = 0; k<20; k++){
            commandWithArgs[k] = commandWithArgs [k+1];
        }


    }

}


 void checkredirect(int x) {

    if(x > 0) {
        x--;
        if (!strcmp(commandWithArgs[x], ">")) {
            redirect = 1;   //found redirect command
        } else if (!strcmp(commandWithArgs[x], ">>")) {
            redirect = 2;   //found append command
        } else {
            redirect = 0;   //no redirection,
        }
    }

}


void runcommand(int x) {


    int i = 0;


         // if arg is exit then exit the shell
        if (!strcmp(commandWithArgs[0], "exit")) {
              printf("Exited successfully");
              exit(0); //this line to be kept commented out unless on VM
        }

         //if arg is cd then change directory
          if (!strcmp(commandWithArgs[0], "cd")) {

            getdir = getcwd(buffer, sizeof(buffer));
            dir = strcat(getdir, "/");
           gotodir = strcat(dir, commandWithArgs[1]);

           chdir(gotodir);


          printf("change directory successful \n");
         }
        else {
                time(&beforSyscallTimsStamp);
              pid_t pid = fork();
              if (pid == 0)      //then I'm the child
              {
                  FILE *fp;

                  if (redirect == 1) {


                      fp = freopen(appendRedirectPath, "w", stdout);

                      printf("This ouput has been redirected to  a file.txt\n");

                      //fclose(fp);
                  }

                  if (redirect == 2) {

                      fp = freopen(appendRedirectPath, "a", stdout);

                      printf("This output has been appended to a file.txt\n");
                  }


                  if (execvp(*commandWithArgs, commandWithArgs) < 0) {
                      printf("%s: command not found \n", commandWithArgs[0]);

                  }
                  printf("hello from child and now im going away");
                  exit(1);
                  //fclose(fp);

              } else if (pid == -1) {

                  printf("Fork() unsuccessfull");
              } else        //then I'm the parent
              {
                  int status;
                  wait(&status);
                  time(&afterSyscallTimeStamp);
                  if(timeFlag == 1){
                    reportTime();
                  }

              }
          }

}





int getcommand()

{   char *testforstring;
    int i =0;

    //clear command array
    for(i=0; i<20; i++){
        commandWithArgs[i] = NULL;
    }
    //reset i
    i = 0;


    fgets(input, MAX, stdin);   //get input from user and store as a string
    time(&afterInputTimeStamp);     //time stamp after user presses enter


    testforstring = strstr(input, ">");
    if(testforstring != NULL){     //found redirect command
        redirect = 1;
    }


    testforstring = strstr(input, ">>");
    if(testforstring != NULL){     //found append command
        redirect = 2;
    }



        //first token was and add it to command with args
        commandWithArgs[i] = strtok(NULL,delim);


    //get first token and add it to command with args
   commandWithArgs[i] = strtok(input,delim);


    //gets each successive token from input and place in command array
    while(commandWithArgs[i]!=NULL)
    {
        commandWithArgs[++i] = strtok(NULL,delim);
    }


    //if redirect commands were detected then save path and remove from arg list
    if(redirect == 1 || redirect == 2){

        appendRedirectPath = commandWithArgs[i-1];
        commandWithArgs[i-1] = NULL;
    }



    return i;
}


void reportTime()

{

    double userTime = beforSyscallTimsStamp - afterInputTimeStamp;
    double sysTime = afterSyscallTimeStamp - beforSyscallTimsStamp;
    double realTime = userTime + sysTime;


    printf("\n Time Report\n\nreal:\t%f\nuser:\t%f\nsys:\t%f", realTime, userTime, sysTime);




}
















