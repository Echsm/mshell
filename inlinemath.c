#include <bits/types/stack_t.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define UNARY 'U'
#define BINARY 'B'
#define NUMBER 'N'
#define POPEN 'O'
#define PCLOSE 'C'

//PROBLEM KLAMMEN LEEKEN MEMORY DIE MÜSSEN NOCH IM SHUNTING YARD ALGORITHM GEFREET WERDEN!
//PROBLEM ZERODEVISION ERROR

struct Token {
  char value[16];
  char type;
};


struct Token **mtokenize(char *string) {
  struct Token *zero       = malloc(sizeof(struct Token));
  struct Token *binary_add = malloc(sizeof(struct Token));
  strcpy(zero->value, "0.0");
  zero->type = NUMBER;
  strcpy(binary_add->value, "+");
  binary_add->type = BINARY;

  struct Token **tokens    = malloc(sizeof(struct Token *) * 512);
  
  tokens[0] = zero;
  tokens[1] = binary_add;
  int tkn_ptr = 2;


  char *nbuffer = malloc(sizeof(char) * 256);
  int num_ptr = 0;

  int inDecimal = 0;
  int inNumber = 0;
  char lastTokenType = BINARY;

  char c;
  for (int i = 0; string[i] != '\0'; i++) {
    c = string[i];
    //printf("processing %c [%s] - ",c,nbuffer);

    if (c >= '0' && c <= '9') {
      //printf("Digit\n");
      inNumber = 1;
      nbuffer[num_ptr] = c;
      num_ptr++;
      nbuffer[num_ptr] = '\0';
    }
    else if (c == '.') {
      //printf("Point2\n");
      inDecimal = 1;
      nbuffer[num_ptr] = c;
      num_ptr++;
      nbuffer[num_ptr] = '\0';
    }
    else if (c == '(') {
      //printf("P Open\n");
      tokens[tkn_ptr] = malloc(sizeof(struct Token));
      tokens[tkn_ptr]->value[0] = c;
      tokens[tkn_ptr]->value[1] = '\0';
      tokens[tkn_ptr]->type = POPEN;

      tkn_ptr++;
      tokens[tkn_ptr] = NULL;
      lastTokenType = POPEN;     
    }
    else if (c == ')') {
      //printf("P Close\n");

        tokens[tkn_ptr] = malloc(sizeof(struct Token));
        strcpy(tokens[tkn_ptr]->value, nbuffer);
        if (inDecimal == 0) {
          strcat(tokens[tkn_ptr]->value, ".0");
        }
        tokens[tkn_ptr]->type = NUMBER;
        nbuffer[0] = '\0';
        num_ptr = 0;
        tkn_ptr++;
        tokens[tkn_ptr] = NULL;
        lastTokenType = NUMBER;
        inNumber = 0;
        inDecimal = 0;

      tokens[tkn_ptr] = malloc(sizeof(struct Token));
      tokens[tkn_ptr]->value[0] = c;
      tokens[tkn_ptr]->value[1] = '\0';
      tokens[tkn_ptr]->type = PCLOSE;

      tkn_ptr++;
      tokens[tkn_ptr] = NULL;
      lastTokenType = PCLOSE;     
    }
    else if (c == '+' || c == '-' || c == '/' || c == '*' || c == '^' || c == '%') {
      //printf("Opeartor\n");
      //BINARY AND SEND NU
      if (inNumber == 1) {
        tokens[tkn_ptr] = malloc(sizeof(struct Token));
        strcpy(tokens[tkn_ptr]->value, nbuffer);
        if (inDecimal == 0) {
          strcat(tokens[tkn_ptr]->value, ".0");
        }
        tokens[tkn_ptr]->type = NUMBER;
        nbuffer[0] = '\0';
        num_ptr = 0;
        tkn_ptr++;
        tokens[tkn_ptr] = NULL;
        lastTokenType = NUMBER;
        inNumber = 0;
        inDecimal = 0;

        tokens[tkn_ptr] = malloc(sizeof(struct Token));
        tokens[tkn_ptr]->value[0] = c;
        tokens[tkn_ptr]->value[1] = '\0';
        tokens[tkn_ptr]->type = BINARY;
        tkn_ptr++;
        tokens[tkn_ptr] = NULL;
        lastTokenType = BINARY;     
      //BINARY NO NUMBER
      } else if (lastTokenType == PCLOSE) {
        tokens[tkn_ptr] = malloc(sizeof(struct Token));
        tokens[tkn_ptr]->value[0] = c;
        tokens[tkn_ptr]->value[1] = '\0';
        tokens[tkn_ptr]->type = BINARY;

        tkn_ptr++;
        tokens[tkn_ptr] = NULL;
        lastTokenType = BINARY;     

      }
      //UNARY
      else if (lastTokenType == POPEN || lastTokenType == BINARY || lastTokenType == UNARY) {
        tokens[tkn_ptr] = malloc(sizeof(struct Token));
        tokens[tkn_ptr]->value[0] = c;
        tokens[tkn_ptr]->value[1] = '\0';
        tokens[tkn_ptr]->type = UNARY;
        tkn_ptr++;
        tokens[tkn_ptr] = NULL;
        lastTokenType = UNARY;     
      }
    }
  }
  if (inNumber) {
        tokens[tkn_ptr] = malloc(sizeof(struct Token));
        strcpy(tokens[tkn_ptr]->value, nbuffer);

        if (inDecimal == 0) {
          strcat(tokens[tkn_ptr]->value, ".0");
        }
        tokens[tkn_ptr]->type = NUMBER;

        nbuffer[0] = '\0';
        num_ptr = 0;
        tkn_ptr++;
        tokens[tkn_ptr] = NULL;
        lastTokenType = NUMBER;     

  }
  //printf("Finished Tokenizing\n");
  return tokens;
}

