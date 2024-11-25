#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {
  static char instr[32768];
  static char stack[32768];
  FILE* input;
  short ip = 0;
  short sp = 1;
  short tc = 0;
  short cc = 0;

  if (argc != 2) {
    fprintf(stderr, "Usage: sf <file>\n");
    return 2;
  }

  input = fopen(argv[1], "r");
  if (!fread(instr, 1, 32768 - 1, input)) {
    fprintf(stderr, "Error: failed to read file %s\n", argv[1]);
    return 1;
  }

  START:
  switch (instr[ip++]) {
    
    case '+':
      stack[sp] += 1;
      goto START;
    
    case '-':
      stack[sp] -= 1;
      goto START;
    
    case '<':
      sp += 1;
      stack[sp] = stack[sp - 1];
      goto START;

    case '=':
      {
        char tmp = stack[sp];
        stack[sp] = stack[sp - 1];
        stack[sp - 1] = stack[sp - 2];
        stack[sp - 2] = tmp;
      }
      goto START;
    
    case '>':
      sp -= 1;
      if (sp == 0) {
        return stack[sp];
      }
      goto START;
    
    case ',':
      sp += 1;
      stack[sp] = fgetc(stdin);
      goto START;
    
    case '.':
      fputc(stack[sp], stdout);
      fputc('\n', stdout);
      sp -= 1;
      if (sp == 0) {
        return stack[sp];
      }
      goto START;
    
    case '[':
      cc += 1;
      if (stack[sp] != 0) {
        goto START;
      }
      tc = cc;  
      SEEKRIGHT:
      switch (instr[ip++]) {
        
        case '[':
          cc += 1;
          goto SEEKRIGHT;
        
        case ']':
          if (cc == tc) {
            goto START;
          }
          cc -= 1;
          goto SEEKRIGHT;

        default:
          goto SEEKRIGHT;
      }

    case ']':
      cc -= 1;
      if (stack[sp] == 0) {
        goto START;
      }
      ip -= 2;
      tc = cc;
      SEEKLEFT:
      switch (instr[ip--]) {

        case '[':
          if (cc == tc) {
            ip += 1;
            goto START;
          }
          cc += 1;
          goto SEEKLEFT;

        case ']':
          cc -= 1;
          goto SEEKLEFT;
        
        default:
          goto SEEKLEFT;
      }

    default:
      goto START;
  }

  return stack[sp];
}
