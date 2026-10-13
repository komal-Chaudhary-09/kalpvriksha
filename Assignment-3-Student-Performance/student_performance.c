#include <stdio.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define NAME_LENGTH 50

struct students {
    int rollNumber;
    char name[NAME_LENGTH];
    int marks1;
    int marks2;
    int marks3;
};


int total_marks(const struct students *s) {
    return s->marks1 + s->marks2 + s->marks3;
}


float average_marks(const struct students *s) {
    int total = total_marks(s);
    return (float)total / 3;
}


char gradee(float average) {
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}


void performance(char grade) {
    int stars;

    if (grade == 'A')
        stars = 5;
    else if (grade == 'B')
        stars = 4;
    else if (grade == 'C')
        stars = 3;
    else if (grade == 'D')
        stars = 2;
    else
        stars = 0;

    for (int i = 0; i < stars; i++) {
        printf("*");
    }

    printf("\n");
}


int valid_name(const char name[]) {
    if (name[0] == '\0')
        return 0;

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]))
            return 0;
    }

    return 1;
}


int valid_marks(const struct students *s) {
    return s->marks1 >= 0 && s->marks1 <= 100 &&
           s->marks2 >= 0 && s->marks2 <= 100 &&
           s->marks3 >= 0 && s->marks3 <= 100;
}


int inputStudents(struct students s[], int n) {
    for (int i = 0; i < n; i++) {
        if (scanf("%d %49s %d %d %d",
                  &s[i].rollNumber,
                  s[i].name,
                  &s[i].marks1,
                  &s[i].marks2,
                  &s[i].marks3) != 5) {
            return 0;
        }

        if (!valid_name(s[i].name) ||
            !valid_marks(&s[i])) {
            return 0;
        }
    }

    return 1;
}


void displayStudentReport(const struct students *s) {
    int total = total_marks(s);
    float average = average_marks(s);
    char grade = gradee(average);

    printf("Roll: %d\n", s->rollNumber);
    printf("Name: %s\n", s->name);
    printf("Total: %d\n", total);
    printf("Average: %.2f\n", average);
    printf("Grade: %c\n", grade);

    if (average < 35) {
        return;
    }

    printf("Performance: ");
    performance(grade);
}


void displayAllReports(const struct students s[], int n) {
    for (int i = 0; i < n; i++) {
        displayStudentReport(&s[i]);

        if (i < n - 1) {
            printf("\n");
        }
    }
}


void printRollNumbers(const struct students s[], int index, int n) {
    if (index == n)
        return;

    printf("%d", s[index].rollNumber);

    if (index < n - 1) {
        printf(" ");
    }

    printRollNumbers(s, index + 1, n);
}

int main(void) {
    struct students s[MAX_STUDENTS];
    int n;

    if (scanf("%d", &n) != 1 ||
        n < 1 || n > MAX_STUDENTS) {
        printf("Invalid input\n");
        return 1;
    }

    if (!inputStudents(s, n)) {
        printf("Invalid input\n");
        return 1;
    }

    displayAllReports(s, n);

    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(s, 0, n);
    printf("\n");

    return 0;
}