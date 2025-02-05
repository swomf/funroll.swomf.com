#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Hopefully a USE flag list doesn't get any longer. */
#define MAX_LINE_LENGTH 512

/* Dir where we extract all of our docs' code blocks into conf files */
#define TANGLE_DIR "gentoo_install"

/* C99 assumed */
#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)                                                       \
  do {                                                                         \
  } while (0)
#endif

#define expect(a, ...)                                                         \
  do {                                                                         \
    if (!(a)) {                                                                \
      fprintf(stderr, __VA_ARGS__);                                            \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

int
prepare_parent_dir_for(const char* path)
{
  char* dir = strdup(path);
  expect(dir != NULL,
         "Error: strdup failure (prepare_parent_dir_for(\"%s\"))",
         path);

  /* Recursively mkdir by switching '/' with null terminators one-by-one.
   * Ignore the last '/'-delimited field; that is the filename.
   */
  for (char* p = dir + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      DEBUG_PRINT(" * mkdir %s\n", dir);
      if (mkdir(dir, 0755) != 0 && errno != EEXIST) {
        free(dir);
        return -1;
      }
      *p = '/';
    }
  }

  free(dir);
  return 0;
}

int
main(int argc, char** argv)
{
  expect(argc == 2, "%s", "Error: First argument should be a markdown file.");
  const char* input_markdown_filename = argv[1];
  expect(strlen(input_markdown_filename) >= 3 &&
           strcmp(input_markdown_filename + strlen(input_markdown_filename) - 3,
                  ".md") == 0,
         "Error: Markdown file expected. (\"%s\" does not end in .md)",
         input_markdown_filename);

  /* Multiple code blocks can reference the same conf file; then, we append.
   * However, this means that conf files should be erased anew when generation
   * is performed multiple times, i.e.
   *     rm -rf repo_dir/gentoo_install
   */
  FILE* input_markdown_file = fopen(input_markdown_filename, "r");
  expect(input_markdown_file != NULL,
         "Error: failed to open file %s",
         input_markdown_filename);
  FILE* output_conf_file = NULL;

  char line[MAX_LINE_LENGTH];
  char prepended_path[MAX_LINE_LENGTH];
  int inside_code_block = 0;
  int inside_conf_file_code_block = 0;
  /* Conf code blocks should start with triple backticks and have a path=/abc
   * next to lang */
  while (fgets(line, sizeof(line), input_markdown_file)) {
    /* Strip trailing newline */
    line[strcspn(line, "\n")] = 0;

    if (strncmp(line, "```", 3) == 0) {
      inside_code_block = !inside_code_block;
      if (!inside_code_block) {
        inside_conf_file_code_block = 0;
        continue;
      }

      for (char* token = strtok(line, " "); token != NULL;
           token = strtok(NULL, " ")) {
        if (strncmp(token, "path=", 5) == 0) {
          /* close previous file */
          if (output_conf_file != NULL) {
            fclose(output_conf_file);
          }

          const char* path = token + 5;
          DEBUG_PRINT("Path identified as %s\n", path);
          expect(
            *path == '/',
            "Error: Expected absolute path to start with / (%s has path=%s)",
            input_markdown_filename,
            path);
          snprintf(prepended_path, MAX_LINE_LENGTH, "%s%s", TANGLE_DIR, path);

          /* Equivalent to mkdir -p $(dirname prepended_path) */
          DEBUG_PRINT("Preparing parent dirs for %s\n", prepended_path);
          expect(prepare_parent_dir_for(prepended_path) != -1,
                 "Failed to create (%s) dir: %s",
                 strerror(errno),
                 prepended_path);

          /* then append to the conf file itself */
          DEBUG_PRINT("Appending to conf file %s\n", prepended_path);
          output_conf_file = fopen(prepended_path, "a");
          expect(output_conf_file != NULL,
                 "Error: failed to open file %s",
                 prepended_path);

          inside_conf_file_code_block = 1;
          break;
        }
      }
    } else if (inside_conf_file_code_block) {
      /* We already stripped the newline */
      fprintf(output_conf_file, "%s\n", line);
      DEBUG_PRINT("\t%s\n", line);
    }
  }

  fclose(input_markdown_file);

  return 0;
}
