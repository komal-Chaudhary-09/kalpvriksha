
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_STUDENTS 100
#define NAME_LENGTH 50
#define SUBJECT_COUNT 3
#define MIN_MARKS 0
#define MAX_MARKS 100
#define MIN_ROLL 1
#define MAX_ROLL 9999

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

        if (strchr(input, '\n') == NULL && !feof(stdin)) {
            int ch;

            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            printf("Input is too long. Try again.\n");
            continue;
        }

        errno = 0;
        number = strtol(input, &end, 10);

        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (end == input || *end != '\0' || errno == ERANGE
            || number < min || number > max) {
            printf("Invalid input. Enter a number between %d and %d.\n",
                   min, max);
            continue;
        }

        *value = (int)number;
        return 1;
    }
}

int readName(char name[], int size)
{
    char input[256];
    size_t length;
    int ch;
    int hasCharacter;

    while (1) {
        printf("Enter student name: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }

        length = strlen(input);

        if (length > 0 && input[length - 1] == '\n') {
            input[--length] = '\0';
        } else if (!feof(stdin)) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            printf("Name is too long. Maximum %d characters allowed.\n",
                   size - 1);
            continue;
        }

        hasCharacter = 0;

        for (size_t i = 0; input[i] != '\0'; i++) {
            if (!isspace((unsigned char)input[i])) {
                hasCharacter = 1;
                break;
            }
        }

        if (!hasCharacter) {
            printf("Name cannot be empty.\n");
            continue;
        }

        if (length >= (size_t)size) {
            printf("Name is too long. Maximum %d characters allowed.\n",
                   size - 1);
            continue;
        }

        strcpy(name, input);
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
        default:  return 0;
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
    while (1) {
        if (!readInteger("Enter roll number (1-9999): ",
                         MIN_ROLL, MAX_ROLL, &student->roll)) {
            return 0;
        }

        if (isDuplicateRoll(students, count, student->roll)) {
            printf("Roll number already exists. Enter a unique number.\n");
            continue;
        }

        break;
    }

    if (!readName(student->name, NAME_LENGTH)) {
        return 0;
    }

    printf("Enter marks for three subjects (0-100).\n");

    for (int i = 0; i < SUBJECT_COUNT; i++) {
        char prompt[40];

        snprintf(prompt, sizeof(prompt), "Subject %d marks: ", i + 1);

        if (!readInteger(prompt, MIN_MARKS, MAX_MARKS,
                         &student->marks[i])) {
            return 0;
        }
    }

    return 1;
}

int inputStudents(struct Student *students, int count)
{
    for (int i = 0; i < count; i++) {
        printf("\n--- Student %d ---\n", i + 1);

        if (!readStudent(&students[i], students, i)) {
            return 0;
        }
    }

    return 1;
}

void displayStudentReport(const struct Student *student,
                          int total, float average, char grade)
{
    printf("\n------------------------------\n");
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

void displayReports(const struct Student *students, int count)
{
    printf("\n===== STUDENT PERFORMANCE REPORT =====\n");

    for (int i = 0; i < count; i++) {
        const struct Student *current = students + i;
        int total = calculateTotal(current);
        float average = calculateAverage(total);
        char grade = calculateGrade(average);

        displayStudentReport(current, total, average, grade);

        if (average < 35) {
            continue;
        }

        printPerformance(grade);
    }
}

void printrolls(const struct Student *students,
                int count, int index)
{
    if (index >= count) {
        return;
    }

    printf(" %d", students[index].roll);

    printrolls(students, count, index + 1);
}

int main(void)
{
    struct Student students[MAX_STUDENTS];

    if (!readInteger("Enter number of students (1-100): ",
                     1, MAX_STUDENTS, &totalStudents)) {
        printf("\nInput ended unexpectedly.\n");
        return 1;
    }

    if (!inputStudents(students, totalStudents)) {
        printf("\nUnable to read student data. Program terminated.\n");
        return 1;
    }

    displayReports(students, totalStudents);

    printf("\nList of Roll Numbers (via recursion):");
    printrolls(students, totalStudents, 0);
    printf("\n");

    return 0;
}
