#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define len 512


char ** get_input(char * input);
void Execute(char input[]) {

  char * inputptr;
  pid_t pid = getpid();
  pid_t child_pid;
  inputptr = input;
  int flag = 5;
  pid_t proArray[50]; //max count of proccess you can do
  int prc = 0;
  int i = 0, startIndex = 0, substrL = 0, stat_loc;
  while (input[i] != '\0') {
    char * substr = (char * ) malloc((i + 3) * sizeof(char));
    //char *substr=(char*) malloc(len*sizeof(char));

    if (input[i] == '|') {
      //manuel strcpy
      memcpy(substr, & input[substrL], i - substrL);
      substr[i - substrL] = '\0';
      // "||" or ";;" case
      if (strcmp(substr, "") == 0) {
        free(substr);
        substrL += 1;
        i++;
        continue;
      }

      int a;
      for (a = 0; substr[a] != '\0'; a++);
      substrL += a + 1;
      char ** str = get_input(substr);
      // case "|     |"
      if (str[0] == NULL) {

        proArray[prc++] = fork();
        if (proArray[prc - 1] == 0) {
          //if (child_pid == 0 )  {
          execvp(str[0], str);
          printf("Command \"%s\" does not exist or cannot be executed.\n", str[0]);
          exit(1);
        } else {
          waitpid(proArray[prc - 1], & stat_loc, WUNTRACED);
        }
        free(substr);
      }
      // case quit|
      else if (strcmp(str[0], "quit") == 0) {
        flag = 1;
      }
      //case  command|
      else {
        proArray[prc++] = fork();
        if (proArray[prc - 1] == 0) {
          execvp(str[0], str);
          printf("Command \"%s\" does not exist or cannot be executed.\n", str[0]);
          exit(1);
        } else {
          waitpid(proArray[prc - 1], & stat_loc, WUNTRACED);
          //waitpid(child_pid, &stat_loc, WUNTRACED);
        }
        free(substr);
      }
      //else
    }

    ///parallel -----------------------------------------------------------------------------------------
    /// -------------------------------------------------------------------------------------------------
    else if (input[i] == ';') {
      memcpy(substr, & input[substrL], i - substrL);
      substr[i - substrL] = '\0';

      if (strcmp(substr, "") == 0) {
        free(substr);
        substrL += 1;
        i++;
        continue;
      }
      int a;
      for (a = 0; substr[a] != '\0'; a++);
      substrL += a + 1;
      char ** str = get_input(substr);
      // case ";     ;"
      if (str[0] == NULL) {
        proArray[prc++] = fork();
        if (proArray[prc - 1] == 0) {
          execvp(str[0], str);
          printf("Command \"%s\" does not exist or cannot be executed.\n", str[0]);
          kill(proArray[prc - 1], SIGKILL);
        }
        free(substr);
      } else if (strcmp(str[0], "quit") == 0) {
        flag = 1;
      } else {
        proArray[prc++] = fork();
        if (proArray[prc - 1] == 0) {
          execvp(str[0], str);
          exit(1);
          printf("Command \"%s\" does not exist or cannot be executed.\n", str[0]);
        }
        free(substr);
      }
    } // else if ; end
    i++;
  } //inner while end
  if (flag == 1) {
    int p;
    exit(0);
  }
}

char ** get_input(char * input) {
  char ** command = malloc(8 * sizeof(char * ));
  char * separator = " ";
  char * parsed;
  int index = 0;

  parsed = strtok(input, separator);
  // is_char can be usabel or pass if see second "  "
  while (parsed != NULL) {
    command[index] = parsed;
    index++;
    // executes at most 47 right side argumetns -> docs.
    parsed = strtok(NULL, separator);
    if (index > 48) {
      printf("You passed the maximum number of arguments");
      break;
    }
  }
  command[index] = NULL;
  return command;
}

int main(int argc, char * argv[]) {
  char input[len] = "";
  char * inputptr;
  inputptr = input;
  int flag = 5;
// interactive mode
  if (argv[1] == NULL) {
    int a = 0;
    while (1) {
      printf("myshell:>");
      Execute(input);
      gets(inputptr);
    } //inner while end
  } // outer while
  else {
    char line[len];
    FILE * f;
    char command[len];
    f = fopen(argv[1], "r");
    if (f != NULL) {
      while (fgets(line, 499, f) != NULL) {
        sprintf(command, "echo \"%s\"", line);
        system(command);
        Execute(line);
      }
    } else {
      printf("The batch file  %s does not exist or cannot be opened. \n", argv[1]);
    }
  }
  return 0;
}
