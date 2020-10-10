#include <stdarg.h>
#include <stdbool.h>

// container
// Vector
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

typedef struct EntryList EntryList;

// Map
struct EntryList {
    const char *key;
    void *val;
    EntryList *next;
};

typedef struct {
    EntryList **root;
    size_t size;
    size_t nentry;
} Map;

Map *map_init();
bool map_empty(const Map *map);
size_t map_size(const Map *map);
void *map_find(const Map *map, const char *key);
void map_insert(Map *map, const char *key, void *val);
void map_remove(Map *map, const char *key);

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
    ND_BLOCK, // block
    ND_FUNC, // 関数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
    NodeKind kind; // ノードの型
    union {
        struct {
            Node *lhs; // 左辺
            Node *rhs; // 右辺
        };
        Node **nexts; // 子ノード
        Vector *stmts; // kindがND_BLOCKの場合のみ使う
        struct{ // kindがND_FUNCの場合のみ使う
            Vector *args;
            const char *fname;
            int len;
        };
    };
    union {
        int val; // kindがND_NUMの場合のみ使う
        int offset; // kindがND_LVARの場合のみ使う
    };
};

extern Node *code[];
void program(void);

// generator
void gen(Node *);

