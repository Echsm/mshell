#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<linux/limits.h>
#include <dirent.h>
#include <termios.h>


#define DEL 127
#define BACKSPACE 8
#define TAB 9
#define NEWLINE 10

#define printv(fmt, ...) do { if (verbose) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

int verbose;
char **path;

void generatePath() {
  printv("Start Generating PATH\n");
  const char *s = getenv("PATH");

  if (s == NULL) {
    printv("PATH not Found");
    return;
  }
  int idx_cmd = 0;
  int idx_dir = 0;
  int idx = 0;
  
  char* dirbuffer = malloc(sizeof(char) * PATH_MAX);

  //persists until programm ends -> No Free
  path = malloc( 1024 * 4096 * sizeof(char *));
  
  dirbuffer[0] = '\0';
  
  
  while (s[idx] != '\0') {
    if (s[idx] == ':') {
      printv("\tScanning [%s] - ",dirbuffer);
      //OPEN DIR AND ADD ALL THE EXECUTABLE FILES TO PATH

      DIR *dir = opendir(dirbuffer);
      if (dir == NULL) {
        printv("failed to open\n");
      } else {
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
          if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {

            char *fullpath = malloc(PATH_MAX * sizeof(char));
            strcpy(fullpath, dirbuffer);
            strcat(fullpath, entry->d_name);
          

            if (access(fullpath, X_OK)) {
              
              int double_idx = 0;
              int exists = 0;
              while (path[double_idx] != NULL) {
                if (strcmp(entry->d_name, path[double_idx]) == 0) {
                  exists = 1;
                  break;
                }
                double_idx++;
              }
              
              if (!exists) {
              path[idx_cmd] = malloc(sizeof(char) * NAME_MAX);
              strcpy(path[idx_cmd], entry->d_name);
              idx_cmd++;
              path[idx_cmd] = NULL;
              }
            }
            free(fullpath);
          }
        }
        printv("finished reading\n");
      }
      closedir(dir);

      dirbuffer[0] = '\0';
      idx_dir = 0;
    } else {
      dirbuffer[idx_dir] = s[idx];
      idx_dir++;
    }

    idx++;
    dirbuffer[idx_dir] = '\0';
  }
  free(dirbuffer);


  printv("Finished generating PATH - found %d executables\n\n", idx_cmd);
}

void printStart() {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    printf("%s> ", cwd);
}

int startswith(char *substring, char *string) {
  int idx = 0;
  while (1) {
    if (substring[idx] == '\0') {
      return 1;
      
    }    
    if (substring[idx] != string[idx] || string[idx] == '\0') {
      return 0;
    }
    idx++;
  }
}

int msh_cd(char **args) {
  if (args[1] == NULL) {
    printf("Argument expected for cd");
  } else {
    if (chdir(args[1]) != 0) {
      perror("msh");
    }
  }
  return 1;
}

char **getAutocompleteFiles(char *filter, char *directory) {
  char **files = malloc(128 * sizeof(char *));
  DIR *dir = opendir(directory);
  if (dir == NULL) {
    printf("\n%s|%s", filter, directory);
    perror("opendir");
  }
  
  struct dirent *entry;
  int idx = 0;
  while ((entry = readdir(dir)) != NULL) {
        // Print only regular files (excluding "." and "..")
        
        if (entry->d_type == DT_DIR) {
            strcat(entry->d_name, "/");
        }

        if (startswith(filter, entry->d_name)) {
          files[idx] = malloc(sizeof(char * ) * NAME_MAX);
          strcpy(files[idx], entry->d_name);
          idx++;
        }
    }
    files[idx] = NULL;
    closedir(dir);
    return files;
}

char **getAutocompleteCommands(char *filter) {
  char **cmds = malloc(sizeof(char *) * 1024);
  int idx_cmds = 0;
  int idx = 0;
  cmds[idx_cmds] = NULL;
  
  while (path[idx] != NULL) {
    if (startswith(filter, path[idx])) {
      cmds[idx_cmds] = path[idx];
      idx_cmds++;
      cmds[idx_cmds] = NULL;
    }
    idx++;  
  }
  return cmds;
}

char **handleAutoComplete(char *line) {
  char **out = NULL;
  int cmdtyped = 0;
  
  //Nothing typed yet -> No autocomplete available
  if (line[0] == '\0') {
    return out;
  }


  char *lastarg = malloc(PATH_MAX);
  lastarg[0] = '\0';

  int idx = 0;
  int lastarg_idx = 0;

  int nSlashes = 0;
  int endsWithSlash = 0;


  while(1) {
    fflush(stdout);

    if (line[idx] == '\0') {
      fflush(stdout);
      break;
    } else if (line[idx] == ' ') {
      cmdtyped = 1;
      lastarg_idx = 0;
      lastarg[0] = '\0';
      nSlashes = 0;
      endsWithSlash = 0;
      
    } else {
      
      if (line[idx] == '/') {
        cmdtyped = 1;
        endsWithSlash = 1;
        nSlashes += 1;
      } else {
        endsWithSlash = 0;
      }
      lastarg[lastarg_idx] = line[idx];
      lastarg_idx++;
      lastarg[lastarg_idx] = '\0';
    }
    idx++; 
  }



 if (cmdtyped == 0) {
   out = getAutocompleteCommands(lastarg);
    } else {

   if (nSlashes == 0) {
     out = getAutocompleteFiles(lastarg, ".");
   } else if (endsWithSlash) {
     out = getAutocompleteFiles("", lastarg);
   } else {
     
     //autocomplete consist of PATH + start of File/Dir
     char *path_buff = malloc(PATH_MAX);
     char *file_buff = malloc(NAME_MAX);
     int path_idx = 0;
     int file_idx = 0;
   
     idx = 0;
     
     while (1) {
       if (lastarg[idx] == '\0') {break;}

       if (nSlashes > 0) {
         path_buff[path_idx] = lastarg[idx];
         path_idx++;
         path_buff[path_idx] = '\0';
         
         if (lastarg[idx] == '/') {
           nSlashes--;
         }

       } else if (nSlashes == 0) {
         file_buff[file_idx] = lastarg[idx];
         file_idx++;
         file_buff[file_idx] = '\0';
       }
       idx++;
     }
     out = getAutocompleteFiles(file_buff, path_buff);
     free(file_buff);
     free(path_buff);
   }
 }
  
  free(lastarg);
  return out;
}


