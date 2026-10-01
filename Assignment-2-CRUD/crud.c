#include <stdio.h>
#include <string.h>

struct User
{
    int id;
    char name[30];
    int age;
};

int main()
{
    struct User u;
    FILE *fp, *temp;
    int choice, id, found;

    while (1)
    {
        printf("\n1 Create\n");
        printf("2 Read\n");
        printf("3 Update\n");
        printf("4 Delete\n");
        printf("5 Exit\n");
        scanf("%d", &choice);

        if (choice == 1)
        {
            fp = fopen("users.txt", "a");

            if (fp == NULL)
            {
                printf("File error");
                continue;
            }

            printf("ID: ");
            scanf("%d", &u.id);

            printf("Name: ");
            scanf("%s", u.name);

            printf("Age: ");
            scanf("%d", &u.age);

            fprintf(fp, "%d %s %d\n", u.id, u.name, u.age);
            fclose(fp);

            printf("User saved\n");
        }

        else if (choice == 2)
        {
            fp = fopen("users.txt", "r");

            if (fp == NULL)
            {
                printf("No file found\n");
                continue;
            }

            while (fscanf(fp, "%d %s %d",
                          &u.id, u.name, &u.age) == 3)
            {
                printf("%d %s %d\n",
                       u.id, u.name, u.age);
            }

            fclose(fp);
        }

        else if (choice == 3)
        {
            printf("Enter ID: ");
            scanf("%d", &id);

            fp = fopen("users.txt", "r");
            temp = fopen("temp.txt", "w");
            found = 0;

            if (fp == NULL)
            {
                printf("No file found\n");
                continue;
            }

            while (fscanf(fp, "%d %s %d",
                          &u.id, u.name, &u.age) == 3)
            {
                if (u.id == id)
                {
                    printf("New name: ");
                    scanf("%s", u.name);

                    printf("New age: ");
                    scanf("%d", &u.age);

                    found = 1;
                }

                fprintf(temp, "%d %s %d\n",
                        u.id, u.name, u.age);
            }

            fclose(fp);
            fclose(temp);

            remove("users.txt");
            rename("temp.txt", "users.txt");

            if (found)
                printf("Updated\n");
            else
                printf("ID not found\n");
        }

        else if (choice == 4)
        {
            printf("Enter ID: ");
            scanf("%d", &id);

            fp = fopen("users.txt", "r");
            temp = fopen("temp.txt", "w");
            found = 0;

            if (fp == NULL)
            {
                printf("No file found\n");
                continue;
            }

            while (fscanf(fp, "%d %s %d",
                          &u.id, u.name, &u.age) == 3)
            {
                if (u.id == id)
                {
                    found = 1;
                    continue;
                }

                fprintf(temp, "%d %s %d\n",
                        u.id, u.name, u.age);
            }

            fclose(fp);
            fclose(temp);

            remove("users.txt");
            rename("temp.txt", "users.txt");

            if (found)
                printf("Deleted\n");
            else
                printf("ID not found\n");
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