#ifndef FFLAGS_FFLAGS_H
#define FFLAGS_FFLAGS_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* 操作宏 */
#define ff_defArg(name, is_default) ff_DefineArg ffu_ ## name ## _define[] = {
#define ff_argRule(short, long, arg_type, mark_) \
    {.short_opt=short, .long_opt=#long, .type=ff_ ## arg_type ## _argument, .mark=mark_},
#define ff_argRule_(short, long, arg_type, mark_) \
    {.short_opt=short, .long_opt=long, .type=ff_ ## arg_type ## _argument, .mark=mark_},
#define ff_endArg(name, is_default_) \
    {.mark=0},};                    \
    ff_Child ffu_ ## name ## _child = {.is_default=(is_default_), .child_name=#name, .self_process=false, .define=ffu_ ## name ## _define}

#define ff_selfProcessChild(name, is_default_) ff_Child ffu_ ## name ## _child = {.is_default=(is_default_), .child_name=#name, .self_process=true, .define=NULL}

#define ff_child(name) (&(ffu_ ## name ## _child))
#define ff_childList(name, ...) ff_Child *ffu_ ## name ## _child_list[] = {__VA_ARGS__, NULL}

#define ff_initFFlags(argc, argv, d, slash, f, cl) (ff_makeFFlags((argc), (argv), (d), (slash), (f), (ffu_ ## cl ## _child_list)))
#define ff_getChild(ff) ff_childType(ff)

/* 数据定义 */
enum optType {
    ff_not_argument = 0,
    ff_can_argument = 1,
    ff_must_argument = 2,
};

typedef enum optType optType;
typedef struct ff_FFlags ff_FFlags;
typedef struct ff_Child ff_Child;
typedef struct ff_DefineArg ff_DefineArg;

struct ff_Child {
    bool is_default;
    bool self_process;  // 自行除了参数
    const char *child_name;
    struct ff_DefineArg *define;  // 列表
};

struct ff_DefineArg {
    const char short_opt;
    const char *long_opt;
    enum optType type;
    int mark;  // 标识
};

ff_FFlags *ff_makeFFlags(int argc, char **argv, bool del_first, bool allown_slash, FILE *error_file, ff_Child *child[]);
const char *ff_childType(ff_FFlags *ff);
void ff_freeFFlags(ff_FFlags *ff);

int ff_getopt(char **arg, ff_FFlags *ff);
bool ff_getopt_wild(char **arg, ff_FFlags *ff);
bool ff_getopt_wild_after(char **arg, ff_FFlags *ff);
bool ff_getopt_wild_before(char **arg, ff_FFlags *ff);
int ff_get_process_argv(char * **argv, ff_FFlags *ff);

#endif //FFLAGS_FFLAGS_H
