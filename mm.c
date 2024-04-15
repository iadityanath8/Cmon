#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))
#define TEST 1

void call_command(){

}

int monitor_directories(){
  int fd, wd;
  char buffer[BUF_LEN];

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

    int i = 0;
    while (i < length) {
      struct inotify_event *event = (struct inotify_event *)&buffer[i];

      if (event->len) {
        if (event->mask & IN_CREATE) {
#if TEST
          printf("File %s created.\n", event->name);
#endif
        }
        if (event->mask & IN_MODIFY) {
          call_command();
#if TEST
          printf("File %s modified.\n", event->name);
#endif
        }
        if (event->mask & IN_DELETE) {
#if TEST
          printf("File %s deleted.\n", event->name);
#endif
        }
      }
      i += EVENT_SIZE + event->len;
    }
  }

  inotify_rm_watch(fd, wd);
  close(fd);

  return 0;

}

int main(int argc,char* argv[]) {
  int a = system("ls -a");
  if(a == -1){
    perror("Failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}





