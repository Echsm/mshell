#include <linux/limits.h>
#include <stdatomic.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <dirent.h>
#include<unistd.h>
#include"wildcard.h"


int ptrlen(void **ptr) {
  int idx = 0;
  while (ptr[idx] != NULL) {
    idx++;
  }
  return idx;
}

void ptrcat(void **dst, void **src) {
  int idx_dst = 0;
  int idx_src = 0;
  
  while (dst[idx_dst] != NULL) {
    idx_dst++;
  }
  
  while (src[idx_src] != NULL) {
    dst[idx_dst+idx_src] = src[idx_src];
    idx_src++;
  }
  dst[idx_dst+idx_src] = NULL;
}

void ptrcpy(void **dst, void **src) {
  int idx = 0;
  while (src[idx] != NULL) {
    dst[idx] = src[idx];
    idx++;
  }
  dst[idx] = NULL;
}
int matches(char *wildcard, char *name) {
  char *w_buff = wildcard;
  char *n_buff = name;

  int w_size = strlen(wildcard);
  int n_size = strlen(name);

  if (n_buff[0] == '.') {
    if (n_buff[1] == '\0') return 0;
    else if (n_buff[1] == '.') {if (n_buff[2] == '\0') return 0;}
  }
  w_buff = wildcard;
  n_buff = name;
  //printf("Started Consuming from the Left\n");
  if (n_buff[0] == '.' && wildcard[0] != '.') return 0;
  while (1) {
    //printf("n:%s, %d\nw:%s, %d\n============\n",n_buff,n_size,w_buff,w_size);

    if (w_buff[0] == '\0' && n_buff[0] == '\0') {
      return 1;
    } else if (w_buff[0] == '\0') {
      return 0;
    } else if (n_buff[0] == '\0') {
      break;
    } else if (w_buff[0] == n_buff[0] || (w_buff[0] == '?' && n_buff[0] != '\0')) {
      w_buff = &(w_buff[1]);
      n_buff = &(n_buff[1]);
      w_size--;
      n_size--;
    } else if (w_buff[0] == '*') {
      break;
    } else {
      return 0;
    }
  }
  //printf("Started Consuming from the Right\n");
  while (1) {
      //printf("n:%s, %d\nw:%s, %d\n==========\n",n_buff,n_size,w_buff,w_size);

    if (w_size == 0 && n_size == 0) {
      return 1;
    } else if (w_size == 0) {
      return 0;
    } else if (n_size == 0) {
      break;
    } else if (n_buff[n_size-1] == w_buff[w_size-1] || (w_buff[w_size-1] == '?' && n_buff[n_size-1] != '\0')) {
      n_size--;
      w_size--;
    } else if (w_buff[w_size-1] == '*') {
      break;
    } else {
      return 0;
    }
  }

  if (w_buff[w_size-1] == '*' && w_size == 1) {
    return 1;
  } else {
    return 0;
  }
}


char **matchesInDir(char* path, char *wildcard, char *tail) {
  DIR *dir;
  if (path == NULL) {
    dir = opendir("./");
  } else {
    dir = opendir(path);
  }
  
  if (dir == NULL) {
    //printf("failed to open\n");
    return NULL;
  } 

  struct dirent *entry;

  char **out = malloc(256 * sizeof(char *));
  int out_idx = 0;
  out[out_idx] = NULL;
  
  while ((entry = readdir(dir)) != NULL) {
    if ((tail == NULL && "(entry->d_type == DT_REG || entry->d_type == DT_LNK )") || tail != NULL &&entry->d_type == DT_DIR) {
      if (matches(wildcard, entry->d_name)) {
        out[out_idx] = malloc(sizeof(char) * 256);
        if (path != NULL) {
          strcpy(out[out_idx], path);
          //strcat(out[out_idx], "/");  
        } else {
          strcpy(out[out_idx], "");
        }
        strcat(out[out_idx], entry->d_name);

        if (tail != NULL) {
          //strcat(out[out_idx], "/");
          strcat(out[out_idx], tail);;
        }

        //printf("%s\n",out[out_idx]);
        out_idx++;
      }
    }
  }
  //printf("finished reading\n");
  closedir(dir);

  if (out[0] == NULL) {
    return NULL;
  }
  out[out_idx] = NULL;
  return out;
}

