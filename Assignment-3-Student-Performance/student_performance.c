
#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100

struct Student {
    int rollNumber;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};

int calculateTotal(struct Student student) {
    return student.marks1 + student.marks2 + student.marks3;
}

float calculateAverage(int total) {
return total / 3.0;
}

char calculateGrade(float average) {
    if (average >= 85) {
        return 'A';
    } else if (average >= 70) {
        return 'B';
    } else if (average >= 50) {
        return 'C';
    } else if (average >= 35) {
        return 'D';
    } else {
        return 'F';
    }
}

void printPerformance(char grade) {
int stars = 0;

if (grade == 'A') {
        stars = 5;
} else if (grade == 'B') {
        stars = 4;
    } else if (grade == 'C') {
        stars = 3;
    } else if (grade == 'D') {
        stars = 2;
    }

    for (int i = 0; i < stars; i++) {
        printf("*");
    }

    printf("\n");
}

void printRollNumbers(struct Student students[], int index, int n) {
if (index >= n) {
        return;
    }

    printf("%d", students[index].rollNumber);

    if (index < n - 1) {
        printf(" ");
    }

    printRollNumbers(students, index + 1, n);
}

int main() {
    int n;

    printf("Enter number of students: ");
    scanf("%d", &n);

    struct Student students[MAX_STUDENTS];

    for (int i = 0; i < n; i++) {
        scanf("%d %s %d %d %d",
              &students[i].rollNumber,
              students[i].name,
              &students[i].marks1,
              &students[i].marks2,
              &students[i].marks3);
    }

    for (int i = 0; i < n; i++) {
        int total = calculateTotal(students[i]);
        float average = calculateAverage(total);
        char grade = calculateGrade(average);

        printf("Roll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

    if (average < 35) {
     printf("Performance: \n");
     continue;
 }

    printf("Performance: ");
    printPerformance(grade);
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, 0, n);
    printf("\n");

    return 0;
}