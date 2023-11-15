#include "include/lexer.h"
// #include "include/macros.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

lexer_T *init_lexer(char *src)
{
    lexer_T *lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->src = src;
    lexer->src_size = strlen(src); // FIXME: Can this be passed by the caller?
    lexer->i = 0;
    lexer->c = src[lexer->i];

    return lexer;
}

void lexer_advance(lexer_T *lexer)
{
    if (lexer->i < lexer->src_size && lexer->c != '\0')
    {
        lexer->i += 1;
        lexer->c = lexer->src[lexer->i];
    }
}

char lexer_peek(lexer_T *lexer, int offset)
{
    size_t tmp_i = lexer->i + offset;
    assert(tmp_i < lexer->src_size && lexer->src[tmp_i] != '\0');
    return lexer->src[tmp_i];
}

token_T *lexer_advance_with(lexer_T *lexer, token_T *token)
{
    lexer_advance(lexer);
    return token;
}

token_T *lexer_advance_current(lexer_T *lexer, int type)
{
    char *value = calloc(2, sizeof(char));
    value[0] = lexer->c;
    value[1] = '\0';

    token_T *token = init_token(value, type);
    lexer_advance(lexer);

    return token;
}

void lexer_skip_whitespace(lexer_T *lexer)
{
    while (lexer->c == '\r' || lexer->c == '\n' || lexer->c == ' ' || lexer->c == '\t')
        lexer_advance(lexer);
}

token_T *lexer_parse_id(lexer_T *lexer)
{
    char *value = calloc(1, sizeof(char));

    while (isalnum(lexer->c))
    {
        value = realloc(value, (strlen(value) + 2) * sizeof(char));
        strcat(value, (char[]){lexer->c, '\0'});
        lexer_advance(lexer);
    }

    return init_token(value, TOKEN_ID);
}

token_T *lexer_parse_int(lexer_T *lexer)
{
    char *value = calloc(1, sizeof(char));

    while (isdigit(lexer->c))
    {
        value = realloc(value, (strlen(value) + 2) * sizeof(char));
        strcat(value, (char[]){lexer->c, '\0'});
        lexer_advance(lexer);
    }

    return init_token(value, TOKEN_INT);
}

token_T *lexer_next_token(lexer_T *lexer)
{
    lexer_skip_whitespace(lexer);

    while (lexer->c != '\0')
    {
        if (isalpha(lexer->c))
            return lexer_parse_id(lexer);

        if (isdigit(lexer->c))
            // return lexer_advance_with(lexer, lexer_parse_int(lexer));
            return lexer_parse_int(lexer);

        switch (lexer->c)
        {
        case '=':
            if (lexer_peek(lexer, 1) == '>')
                return lexer_advance_with(lexer, lexer_advance_with(lexer, init_token("=>", TOKEN_RIGHT_ARROW)));
            return lexer_advance_current(lexer, TOKEN_EQUALS);
        case '(':
            return lexer_advance_current(lexer, TOKEN_LPAREN);
        case ')':
            return lexer_advance_current(lexer, TOKEN_RPAREN);
        case '{':
            return lexer_advance_current(lexer, TOKEN_LBRACE);
        case '}':
            return lexer_advance_current(lexer, TOKEN_RBRACE);
        case '<':
            return lexer_advance_current(lexer, TOKEN_LT);
        case '>':
            return lexer_advance_current(lexer, TOKEN_GT);
        case ':':
            return lexer_advance_current(lexer, TOKEN_COLON);
        case ',':
            return lexer_advance_current(lexer, TOKEN_COMMA);
        case ';':
            return lexer_advance_current(lexer, TOKEN_SEMI);

        case '\0':
            break;

        default:
            printf("{Lexel}: Unexpected character '%c'\n", lexer->c);
            exit(1);
        }
    }

    return init_token(NULL, TOKEN_EOF);
}
