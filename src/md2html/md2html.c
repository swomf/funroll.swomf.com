/*
 * MD4C: Markdown parser for C
 * (http://github.com/mity/md4c)
 *
 * Copyright (c) 2016-2024 Martin Mitáš
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cmdline.h"
#include "md4c-html.h"

/* Global options. */
static unsigned parser_flags = 0;
#ifndef MD4C_USE_ASCII
static unsigned renderer_flags =
  MD_HTML_FLAG_DEBUG | MD_HTML_FLAG_SKIP_UTF8_BOM;
#else
static unsigned renderer_flags = MD_HTML_FLAG_DEBUG;
#endif
static int want_fullhtml = 0;
static int want_stat = 0;
static int want_replay_fuzz = 0;

static const char* html_title = NULL;
static const char* html_desc = NULL;
static const char* html_url = NULL;
static const char* css_path = NULL;

static const char* input_path = NULL;
static const char* output_path = NULL;

/*********************************
 ***  Simple grow-able buffer  ***
 *********************************/

/* We render to a memory buffer instead of directly outputting the rendered
 * documents, as this allows using this utility for evaluating performance
 * of MD4C (--stat option). This allows us to measure just time of the parser,
 * without the I/O.
 */

struct membuffer
{
  char* data;
  size_t asize;
  size_t size;
};

static void
membuf_init(struct membuffer* buf, MD_SIZE new_asize)
{
  buf->size = 0;
  buf->asize = new_asize;
  buf->data = malloc(buf->asize);
  if (buf->data == NULL) {
    fprintf(stderr, "membuf_init: malloc() failed.\n");
    exit(1);
  }
}

static void
membuf_fini(struct membuffer* buf)
{
  if (buf->data)
    free(buf->data);
}

static void
membuf_grow(struct membuffer* buf, size_t new_asize)
{
  buf->data = realloc(buf->data, new_asize);
  if (buf->data == NULL) {
    fprintf(stderr, "membuf_grow: realloc() failed.\n");
    exit(1);
  }
  buf->asize = new_asize;
}

static void
membuf_append(struct membuffer* buf, const char* data, MD_SIZE size)
{
  if (buf->asize < buf->size + size)
    membuf_grow(buf, buf->size + buf->size / 2 + size);
  memcpy(buf->data + buf->size, data, size);
  buf->size += size;
}

/*******************
 *** URL Helpers ***
 *******************/

/**
 * Trim off index.html or .md from the end of a path.
 *
 * Call `free()` afterwards.
 *
 * EXAMPLES:
 * * a/b/c/index.html -> a/b/c
 * * a/b/c/index.md -> a/b/c/index
 * * index.html -> (empty string)
 */
static char*
trim_url_end(const char* filename)
{
  const char* dot = strrchr(filename, '.');
  const char* slash = strrchr(filename, '/');

  /* trim a/b/c.md into a/b/c */
  if (slash != NULL && strcmp(slash, "index.html") == 0)
    return strndup(filename, slash - filename);
  /* trim a/b/index.html into a/b */
  else if (dot != NULL && strcmp(dot, ".md") == 0)
    return strndup(filename, dot - filename);
  /* trim index.html into nothing */
  else if (strcmp(filename, "index.html") == 0)
    return strdup("");
  return strdup(filename);
}

/**********************
 ***  Main program  ***
 **********************/

static void
process_output(const MD_CHAR* text, MD_SIZE size, void* userdata)
{
  membuf_append((struct membuffer*)userdata, text, size);
}

