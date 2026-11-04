
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateMatrix(int *matrix, int n)
{
    int *p = matrix;
    int *end = matrix + n * n;

    while (p < end)
    {
        *p = rand() % 256;
        p++;
    }
}

void displayMatrix(const int *matrix, int n)
{
    const int *p = matrix;
    const int *end = matrix + n * n;

    while (p < end)
    {
        printf("%d", *p);
        p++;

        if ((p - matrix) % n == 0)
            printf("\n");
        else
            printf(" ");
    }
}

void rotateMatrix(int *matrix, int n)
{
    int i, j, temp;

    for (i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            int *p = matrix + i * n + j;
            int *q = matrix + j * n + i;

            temp = *p;
            *p = *q;
            *q = temp;
        }
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n / 2; j++)
        {
            int *p = matrix + i * n + j;
            int *q = matrix + i * n + (n - 1 - j);

            temp = *p;
            *p = *q;
            *q = temp;
        }
    }
}

void smoothMatrix(int *matrix, int n)
{
    int r, c, nr, nc;

    for (r = 0; r < n; r++)
    {
        for (c = 0; c < n; c++)
        {
            int sum = 0;
            int count = 0;

            for (nr = r - 1; nr <= r + 1; nr++)
            {
                for (nc = c - 1; nc <= c + 1; nc++)
                {
                    if (nr >= 0 && nr < n &&
                        nc >= 0 && nc < n)
                    {
                        int *p = matrix + nr * n + nc;
                        sum += (*p & 255);
                        count++;
                    }
                }
            }

            int average = sum / count;
            int *current = matrix + r * n + c;

            *current = (*current & 255) | (average << 8);
        }
    }

    int *p = matrix;
    int *end = matrix + n * n;

    while (p < end)
    {
        *p = *p >> 8;
        p++;
    }
}

int main(void)
{
    int n;

    srand((unsigned int)time(NULL));

    printf("Enter matrix size (2-10): ");
    

    if (scanf("%d", &n) != 1 || n < 2 || n > 10)
    {
        printf("Invalid matrix size.\n");
        return 1;
    }
    printf("\n");

    int *matrix = malloc((size_t)n * n * sizeof(int));

    if (matrix == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    generateMatrix(matrix, n);

    printf("Original Randomly Generated Matrix:\n");
    displayMatrix(matrix, n);

    rotateMatrix(matrix, n);
 
    printf("\n");

    printf("Matrix after 90 degree Clockwise Rotation:\n");
    displayMatrix(matrix, n);

    smoothMatrix(matrix, n);

    printf("\n");

    printf("Matrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, n);

    free(matrix);
    matrix = NULL;

    return 0;
}
