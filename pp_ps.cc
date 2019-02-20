//******************************************************************************
// File:          pp_ps.cc
// Author:        Wesley Ryder
// Description:   This code is a worse implemenation of the unix tool "ps"
//                but is strictly to help my understanding of the proc file
//                system (>0.0)>
//******************************************************************************

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <dirent.h>
#include <vector>
using namespace std;

int const PROC_TYPE           = 4;
int const MAX_ARGC_COUNT      = 2;
int const ARG_POS             = 1;



//******************************************************************************
// Struct:        PID
// Traits:        u_int     pid, process ID
//                string    cmd, command
//                char      state, State
//                float     cpu, CPU percentage
//                float     mem, MEM percentage
//                u_long    VSZ, Virtural memory size
//                u_long    RSS, Resident set size
//                u_int     cpu_exec, Last excuted
// Description:   Structure to represent the /proc/pid/---- info
//******************************************************************************
struct PID {
  u_int pid;
  // Command
  string cmd;
  // State
  char state;
  // % Cpu
  float cpu;
  // % Memory
  float mem;
  // Virtural memory size
  u_long VSZ;
  // Resident set size
  u_long RSS;
  // Cpu last excuted
  u_int cpu_exec;
};


//******************************************************************************
// Function:      get_cmd()
// Arguments:     int argc
//                char** argv
// Description:   To retrieve the inline arguments passed and return a code
//                corosponding to the argument, if non recognized cmd then fail
//
// Commdands:     <-cpu> sort the rows based on “%CPU” (from highest to lowest)
//                <-mem> sort the rows based on “%MEM (from highest to lowest)
//                <-pid> sort the rows based on "PID" (from lowest to highest)
//                <-com> sort the rows lexicographically by command name
//******************************************************************************
int get_cmd(int argc, char** argv)
{
  string cmd;
  int cmd_code;
  cmd = argv[ARG_POS];

  if(cmd == "-cpu")
    {cmd_code = 1;}
  else if(cmd == "-mem")
    {cmd_code = 2;}
  else if(cmd == "-pid")
    {cmd_code = 3;}
  else if(cmd == "-com")
    {cmd_code = 4;}
  else
  {
    fprintf(stderr, "Error: ./pp_ps <command>: Unknown argument '%s'\n", argv[ARG_POS]);
    exit(-1);
  }

  return cmd_code;
}



//******************************************************************************
// Function:      main()
// Arguments:     int argc
//                char** argv
// Description:   To run the main of the program, diving into that proc folder
//                kappa
//******************************************************************************
int main(int argc, char** argv) {

  // Nessary Variables
  string PROC           = "/proc";
  string cmd            = "";
  int cmd_code          = 0;
  int pid;
  int read;

  // Vector holding type struct (PID)
  vector<PID> p_pid;

  // Pointers
  struct dirent *ent;
  DIR           *dir;
  FILE          *fp;


  cmd_code = get_cmd(argc, argv);
  dir = opendir ("/proc");
  if (dir != NULL)
  {
    // print all the pid's within /proc
    while ((ent = readdir (dir)) != NULL)
    {
      pid = atoi(ent -> d_name);
      if((ent -> d_type == PROC_TYPE) && (pid > 0))
      {
        char tmp[255];
        char* line_buffer = NULL;
        size_t  blength;

        // Directory ed->d_name
        sprintf(tmp, "/proc/%d/stat", pid);
        // Open stat file of current pid
        fp = fopen(tmp, "r");
        if(!fp)
        {
          fprintf(stderr, "Error: can't open file %s\n", tmp);
          exit(-1);
        }
        while( (read = getline(&line_buffer, &blength, fp)) != -1)
        {
          printf("Line: %s\n", line_buffer);
          free(line_buffer);
        }
        //cout << "pid " << pid << endl;
        //printf ("%s\n", ent->d_name
      }
    }
    closedir (dir);
  }
  else
  {
    // Could not open directory
    perror("Error: Unable to open directory");
    exit(-1);
  }

  return 0;
}
