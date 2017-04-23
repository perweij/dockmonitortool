/*

    dockmonitortool.c

    See README about what it does.
    See LICENSE for licensing details.

    Per Weijnitz <per.weijnitz@gmail.com>


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "config.h"



//// CONSTANTS

// The file in sysfs corresponding to the dock. It is a file
// that contains int 0 when no dock is present, and not 0 when
// a dock is present. This may vary between different hardware, so
// you need to find the file that is right for your system.
#define DOCK "/sys/devices/platform/hp-wmi/dock"

// Define the two commands which are your two perfekt display setups,
// one for when the computer is docked, and one for when it's not.
#define CMD_NODOCK { "/usr/bin/xrandr", "--output", "DP3", "--off", "--output", "DP2", "--off", "--output", "DP1", "--off", "--output", "HDMI3", "--off", "--output", "HDMI2", "--off", "--output", "HDMI1", "--off", "--output", "LVDS1", "--mode", "1366x768", "--pos", "0x0", "--rotate", "normal", "--output", "VGA1", "--off", NULL }
#define CMD_DOCK   { "/usr/bin/xrandr", "--output", "DP3", "--off", "--output", "DP2", "--off", "--output", "DP1", "--mode", "1920x1200", "--pos", "0x0", "--rotate", "normal", "--output", "HDMI3", "--off", "--output", "HDMI2", "--off", "--output", "HDMI1", "--off", "--output", "LVDS1", "--off", "--output", "VGA1", "--off", NULL }



// some macros
#define LOG(...)         do{ fprintf(stderr, __VA_ARGS__); } while(0)
#define DIE(...)         do{ fprintf(stderr, __VA_ARGS__); \
                             exit(EXIT_FAILURE); } while(0)
#define DIE_PERROR(...)  do{ fprintf(stderr, __VA_ARGS__); perror(0); \
                             exit(EXIT_FAILURE); } while(0)


// prototypes
int read_int (const char* file_name);
void execcmd(const char* cmd[]);



/*
 * Function: read_int
 * -----------------------
 *   Reads a single integer from the given file.
 *
 *   file_name: name of the file to read from.
 *
 *   returns: an integer.
 *
 *   todo: switch to strtol, add error checks
 */
int read_int (const char* file_name) {
  FILE* file = fopen (file_name, "r");
  int i = 0;

  fscanf (file, "%d", &i);    
  fclose (file);

  return i;
}



/*
 * Function: execcmd
 * -----------------------
 *   Execute a command, and block while it is executing.
 *
 *   cmd: the command to execute, including its arguments.
 *
 */
void execcmd(const char* cmd[]) {
  pid_t pid = -1;

  pid = fork();
  if (pid == -1) {
    perror(0);
    _exit(EXIT_FAILURE);
  } else if (pid > 0) {
    // parent
    int status;
    waitpid(pid, &status, 0);
  } else {
    // child
    setsid();
    execv(cmd[0], (char * const*)cmd);
    perror("execv");
    _exit(EXIT_FAILURE);   // exec never returns
  }
}



int main(int argc, char *argv[]) {
  int dockstatus = -1;
  int nextval = -1;
  const char *cmd_dock[] = CMD_DOCK;
  const char *cmd_nodock[] = CMD_NODOCK;

  
  while(1) {
    nextval = read_int(DOCK);
    if(nextval != dockstatus) {
      dockstatus = nextval;
      LOG("DOCK IS %d\n", dockstatus);
      sleep(1);
      
      if(dockstatus == 0) {
        execcmd(cmd_nodock);
      } else {
        execcmd(cmd_dock);
      }
    }
    sleep(2);
  }

 
  return 0;
}


/* eof */