char *readline() {
  int arg_idx = 0;
  
  char *buffer = malloc(128);
  char c;
  int escape_sequence = 0;
  int idx = 0;

  buffer[idx] = '\0';
  while (read(STDIN_FILENO, &c, 1) == 1) {
    // if were not in an escape sequence AND C is a Printable Character and C is NOT DEL
    
    if (!escape_sequence && c >= 32 && c != DEL) {
      write(STDOUT_FILENO, &c, sizeof(char));
      buffer[idx] = c;
      idx++;
      buffer[idx] = '\0';
      } else if (!escape_sequence && c == DEL && idx > 0) {
        write(STDOUT_FILENO, "\b \b", 3 * sizeof(char));
        idx--;
        buffer[idx] = '\0';

      } else if (!escape_sequence && c == '\n') {
        write(STDOUT_FILENO, "\n", sizeof(char));

        break;
        
      } else if (!escape_sequence && c == '\t') {
        buffer[idx] = '\0';
        fflush(stdout);
          
        char **buff = handleAutoComplete(buffer);

        //Nothing to Autocomplete
        if (buff == NULL || buff[0] == NULL) {}


        //ONLY ONE OPTION -> PRINT OUT OPTION
        else if (buff[0] != NULL && buff[1] == NULL) {
          //GEHE SCHRITTE ZURÜCK BIS DU AUF " " oder "/" triffst oder IDX = -1 ist. Dann printe den wert
          while (1) {
            idx--;
            if (idx==-1 || buffer[idx] == '/' || buffer[idx] == ' ') {
              strcat(buffer, buff[0]);
              printf("%s",buff[0]);
              idx = 0;
              while (buffer[idx] != '\0') {
                idx++;
              }
              break;
            } else {
              buffer[idx] = '\0';
              printf("\b");  
            }
            
          }
        }

          //Multiple Options -> Print Out
        else {
          int optidx = 1;
          printf("\n%s",buff[0]);
          while (buff[optidx] != NULL) {
            printf("\t%s", buff[optidx]);
            optidx++;
          }
          printf("\n");
          printStart();
          printf("%s", buffer);
        }
          fflush(stdout);
          free(buff);
      }
  }
  return buffer;
}


char **tokenize(char* line) {
  int tokens_buffsize = 64;
  int token_buffsize = 64;

  //platz für 64 pointer zu tokens
  char **tokens = malloc(tokens_buffsize * sizeof(char*));

  int inString = 0;
  int token_position = 0;
  int token_length = 0;
  int char_position = 0;
  char c;
  
  tokens[token_position] = malloc(token_buffsize * sizeof(char));
  while (1) {
    c = line[char_position];
    char_position++;
    if (c == '\0' || c == '\n') {
      tokens[token_position][token_length] = '\0';
      token_position++;
      tokens[token_position] = NULL;
      break;
    } else if (c == ' ' && ~inString) {
      tokens[token_position][token_length] = '\0';
      token_position++;
      tokens[token_position] = malloc(token_buffsize * sizeof(char));
      token_length = 0;
    } else if (c == '"') {
      inString = ~inString;
    } else if (c) {
      tokens[token_position][token_length] = c;
      token_length++;
    } 
  }
  return tokens;
}

int msh_launch(char **args) {
  pid_t pid;
  pid_t wpid;
  int status;

  pid = fork();

  //Child Process
  if (pid == 0) {
    execvp(args[0], args);
  } else if (pid < 0) {
    perror("Cloning Process Failed");
  } else if (pid > 0) {
    do {

    //fetch status from Process
    wpid = waitpid(pid, &status, WUNTRACED);

    //continue wating ONLY IF Process didnt exit and wasnt signaled to end
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int msh_execute(char **args) {
  if (strcmp(args[0],"cd") == 0) {
    return msh_cd(args);
  } else {
      return msh_launch(args);
    } 
}

void msh_loop() {
  char *line;
  char **args;
  int status;
  do {
    printStart();
    fflush(stdout);
    line  = readline();
    args = tokenize(line);
    status = msh_execute(args);
    free(line);
    free(args);
  } while (status);
}


int main(int argc, char **argv) {
  verbose = 0;
  if (argc > 1) {
    if (strcmp(argv[1],"-v") == 0) {
      verbose = 1;
    }
  }
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;


  // flags are bitmasks, this sets ICANON bit to 0
  newt.c_lflag &= ~(ICANON | ECHO);  // disable canonical mode + echo
  
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  generatePath();
  
  msh_loop();
  return EXIT_SUCCESS;
}
