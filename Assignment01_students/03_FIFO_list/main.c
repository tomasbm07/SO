#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contacto{
	int num;
	char nome[50];
} contacto;

typedef struct Node{
	contacto data;
	struct Node *next;
} node;


//Head of the Linked List
node *head;


/***************************************************************************/
void init(void);
void addContacto(contacto c);
contacto removeContacto();
void printList(void);
void displayMenu(void);
/***************************************************************************/

void init(void){
	head = NULL;
}

void addContacto(contacto c){
	//alocate space for new node
	node *aux = (node *)malloc(sizeof(node));

	//insert at start of queue
	aux->data = c;
	aux->next = head;
	head = aux;
}

contacto removeContacto(){
	node *prox, *anterior;
	contacto c = {0, ""};

	if(head == NULL){
		printf("List is empty!\n");
		return c;
	}

	anterior = NULL;
	//search for the last item
	for(prox = head; prox->next != NULL; prox = prox->next){
		anterior = prox;
	}
		
		if (anterior == NULL){ //Last item in the list -> list is going to be empty
			c = head->data;
			free(head);
			head = NULL;
		} else { //There's more items in queue
			anterior->next = NULL;
			c = prox->data;
			free(prox);
		}
	return c;
}

void printList(){
	if(head == NULL){
		printf("List is empty!\n");
		return;
	}

	while (head != NULL){
		printf("%s, %d\n", head->data.nome, head->data.num);
		head = head->next;
	}
}

void displayMenu(){
	printf("1 - Adicionar contacto\n");
	printf("2 - Remover contacto\n");
	printf("3 - Print List\n");
	printf("0 - Save and Exit\n");
	printf("Choice: ");
}

int main(){
	printf("FIFO Linked List - 'first in first out'\n");
	init();
	int run = 1;
	contacto c;

	while (run != 0){
		displayMenu();
		int choice;
		scanf("%d", &choice);

		switch (choice){
		case 1:
			printf("Nome: ");
			//gets(c.nome);
			char aux[50] = "";
			scanf("%50[^\n]", aux);
			printf("\nNumero: ");
			scanf("%d", &c.num);
			//strcpy(c.nome, aux);
			addContacto(c);
			break;

		case 2:
			c = removeContacto();
			if (strcmp(c.nome, "") != 0)
				printf("Foi removido o contacto de %s\n", c.nome);
			break;

		case 3:
			printList();
			break;

		case 0:
			run = 0;
			break;

		default:
			printf("Numero Invalido\n");
		}
	}
}
