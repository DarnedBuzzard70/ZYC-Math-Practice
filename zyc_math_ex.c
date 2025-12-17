#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

typedef struct {
    int a, b;
    char op;
    int answer;
    int remainder;     // for division with remainder
    int userAns;
    int userRem;
    int correct;
    double seconds;
} Problem;

typedef struct {
    int count;
    int maxValue;
    int allowMul;
    int allowDiv;
    int allowNegative;
    int divRemainderMode; // 0: force exact division, 1: allow remainder
} Config;

static int randint(int lo, int hi) { // inclusive
    return lo + rand() % (hi - lo + 1);
}

static char pick_op(const Config *cfg) {
    char ops[4];
    int n = 0;
    ops[n++] = '+';
    ops[n++] = '-';
    if (cfg->allowMul) ops[n++] = '*';
    if (cfg->allowDiv) ops[n++] = '/';
    return ops[rand() % n];
}

static void gen_problem(const Config *cfg, Problem *p) {
    p->op = pick_op(cfg);
    p->a = randint(1, cfg->maxValue);
    p->b = randint(1, cfg->maxValue);
    p->remainder = 0;

    // Adjust to meet constraints
    if (p->op == '-') {
        if (!cfg->allowNegative && p->a < p->b) {
            int tmp = p->a; p->a = p->b; p->b = tmp;
        }
        p->answer = p->a - p->b;
    } else if (p->op == '+') {
        p->answer = p->a + p->b;
    } else if (p->op == '*') {
        p->answer = p->a * p->b;
    } else if (p->op == '/') {
        if (cfg->divRemainderMode == 0) {
            // force exact division
            p->b = randint(1, cfg->maxValue);
            p->answer = randint(1, cfg->maxValue);
            p->a = p->answer * p->b;
        } else {
            // allow remainder
            if (p->b == 0) p->b = 1;
            p->answer = p->a / p->b;
            p->remainder = p->a % p->b;
        }
    }
}

static void print_problem(const Problem *p, int idx) {
    if (p->op == '/')
        printf("Q%02d: %d %c %d = ?%s\n", idx, p->a, p->op, p->b,
               (p->remainder || p->b==0) ? " (商，余数)" : "");
    else
        printf("Q%02d: %d %c %d = ?\n", idx, p->a, p->op, p->b);
}

int main(void) {
    srand((unsigned)time(NULL));
    Config cfg = {
        .count = 10,
        .maxValue = 50,
        .allowMul = 1,
        .allowDiv = 1,
        .allowNegative = 0,
        .divRemainderMode = 0
    };

    printf("中学生口算练习器（C 语言示例）\n");
    printf("配置: 题目数=%d, 最大数=%d, *=%s, /=%s, 允许负数=%s, 除法模式=%s\n\n",
           cfg.count, cfg.maxValue,
           cfg.allowMul ? "是" : "否",
           cfg.allowDiv ? "是" : "否",
           cfg.allowNegative ? "是" : "否",
           cfg.divRemainderMode ? "带余数" : "整除");

    Problem *probs = calloc(cfg.count, sizeof(Problem));
    if (!probs) return 1;

    int correct = 0;
    double totalSeconds = 0.0;

    for (int i = 0; i < cfg.count; ++i) {
        gen_problem(&cfg, &probs[i]);
        print_problem(&probs[i], i + 1);
        clock_t start = clock();

        if (probs[i].op == '/' && cfg.divRemainderMode) {
            printf("请输入商和余数，以空格分隔：");
            scanf("%d %d", &probs[i].userAns, &probs[i].userRem);
        } else {
            printf("请输入答案：");
            scanf("%d", &probs[i].userAns);
        }

        clock_t end = clock();
        probs[i].seconds = (double)(end - start) / CLOCKS_PER_SEC;
        totalSeconds += probs[i].seconds;

        if (probs[i].op == '/' && cfg.divRemainderMode) {
            probs[i].correct = (probs[i].userAns == probs[i].answer &&
                                probs[i].userRem == probs[i].remainder);
        } else {
            probs[i].correct = (probs[i].userAns == probs[i].answer);
        }

        if (probs[i].correct) {
            ++correct;
            printf("✅ 正确！用时 %.2fs\n\n", probs[i].seconds);
        } else {
            if (probs[i].op == '/' && cfg.divRemainderMode)
                printf("❌ 错误，正确答案：%d 余 %d\n\n", probs[i].answer, probs[i].remainder);
            else
                printf("❌ 错误，正确答案：%d\n\n", probs[i].answer);
        }
    }

    printf("===== 统计 =====\n");
    printf("得分：%d / %d\n", correct, cfg.count);
    printf("总用时：%.2fs，平均每题：%.2fs\n\n", totalSeconds, totalSeconds / cfg.count);

    printf("错题回顾：\n");
    for (int i = 0; i < cfg.count; ++i) {
        if (!probs[i].correct) {
            if (probs[i].op == '/' && cfg.divRemainderMode)
                printf("Q%02d: %d %c %d，正确：%d 余 %d， 你的：%d 余 %d\n",
                       i + 1, probs[i].a, probs[i].op, probs[i].b,
                       probs[i].answer, probs[i].remainder,
                       probs[i].userAns, probs[i].userRem);
            else
                printf("Q%02d: %d %c %d，正确：%d， 你的：%d\n",
                       i + 1, probs[i].a, probs[i].op, probs[i].b,
                       probs[i].answer, probs[i].userAns);
        }
    }

    free(probs);
    return 0;
}