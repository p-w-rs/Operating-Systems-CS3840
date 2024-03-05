#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char str[] = "Geeks";
    printf("%c\n", str[0]);
    printf("%c\n", *(str + 0));
    printf("%x\n", str); // address of G

    printf("%c\n", str[3]);
    printf("%c\n", *(str + 3));
    printf("%x\n", str + 3); // adress of k

    printf("%d\n", str[5]);
    printf("%d\n", *(str + 5));
    printf("%x\n", str + 5); // adress of end of str

    char str2[] = "Geeks\0for\0Geeks";
    printf("%s\n", str2);       // Geeks
    printf("%s\n", str2 + 1);   // eeks
    printf("%s\n\n", str2 + 6); // for

    char *str3 = str2 + 6;
    printf("%s\n", str2);
    printf("%lu\n", strlen(str2));
    printf("%s\n", str3);
    printf("%lu\n", strlen(str3));

    char str4[40] = "GeeksforGeeks";
    str4[13] = '2';
    printf("%s\n", str4);

    // str4 = str3; // doens't work
    for (int i = 0; i <= strlen(str3); i++)
    {
        str4[i] = str3[i];
    }
    // str => "for\0sforGeeks\0"
    printf("%s\n", str4);
    printf("%s\n", str4 + 4);
    return 0;
}