
#include <stdio.h>
#include <stdlib.h>

#define STANDARD_PRICE 150.0
#define PREMIUM_PRICE  250.0
#define RECLINER_PRICE 400.0
#define MIN_TICKETS    1
#define MAX_TICKETS    20
#define DISCOUNT_LIMIT 5
#define DISCOUNT_RATE  0.10

struct customer {
    char name[50];
    int no_of_tickets;
    int ticket_category;
};

struct bill {
    const char *category_name;
    double ticket_price;
    double sub_total;
    double disc;
    double final_bill;
};

int readInput(struct customer *ptr)
{
    if (scanf("%49s", ptr->name) != 1)
        return 0;

    if (scanf("%d", &ptr->no_of_tickets) != 1)
        return 0;

    if (scanf("%d", &ptr->ticket_category) != 1)
        return 0;

    return 1;
}

int validateTickets(const struct customer *ptr)
{
    if (ptr->no_of_tickets < MIN_TICKETS ||
        ptr->no_of_tickets > MAX_TICKETS) {
        printf("Error: Invalid number of tickets\n");
        return 0;
    }
    return 1;
}

int selectCategory(const struct customer *ptr, struct bill *billDetails)
{
    switch (ptr->ticket_category) {
        case 1:
            billDetails->ticket_price = STANDARD_PRICE;
            billDetails->category_name = "Standard";
            break;
        case 2:
            billDetails->ticket_price = PREMIUM_PRICE;
            billDetails->category_name = "Premium";
            break;
        case 3:
            billDetails->ticket_price = RECLINER_PRICE;
            billDetails->category_name = "Recliner";
            break;
        default:
            printf("Error: Invalid ticket category. Choose 1, 2, or 3.\n");
            return 0;
    }
    return 1;
}

void calculateBill(const struct customer *ptr, struct bill *billDetails)
{
    billDetails->sub_total = billDetails->ticket_price * ptr->no_of_tickets;

    if (ptr->no_of_tickets >= DISCOUNT_LIMIT) {
        billDetails->disc = billDetails->sub_total * DISCOUNT_RATE;
    } else {
        billDetails->disc = 0.0;
    }

    billDetails->final_bill = billDetails->sub_total - billDetails->disc;
}

void displayBill(const struct customer *ptr, const struct bill *billDetails)
{
    printf("Customer: %s\n", ptr->name);
    printf("Ticket Category: %s\n", billDetails->category_name);
    printf("Tickets: %d\n", ptr->no_of_tickets);
    printf("Subtotal: %.2f\n", billDetails->sub_total);
    printf("Discount: %.2f\n", billDetails->disc);
    printf("Final Bill: %.2f\n", billDetails->final_bill);
}

int main(void)
{
    struct customer *ptr = malloc(sizeof *ptr);
    struct bill billDetails;

    if (ptr == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    if (!readInput(ptr)) {
        free(ptr);
        return 1;
    }

    if (!validateTickets(ptr) || !selectCategory(ptr, &billDetails)) {
        free(ptr);
        return 1;
    }

    calculateBill(ptr, &billDetails);
    displayBill(ptr, &billDetails);

    free(ptr);
    return 0;
}
