#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))
#define TEST 1
#define UNSAFE 1
#define COMMAND_LEN 1023*10

#define CHECK_TIME(time_val,msg)  {if (time(NULL) - last_print_time > time_val) {\
  printf("File %s %s.\n", event->name,msg);\
  last_print_time = time(NULL);\
}}

#define CHECK_TIME_EXCUTE(time_val,excuter_callBack){\
  if(time(NULL) - last_print_time > time_val){\
    last_print_time = time(NULL);\
    excuter_callBack(cmd_buff);\
  }\
}

void execute_cmdBuffer(char cmd_buffer[COMMAND_LEN]){
#if UNSAFE
  system(cmd_buffer);
#endif
}

int monitor_directories(char cmd_buff[COMMAND_LEN]){
  int fd, wd;
  char buffer[BUF_LEN];
  time_t last_print_time = 0; // Last time a message was printed

  fd = inotify_init();
  if (fd < 0) {
    perror("inotify_init");
    exit(EXIT_FAILURE);
  }

  wd = inotify_add_watch(fd, ".", IN_MODIFY | IN_CREATE | IN_DELETE);
  if (wd < 0) {
    perror("inotify_add_watch");
    exit(EXIT_FAILURE);
  }

  printf("Monitoring current directory for changes...\n");
  while (1) {
    int length = read(fd, buffer, BUF_LEN);
    if (length < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    // Reset last_file for each iteration

    int i = 0;
    while (i < length) {
      struct inotify_event *event = (struct inotify_event *)&buffer[i];

      if (event->len) {
        if (event->mask & IN_CREATE) {
          //CHECK_TIME(0.5,"CREATED");
          CHECK_TIME_EXCUTE(0.5,execute_cmdBuffer);
        }
        if (event->mask & IN_MODIFY) {
          //CHECK_TIME(0.5,"MODIFIED");
          CHECK_TIME_EXCUTE(0.5,execute_cmdBuffer);
        }
        if (event->mask & IN_DELETE) {
          //CHECK_TIME(0.5,"DELETE");
          CHECK_TIME_EXCUTE(0.5,execute_cmdBuffer);
        }
      }
      i += EVENT_SIZE + event->len;
    }
  }

  inotify_rm_watch(fd, wd);
  close(fd);

  return 0;
}

void join_argv(int argc,char** argv,char* cmd_buff){
  size_t cidx = 0; 

  for(int i = 1;i < argc;i++){
    for(size_t j = 0;j < strlen(argv[i]);j++){
      cmd_buff[cidx] = argv[i][j];
      cidx++;
    }
    cmd_buff[cidx] = ' ';
    cidx++;
  }
  
  assert(cidx + 3 < COMMAND_LEN);
  cmd_buff[cidx] = '\0';
}


int main(int argc,char* argv[]) {
  assert(argc > 1);

#if UNSAFE
  char cmd_buff[COMMAND_LEN];
#endif
  
  join_argv(argc,argv,cmd_buff);
  
  monitor_directories(cmd_buff);
  return 0;
}






