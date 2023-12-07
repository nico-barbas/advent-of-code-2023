#include <vector>
#include <string>

int main(void)
{
  FILE *fptr;
  char input[1024 * 16];

  fptr = fopen("test_input.txt", "r");
  if (fptr == nullptr)
  {
    return -1;
  }

  char *cptr = &input[0];
  while (!feof(fptr))
  {
    cptr = fgets(cptr, 1024 * 16, fptr);
    if (ferror(fptr))
    {
      fprintf(stderr, "Error while reading file: %d\n", errno);
      break;
    }
    // puts
  }

  printf("%s\n", input);

  std::vector<std::string> lines = {};

  return 0;
}