#include <stdio.h>
#include <string.h>

int main()
{
    char buf[1024];
    char inc[1024] = "#include     \"preprocess.h\"  ";
    strtok(inc, "\"");
    char *inc_file = strtok(NULL, "\"");
    // printf("%s\n", token);

    // while (token != NULL)
    // {
    //     printf("%s\n", token);
    //     token = strtok(NULL, "\"");
    // }
    FILE *fp = fopen(inc_file, "r");
    while (fgets(buf, sizeof(buf), fp) != NULL)
    {
        printf("%s", buf);
    }
}