static int
process_file(const char* in_path, FILE* in, FILE* out)
{
  size_t n;
  struct membuffer buf_in = { 0 };
  struct membuffer buf_out = { 0 };
  int ret = -1;
  clock_t t0, t1;
  unsigned p_flags = parser_flags;
  unsigned r_flags = renderer_flags;

  membuf_init(&buf_in, 32 * 1024);

  /* Read the input file into a buffer. */
  while (1) {
    if (buf_in.size >= buf_in.asize)
      membuf_grow(&buf_in, buf_in.asize + buf_in.asize / 2);

    n = fread(buf_in.data + buf_in.size, 1, buf_in.asize - buf_in.size, in);
    if (n == 0)
      break;
    buf_in.size += n;
  }

  /* Input size is good estimation of output size. Add some more reserve to
   * deal with the HTML header/footer and tags. */
  membuf_init(&buf_out, (MD_SIZE)(buf_in.size + buf_in.size / 8 + 64));

  /* Special mode for reproduce test case found with fuzzing a tool.
   * We assume file format as produced by test/fuzzers/fuzz-mdhtml.c. */
  if (want_replay_fuzz) {
    if (buf_in.size < 2 * sizeof(unsigned)) {
      fprintf(stderr, "File %s isn't valid fuzz test case.\n", in_path);
      ret = -1;
      goto out;
    }

    /* Override parser and renderer flags with those form the test case. */
    p_flags = ((unsigned*)buf_in.data)[0];
    r_flags = ((unsigned*)buf_in.data)[1];

    /* And get rid of them from the text input to the parser. */
    memmove(buf_in.data,
            buf_in.data + 2 * sizeof(unsigned),
            buf_in.size - 2 * sizeof(unsigned));
    buf_in.size -= 2 * sizeof(unsigned);

    /* Zero the tail we have moved the contents from.
     * It helps in debugging if make it actually a zero-terminated string.
     */
    memset(buf_in.data + buf_in.size, 0, 2 * sizeof(unsigned));
  }

  /* Parse the document. This shall call our callbacks provided via the
   * md_renderer_t structure. */
  t0 = clock();

  ret = md_html(buf_in.data,
                (MD_SIZE)buf_in.size,
                process_output,
                (void*)&buf_out,
                p_flags,
                r_flags);

  t1 = clock();
  if (ret != 0) {
    fprintf(stderr, "Parsing failed.\n");
    goto out;
  }

  /* Write down the document in the HTML format. */
  if (want_fullhtml) {
    char* trimmed_path = trim_url_end(input_path);

    fprintf(out, "<!DOCTYPE html>\n");
    fprintf(out, "<html>\n");
    fprintf(out, "<head>\n");
    fprintf(out, "<title>%s</title>\n", html_title ? html_title : "");
    /* Some metadata is a hardcoded default, since this
     * parser is project-specific anyway
     */

    /* Generic metadata */
#if !defined MD4C_USE_ASCII && !defined MD4C_USE_UTF16
    fprintf(out, "<meta charset=\"UTF-8\">\n");
#endif
    fprintf(out,
            "<meta name=\"viewport\" content=\"width=device-width, "
            "initial-scale=1\">\n");
    fprintf(out,
            "<meta name=\"title\" content=\"%s\"\n>",
            html_title ? html_title : "funroll");
    fprintf(out,
            "<meta name=\"description\" content=\"%s\">\n",
            html_desc ? html_desc : "");
    fprintf(out, "<meta name=\"generator\" content=\"md2html\">\n");

    /* OpenGraph / Facebook */
    fprintf(out, "<meta property=\"og:type\" content=\"website\">\n");
    fprintf(out,
            "<meta property=\"og:url\" content=\"%s/%s\">\n",
            html_url ? html_url : "",
            trimmed_path);
    fprintf(out,
            "<meta property=\"og:title\" content=\"%s\">\n",
            html_title ? html_title : "funroll");
    fprintf(out,
            "<meta property=\"og:description\" content=\"%s\">\n",
            html_desc ? html_desc : "a little bit of gentoo");
    fprintf(out,
            "<meta property=\"og:image\" content=\"%s/%s\">\n",
            html_url ? html_url : "",
            "butterfly-with-text.webp");

    if (css_path != NULL) {
      /* my css is dark-first anyway */
      fprintf(out, "<meta name=\"darkreader-lock\">\n");
      fprintf(out, "<link rel=\"stylesheet\" href=\"%s\">\n", css_path);
    }
    fprintf(out, "</head>\n");
    fprintf(out, "<body>\n");

    free(trimmed_path);
  }

  fwrite(buf_out.data, 1, buf_out.size, out);

  if (want_fullhtml) {
    fprintf(out, "</body>\n");
    fprintf(out, "</html>\n");
  }

  if (want_stat) {
    if (t0 != (clock_t)-1 && t1 != (clock_t)-1) {
      double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
      if (elapsed < 1)
        fprintf(stderr, "Time spent on parsing: %7.2f ms.\n", elapsed * 1e3);
      else
        fprintf(stderr, "Time spent on parsing: %6.3f s.\n", elapsed);
    }
  }

  /* Success if we have reached here. */
  ret = 0;

out:
  membuf_fini(&buf_in);
  membuf_fini(&buf_out);

  return ret;
}

