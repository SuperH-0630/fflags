#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "fflags.h"

typedef struct ff_Argv ff_Argv;

#define free(p) ((((p)!=NULL) ? (free(p), NULL) : NULL), (p)=NULL)
void *(*calloc_)(size_t n, size_t size) = calloc;

static ff_Argv **makeArgv(int argc, char **argv, ff_Argv **base);
static ff_Argv *freeArgv(ff_Argv *argv);
static void freeAllArgv(ff_Argv *argv);

static bool findChild(char *name, ff_Child **result, ff_Child *child[]);

static int getLongOpt(char *opt, char **arg, ff_FFlags *ff);
static void argvToNext(ff_FFlags *ff);
static int getShortOpt(char ch, char **arg, ff_FFlags *ff);
static ff_DefineArg *findDefineByOpt(char *opt, ff_Child *child);
static ff_DefineArg *findDefineByCh(char ch, ff_Child *child);

struct ff_FFlags {
    struct ff_Child *child;  // 数组

    struct ff_Argv *argv;
    struct ff_Argv *done;
    struct ff_Argv *next;

    struct ff_Argv *wild_arg;

    size_t argv_index;  // argv_index
};

struct ff_Argv {
    char *data;
    bool should_free;  // data是否需要被释放
    bool is_arg;  // 是参数而非开关
    bool wild;  // 非开关的参数
    struct ff_Argv *next;
};

ff_FFlags *ff_makeFFlags(int argc, char **argv, ff_Child *child[]) {
    ff_Child *get_child = NULL;
    ff_Argv *ff_argv = NULL;
    bool is_default;
    if (argc > 1 && child != NULL) // 无参数处理
        is_default = findChild(argv[2], &get_child, child);
    else
        return NULL;

    if (is_default)
        makeArgv(argc - 1, argv + 1, &ff_argv);
    else
        makeArgv(argc - 2, argv + 2, &ff_argv);

    if (ff_argv == NULL)
        return NULL;

    ff_FFlags *ff = calloc_(1, sizeof(ff_FFlags));
    ff->child = get_child;
    ff->argv = ff_argv;
    ff->done = ff->argv;
    ff->wild_arg = ff->argv;
    if (ff->done != NULL)
        ff->next = ff->done->next;
    ff->argv_index = 1;
    return ff;
}

void ff_freeFFlags(ff_FFlags *ff) {
    freeAllArgv(ff->argv);
    free(ff);
}

static bool findChild(char *name, ff_Child **result, ff_Child *child[]) {
    ff_Child *default_child = NULL;
    for (NULL; *child != NULL; child++) {
        if (strcmp((*child)->child_name, name) == 0) {
            *result = *child;
            return false;
        } if ((*child)->is_default)
            default_child = *child;
    }
    *result = default_child;
    return true;
}

static ff_Argv **makeArgv(int argc, char **argv, ff_Argv **base) {
    ff_Argv **base_argv = base;
    bool is_wild = false;
    for (int i = 0; i < argc; i++) {
        if (!is_wild && strcmp(argv[i], "--") == 0) {
            is_wild = true;
            continue;  // 跳过
        }

        if (strcmp("-", *argv) == 0 || strcmp("/", *argv) == 0 || strcmp("--", *argv) == 0) {  // 没有参数
            freeArgv(*base_argv);
            *base = NULL;
            return NULL;
        }

        *base = calloc_(1, sizeof(ff_Argv));
        (*base)->data = argv[i];
        (*base)->should_free = false;

        if (is_wild) {
            (*base)->is_arg = true;
            (*base)->wild = true;
        } else if (((*argv[i] != '-') && (*argv[i] != '/')))
            (*base)->is_arg = true;

        base = &((*base)->next);
    }
    return base;
}

static ff_Argv *freeArgv(ff_Argv *argv) {
    ff_Argv *next = argv->next;
    if (argv->should_free)
        free(argv->data);
    free(argv);
    return next;
}

static void freeAllArgv(ff_Argv *argv) {
    while (argv != NULL)
        argv = freeArgv(argv);
}

