#include "include/parser.h"
#include "include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

parser_T *init_parser(lexer_T *lexer)
{
    printf("init_parser\n");
    parser_T *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->token = lexer_next_token(lexer);

    return parser;
}

token_T *parser_eat(parser_T *parser, int type)
{
    printf("parser_eat `%s`\n", token_to_str(parser->token));
    if (parser->token->type != type)
    {
        printf("[Parser]: Unexpected token: `%s`, was expecting: `%s`\n",
               token_to_str(parser->token), token_type_to_str(type));
        exit(1);
    }

    parser->token = lexer_next_token(parser->lexer);
    return parser->token;
}

AST_T *parser_parse(parser_T *parser)
{
    printf("parser_parse\n");
    return parser_parse_compound(parser); // init_ast(AST_NOOP);
}

AST_T *parser_parse_id(parser_T *parser)
{
    printf("A parser_parse_id `%s`\n", token_to_str(parser->token));
    char *value = calloc(strlen(parser->token->value) + 1, sizeof(char));
    strcpy(value, parser->token->value);
    parser_eat(parser, TOKEN_ID);

    if (parser->token->type == TOKEN_EQUALS)
    { // Parse assignment
        printf("B parser_parse_id `%s`\n", token_to_str(parser->token));
        parser_eat(parser, TOKEN_EQUALS);
        AST_T *ast = init_ast(AST_ASSIGNMENT);
        ast->name = value;
        ast->value = parser_parse_expr(parser);
        return ast;
    }

    AST_T *ast = init_ast(AST_VARIABLE);
    ast->name = value;

    if (parser->token->type == TOKEN_COLON)
    {
        printf("C parser_parse_id `%s`\n", token_to_str(parser->token));
        parser_eat(parser, TOKEN_COLON);

        while (parser->token->type == TOKEN_ID)
        {
            ast->data_type = typename_to_int(parser->token->value);
            printf("D parser_parse_id `%s`\n", token_to_str(parser->token));
            parser_eat(parser, TOKEN_ID);

            if (parser->token->type == TOKEN_LT)
            {
                printf("E parser_parse_id `%s`\n", token_to_str(parser->token));
                parser_eat(parser, TOKEN_LT);
                ast->data_type += typename_to_int(parser->token->value);
                printf("F parser_parse_id `%s`\n", token_to_str(parser->token));
                parser_eat(parser, TOKEN_ID);
                printf("G parser_parse_id `%s`\n", token_to_str(parser->token));
                parser_eat(parser, TOKEN_GT);
            }
        }
    }
    else
    {
        if (parser->token->type == TOKEN_LPAREN)
        {
            ast->type = AST_CALL;
            ast->value = parser_parse_list(parser);
        }
    }

    return ast;
}

// AST_T *parser_parse_block(parser_T *parser)
// {
//     parser_eat(parser, TOKEN_LBRACE);
//     AST_T *ast = init_ast(AST_COMPOUND);

//     while (parser->token->type != TOKEN_RBRACE)
//     {
//         list_push(ast->children, parser_parse_expr(parser));
//         // parser_eat(parser, TOKEN_SEMI);
//     }

//     parser_eat(parser, TOKEN_RBRACE);

//     return ast;
// }

AST_T *parser_parse_list(parser_T *parser)
{
    printf("A parser_parse_list\n");
    parser_eat(parser, TOKEN_LPAREN);

    AST_T *ast = init_ast(AST_COMPOUND);

    list_push(ast->children, parser_parse_expr(parser));

    while (parser->token->type == TOKEN_COMMA)
    {
        printf("B parser_parse_list `%s`\n", token_to_str(parser->token));
        parser_eat(parser, TOKEN_COMMA);
        list_push(ast->children, parser_parse_expr(parser));
    }

    parser_eat(parser, TOKEN_RPAREN);
    printf("C parser_parse_list `%s`\n", token_to_str(parser->token));

    if (parser->token->type == TOKEN_COLON)
    {
        parser_eat(parser, TOKEN_COLON);
        printf("D parser_parse_list `%s`\n", token_to_str(parser->token));

        while (parser->token->type == TOKEN_ID)
        {
            printf("E parser_parse_list `%s`\n", token_to_str(parser->token));
            ast->data_type = typename_to_int(parser->token->value);
            parser_eat(parser, TOKEN_ID);

            if (parser->token->type == TOKEN_LT)
            {
                printf("F parser_parse_list `%s`\n", token_to_str(parser->token));
                parser_eat(parser, TOKEN_LT);
                ast->data_type += typename_to_int(parser->token->value);
                printf("G parser_parse_list `%s`\n", token_to_str(parser->token));
                parser_eat(parser, TOKEN_ID);
                printf("H parser_parse_list `%s`\n", token_to_str(parser->token));
                parser_eat(parser, TOKEN_GT);
                // printf("%s\n", token_to_str(parser->token));
            }
        }
    }

    if (parser->token->type == TOKEN_RIGHT_ARROW)
    {
        parser_eat(parser, TOKEN_RIGHT_ARROW);
        ast->type = AST_FUNCTION;
        ast->value = parser_parse_compound(parser);
    }

    return ast;
}

AST_T *parser_parse_int(parser_T *parser)
{
    int int_value = atoi(parser->token->value);
    parser_eat(parser, TOKEN_INT);

    AST_T *ast = init_ast(AST_INT);
    ast->int_value = int_value;

    return ast;
}

AST_T *parser_parse_expr(parser_T *parser)
{
    printf("A parser_parse_expr\n");
    switch (parser->token->type)
    {
    case TOKEN_ID:
    {
        printf("B parser_parse_expr `%s`\n", token_to_str(parser->token));
        return parser_parse_id(parser);
    }
    case TOKEN_LPAREN:
    {
        printf("C parser_parse_expr `%s`\n", token_to_str(parser->token));
        return parser_parse_list(parser);
    }
    case TOKEN_INT:
    {
        printf("D parser_parse_expr `%s`\n", token_to_str(parser->token));
        return parser_parse_int(parser);
    }
    default:
    {
        printf("[Parser]: Unexpected token `%s`\n", token_to_str(parser->token));
        exit(1);
    }
    }
}

AST_T *parser_parse_compound(parser_T *parser)
{
    int should_match_brace = 0;
    if (parser->token->type == TOKEN_LBRACE)
    {
        should_match_brace = 1;
        parser_eat(parser, TOKEN_LBRACE);
    }

    printf("A parser_parse_compound\n");
    AST_T *compound = init_ast(AST_COMPOUND);

    while (parser->token->type != TOKEN_EOF && parser->token->type != TOKEN_RBRACE)
    {
        printf("--> B parser_parse_compound `%s`\n", token_to_str(parser->token));
        // AST_T *child = parser_parse_expr(parser);
        list_push(compound->children, parser_parse_expr(parser));

        if (parser->token->type == TOKEN_SEMI)
        {
            parser_eat(parser, TOKEN_SEMI);
        }
    }

    if (should_match_brace == 1)
        parser_eat(parser, TOKEN_RBRACE);

    return compound;
}
