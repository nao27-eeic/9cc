#include <stdarg.h>
#include <stdbool.h>

// tokenizer
// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT, // 識別子
    TK_NUM, // 整数トークン
    TK_RETURN, // return
    TK_IF, // if
    TK_ELSE, // else
    TK_WHILE, // while
    TK_FOR, // for
    TK_EOF, // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next; // 次の入力トークン
    int val; // kindがTK_NUMのとき，その数値
    char *str; // トークン文字列
    int len; // トークンの長さ
};

// 現在着目しているトークン
extern Token *token;
extern char *user_input;

void error(char *, ...);
Token *tokenize(char *);

typedef struct LVar LVar;

struct LVar {
    LVar *next; // 次の変数かNULL
    char *name; // 変数の名前
    int len;    // 名前の長さ
    int offset; // rbpからのオフセット
};

extern LVar *locals;

// parser
// 抽象構文木のノードの種類
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NEQ, // !=
    ND_LE,  // <
    ND_LEQ, // <=
    ND_ASSIGN, // =
    ND_NUM, // 整数
    ND_LVAR, // 変数
    ND_RETURN, // return
    ND_IF,  // if
    ND_WHILE, // while
    ND_FOR, // for
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    Node **nexts; // 子ノード
    int val; // kindがND_NUMの場合のみ使う
    int offset; // kindがND_LVARの場合のみ使う

};

extern Node *code[];
void program(void);

// generator
void gen(Node *);

// container
typedef struct {
    size_t nmemb;
    size_t nalloc;
    void **data;
} Vector;

Vector *vector_init(size_t nmemb);
void *vector_at(Vector *v, size_t indexc);
void *vector_front(Vector* v);
void *vector_back(Vector* v);
bool vector_empty(Vector* v);
size_t vector_size(Vector *v);
void vector_push_back(Vector *v, void *val);
void vector_pop_back(Vector *v);
void vector_free(Vector *v);
