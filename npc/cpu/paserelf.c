#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

void set_batch();

static char *img_file = NULL;

int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"port"     , required_argument, NULL, 'p'},
    {"help"     , no_argument      , NULL, 'h'},
	{"ftrace"   , required_argument, NULL, 'f'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  // : means it needs args
  while ( (o = getopt_long(argc, argv, "-bhl:f:d:p:", table, NULL)) != -1) {
    switch (o) {
      case 'b': set_batch();      break;
      case 1: img_file = optarg; return 0;	// set to 1 if unrecognize
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

long load_img(uint32_t *inst) {
  if (img_file == NULL) {
	printf("No img_file");
	assert(0);
    return 1; // built-in image size
  }

  FILE *fp = fopen(img_file, "rb");
  if(fp == NULL) assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  printf("The image is %s, size = %ld", img_file, size);

  fseek(fp, 0, SEEK_SET);
  int ret = fread(inst, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  return size;
}

long ReadBinFile(int argc, char **argv,uint32_t *inst){
  long size =0;
	parse_args(argc,argv);
	size = load_img(inst);
	return size;
}