static const CMDLINE_OPTION cmdline_options[] = {
  { 'o', "output", 'o', CMDLINE_OPTFLAG_REQUIREDARG },
  { 'f', "full-html", 'f', 0 },
  { 's', "stat", 's', 0 },
  { 'h', "help", 'h', 0 },
  { 'v', "version", 'v', 0 },

  { 0, "html-title", '1', CMDLINE_OPTFLAG_REQUIREDARG },
  { 0, "html-css", '2', CMDLINE_OPTFLAG_REQUIREDARG },
  { 0, "html-desc", '3', CMDLINE_OPTFLAG_REQUIREDARG },
  { 0, "html-url", '4', CMDLINE_OPTFLAG_REQUIREDARG },

  { 0, "commonmark", 'c', 0 },
  { 0, "github", 'g', 0 },

  { 0, "fcollapse-whitespace", 'W', 0 },
  { 0, "flatex-math", 'L', 0 },
  { 0, "fpermissive-atx-headers", 'A', 0 },
  { 0, "fpermissive-autolinks", 'V', 0 },
  { 0, "fhard-soft-breaks", 'B', 0 },
  { 0, "fpermissive-email-autolinks", '@', 0 },
  { 0, "fpermissive-url-autolinks", 'U', 0 },
  { 0, "fpermissive-www-autolinks", '.', 0 },
  { 0, "fstrikethrough", 'S', 0 },
  { 0, "ftables", 'T', 0 },
  { 0, "ftasklists", 'X', 0 },
  { 0, "funderline", '_', 0 },
  { 0, "fverbatim-entities", 'E', 0 },
  { 0, "fwiki-links", 'K', 0 },

  { 0, "fno-html-blocks", 'F', 0 },
  { 0, "fno-html-spans", 'G', 0 },
  { 0, "fno-html", 'H', 0 },
  { 0, "fno-indented-code", 'I', 0 },

  /* Undocumented option for replaying test cases from fuzzers. */
  { 0, "replay-fuzz", 'r', 0 },

  { 0, NULL, 0, 0 }
};

static void
usage(void)
{
  printf(
    "Usage: md2html [OPTION]... [FILE]\n"
    "Convert input FILE (or standard input) in Markdown format to HTML.\n"
    "\n"
    "General options:\n"
    "  -o  --output=FILE    Output file (default is standard output)\n"
    "  -f, --full-html      Generate full HTML document, including header\n"
    "  -s, --stat           Measure time of input parsing\n"
    "  -h, --help           Display this help and exit\n"
    "  -v, --version        Display version and exit\n"
    "\n"
    "Markdown dialect options:\n"
    "(note these are equivalent to some combinations of the flags below)\n"
    "      --commonmark     CommonMark (this is default)\n"
    "      --github         Github Flavored Markdown\n"
    "\n"
    "Markdown extension options:\n"
    "      --fcollapse-whitespace\n"
    "                       Collapse non-trivial whitespace\n"
    "      --flatex-math    Enable LaTeX style mathematics spans\n"
    "      --fpermissive-atx-headers\n"
    "                       Allow ATX headers without delimiting space\n"
    "      --fpermissive-url-autolinks\n"
    "                       Allow URL autolinks without '<', '>'\n"
    "      --fpermissive-www-autolinks\n"
    "                       Allow WWW autolinks without any scheme (e.g. "
    "'www.example.com')\n"
    "      --fpermissive-email-autolinks  \n"
    "                       Allow e-mail autolinks without '<', '>' and "
    "'mailto:'\n"
    "      --fpermissive-autolinks\n"
    "                       Same as --fpermissive-url-autolinks "
    "--fpermissive-www-autolinks\n"
    "                       --fpermissive-email-autolinks\n"
    "      --fhard-soft-breaks\n"
    "                       Force all soft breaks to act as hard breaks\n"
    "      --fstrikethrough Enable strike-through spans\n"
    "      --ftables        Enable tables\n"
    "      --ftasklists     Enable task lists\n"
    "      --funderline     Enable underline spans\n"
    "      --fwiki-links    Enable wiki links\n"
    "\n"
    "Markdown suppression options:\n"
    "      --fno-html-blocks\n"
    "                       Disable raw HTML blocks\n"
    "      --fno-html-spans\n"
    "                       Disable raw HTML spans\n"
    "      --fno-html       Same as --fno-html-blocks --fno-html-spans\n"
    "      --fno-indented-code\n"
    "                       Disable indented code blocks\n"
    "\n"
    "HTML generator options:\n"
    "      --fverbatim-entities\n"
    "                       Do not translate entities\n"
    "      --html-title=TITLE Sets the title of the document\n"
    "      --html-desc=DESC Sets the description of the document\n"
    "      --html-css=URL   In full HTML mode add a css link\n"
    "\n");
}

