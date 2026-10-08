#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct User
{
    int id;
    char name[30];
    int age;
};

void clearInput()
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

int getInt(const char *message, int *value)
{
    printf("%s", message);

    if (scanf("%d", value) != 1)
    {
        clearInput();
        return 0;
    }

    clearInput();
    return 1;
}

int getName(const char *message, char name[], int size)
{
    printf("%s", message);

    if (fgets(name, size, stdin) == NULL)
    {
        return 0;
    }

    name[strcspn(name, "\n")] = '\0';

    if (strlen(name) == 0)
    {
        return 0;
    }

    return 1;
}

int idExists(int id)
{
    FILE *fp;
    struct User u;

    fp = fopen("users.txt", "r");

    if (fp == NULL)
    {
        return 0;
    }

    while (fscanf(fp, "%d %29s %d", &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void createUser()
{
    FILE *fp;
    struct User u;

    if (!getInt("ID: ", &u.id))
    {
        printf("Invalid ID\n");
        return;
    }

    if (idExists(u.id))
    {
        printf("ID already exists\n");
        return;
    }

    if (!getName("Name: ", u.name, sizeof(u.name)))
    {
        printf("Invalid name\n");
        return;
    }

    if (!getInt("Age: ", &u.age))
    {
        printf("Invalid age\n");
        return;
    }

    if (u.age < 1 )
    {
        printf("Invalid age\n");
        return;
    }

    fp = fopen("users.txt", "a");

    if (fp == NULL)
    {
        printf("File error\n");
        return;
    }

    fprintf(fp, "%d %s %d\n", u.id, u.name, u.age);

    fclose(fp);

    printf("User saved\n");
}

void readUsers()
{
    FILE *fp;
    struct User u;

    fp = fopen("users.txt", "r");

    if (fp == NULL)
    {
        printf("No file found\n");
        return;
    }

    while (fscanf(fp, "%d %29s %d", &u.id, u.name, &u.age) == 3)
    {
        printf("%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(fp);
}

int processFile(int id, int update)
{
    FILE *fp;
    FILE *temp;
    struct User u;
    int found = 0;

    fp = fopen("users.txt", "r");

    if (fp == NULL)
    {
        printf("No file found\n");
        return 0;
    }

    temp = fopen("temp.txt", "w");

    if (temp == NULL)
    {
        fclose(fp);
        printf("File error\n");
        return 0;
    }

    while (fscanf(fp, "%d %29s %d", &u.id, u.name, &u.age) == 3)
    {
        if (u.id == id)
        {
            found = 1;

            if (update)
            {
                if (!getName("New name: ", u.name, sizeof(u.name)))
                {
                    fclose(fp);
                    fclose(temp);
                    remove("temp.txt");
                    printf("Invalid name\n");
                    return 0;
                }

                if (!getInt("New age: ", &u.age))
                {
                    fclose(fp);
                    fclose(temp);
                    remove("temp.txt");
                    printf("Invalid age\n");
                    return 0;
                }

                if (u.age < 1)
                {
                    fclose(fp);
                    fclose(temp);
                    remove("temp.txt");
                    printf("Invalid age\n");
                    return 0;
                }
            }
            else
            {
                continue;
            }
        }

        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    if (remove("users.txt") != 0)
    {
        remove("temp.txt");
        printf("File error\n");
        return 0;
    }

    if (rename("temp.txt", "users.txt") != 0)
    {
        printf("File error\n");
        return 0;
    }

    return found;
}

void updateUser()
{
    int id;
    int found;

    if (!getInt("Enter ID: ", &id))
    {
        printf("Invalid ID\n");
        return;
    }

    found = processFile(id, 1);

    if (found)
    {
        printf("Updated\n");
    }
    else
    {
        printf("ID not found\n");
    }
}

void deleteUser()
{
    int id;
    int found;

    if (!getInt("Enter ID: ", &id))
    {
        printf("Invalid ID\n");
        return;
    }

    found = processFile(id, 0);

    if (found)
    {
        printf("Deleted\n");
    }
    else
    {
        printf("ID not found\n");
    }
}

int main()
{
    int choice;

    while (1)
    {
        printf("\n1 Create\n");
        printf("2 Read\n");
        printf("3 Update\n");
        printf("4 Delete\n");
        printf("5 Exit\n");

        if (!getInt("Choice: ", &choice))
        {
            printf("Wrong choice\n");
            continue;
        }

        if (choice == 1)
        {
            createUser();
        }
        else if (choice == 2)
        {
            readUsers();
        }
        else if (choice == 3)
        {
            updateUser();
        }
        else if (choice == 4)
        {
            deleteUser();
        }
        else if (choice == 5)
        {
            break;
        }
        else
        {
            printf("Wrong choice\n");
        }
    }

    return 0;
}