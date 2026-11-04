#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct emp {
    int id;
    char name[50];
    char desig[50];
    struct emp *boss;
    struct emp *child;     
    struct emp *sibling;   
    struct emp *next;     
};

struct emp *head = NULL;

struct emp *find(int id)
{
    struct emp *p = head;
    while (p != NULL && p->id != id)
        p = p->next;
    return p;
}

int countChildren(struct emp *e)
{
    int n = 0;
    struct emp *c;
    for (c = e->child; c != NULL; c = c->sibling)
        n++;
    return n;
}

void addChild(struct emp *boss, struct emp *e)
{
    struct emp *c;
    e->sibling = NULL;
    if (boss->child == NULL) {
        boss->child = e;
        return;
    }
    c = boss->child;
    while (c->sibling != NULL)
        c = c->sibling;
    c->sibling = e;
}

void removeChild(struct emp *boss, struct emp *e)
{
    struct emp **p = &boss->child;
    while (*p != e)
        p = &(*p)->sibling;
    *p = e->sibling;
}


int countBelow(struct emp *e)
{
    int total = 0;
    struct emp *c;
    for (c = e->child; c != NULL; c = c->sibling)
        total = total + 1 + countBelow(c);
    return total;
}

void show(struct emp *e, int level)
{
    int i;
    struct emp *c;
    for (i = 0; i < level; i++)
        printf("    ");
    printf("%d - %s - %s\n", e->id, e->name, e->desig);
    for (c = e->child; c != NULL; c = c->sibling)
        show(c, level + 1);
}

void addEmployee(void)
{
    int id, mid;
    struct emp *e, *boss = NULL, *last;

    printf("Enter Employee ID: ");
    scanf("%d", &id);
    if (id <= 0 || find(id) != NULL) {
        printf("Invalid or duplicate Employee ID.\n");
        return;
    }

    e = (struct emp *)malloc(sizeof(struct emp));
    if (e == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    e->id = id;

    printf("Enter Name: ");
    scanf(" %49[^\n]", e->name);
    printf("Enter Designation: ");
    scanf(" %49[^\n]", e->desig);
    printf("Enter Manager ID (0 for top-level): ");
    scanf("%d", &mid);

    if (mid != 0) {
        boss = find(mid);
        if (boss == NULL) {
            printf("Manager not found.\n");
            free(e);
            return;
        }
    }

    e->boss = boss;
    e->child = NULL;
    e->sibling = NULL;
    e->next = NULL;

    
    if (head == NULL)
        head = e;
    else {
        last = head;
        while (last->next != NULL)
            last = last->next;
        last->next = e;
    }

    if (boss != NULL)
        addChild(boss, e);

    printf("Employee added successfully.\n");
}

void removeEmployee(void)
{
    int id;
    struct emp *e, *target, *s, *c, **p;

    printf("Enter Employee ID to remove: ");
    scanf("%d", &id);

    e = find(id);
    if (e == NULL) {
        printf("Employee not found.\n");
        return;
    }
    if (strcmp(e->desig, "CEO") == 0) {
        printf("The CEO cannot be removed.\n");
        return;
    }
    if (e->boss == NULL && e->child != NULL) {
        printf("Cannot remove: no superior to take over the reports.\n");
        return;
    }

    
    target = e->boss;
    if (e->boss != NULL) {
        for (s = e->boss->child; s != NULL; s = s->sibling) {
            if (s == e)
                continue;
            if (target == e->boss || countChildren(s) < countChildren(target))
                target = s;
        }
        removeChild(e->boss, e);
    }

    
    while (e->child != NULL) {
        c = e->child;
        e->child = c->sibling;
        c->boss = target;
        addChild(target, c);
    }

   
    p = &head;
    while (*p != e)
        p = &(*p)->next;
    *p = e->next;

    free(e);
    printf("Employee removed successfully.\n");
}

void showEmployee(void)
{
    int id;
    struct emp *e;

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    e = find(id);
    if (e == NULL) {
        printf("Employee not found.\n");
        return;
    }

    printf("\n");
    show(e, 0);
    printf("\nTotal Employees Under %d: %d\n", id, countBelow(e));
}

void showAll(void)
{
    struct emp *e;
    int total = 0;

    printf("\n===== ORGANIZATION HIERARCHY =====\n\n");
    for (e = head; e != NULL; e = e->next) {
        total++;
        if (e->boss == NULL)
            show(e, 0);
    }
    printf("\nTotal Employees: %d\n", total);
}

void freeAll(void)
{
    struct emp *e;
    while (head != NULL) {
        e = head;
        head = head->next;
        free(e);
    }
}

void menu(void)
{
    printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main(void)
{
    int choice;

    while (1) {
        menu();
        if (scanf("%d", &choice) != 1)
            break;

        switch (choice) {
            case 1: addEmployee(); break;
            case 2: removeEmployee(); break;
            case 3: showEmployee(); break;
            case 4: showAll(); break;
            case 5: freeAll(); return 0;
            default: printf("Invalid choice.\n");
        }
    }

    freeAll();
    return 0;
}