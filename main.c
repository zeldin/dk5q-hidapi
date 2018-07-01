#include <stdio.h>
#include <stdlib.h>

#include "dk5q.h"

int main(int argc, char* argv[])
{
  dk5q_handle handle;

  if (!(handle = dk5q_open(0))) {
    fprintf(stderr, "Failed to open DK5Q\n");
    return EXIT_FAILURE;
  }

  int i;
  for(i=0x19; i<0xa7; i++)
    dk5q_set_key_rgb(handle, i, 0xb4, 0x01, 0xfc);
  dk5q_apply(handle);

  dk5q_close(handle);
  return EXIT_SUCCESS;
}
