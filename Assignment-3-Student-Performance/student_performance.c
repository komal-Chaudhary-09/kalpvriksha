
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_STUDENTS 100
#define NAME_LENGTH 50
#define SUBJECT_COUNT 3
#define MIN_ROLL 1
#define MAX_ROLL 100
#define MIN_MARKS 0
#define MAX_MARKS 100

int totalStudents = 0;

struct Student {
    int roll;
    char name[NAME_LENGTH];
    int marks[SUBJECT_COUNT];
};

int readInteger(const char *prompt, int min, int max, int *value)
{
    char input[128];
    char *end;
    long number;

    while (1) {
        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }

        errno = 0;
        number = strtol(input, &end, 10);

        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (end == input || *end != '\0' ||
            errno == ERANGE || number < min || number > max) {
            printf("Invalid input. Enter a number between %d and %d.\n",
                   min, max);
            continue;
        }

        *value = (int)number;
        return 1;
    }
}

int isDuplicateRoll(const struct Student *students,
                    int count, int roll)
{
    for (int i = 0; i < count; i++) {
        if (students[i].roll == roll) {
            return 1;
        }
    }

    return 0;
}

int readStudent(struct Student *student,
                const struct Student *students, int count)
{
    char line[256];
    char extra;
    int roll;
    int marks1, marks2, marks3;
    char name[NAME_LENGTH];

    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return 0;
        }

        if (strchr(line, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("Invalid student record.\n");
            continue;
        }

        if (sscanf(line, "%d %49s %d %d %d %c",
                   &roll, name, &marks1, &marks2, &marks3,
                   &extra) != 5) {
            printf("Invalid student record. Try again.\n");
            continue;
        }

        if (roll < MIN_ROLL || roll > MAX_ROLL) {
            printf("Roll number must be between 1 and 100.\n");
            continue;
        }

        if (isDuplicateRoll(students, count, roll)) {
            printf("Roll number already exists. Try again.\n");
            continue;
        }

        if (marks1 < MIN_MARKS || marks1 > MAX_MARKS ||
            marks2 < MIN_MARKS || marks2 > MAX_MARKS ||
            marks3 < MIN_MARKS || marks3 > MAX_MARKS) {
            printf("Marks must be between 0 and 100.\n");
            continue;
        }

        student->roll = roll;
        strcpy(student->name, name);
        student->marks[0] = marks1;
        student->marks[1] = marks2;
        student->marks[2] = marks3;

        return 1;
    }
}

int calculateTotal(const struct Student *student)
{
    int total = 0;

    for (int i = 0; i < SUBJECT_COUNT; i++) {
        total += student->marks[i];
    }

    return total;
}

float calculateAverage(int total)
{
    return (float)total / SUBJECT_COUNT;
}

char calculateGrade(float average)
{
    if (average >= 85) {
        return 'A';
    } else if (average >= 70) {
        return 'B';
    } else if (average >= 50) {
        return 'C';
    } else if (average >= 35) {
        return 'D';
    }

    return 'F';
}

int starsForGrade(char grade)
{
    switch (grade) {
        case 'A': return 5;
        case 'B': return 4;
        case 'C': return 3;
        case 'D': return 2;
        default: return 0;
    }
}

void displayStudentReport(const struct Student *student,
                          int total, float average, char grade)
{
    printf("Roll: %d\n", student->roll);
    printf("Name: %s\n", student->name);
    printf("Total: %d\n", total);
    printf("Average: %.2f\n", average);
    printf("Grade: %c\n", grade);
}

void printPerformance(char grade)
{
    int stars = starsForGrade(grade);

    printf("Performance: ");

    for (int i = 0; i < stars; i++) {
        printf("*");
    }

    printf("\n");
}

int inputStudents(struct Student *students, int count)
{
    for (int i = 0; i < count; i++) {
        if (!readStudent(&students[i], students, i)) {
            return 0;
        }
    }

    return 1;
}

void displayReports(const struct Student *students, int count)
{
    for (int i = 0; i < count; i++) {
        const struct Student *current = students + i;
        int total = calculateTotal(current);
        float average = calculateAverage(total);
        char grade = calculateGrade(average);

        displayStudentReport(current, total, average, grade);

        if (average < 35) {
            printf("\n");
            continue;
        }

        printPerformance(grade);

        if (i < count - 1) {
            printf("\n");
        }
    }
}

void printRollNumbers(const struct Student *students,
                      int count, int index)
{
    if (index >= count) {
        return;
    }

    if (index > 0) {
        printf(" ");
    }

    printf("%d", students[index].roll);
    printRollNumbers(students, count, index + 1);
}

int main(void)
{
    struct Student students[MAX_STUDENTS];

    if (!readInteger("", 1, MAX_STUDENTS, &totalStudents)) {
        return 1;
    }

    if (!inputStudents(students, totalStudents)) {
        return 1;
    }

    displayReports(students, totalStudents);

    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(students, totalStudents, 0);
    printf("\n");

    return 0;
}