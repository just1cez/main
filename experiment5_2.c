#include <stdio.h>
#include <string.h>
#include <ctype.h>

int wordcount(char* s) {
    int count = 0;
    char word[1000] = {};
    char words[1000][1000] = {};
    char* p = s;
    while (*p != '\0') {
        int index = 0;
        if (!isalpha(*p)) {
            p++;
            continue;
        }
        if (*p == '\0') {
            break;
        }
        while (*p != '\0' && isalpha(*p)) {
            word[index] = tolower(*p);
            index++;
            p++;
        }
        word[index] = '\0';
        int new = 1;
        for (int i = 0;i < count;i++) {
            if (strcmp(word, words[i]) == 0) {
                new = 0;
                break;
            }
        }
        if (new) {
            strcpy(words[count],word);
            count++;
        }
    }
    return count;
}

int main() {
    char s[1000] = {};
    scanf("%[^\n]", s);
    getchar();
    printf("%d",wordcount(s));
}