int precidence(struct Token *t) {
  char c = t->value[0];
  switch (c) {
    case '+': return 0;
    case '-': return 0;
    case '*': return 1;
    case '/': return 1;
    case '^': return 2;
  }
  return -1;
}

char associativity(struct Token *t) {
    char c = t->value[0];
    switch (c) {
    case '+': return 'l';
    case '-': return 'l';
    case '*': return 'l';
    case '/': return 'l';
    case '^': return 'r';
  }
  return '\0';
}

int isBalanced(char *string) {
  int plevel = 0;
  for (int i = 0; string[i] != '\0'; i++) {
    if (plevel < 0) return 0;
    if (string[i] == '(') plevel++;
    else if (string[i] == ')') plevel--;
  }
  if (plevel == 0) return 1;
  else return 0;
}

void printTokens(struct Token **tokens) {
  for (int i = 0; tokens[i] != NULL; i++) {
    printf("%d\t%c\t%s\n",i, tokens[i]->type,tokens[i]->value);
  }
}

void printToken(struct Token *token) {
  printf("%c\t%s\n", token->type,token->value);

}


char *mathEvaluateExpression(char *expr) {
  if (!isBalanced(expr)) {
    printf("Expression not Balanced!\n");
    return "Parenthesis Mismatch";
  }

  struct Token **tokens = mtokenize(expr);
  //printTokens(tokens);

  struct Token **stack = malloc(sizeof(struct Token *) * 128);
  int stackptr = 0;
  struct Token **queue = malloc(sizeof(struct Token *) * 128);
  int queueptr = 0;

  //printf("Starting Shunting Yard Algorithm\n");
  for (int i = 0; tokens[i] != NULL; i++) {
    struct Token *tok = tokens[i];
    //printToken(tok);
    if (tok->type == NUMBER) {
      queue[queueptr] = tok;
      queueptr++;
    }

    if (tok->type == UNARY) {
      stack[stackptr] = tok;
      stackptr++;
    }

    if (tok->type == BINARY) {
      while(stackptr > 0 && stack[stackptr-1]->type != POPEN && (precidence(stack[stackptr-1]) > precidence(tok) || (precidence(stack[stackptr-1]) == precidence(tok) && associativity(tok) == 'l'))) {
        queue[queueptr] = stack[stackptr-1];
        queueptr++;
        stackptr--;
      }
      stack[stackptr] = tok;
      stackptr++;
    }

    if (tok->type == POPEN) {
      stack[stackptr] = tok;
      stackptr++;
    }

    if (tok ->type == PCLOSE) {
      while (stack[stackptr-1]->type != POPEN) {
        queue[queueptr] = stack[stackptr-1];
        queueptr++;
        stackptr--;
      }
      stackptr--;
      if (stack[stackptr-1]->type == UNARY) {
        queue[queueptr] = stack[stackptr-1];
        queueptr++;
        stackptr--;
      }
    }
  }

  while(stackptr > 0) {
    queue[queueptr] = stack[stackptr-1];
    queueptr++;
    stackptr--;
  }
  free(tokens);
  free(stack);
  //printf("Reverse Polish:\n");
  //printTokens(queue);

  double *numstack = malloc(sizeof(double) * 64);
  int numidx = 0;
  struct Token *tok;
  for (int i = 0; i < queueptr; i++) {
    //printf("STACK numidx [%d]:", numidx);
    //for (int j  = 0; j < numidx; j++) printf("%lf|", numstack[j]);
    //printf("\n");
    tok = queue[i];
    if (tok->type == NUMBER) {
        //printf("Putting %s on the Number Stack\n", tok->value);
        numstack[numidx] = strtod(tok->value, NULL);
        numidx++;
    } else if (tok->type == UNARY) {
        //printf("%s %lf\n", tok->value, numstack[numidx-1]);
        switch (tok->value[0]) {
          case '-':
            numstack[numidx-1] = 0.0 - numstack[numidx-1];
            break;
          case '+':
            numstack[numidx-1] = 0.0 + numstack[numidx-1];
            break;
          case '*':
            numstack[numidx-1] = 1.0 * numstack[numidx-1];
            break;
          case '/':
            numstack[numidx-1] = 1.0 / numstack[numidx-1];
            break;
          case '^':
            break;
            numstack[numidx-1] = pow(1.0, numstack[numidx-1]);
            break;
        }
      } else if (tok->type == BINARY) {
        //printf("%lf %s %lf\n",numstack[numidx-2], tok->value, numstack[numidx-1]);

        switch (tok->value[0]) {
          case '-':
            numstack[numidx-2] = numstack[numidx-2] - numstack[numidx-1];
            break;
          case '+':
            numstack[numidx-2] = numstack[numidx-2] + numstack[numidx-1];
            break;
          case '*':
            numstack[numidx-2] = numstack[numidx-2] * numstack[numidx-1];
            break;
          case '/':
            numstack[numidx-2] = numstack[numidx-2] / numstack[numidx-1];
            break;
          case '^': numstack[numidx-2] =  pow(numstack[numidx-2], numstack[numidx-1]);
            break;
        }
        numidx--;



}
            
      
  }
  double n = numstack[0];

  char *buff = malloc(16);
  
  if ( fabs((n - (double)round(n))) > 0.01) {
    sprintf(buff, "%.3lf", n);
  } else {
    sprintf(buff, "%d", (int)round(n));

  }
  return buff; 
}
