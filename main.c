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

enum TransactionType
{
    Deposit,Withdrawal,Send,Recieve
};


typedef enum TransactionType TransactionType;

char* transaction_enum_to_string(TransactionType type)
{
    switch (type)
    {
        case Deposit:
            return "Deposit";
        case Withdrawal:
            return "Withdrawal";
        case Send:
            return "Send";
        case Recieve:
            return "Recieve";
        default:
            return "Unknown";
    }
}


struct Transaction
{
    TransactionType type;
    double value;
};

typedef struct Transaction Transaction;

struct Account
{
    int id;
    int pass;
    char first_name[MAX_STRING_SIZE];
    char last_name[MAX_STRING_SIZE];
    double balance;
    Transaction history[MAX_TRANSACTION_HISTORY];
    int history_length;
};

typedef struct Account Account;

Account accounts[MAX_ACCOUNT_COUNT];
int account_count = 0;

int selection(char selections[MAX_CHOICE_COUNT][MAX_STRING_SIZE],int selection_count)
{
    int choice = -1;
    while(1){
        printf("\nChoose an option:\n");
        for(int i = 0; i < selection_count; i++)
        {   
            printf("%u. %s\n",i+1,selections[i]);
        }
        scanf("%u",&choice);
        if(choice <= 0 || choice > selection_count)
        {
            printf("\nInvalid Choice\n");
        }
        else
        {
            break;
        }
    }
    return choice;
}

void pressReturnToContinue() {
    printf("Press return to continue...");
    // Consume any remaining characters in the input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    // Wait for return key press
    getchar();
}

Account* login()
{
     while(1){
                clrscr();
                int account_id, account_pass;
                printf("Log-in\n\nID: ");
                scanf("%u",&account_id);
                printf("Pass: ");
                scanf("%u",&account_pass);
                
                for(int i =0;i<account_count;i++)
                {
                    if(accounts[i].id == account_id && accounts[i].pass == account_pass)
                    {
                        return &accounts[i];
                    }
                }
                printf("Invalid credentials!\n\n");
                pressReturnToContinue();
                return NULL;
            }
}

Account* login_or_create_acc()
{
    clrscr();
    printf("Welcome to %s\n",BANK_NAME);
    
    char start_choices[MAX_CHOICE_COUNT][MAX_STRING_SIZE] = {"Log-in","Create a new account"};

    int selected = selection(start_choices,2);
    switch(selected)
    {
        case 1:{
            return login();
            break;
               }
        case 2:{
            clrscr();
            if (account_count >= MAX_ACCOUNT_COUNT) {
                printf("All accounts are full\n");
                return NULL; 
            }
            char first_name[MAX_STRING_SIZE];
            char last_name[MAX_STRING_SIZE];
            int pass;
            printf("Account Creation\n\nFirst Name: ");
            scanf("%s", &first_name);
            printf("Last Name: ");
            scanf("%s", &last_name);
            printf("Pass: ");
            scanf("%u", &pass);

            Account new;
            new.id = account_count;
            new.pass = pass;
            
            strncpy(new.first_name, first_name, MAX_STRING_SIZE - 1);
            new.first_name[MAX_STRING_SIZE - 1] = '\0';
            strncpy(new.last_name, last_name, MAX_STRING_SIZE - 1);
            new.last_name[MAX_STRING_SIZE - 1] = '\0';

            new.balance = 0.0;
            new.history_length = 0;

            accounts[account_count] = new;
            account_count++;
            
            clrscr();
            printf("Congrats! Your account was created!\n(IMPORTANT) Account ID: %u\nYou will need this during login each time\n\n",new.id);
            pressReturnToContinue();
            return login();
            break;
               }
        default:

            break;
    }

}

void AddToTransactionHistory(Account* acc,Transaction tr)
{
    if(acc->history_length < MAX_TRANSACTION_HISTORY)
    {
        acc->history[acc->history_length] = tr;
        acc->history_length++;
    }
}

int main()
{
    while(1){
        Account* acc = NULL;
        while(acc == NULL) acc = login_or_create_acc();

        char choices[MAX_CHOICE_COUNT][MAX_STRING_SIZE] = {"Check Balance","Deposit Money","Withdraw Money","View Transaction History","Send Money","Logout"};

        int loggedin = 1;
        double amt;
        while(loggedin)
        {
            clrscr();
            printf("Welcome, %s!\n\nYour Account ident: %u\n\n",acc->first_name,acc->id);

            int choice = selection(choices,6);

            switch (choice)
            {
            case 1:
                clrscr();
                printf("Your Balance on account: $%lf\n",acc->balance);
                pressReturnToContinue();
                break;
            case 2:
                clrscr();
                printf("Enter the amount to deposit ($): ");
                scanf("%lf",&amt);
                if(amt <= 0)
                {
                    printf("Invalid Input!\n");
                    pressReturnToContinue();
                }
                else{
                    acc->balance += amt;

                    Transaction tr = {Deposit,amt};
                    AddToTransactionHistory(acc,tr);

                    printf("$%lf were added to your account!\n",amt);
                    pressReturnToContinue();
                }
                break;
            case 3:
                clrscr();
                printf("Enter the amount to widthdraw ($): ");
                scanf("%lf",&amt);
                if(amt <= 0)
                {
                    printf("Invalid Input!\n");
                    pressReturnToContinue();
                }
                else{
                    if(acc->balance >= amt)
                    {
                        acc->balance -= amt;

                        Transaction tr = {Withdrawal,amt};
                        AddToTransactionHistory(acc,tr);

                        printf("$%lf were withdrawn from your account!\n",amt);
                    }
                    else
                    {
                        printf("Not enough money on account! ($%lf)\n",acc->balance);
                    }
                     pressReturnToContinue();
                }
               
                break;
            case 4:
                clrscr();
                printf("Transaction history\n\n");
                for(int i =acc->history_length-1;i>=0;i--)
                {
                    printf("%s | $%lf\n",transaction_enum_to_string(acc->history[i].type),acc->history[i].value);
                }
                printf("\n");
                pressReturnToContinue();
                break;
            case 5:
                clrscr();
                printf("Send Money\n\n");
                int target_acc;
                double amt;
                printf("Target Account ID: ");
                scanf("%d",&target_acc);
                printf("Amount ($): ");
                scanf("%lf",&amt);

                if(acc->balance >= amt)
                {

                    int success = 0;
                    for(int i =0;i<account_count;i++)
                    {
                        if(accounts[i].id == target_acc)
                        {
                            accounts[i].balance+=amt;
                            acc->balance-=amt;

                            Transaction to;
                            to.type = Recieve;
                            to.value = amt;

                            Transaction from;
                            from.type = Send;
                            from.value = amt;

                            AddToTransactionHistory(&accounts[i],to);
                            AddToTransactionHistory(acc,from);

                            clrscr();
                            printf("\nTransaction Successful!\n");
                            pressReturnToContinue();
                            success = 1;
                            break;
                        }
                    }
                    if(success) break;

                    printf("Account not Found!\n");
                    pressReturnToContinue();
                }
                else
                {
                    printf("Not enough money on account! ($%lf)\n",acc->balance);
                    pressReturnToContinue();
                }
                break;
            case 6:
                loggedin = 0;
                break;
            default:
                break;
            }
        }
    }
}
