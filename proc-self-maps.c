#include <stdio.h>
#include <string.h>
#include <assert.h>

#define NAME_LEN 80
struct proc_maps_line {
  char *start;
  char *end;
  char rwxp[4];
  int read, write, execute; // Not used in this versoin of the code
  char name[NAME_LEN]; // for debutting only
  int is_register_context; //for register context
  int data_size;
};

// Returns 0 on success
int match_one_line(FILE *proc_maps_file,
                   struct proc_maps_line *proc_maps_line, char *filename) {
  unsigned long int start, end;
  char rwxp[4];
  char tmp[10];
  int rc = fscanf(proc_maps_file, "%lx-%lx %4c %*s %*s %*[0-9 ]%[^\n]\n",
                    &start, &end, rwxp, filename);
  if (rc == EOF || rc == 0) { 
    proc_maps_line -> start = NULL;
    proc_maps_line -> end = NULL;
    return EOF;
  }
  if (rc == 3) {
    filename[0] = '\0';
  } else {
    assert( rc == 4 );
    strncpy(proc_maps_line -> name, filename, NAME_LEN-1);
    proc_maps_line->name[NAME_LEN-1] = '\0';
  }
  proc_maps_line -> start = (void *)start;
  proc_maps_line -> end = (void *)end;
  memcpy(proc_maps_line->rwxp, rwxp, 4);
  proc_maps_line->is_register_context=0;
  proc_maps_line->data_size=end-start;
  return 0;
}

int proc_self_maps(struct proc_maps_line proc_maps[]) {
  // NOTE:  fopen calls malloc() and potentially extends the heap segment.
  FILE *proc_maps_file = fopen("/proc/self/maps", "r");
  char filename [100]; // for debugging
  int i = 0;
  int rc = -2; // any value that will not terminate the 'for' loop.
  for (i = 0; rc != EOF; i++) {
    rc = match_one_line(proc_maps_file, &proc_maps[i], filename);
#ifdef DEBUG
    if (rc == 0) {
      printf("proc_self_maps: filename: %s\n", filename); // for debugging
    }
#endif
  }
  fclose(proc_maps_file);
  return 0;
}

#ifdef STANDALONE
int main(int argc, char *argv[]) {
  struct proc_maps_line proc_maps[1000];
  assert( proc_self_maps(proc_maps) == 0 );
  printf("    *** Memory segments of %s ***\n", argv[0]);
  int i = 0;
  for (i = 0; proc_maps[i].start != NULL; i++) {
    printf("%s (%c%c%c)\n"
           "  Address-range: %p - %p\n",
           proc_maps[i].name,
           proc_maps[i].rwxp[0], proc_maps[i].rwxp[1], proc_maps[i].rwxp[2],
           proc_maps[i].start, proc_maps[i].end);
  }
  return 0;
}
#endif
