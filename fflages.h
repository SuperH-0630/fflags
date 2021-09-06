#ifndef FFLAGS_FFLAGES_H
#define FFLAGS_FFLAGES_H
#include <stdbool.h>
#include "stdlib.h"

enum optType {
    ff_not_argument = 0,
    ff_can_argument = 1,
    ff_must_argument = 2,
};

typedef enum optType optType;
typedef struct ff_FFlags ff_FFlags;
typedef struct ff_Child ff_Child;
typedef struct ff_DefineArg ff_DefineArg;

extern void *(*calloc_)(size_t, size_t);

struct ff_Child {
    bool is_default;
    char *child_name;
    struct ff_DefineArg *define;  // 列表
};

struct ff_DefineArg {
    char short_opt;
    char *long_opt;
    optType type;
    int mark;  // 标识
};

ff_FFlags *ff_makeFFlags(int argc, char **argv, ff_Child *child[]);
void ff_freeFFlags(ff_FFlags *ff);

void printInfo_Debug(ff_FFlags *ff);
int getOpt(char **arg, ff_FFlags *ff);
bool getWildOpt(char **arg, ff_FFlags *ff);

#endif //FFLAGS_FFLAGES_H
