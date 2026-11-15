#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

void generateMatrix(long **grid, int n)
{
    long **pp = grid;
    long **end = grid + n * n;

    while (pp < end)
    {
        **pp = rand() % 256;
        pp++;
    }
}

void displayMatrix(long **grid, int n)
{
    long **pp = grid;
    long **end = grid + n * n;

    while (pp < end)
    {
        printf("%ld", **pp);
        pp++;

        if ((pp - grid) % n == 0)
            printf("\n");
        else
            printf(" ");
    }
}

void rotateMatrix(long **grid, int n)
{
    int i, j;
    long *temp;

    for (i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            long **p = grid + i * n + j;
            long **q = grid + j * n + i;

            temp = *p;
            *p = *q;
            *q = temp;
        }
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n / 2; j++)
        {
            long **p = grid + i * n + j;
            long **q = grid + i * n + (n - 1 - j);

            temp = *p;
            *p = *q;
            *q = temp;
        }
    }
}

void smoothMatrix(long **grid, int n)
{
    int r, c, nr, nc;

    for (r = 0; r < n; r++)
    {
        for (c = 0; c < n; c++)
        {
            long sum = 0;
            int count = 0;

            for (nr = r - 1; nr <= r + 1; nr++)
            {
                for (nc = c - 1; nc <= c + 1; nc++)
                {
                    if (nr >= 0 && nr < n &&
                        nc >= 0 && nc < n)
                    {
                        long *p = *(grid + nr * n + nc);
                        sum += (*p & 255);
                        count++;
                    }
                }
            }

            long average = sum / count;
            long *current = *(grid + r * n + c);

            *current = (*current & 255) | (average << 8);
        }
    }

    long **pp = grid;
    long **end = grid + n * n;

    while (pp < end)
    {
        **pp = **pp >> 8;
        pp++;
    }
}

int readInt(int *value)
{
    char buffer[32];
    char *endptr;
    long num;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        return 0;

    num = strtol(buffer, &endptr, 10);

    if (endptr == buffer)
        return 0;

    while (isspace((unsigned char)*endptr))
        endptr++;

    if (*endptr != '\0')
        return 0;

    *value = (int)num;
    return 1;
}

int main(void)
{
    int n;

    srand((unsigned int)time(NULL));

    printf("Enter matrix size (2-10): ");

    if (!readInt(&n) || n < 2 || n > 10)
    {
        printf("Invalid matrix size.\n");
        return 1;
    }
    printf("\n");

    long *data = malloc((size_t)n * n * sizeof(long));
    long **grid = malloc((size_t)n * n * sizeof(long *));

    if (data == NULL || grid == NULL)
    {
        printf("Memory allocation failed.\n");
        free(data);
        free(grid);
        return 1;
    }

    long **pp = grid;
    long *dp = data;
    long **gend = grid + n * n;

    while (pp < gend)
    {
        *pp = dp;
        pp++;
        dp++;
    }

    generateMatrix(grid, n);

    printf("Original Randomly Generated Matrix:\n");
    displayMatrix(grid, n);

    rotateMatrix(grid, n);

    printf("\n");
    printf("Matrix after 90 degree Clockwise Rotation:\n");
    displayMatrix(grid, n);

    smoothMatrix(grid, n);

    printf("\n");
    printf("Matrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(grid, n);

    free(grid);
    free(data);
    grid = NULL;
    data = NULL;

    return 0;
}