static ff_DefineArg *findDefineByCh(char ch, ff_Child *child) {
    for (ff_DefineArg *define = child->define; define->mark != 0; define++) {
        if (define->short_opt == ch)
            return define;
    }
    return NULL;
}

static ff_DefineArg *findDefineByOpt(char *opt, ff_Child *child) {
    for (ff_DefineArg *define = child->define; define->mark != 0; define++) {
        if (strcmp(define->long_opt, opt) == 0)
            return define;
    }
    return NULL;
}

static int getShortOpt(char ch, char **arg, ff_FFlags *ff) {
    ff_DefineArg *define = findDefineByCh(ch, ff->child);
    if (define == NULL)
        return 0;

    int mark = define->mark;
    if (define->type == ff_must_argument) {
        if (ff->next == NULL || ff->next->wild || !ff->next->is_arg) {  // 不是合法参数
            *arg = NULL;
            mark = 0;
        } else {
            *arg = ff->next->data;
            ff->next = ff->next->next;
        }
    } else if (define->type == ff_can_argument) {
        if (ff->next != NULL && !ff->next->wild && ff->next->is_arg) {  // 是合法参数
            *arg = ff->next->data;
            ff->next = ff->next->next;
        } else
            *arg = NULL;
    } else
        *arg = NULL;
    return mark;
}

static int getLongOpt(char *opt, char **arg, ff_FFlags *ff) {
    ff_DefineArg *define = findDefineByOpt(opt, ff->child);
    if (define == NULL)
        return 0;

    int mark = define->mark;
    if (define->type == ff_must_argument) {
        if (ff->next == NULL || ff->next->wild || !ff->next->is_arg) {  // 不是合法参数
            *arg = NULL;
            mark = 0;
        } else {
            *arg = ff->next->data;
            ff->next = ff->next->next;
        }
    } else if (define->type == ff_can_argument) {
        if (ff->next != NULL && !ff->next->wild && ff->next->is_arg) {  // 是合法参数
            *arg = ff->next->data;
            ff->next = ff->next->next;
        } else
            *arg = NULL;
    } else
        *arg = NULL;
    return mark;
}

static void argvToNext(ff_FFlags *ff) {
    ff->done = ff->next;
    if (ff->done != NULL)
        ff->next = ff->done->next;
    else
        ff->next = NULL;
    ff->argv_index = 1;  // 例如 -xy index=1表示该参数的 'x' 字符
}

int ff_getopt(char **arg, ff_FFlags *ff) {
    if (ff->child == NULL)
        return -2;

    while (ff->done != NULL) {
        if (*(ff->done->data) == '-') {
            if (*(ff->done->data + 1) == '-') {  // 长参数
                int mark = getLongOpt(ff->done->data + 2, arg, ff);
                argvToNext(ff);
                return mark;
            } else {
                int mark = getShortOpt(ff->done->data[ff->argv_index], arg, ff);
                ff->argv_index++;
                if (ff->done->data[ff->argv_index] == '\0')
                    argvToNext(ff);
                return mark;
            }
        } else if (*(ff->done->data) == '/') {
            if (strlen(ff->done->data) > 2) {  // 长参数
                int mark = getLongOpt(ff->done->data + 1, arg, ff);
                argvToNext(ff);
                return mark;
            } else {
                int ch = getShortOpt(*ff->done->data, arg, ff);
                argvToNext(ff);
                return ch;
            }
        } else {
            ff->done->wild = true;
            ff->done->is_arg = true;
            argvToNext(ff);
            continue;  // 跳过该参数
        }
    }

    return -1;
}

bool ff_getopt_wild(char **arg, ff_FFlags *ff) {
    ff_Argv *wild = ff->wild_arg;
    if (wild == NULL) {
        *arg = NULL;
        return false;
    }

    for (NULL; wild != NULL; wild = wild->next) {
        if (wild->wild) {
            *arg = wild->data;
            ff->wild_arg = wild->next;
            return true;
        }
    }

    *arg = NULL;
    ff->wild_arg = NULL;
    return false;
}
