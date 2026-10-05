#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#define MAX_INPUT_SIZE 500

#define INVALID "Error: Invalid expression."
#define DIV_ZERO "Error: Division by zero."

void skipSpaces(char str[], int *i)
{
    while (isspace((unsigned char)str[*i]))
    {
        (*i)++;
    }
}

int getNumber(char str[], int *i, long long *number)
{
    long long value = 0;
    int digit;

    if (!isdigit((unsigned char)str[*i]))
    {
        return 0;
    }

    while (isdigit((unsigned char)str[*i]))
    {
        digit = str[*i] - '0';

        if (value > (LLONG_MAX - digit) / 10)
        {
            return 0;
        }

        value = value * 10 + digit;
        (*i)++;
    }

    *number = value;

    return 1;
}

int calculate(long long a, long long b, char op, long long *answer)
{
    if (op == '/' && b == 0)
    {
        return -1;
    }

    if (op == '+')
    {
        if (b > 0 && a > LLONG_MAX - b)
        {
            return 0;
        }

        if (b < 0 && a < LLONG_MIN - b)
        {
            return 0;
        }

        *answer = a + b;
    }
    else if (op == '-')
    {
        if (b > 0 && a < LLONG_MIN + b)
        {
            return 0;
        }

        if (b < 0 && a > LLONG_MAX + b)
        {
            return 0;
        }

        *answer = a - b;
    }
    else if (op == '*')
    {
        if (a != 0 && b != 0)
        {
            if (a == LLONG_MIN && b == -1)
            {
                return 0;
            }

            if (b == LLONG_MIN && a == -1)
            {
                return 0;
            }

            if (a > 0 && b > 0 && a > LLONG_MAX / b)
            {
                return 0;
            }

            if (a > 0 && b < 0 && b < LLONG_MIN / a)
            {
                return 0;
            }

            if (a < 0 && b > 0 && a < LLONG_MIN / b)
            {
                return 0;
            }

            if (a < 0 && b < 0 && a < LLONG_MAX / b)
            {
                return 0;
            }
        }

        *answer = a * b;
    }
    else if (op == '/')
    {
        if (a == LLONG_MIN && b == -1)
        {
            return 0;
        }

        *answer = a / b;
    }
    else
    {
        return 0;
    }

    return 1;
}

int evaluate(char str[], long long *answer)
{
    int i = 0;
    int status;
    char op;
    long long number;
    long long result = 0;
    long long term;
    long long temp;

    skipSpaces(str, &i);

    if (!getNumber(str, &i, &number))
    {
        return 0;
    }

    term = number;

    while (1)
    {
        skipSpaces(str, &i);

        if (str[i] == '\0' || str[i] == '\n')
        {
            break;
        }

        if (str[i] != '+' && str[i] != '-' &&
            str[i] != '*' && str[i] != '/')
        {
            return 0;
        }

        op = str[i];
        i++;

        skipSpaces(str, &i);

        if (!getNumber(str, &i, &number))
        {
            return 0;
        }

        if (op == '*' || op == '/')
        {
            status = calculate(term, number, op, &temp);

            if (status == -1)
            {
                return -1;
            }

            if (status == 0)
            {
                return 0;
            }

            term = temp;
        }
        else
        {
            status = calculate(result, term, '+', &temp);

            if (status == 0)
            {
                return 0;
            }

            result = temp;

            if (op == '+')
            {
                term = number;
            }
            else
            {
                if (number > 0)
                {
                    term = -number;
                }
                else
                {
                    return 0;
                }
            }
        }
    }

    status = calculate(result, term, '+', &temp);

    if (status == 0)
    {
        return 0;
    }

    *answer = temp;

    return 1;
}

int main()
{
    char str[MAX_INPUT_SIZE];
    long long answer;
    int i = 0;
    int ch;
    int status;

    if (fgets(str, MAX_INPUT_SIZE, stdin) == NULL)
    {
        printf("%s", INVALID);
        return 0;
    }

    while (str[i] != '\0' && str[i] != '\n')
    {
        i++;
    }

    if (str[i] == '\0' && !feof(stdin))
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }

        printf("%s", INVALID);
        return 0;
    }

    status = evaluate(str, &answer);

    if (status == -1)
    {
        printf("%s", DIV_ZERO);
        return 0;
    }

    if (status == 0)
    {
        printf("%s", INVALID);
        return 0;
    }

    printf("%lld", answer);

    return 0;
}