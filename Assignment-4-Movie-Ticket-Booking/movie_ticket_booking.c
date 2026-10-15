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
    char        name[50];
    int         no_of_tickets;
    int         ticket_category;
    const char *category_name;
    double      ticket_price;
    double      sub_total;
    double      disc;
    double      final_bill;
};


void readInput(struct customer *ptr)
{
    scanf("%49s", ptr->name);
    scanf("%d", &ptr->no_of_tickets);
    scanf("%d", &ptr->ticket_category);
}


int validateTickets(const struct customer *ptr)
{
    if (ptr->no_of_tickets < MIN_TICKETS || ptr->no_of_tickets > MAX_TICKETS) {
        printf("Error: Invalid number of tickets\n");
        return 0;
    }
    return 1;
}


int selectCategory(struct customer *ptr)
{
    switch (ptr->ticket_category) {
        case 1:
            ptr->ticket_price = STANDARD_PRICE;
            ptr->category_name = "Standard";
            break;
        case 2:
            ptr->ticket_price = PREMIUM_PRICE;
            ptr->category_name = "Premium";
            break;
        case 3:
            ptr->ticket_price = RECLINER_PRICE;
            ptr->category_name = "Recliner";
            break;
        default:
            printf("Error: Invalid ticket category. Choose 1, 2, or 3.\n");
            return 0;
    }
    return 1;
}


void calculateBill(struct customer *ptr)
{
    ptr->sub_total = ptr->ticket_price * ptr->no_of_tickets;

    if (ptr->no_of_tickets >= DISCOUNT_LIMIT) {
        ptr->disc = ptr->sub_total * DISCOUNT_RATE;
    } else {
        ptr->disc = 0.0;
    }

    ptr->final_bill = ptr->sub_total - ptr->disc;
}


void displayBill(const struct customer *ptr)
{
    printf("Customer: %s\n", ptr->name);
    printf("Ticket Category: %s\n", ptr->category_name);
    printf("Tickets: %d\n", ptr->no_of_tickets);
    printf("Subtotal: %.2f\n", ptr->sub_total);
    printf("Discount: %.2f\n", ptr->disc);
    printf("Final Bill: %.2f\n", ptr->final_bill);
}

int main(void)
{
    
    struct customer *ptr = malloc(sizeof *ptr);

    if (ptr == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    readInput(ptr);

    if (!validateTickets(ptr) || !selectCategory(ptr)) {
        free(ptr);
        return 1;
    }

    calculateBill(ptr);
    displayBill(ptr);

    free(ptr);
    return 0;
}