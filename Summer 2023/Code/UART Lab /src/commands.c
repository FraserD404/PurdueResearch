#include "stm32f0xx.h"
#include "tty.h"
#include "commands.h"
#include <string.h>
#include <stdio.h>

void dino(int argc, char *argv[])
{
    const char str[] =
    "   .-~~^-.\n"
    " .'  O    \\\n"
    "(_____,    \\\n"
    " `----.     \\\n"
    "       \\     \\\n"
    "        \\     \\\n"
    "         \\     `.             _ _\n"
    "          \\       ~- _ _ - ~       ~ - .\n"
    "           \\                              ~-.\n"
    "            \\                                `.\n"
    "             \\    /               /       \\    \\\n"
    "              `. |         }     |         }    \\\n"
    "                `|        /      |        /       \\\n"
    "                 |       /       |       /          \\\n"
    "                 |      /`- _ _ _|      /.- ~ ^-.     \\\n"
    "                 |     /         |     /          `.    \\\n"
    "                 |     |         |     |             -.   ` . _ _ _ _ _ _\n"
    "                 |_____|         |_____|                ~ . _ _ _ _ _ _ _ >\n";
    puts(str);
}

struct commands_t cmds[] = {
        { "dino", dino },
};

void add(int argc, char *argv[])
{
  int sum = 0;
  for(int i=1; i < argc; i++) {
      sum += strtol(argv[i], 0, 0);
  }
  printf("The sum is %d\n", sum);
}

void mul(int argc, char *argv[])
{
  int prod = 1;
  for(int i=1; i < argc; i++) {
    prod *= strtol(argv[i], 0, 0);
  }
  printf("The product is %d\n", prod);
}

// Define your bird command function here
void bird(int argc, char *argv) {
        const char str[] = "                                    /\\_/\\\n";
        puts(str);

}

struct commands_t usercmds[] = {
  { "add",    add },
  { "mul",    mul },
  // define the bird command pointer here
  {"bird", bird}
};


// A weak definition that can be overridden by a better one.
__attribute((weak)) struct commands_t usercmds[] = {
        { 0, 0 }
};

void exec(int argc, char *argv[])
{
    //for(int i=0; i<argc; i++)
    //    printf("%d: %s\n", i, argv[i]);
    for(int i=0; usercmds[i].cmd != 0; i++)
        if (strcmp(usercmds[i].cmd, argv[0]) == 0) {
            usercmds[i].fn(argc, argv);
            return;
        }
    for(int i=0; i<sizeof cmds/sizeof cmds[0]; i++)
        if (strcmp(cmds[i].cmd, argv[0]) == 0) {
            cmds[i].fn(argc, argv);
            return;
        }
    printf("%s: No such command.\n", argv[0]);
}


void parse_command(char *c)
{
    char *argv[20];
    int argc=0;
    int skipspace=1;
    for(; *c; c++) {
        if (skipspace) {
            if (*c != ' ' && *c != '\t') {
                argv[argc++] = c;
                skipspace = 0;
            }
        } else {
            if (*c == ' ' || *c == '\t') {
                *c = '\0';
                skipspace=1;
            }
        }
    }
    if (argc > 0) {
        argv[argc] = "";
        exec(argc, argv);
    }
}


static void insert_echo_string(const char *s)
{
    // This puts a string into the *input* stream, so it can be edited.
    while(*s)
        insert_echo_char(*s++);
}

void command_shell(void)
{
  char line[100];
  int len = strlen(line);
  puts("This is the STM32 command shell.");
  for(;;) {
      printf("> ");
      fgets(line, 99, stdin);
      line[99] = '\0';
      len = strlen(line);
      if (line[len-1]) == '\n'
          line[len-1] = '\0';
      parse_command(line);
  }
}