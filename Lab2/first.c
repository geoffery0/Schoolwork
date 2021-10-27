#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct listelement
{
struct listelement *next, *prev;
char text[1000];
}listelement;



void push(listelement **head){
    char str[1000];
    listelement *newnode = (listelement*)malloc(sizeof(listelement));
    listelement *temp;
    printf("Insert text: ");
    scanf("%s", str);
    newnode->next = NULL;
    newnode->prev = NULL;
    strcpy(newnode->text,str);
    if (*head == NULL)
    {
        *head = newnode;
    
    }else{
        temp = *head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newnode;
        temp->next->prev = temp;
        temp = temp->next;
        temp->next = NULL;


        strcpy(temp->text,str);
    }
    printf("done push string\n");

}

void printlist(listelement *head){
    while(head != NULL){
        printf("%s",head->text);
        printf("\n");
        head = head->next;
    }
}

int delete(listelement **head){
    int check;
    listelement *temp;
    scanf("%d", &check);
    if (check ==1 && *head !=NULL)
    {
        if((*head)->next ==NULL){
            free(*head);
            return 1;
        }else{
            (*head)->next->prev = NULL;
            temp = (*head)->next;
            free(*head);
            *head = temp;
            return 1;
        }
    }else{
        temp = *head;
        for (int i = 1; i < check; i++)
        {
            if (temp->next == NULL)
                return 0;
            temp = temp->next;
        }
        if(temp->next !=NULL)
            temp->next->prev = temp->prev;
        temp->prev->next = temp->next;
        free(temp);
        return 1;
        
    }
    
}

void end(listelement *head){
    listelement *temp;
    while(head != NULL){
        temp = head->next;
        free(head);
        head = temp;
    }
}

listelement *head = NULL;

void main()
{

int check;

while(1){
    printf("1 push string\n");
    printf("2 print list\n");
    printf("3 delete item\n");
    printf("4 end program\n");
    scanf("%d",&check);
    if(check ==1)
        push(&head);
    if(check == 2)
        printlist(head);
    if(check == 3)
        delete(&head);
    if(check == 4){
        end(head);
        break;
        }
    }
}