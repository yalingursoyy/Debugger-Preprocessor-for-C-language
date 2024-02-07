#ifndef DEBUGHDR_H_
#define DEBUGHDR_H_

struct tr {
    char name;
    int value;
};

#define MAX_TRACES 20

struct tr Traces[MAX_TRACES];
int nTr = 0;

void add_TT(char var, int initialValue) {
    for (int i = 0; i < nTr; i++) {
        if (Traces[i].name == var) {
            printf("Warning: Variable '%c' is already being traced.\n", var);
            return;
        }
    }
    if (nTr < MAX_TRACES) {
        Traces[nTr].name = var;
        Traces[nTr].value = initialValue;
        nTr++;
    }
    else {
        printf("Error: Traces table is full.\n");
    }
}

void display_TT(char var) {
    for (int i = 0; i < nTr; i++) {
        if (Traces[i].name == var) {
            printf("%c: %d\n", var, Traces[i].value);
            getchar();
            return;
        }
    }
    printf("Error: Variable '%c' not found in Traces table.\n", var);
}

void update_TT(char var, int val) {
    for (int i = 0; i < nTr; i++) {
        if (Traces[i].name == var) {
            Traces[i].value = val;
            return;
        }
    }
    printf("Error: Variable '%c' not found in Traces table.\n", var);
}

#endif /* DEBUGHDR_H_ */