static void
version(void)
{
  printf("%d.%d.%d\n", MD_VERSION_MAJOR, MD_VERSION_MINOR, MD_VERSION_RELEASE);
}

static int
cmdline_callback(int opt, char const* value, void* data)
{
  (void)data; /* unused parameter */

  switch (opt) {
    case 0:
      if (input_path) {
        fprintf(stderr,
                "Too many arguments. Only one input file can be "
                "specified.\n");
        fprintf(stderr, "Use --help for more info.\n");
        exit(1);
      }
      input_path = value;
      break;

    case 'o':
      output_path = value;
      break;
    case 'f':
      want_fullhtml = 1;
      break;
    case 's':
      want_stat = 1;
      break;
    case 'r':
      want_replay_fuzz = 1;
      break;
    case 'h':
      usage();
      exit(0);
      break;
    case 'v':
      version();
      exit(0);
      break;

    case '1':
      html_title = value;
      break;
    case '2':
      css_path = value;
      break;
    case '3':
      html_desc = value;
      break;
    case '4':
      html_url = value;
      break;

    case 'c':
      parser_flags |= MD_DIALECT_COMMONMARK;
      break;
    case 'g':
      parser_flags |= MD_DIALECT_GITHUB;
      break;

    case 'E':
      renderer_flags |= MD_HTML_FLAG_VERBATIM_ENTITIES;
      break;
    case 'A':
      parser_flags |= MD_FLAG_PERMISSIVEATXHEADERS;
      break;
    case 'I':
      parser_flags |= MD_FLAG_NOINDENTEDCODEBLOCKS;
      break;
    case 'F':
      parser_flags |= MD_FLAG_NOHTMLBLOCKS;
      break;
    case 'G':
      parser_flags |= MD_FLAG_NOHTMLSPANS;
      break;
    case 'H':
      parser_flags |= MD_FLAG_NOHTML;
      break;
    case 'W':
      parser_flags |= MD_FLAG_COLLAPSEWHITESPACE;
      break;
    case 'U':
      parser_flags |= MD_FLAG_PERMISSIVEURLAUTOLINKS;
      break;
    case '.':
      parser_flags |= MD_FLAG_PERMISSIVEWWWAUTOLINKS;
      break;
    case '@':
      parser_flags |= MD_FLAG_PERMISSIVEEMAILAUTOLINKS;
      break;
    case 'V':
      parser_flags |= MD_FLAG_PERMISSIVEAUTOLINKS;
      break;
    case 'T':
      parser_flags |= MD_FLAG_TABLES;
      break;
    case 'S':
      parser_flags |= MD_FLAG_STRIKETHROUGH;
      break;
    case 'L':
      parser_flags |= MD_FLAG_LATEXMATHSPANS;
      break;
    case 'K':
      parser_flags |= MD_FLAG_WIKILINKS;
      break;
    case 'X':
      parser_flags |= MD_FLAG_TASKLISTS;
      break;
    case '_':
      parser_flags |= MD_FLAG_UNDERLINE;
      break;
    case 'B':
      parser_flags |= MD_FLAG_HARD_SOFT_BREAKS;
      break;

    default:
      fprintf(stderr, "Illegal option: %s\n", value);
      fprintf(stderr, "Use --help for more info.\n");
      exit(1);
      break;
  }

  return 0;
}

int
main(int argc, char** argv)
{
  FILE* in = stdin;
  FILE* out = stdout;
  int ret = 0;

  if (cmdline_read(cmdline_options, argc, argv, cmdline_callback, NULL) != 0) {
    usage();
    exit(1);
  }

  if (input_path != NULL && strcmp(input_path, "-") != 0) {
    in = fopen(input_path, "rb");
    if (in == NULL) {
      fprintf(stderr, "Cannot open %s.\n", input_path);
      exit(1);
    }
  }
  if (output_path != NULL && strcmp(output_path, "-") != 0) {
    out = fopen(output_path, "wt");
    if (out == NULL) {
      fprintf(stderr, "Cannot open %s.\n", output_path);
      exit(1);
    }
  }

  ret = process_file((input_path != NULL) ? input_path : "<stdin>", in, out);
  if (in != stdin)
    fclose(in);
  if (out != stdout)
    fclose(out);

  return ret;
}
