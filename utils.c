#include "utils.h"

int atoi_(char *text) {
    int i;
    if (text[0] == '0') {
        if (text[1] == 'x' || text[1] == 'X') {
            sscanf(text, "%x", &i);
        } else {
            sscanf(text, "%o", &i);
        }
    } else {
        i = atoi(text);
    }
    return i;
}

char atoc(char *text) {
    int len = strlen(text);
    if (len == 1) {
        return text[0];
    } else if (len == 2) {
        switch (text[1]) {
        case 'a':
            return 7;
        case 'b':
            return 8;
        case 'f':
            return 12;
        case 'n':
            return 10;
        case 'r':
            return 13;
        case 't':
            return 9;
        case 'v':
            return 11;
        case '\\':
            return 92;
        case '?':
            return 63;
        case '\'':
            return 39;
        case '"':
            return 34;
        case '0':
            return 0;
        default:    // never reach
            return -1;
        }
    } else {
        char c;
        if (text[1] == 'x') {
            sscanf(text+2, "%x", &c);
        } else {
            sscanf(text+1, "%o", &c);
        }
        return c;
    }
}