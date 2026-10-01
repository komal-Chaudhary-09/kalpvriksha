#include <stdio.h>
#include <ctype.h>

int main()
{
    char str[500];
    int i = 0;
    int a, b;
    char sign;

    fgets(str, 500, stdin);

    while (str[i] == ' ')
        i++;

    if (!isdigit(str[i]))
    {
        printf("Invald expression");
        return 0;
    }

    a = 0;

    while (isdigit(str[i]))
    {
        a = a * 10 + str[i] - '0';
        i++;
    }

    while (1)
    {
        while (str[i] == ' ')
            i++;

        if (str[i] == '\n' || str[i] == '\0')
            break;

        sign = str[i];

        if (sign != '+' && sign != '-' && sign != '*' && sign != '/')
        {
            printf("Invalid expression");
            return 0;
        }

        i++;

        while (str[i] == ' ')
            i++;

        if (!isdigit(str[i]))
        {
            printf("Invalid expression");
            return 0;
        }

        b = 0;

        while (isdigit(str[i]))
        {
            b = b * 10 + str[i] - '0';
            i++;
        }

        if (sign == '+')
            a = a + b;

        else if (sign == '-')
            a = a - b;

        else if (sign == '*')
            a = a * b;

        else
        {
            if (b == 0)
            {
                printf("Error Division by zero.");
                return 0;
            }

            a = a / b;
        }
    }

    printf("%d", a);

    return 0;
}