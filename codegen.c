#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

bool is_stack_aligned = true;

int gen_label_num() {
    static int num = 0;
    return num++;
}

void gen_push(const char *reg) {
    printf("    push %s\n", reg);
    is_stack_aligned ^= true;
}

void gen_pop(const char *reg) {
    printf("    pop %s\n", reg);
    is_stack_aligned ^= true;
}

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    gen_push("rax");
}

void gen(Node *node) {
    if (node->kind == ND_RETURN) {
        gen(node->lhs);
        gen_pop("rax");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        is_stack_aligned = true;
        printf("    ret\n");
        return;
    }

    if (node->kind == ND_IF) {
        gen(node->nexts[0]);
        gen_pop("rax");
        printf("    cmp rax, 0\n");

        int n0 = gen_label_num();
        printf("    je .L%d\n", n0);
        gen(node->nexts[1]);
        if (node->nexts[2]) {
            int n1 = gen_label_num();
            printf("    jmp .L%d\n", n1);
            printf(".L%d:\n", n0);
            gen(node->nexts[2]);
            printf(".L%d:\n", n1);
        } else {
            printf(".L%d:\n", n0);
        }
        return;
    }

    if (node->kind == ND_WHILE) {
        int n0 = gen_label_num();
        int n1 = gen_label_num();

        printf(".L%d:\n", n0);
        gen(node->nexts[0]);
        gen_pop("rax");
        printf("    cmp rax, 0\n");
        printf("    je .L%d\n", n1);
        gen(node->nexts[1]);
        gen_pop("rax");
        printf("    jmp .L%d\n", n0);
        printf(".L%d:\n", n1);
        gen_push("rax");

        return;
    }

    if (node->kind == ND_FOR) {
        int n0 = gen_label_num();
        int n1 = gen_label_num();

        gen(node->nexts[0]);
        printf(".L%d:\n", n0);
        gen(node->nexts[1]);
        gen_pop("rax");
        printf("    cmp rax, 0\n");
        printf("    je .L%d\n", n1);
        gen(node->nexts[3]);
        gen_pop("rax");
        gen(node->nexts[2]);
        gen_pop("rax");
        printf("    jmp .L%d\n", n0);
        printf(".L%d:\n", n1);
        gen_push("rax");

        return;
    }

    if (node->kind == ND_BLOCK) {
        const int n = vector_size(node->stmts);
        for (int i = 0; i < n; ++i) {
            Node *nd = vector_at(node->stmts, i);
            gen(nd);
            gen_pop("rax");
        }
        gen_push("rax");

        return;
    }

    if (node->kind == ND_FUNC) {
        const int n = vector_size(node->args);
        const char* regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        if (!is_stack_aligned) {
            printf("    sub rsp, 8\n");
        }

        for (int i = n - 1; i >= 0; --i) {
            Node *nd = vector_at(node->args, i);
            gen(nd);
        }
        for (int i = 0; i < n; ++i) {
            gen_pop(regs[i]);
        }

        char *fname = calloc(node->len+1, sizeof(char));
        memcpy(fname, node->fname, node->len);
        printf("    call %s\n", fname);

        if (!is_stack_aligned) {
            printf("    add rsp, 8\n");
        }
        gen_push("rax");

        return;
    }

    switch (node->kind) {
        case ND_NUM:
            printf("    push %d\n", node->val);
            is_stack_aligned ^= true;
            return;
        case ND_LVAR:
            gen_lval(node);
            gen_pop("rax");
            printf("    mov rax, [rax]\n");
            gen_push("rax");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);

            gen_pop("rdi");
            gen_pop("rax");
            printf("    mov [rax], rdi\n");
            gen_push("rdi");
            return;
    }


    gen(node->lhs);
    gen(node->rhs);

    gen_pop("rdi");
    gen_pop("rax");

    switch (node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NEQ:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LE:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LEQ:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }

    gen_push("rax");
}

