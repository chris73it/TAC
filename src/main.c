#include "include/tac.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Please, specify at least one input file\n");
        return 1;
    }

    for (size_t file_index = 1; file_index < argc; ++file_index)
    {
        char *curr_file = argv[file_index];
        printf("Compiling %s...\n", curr_file);
        tac_compile_file(curr_file);
        printf("...done!\n");
    }

    return 0;
}
