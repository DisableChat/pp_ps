//******************************************************************************
// File:          pp_ps.cc
// Author:        Wesley Ryder
// Description:   This code is a worse implemenation of the unix tool "ps"
//                but is strictly to help my understanding of the proc file
//                system (>0.0)>
//******************************************************************************

#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <dirent.h>
#include <vector>
#include <math.h>
#include <unistd.h>
#include <iomanip>
using namespace std;

int const PROC_TYPE           = 4;
int const MAX_ARGC_COUNT      = 2;
int const ARG_POS             = 1;



//******************************************************************************
// Struct:        PID
// Traits:        int u       pid, process ID
//                string      cmd, command
//                char        state, State
//                float       cpu, CPU percentage
//                float       mem, MEM percentage
//                long u      VSZ, Virtural memory size
//                long u      RSS, Resident set size
//                int u       cpu_exec, Last excuted
//                long u      stime, process since kernal mode
//                long long u uptime, time scheduled in user mode
//                long long u starttime, time process after system boot
// Description:   Structure to represent the /proc/pid/---- info
//******************************************************************************
struct PID {
  unsigned long pid;
  // Command
  string cmd;
  // State
  char state;
  // % Cpu
  long double cpu;
  // % Memory
  float mem;
  // Virtural memory size
  unsigned long long int VSZ;
  // Resident set size
  unsigned long long int RSS;
  // Cpu last excuted
  unsigned int cpu_exec;
  // Amount of time that this process has been scheduled in kernal mode
  unsigned long int stime;
  // Amount of time that this process has been secheduled in user mode
  unsigned long int utime;
  // Time porcess started after system boot
  unsigned long long int starttime;
};


//******************************************************************************
// Function:      compare_cpu()
// Arguments:     const PID (struct) x
//                const PID (struct) y
// Description:   Compares passed in struct.cpu values
//******************************************************************************
bool compare_cpu(const PID &x, const PID &y)
{
  return x.cpu > y.cpu;
}

//******************************************************************************
// Function:      compare_mem()
// Arguments:     const PID (struct) x
//                const PID (struct) y
// Description:   Compares passed in struct.mem values
//******************************************************************************
bool compare_mem(const PID &x, const PID &y)
{
  return x.mem > y.mem;
}

//******************************************************************************
// Function:      compare_pid()
// Arguments:     const PID (struct) x
//                const PID (struct) y
// Description:   Compares passed in struct.pid values
//******************************************************************************
bool compare_pid(const PID &x, const PID &y)
{
  return x.pid < y.pid;
}

//******************************************************************************
// Function:      compare_cmd()
// Arguments:     const PID (struct) x
//                const PID (struct) y
// Description:   Compares passed in struct.cmd values
//******************************************************************************
bool compare_cmd(const PID &x, const PID &y)
{
  return x.cmd < y.cmd;
}



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

  // Checking if passing exactly one command
  if(argc != MAX_ARGC_COUNT)
  {
    fprintf(stderr, "Error: ./pp_ps <command>: No argument '%d'\n", argc);
    exit(-1);
  }

  // Setting cmd value
  cmd = argv[ARG_POS];

  // Set code value corosponding to cmd passed
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
// Function:      parse_line()
// Arguments:     string line_buffer
// Description:   Parses the /proc/<pid>/stat info and then inserts corrosponding
//                struct attributes. Returning the struct itself when finished
//******************************************************************************
PID parse_line(string line_buffer)
{
    int index = 1;
    PID tmp_pid;
    string tmp;
    stringstream iss(line_buffer);
    while(iss >> tmp)
    {
      //cout << tmp << endl;
      if(index == 1)
      {
        tmp_pid.pid = stoul(tmp);
        //cout << tmp_pid.pid << endl;
      }
      else if(index == 2)
      {
        if( tmp.front() == '(' && tmp.back() != ')')
        {
          string tmp2;
          const int lim = 1000;
          int lim_counter = 0;
          char tmp_char = ' ';

          while(tmp_char != ')' && lim_counter < lim)
          {
            tmp2 = tmp;
            iss >> tmp;
            tmp = tmp2 + tmp;
            lim_counter++;
            tmp_char = tmp.back();
          }
          if(lim_counter >= lim) {perror("command name was invalid\n");}
        }
        tmp.erase(remove(tmp.begin(), tmp.end(), ')'), tmp.end());
        tmp.erase(remove(tmp.begin(), tmp.end(), '('), tmp.end());
        tmp_pid.cmd = tmp;
        //cout << tmp_pid.cmd << endl;
      }
      else if(index == 3)
      {
        tmp_pid.state = tmp[0];
        //cout << tmp_pid.state << endl;
      }
      else if(index == 14)
      {
        tmp_pid.utime = stoul(tmp);
        //cout << tmp_pid.utime << endl;
      }
      else if(index == 15)
      {
        tmp_pid.stime = stoul(tmp);
        //cout << tmp_pid.stime << endl;
      }
      else if(index == 22)
      {
        tmp_pid.starttime = stoull(tmp);
        //cout << tmp_pid.starttime << endl;
      }
      else if(index == 23)
      {
        tmp_pid.VSZ = stoull(tmp);
        //cout << tmp_pid.VSZ << endl;
      }
      else if(index == 24){
        tmp_pid.RSS = stoull(tmp);
        //cout << tmp_pid.RSS << endl;
      }
      else if(index == 39)
      {
        tmp_pid.cpu_exec = stoull(tmp);
        //cout << tmp_pid.cpu_exec << endl;
      }
      index++;
    }
    return tmp_pid;
}


