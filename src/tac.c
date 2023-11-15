#include "include/tac.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/io.h"
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>

void tac_compile(char *src)
{
    lexer_T *lexer = init_lexer(src);
    parser_T *parser = init_parser(lexer);
    AST_T *root = parser_parse(parser);

    // printf("%ld\n", root->children->size);

    token_T *tok = NULL;
    while ((tok = lexer_next_token(lexer))->type != TOKEN_EOF)
    {
        // printf("TOK %s (%d) (n:%d)\n", tok->value, tok->type, lexer->i);
        printf("%s\n", token_to_str(tok));
        // printf("TOK %s (%d)\n", tok->value, tok->type);
        // printf("TOK %s\n", tok->value);
        // sleep(1);
    }
}

void tac_compile_file(const char *filename)
{
    char *src = tac_read_file(filename);
    tac_compile(src);
    free(src);
}