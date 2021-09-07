#include "../fflags.h"
#include <stdio.h>

ff_defArg(main, true)
    ff_argRule(m, message, not, 1)
    ff_argRule(w, msg2, not, 2)
    ff_argRule(e, msg3, can, 3)
    ff_argRule_('r', "msg4", must, 4)
ff_endArg(main, true);

ff_childList(sys, ff_child(main));

int main(int argc, char **argv) {
    ff_FFlags *ff = ff_initFFlags(argc, argv, sys);
    if (ff == NULL)
        return 1;

    char *text = NULL;
    int mark = 0;

    while (1) {
        mark = ff_getopt(&text, ff);
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
    while (ff_getopt_wild(&text, ff)) {
        printf("wild: %s\n", text);
    }

    ff_freeFFlags(ff);
    return 0;
}