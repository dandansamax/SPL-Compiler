#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *get_rel_path(const char *file_path)
{
    int idx = 0;
    int after_last_idx = 0;
    char c = file_path[idx++];
    while (c != '\0')
    {
        if (c == '/')
        {
            after_last_idx = idx;
        }
        c = file_path[idx++];
    }
    char *rel_pwd;
    rel_pwd = (char *)malloc(sizeof(char) * (after_last_idx + 1));
    strcpy(rel_pwd, file_path);
    rel_pwd[after_last_idx] = '\0';
    return rel_pwd;
}

char *get_filename(const char *file_path)
{
    int idx = 0;
    int after_last_idx = 0;
    char c = file_path[idx++];
    while (c != '\0')
    {
        if (c == '/')
        {
            after_last_idx = idx;
        }
        c = file_path[idx++];
    }
    char *filename = (char *)malloc(sizeof(char) * idx);
    strcpy(filename, file_path + after_last_idx);
    return filename;
}

void test(const char *input)
{
    printf("inside: %s\n", input);
}

int main()
{
    char *test = "123123/";
    printf("%s|%s\n", get_rel_path(test), get_filename(test));
}