char **splitWildcard(char *wildcard) {
  char *path = NULL;
  char *middle = NULL;
  char *tail = NULL;
  int idx = 0;

  int firstWildcard = -1;
  int lastSlash = -1;
  int firstSlash = -1;
  while (wildcard[idx] != '\0') {
    if (wildcard[idx] == '/' && firstWildcard == -1) {
      lastSlash = idx;
    } else if ((wildcard[idx] == '?' || wildcard[idx] == '*') && firstWildcard == -1) {
      firstWildcard = idx;
    } else if (wildcard[idx] == '/' && firstWildcard != -1 && firstSlash == -1) {
      firstSlash = idx;
    }
    idx++;
  }

  //printf("lS:%d\nfS:%d\nf*:%d\n", lastSlash, firstSlash, firstWildcard);


  if (firstWildcard != -1) {
    middle = malloc(PATH_MAX);
    if (lastSlash == -1 && firstSlash  == -1) {
      strcpy(middle, wildcard);
    } else if (lastSlash == -1) {
      strncpy(middle, wildcard, firstSlash);
      middle[firstSlash] = '\0';
      middle[firstSlash+1] = '\0'; 
    } else if (firstSlash == -1) {
      strcpy(middle, &(wildcard[lastSlash+1]));
    } else  {
      strncpy(middle, &(wildcard[lastSlash+1]), firstSlash - lastSlash -1 );
      middle[firstSlash - lastSlash-1] = '\0';
    }
    
    if (lastSlash != -1) {
      path = malloc(PATH_MAX);
      strncpy(path, wildcard, lastSlash+1);
      path[lastSlash+1] = '\0';
    }

    if (firstSlash != -1) {
      tail = malloc(PATH_MAX);
      strcpy(tail, &(wildcard[firstSlash]));
    }
  } else {
    return NULL;
  }

  //printf("path  : %s\n", path);
  //printf("middle: %s\n", middle);
  //printf("tail  : %s\n", tail);

  char **ret = malloc(sizeof(char *) * 3);
  ret[0] = path;
  ret[1] = middle;
  ret[2] = tail;
  return ret;
}

char **expand(char* wildcard) {
  char **split = splitWildcard(wildcard);
  char *path = split[0];
  char *middle = split[1];
  char *tail = split[2];
  free(split);
  return matchesInDir(path, middle, tail);
  
  
}

int containsWildcard(char *arg) {
  for (int i = 0; arg[i] != '\0'; i++) {
    if (arg[i] == '?' || arg[i] == '*') return 1;
  }
  return 0;
}

char **generateMatches(char *wildcard) {
  char **stack = malloc(sizeof(char *) * 1024);
  char **out = malloc(sizeof(char *) * 1024);
  int stackptr = 0;
  int outptr = 0;

  out[0] = NULL;
  stack[stackptr] = malloc(PATH_MAX);
  strcpy(stack[stackptr], wildcard);
  stackptr++;
  stack[stackptr] = NULL;
   while (1) {
    //printf("STACK [%d]:", stackptr);
    //for (int idx = 0; stack[idx] != NULL; idx++) {printf("%s|", stack[idx]);}
    //printf("\n");

    if (stackptr == 0) break;
    else if (stack[stackptr-1] == NULL) stackptr--;
    else if (containsWildcard(stack[stackptr-1])) {
      char **substack = expand(stack[stackptr-1]);
      if (substack == NULL) {
        //printf("SUBSTACK IS EMPTY;\n");
        free(stack[stackptr-1]);
        stack[stackptr-1] = NULL;
        stackptr--;
      } else {
        free(stack[stackptr-1]);
        stack[stackptr-1] = NULL;
        ptrcat((void *)stack, (void *)substack);
        stackptr = ptrlen((void *)stack);
      }

      
    } else if (containsWildcard(stack[stackptr-1]) == 0) {
      if (access(stack[stackptr-1], F_OK) == 0) {
        out[outptr] = stack[stackptr-1];
        outptr++;
        out[outptr] = NULL;
        stack[stackptr-1] = NULL;
      } else {
        free(stack[stackptr-1]);
      }
      stack[stackptr-1] = NULL;
      stackptr--;      
    }
  }
  free(stack);
  return out;
}

//int main(int argc, char **argv) {
//
//  char **out = generateMatches("/home/*/D*/");
//  int idx = 0;
//  
//  while (out[idx] != NULL) {
//    printf("%s\n",out[idx]);
//    idx++;
//  }
//  
//}
