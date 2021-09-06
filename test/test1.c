#include "../fflages.h"
#include <stdio.h>

ff_DefineArg main_define[] = {
        {.short_opt='m', .long_opt="message", .type=ff_not_argument, .mark=1},
        {.short_opt='w', .long_opt="msg2", .type=ff_not_argument, .mark=2},
        {.short_opt='e', .long_opt="msg3", .type=ff_can_argument, .mark=3},
        {.short_opt='r', .long_opt="msg4", .type=ff_must_argument, .mark=4},
        {.mark=0},
};

ff_Child main_child = {.is_default=true, .child_name="main", .define=main_define};
ff_Child *child_list[] = {&main_child, NULL};

int main(int argc, char **argv) {
    printf("HelloWorld\n");
    ff_FFlags *ff = ff_makeFFlags(argc, argv, child_list);
    if (ff == NULL)
        return 1;

    char *text = NULL;
    int mark = 0;

    while (1) {
        mark = getOpt(&text, ff);
        switch (mark) {
            case 1:
                printf("mark = 1\n");
                break;
            case 2:
                printf("mark = 2\n");
                break;
            case 3:
                printf("mark = 3 %s\n", text);
                break;
            case 4:
                printf("mark = 4 %s\n", text);
                break;
            case 0:
                printf("error\n");
                goto out;
            default:
                goto out;
        }
    }

    out:
    while (getWildOpt(&text, ff)) {
        printf("wild: %s\n", text);
    }

    ff_freeFFlags(ff);
    return 0;
}