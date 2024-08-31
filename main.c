#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BANK_NAME "Good Bank Inc."
#define MAX_STRING_SIZE 512
#define MAX_CHOICE_COUNT 10
#define MAX_ACCOUNT_COUNT 100
#define MAX_TRANSACTION_HISTORY 20

#if defined(_WIN32)
#define clrscr() system("cls")
#else
#define clrscr() system("clear")
#endif

typedef enum {
    Deposit, Withdrawal, Send, Receive
} TransactionType;

const char* transaction_enum_to_string(TransactionType type) {
    switch (type) {
        case Deposit:    return "Deposit";
        case Withdrawal: return "Withdrawal";
        case Send:       return "Send";
        case Receive:    return "Receive";
        default:         return "Unknown";
    }
}

typedef struct {
    TransactionType type;
    double value;
} Transaction;

typedef struct {
    int id;
    int pass;
    char first_name[MAX_STRING_SIZE];
    char last_name[MAX_STRING_SIZE];
    double balance;
    Transaction history[MAX_TRANSACTION_HISTORY];
    int history_length;
} Account;

Account accounts[MAX_ACCOUNT_COUNT];
size_t account_count = 0;

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void press_return_to_continue() {
    printf("Press return to continue...");
    clear_input_buffer();
    getchar();
}

int selection(const char selections[MAX_CHOICE_COUNT][MAX_STRING_SIZE], int selection_count) {
    int choice = -1;
    while (1) {
        printf("\nChoose an option:\n");
        for (int i = 0; i < selection_count; i++) {
            printf("%d. %s\n", i + 1, selections[i]);
        }
        if (scanf("%d", &choice) != 1 || choice <= 0 || choice > selection_count) {
            printf("\nInvalid Choice\n");
            clear_input_buffer();
        } else {
            clear_input_buffer();
            break;
        }
    }
    return choice;
}

Account* login() {
    clrscr();
    while (1) {
        int account_id, account_pass;
        printf("Log-in\n\nID: ");
        if (scanf("%d", &account_id) != 1) {
            clear_input_buffer();
            printf("Invalid input!\n");
            continue;
        }
        printf("Pass: ");
        if (scanf("%d", &account_pass) != 1) {
            clear_input_buffer();
            printf("Invalid input!\n");
            continue;
        }
        for (size_t i = 0; i < account_count; i++) {
            if (accounts[i].id == account_id && accounts[i].pass == account_pass) {
                return &accounts[i];
            }
        }
        printf("Invalid credentials!\n\n");
        press_return_to_continue();
        return NULL;
    }
}

Account* create_account() {
    clrscr();
    if (account_count >= MAX_ACCOUNT_COUNT) {
        printf("All accounts are full\n");
        return NULL;
    }

    Account new_account;
    new_account.id = account_count;
    new_account.history_length = 0;
    new_account.balance = 0.0;

    printf("Account Creation\n\n");
    printf("First Name: ");
    fflush(stdout);
    scanf("%511s", new_account.first_name);
    printf("Last Name: ");
    fflush(stdout);
    scanf("%511s", new_account.last_name);
    printf("Pass: ");
    fflush(stdout);
    if (scanf("%d", &new_account.pass) != 1) {
        clear_input_buffer();
        printf("Invalid input for password!\n");
        return NULL;
    }

    accounts[account_count++] = new_account;
    clrscr();
    printf("Congrats! Your account was created!\n(IMPORTANT) Account ID: %d\nYou will need this during login each time\n\n", new_account.id);
    press_return_to_continue();
    return login();
}

Account* login_or_create_acc() {
    clrscr();
    printf("Welcome to %s\n", BANK_NAME);

    char start_choices[2][MAX_STRING_SIZE] = {"Log-in", "Create a new account"};
    int selected = selection(start_choices, 2);

    switch (selected) {
        case 1: return login();
        case 2: return create_account();
        default: return NULL;
    }
}

void add_to_transaction_history(Account* acc, Transaction tr) {
    if (acc->history_length < MAX_TRANSACTION_HISTORY) {
        acc->history[acc->history_length++] = tr;
    }
}

void display_transaction_history(const Account* acc) {
    clrscr();
    printf("Transaction history\n\n");
    for (int i = acc->history_length - 1; i >= 0; i--) {
        printf("%s | $%.2lf\n", transaction_enum_to_string(acc->history[i].type), acc->history[i].value);
    }
    printf("\n");
    press_return_to_continue();
}

void send_money(Account* sender) {
    clrscr();
    int target_acc;
    double amt;
    printf("Target Account ID: ");
    if (scanf("%d", &target_acc) != 1 || target_acc < 0) {
        clear_input_buffer();
        printf("Invalid input!\n");
        press_return_to_continue();
        return;
    }
    printf("Amount ($): ");
    if (scanf("%lf", &amt) != 1 || amt <= 0) {
        clear_input_buffer();
        printf("Invalid amount!\n");
        press_return_to_continue();
        return;
    }

    if (sender->balance < amt) {
        printf("Not enough money on account! ($%.2lf)\n", sender->balance);
        press_return_to_continue();
        return;
    }

    for (size_t i = 0; i < account_count; i++) {
        if (accounts[i].id == target_acc) {
            accounts[i].balance += amt;
            sender->balance -= amt;

            add_to_transaction_history(&accounts[i], (Transaction){Receive, amt});
            add_to_transaction_history(sender, (Transaction){Send, amt});

            clrscr();
            printf("\nTransaction Successful!\n");
            press_return_to_continue();
            return;
        }
    }

    printf("Account not found!\n");
    press_return_to_continue();
}

int main() {
    while (1) {
        Account* acc = NULL;
        while (!acc) acc = login_or_create_acc();

        const char choices[6][MAX_STRING_SIZE] = {
            "Check Balance", "Deposit Money", "Withdraw Money", "View Transaction History", "Send Money", "Logout"
        };

        while (1) {
            clrscr();
            printf("Welcome, %s!\n\nYour Account ID: %d\n\n", acc->first_name, acc->id);

            int choice = selection(choices, 6);
            double amt;

            switch (choice) {
                case 1:
                    clrscr();
                    printf("Your Balance: $%.2lf\n", acc->balance);
                    press_return_to_continue();
                    break;
                case 2:
                    clrscr();
                    printf("Enter the amount to deposit ($): ");
                    if (scanf("%lf", &amt) == 1 && amt > 0) {
                        acc->balance += amt;
                        add_to_transaction_history(acc, (Transaction){Deposit, amt});
                        printf("$%.2lf were added to your account!\n", amt);
                    } else {
                        clear_input_buffer();
                        printf("Invalid amount!\n");
                    }
                    press_return_to_continue();
                    break;
                case 3:
                    clrscr();
                    printf("Enter the amount to withdraw ($): ");
                    if (scanf("%lf", &amt) == 1 && amt > 0) {
                        if (acc->balance >= amt) {
                            acc->balance -= amt;
                            add_to_transaction_history(acc, (Transaction){Withdrawal, amt});
                            printf("$%.2lf were withdrawn from your account!\n", amt);
                        } else {
                            printf("Not enough money on account! ($%.2lf)\n", acc->balance);
                        }
                    } else {
                        clear_input_buffer();
                        printf("Invalid amount!\n");
                    }
                    press_return_to_continue();
                    break;
                case 4:
                    display_transaction_history(acc);
                    break;
                case 5:
                    send_money(acc);
                    break;
                case 6:
                    return 0;
                default:
                    break;
            }
        }
    }
}