//******************************************************************************
// Function:      display()
// Arguments:     vector<PID> p_pid
//                int cmd_code
// Description:   Displays to terminal all nessary proc info
//******************************************************************************
void display(vector<PID> p_pid, int cmd_code)
{
  const int LINES_OUTPUT = 50;

  system("tabs 19");
  cout << fixed;
  cout << setprecision(6);

  if(cmd_code == 1){sort(p_pid.begin(), p_pid.end(), compare_cpu);}
  else if(cmd_code == 2){sort(p_pid.begin(), p_pid.end(), compare_mem);}
  else if(cmd_code == 3){sort(p_pid.begin(), p_pid.end(), compare_pid);}
  else if(cmd_code == 4){sort(p_pid.begin(), p_pid.end(), compare_cmd);}

  cout << "PID\tCommand\tState  %CPU\t%Mem\tVSZ\tRSS\tCore" << endl;
  cout << "--------------------------------------------------------------" <<
  "---------------------------------------------------------" << endl;
  for(int i = 0; i < p_pid.size(); i++)
  {
    cout << p_pid[i].pid << "\t" << p_pid[i].cmd << "\t" << p_pid[i].state <<
    "      " << p_pid[i].cpu << "\t" << p_pid[i].mem << "\t" << p_pid[i].VSZ <<
    "\t" << p_pid[i].RSS << "\t" << p_pid[i].cpu_exec << "\n";
  }
}

//******************************************************************************
// Function:      main()
// Arguments:     int argc
//                char** argv
// Description:   To run the main of the program, diving into that proc folder
//                kappa
//******************************************************************************
int main(int argc, char** argv)
{
  // Nessary Variables
  string PROC             = "/proc";
  string uptime           = "uptime";
  long double uptime_val  = 0;
  string cmd              = "";
  int cmd_code            = 0;
  int pid;
  int read;

  // Vector holding type struct (PID)
  vector<PID> p_pid;

  // Pointers
  struct dirent *ent;
  DIR           *dir;
  FILE          *fp;

  // Available mem
  long phys_pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  long phys_mem_size = phys_pages * page_size;

  cmd_code = get_cmd(argc, argv);
  dir = opendir ("/proc");
  if (dir != NULL)
  {
    // print all the pid's within /proc
    while ((ent = readdir (dir)) != NULL)
    {
      // Directory ed->d_name
      if(uptime.compare(ent -> d_name) == 0)
      {
        char tmp[255];
        char* line_buffer = NULL;
        size_t  blength;
        sprintf(tmp, "/proc/%s", "uptime");
        fp = fopen(tmp, "r");
        getline(&line_buffer, &blength, fp);
        int i = 1;
        string tmp_;
        istringstream iss(line_buffer);

        while(iss){
          string tmp_;
          iss >> tmp_;
          if(i == 1)
          {
            uptime_val = stold(tmp_);
          }
          i++;
        }
        fclose(fp);
      }
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
          //printf("Line: %s\n", line_buffer);
          PID process;
          process = parse_line(line_buffer);

          process.mem = ((process.RSS * getpagesize() * 100.0) / phys_mem_size);
          long double real_time;
          real_time = uptime_val - (process.starttime/sysconf(_SC_CLK_TCK));
          long double process_time = (process.utime / sysconf(_SC_CLK_TCK) ) + ( process.stime / sysconf(_SC_CLK_TCK) );
          process.cpu = process_time * 100.0 /real_time;

          // Pushing the process on the vector of type struct
          p_pid.push_back(process);
          free(line_buffer);
        }
      }
    }
    // Display the vector
    display(p_pid, cmd_code);
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
