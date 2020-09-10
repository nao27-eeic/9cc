#include <stdarg.h>

// tokenizer
// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT, // 識別子
    TK_NUM, // 整数トークン
    TK_RETURN, // return
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

extern void error(char *, ...);
extern Token *tokenize(char *);

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
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    int val; // kindがND_NUMの場合のみ使う
    int offset; // kindがND_LVARの場合のみ使う

};

extern Node *code[];
extern void program();

// generator
void gen(Node *);
