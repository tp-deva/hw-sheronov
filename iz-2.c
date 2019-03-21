#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHANKSIZE 8
#define TRUE      1
#define FALSE     0

typedef struct Node{
  char*        value;
  struct Node* next;
} Node;

void   push(Node** head, char* value);
Node*  pop(Node** head);

typedef struct Node_int{
    int*             value;
    int              count;
    struct Node_int* next;
} Node_int;

void       push_int(Node_int** head, int* value, int count);
Node_int*  pop_int(Node_int** head);

void read_expression(char** expression);
int parse_expression(char** expression, char** answer);
void calculate(char** answer);
int calculate_intersection(int* A, int A_count, int* B, int B_count, int** result);
int calculate_difference(int* A, int A_count, int* B, int B_count, int** result);
int calculate_union(int* A, int A_count, int* B, int B_count, int** result);

int main() {
  char* expression = NULL;
  char* answer = NULL;

  read_expression(&expression);
  if (expression) {
    if (parse_expression(&expression, &answer) < 0) {
      free(answer);
      free(expression);
      printf("[error]");
      return 0;
    }
    calculate(&answer);
    free(answer);
    free(expression);
  }

  return 0;
}

void read_expression(char** expression) {
  char chank[CHANKSIZE];
  int  char_count = 0;
  int is_new_chank = TRUE;

  while (fgets(chank, sizeof(chank), stdin)) {
    if (is_new_chank) {
      *expression = (char*)malloc(sizeof(char));
      is_new_chank = FALSE;
    }
    *expression = realloc((*expression), sizeof(char) * (char_count + strlen(chank) + 1));
    memcpy((*expression) + char_count, chank, strlen(chank) + 1);
    char_count += strlen(chank);
  }

  if (*expression != NULL) {
    for (int i = 0, j = 0; (*expression)[i] != '\0'; i++, j++) {
      if ((*expression)[i] == ' ') {
        j++;
        for (;(*expression)[i] != '\0'; i++, j++) {
          (*expression)[i] = (*expression)[j];
        }
        i = 0;
        j = 0;
      }
    }
  }
}

int parse_expression(char** expression, char** answer) {
  char* exp = *expression;
  *answer = malloc(sizeof(char) * (strlen(exp) + 1));
  *answer[0] = '\0';
  Node* head = NULL;
  int is_open = FALSE;

  for (int i = 0; exp[i] != '\0'; i++) {
    if (exp[i] == '(') {
      push(&head, "(");
      is_open = TRUE;
    }

    if (exp[i] == 'U') {
      if ((head == NULL) || (!strcmp(head->value, "("))) {
        push(&head, "U");
      } else {
        while ((head != NULL) && 
            ((!strcmp(head->value, "U")) || 
             (!strcmp(head->value, "\\")) ||
             (!strcmp(head->value, "^"))))
        {
          Node* reserv_ptr = pop(&head);
          strncat(*answer, reserv_ptr->value, 1);
          free(reserv_ptr->value);
          free(reserv_ptr);
        }
        push(&head, "U");
      }
    }
    if (exp[i] == '\\') {
      if ((head == NULL) || (!strcmp(head->value, "("))) {
        push(&head, "\\");
      } else {
        while ((head != NULL) && 
            ((!strcmp(head->value, "U")) || 
             (!strcmp(head->value, "\\")) ||
             (!strcmp(head->value, "^"))))
        {
          Node* reserv_ptr = pop(&head);
          strncat(*answer, reserv_ptr->value, 1);
          free(reserv_ptr->value);
          free(reserv_ptr);
        }
        push(&head, "\\");
      }
    }
    if (exp[i] == '^') {
      if ((head == NULL) || (strcmp(head->value, "^"))) {
        push(&head, "^");
      } else {
        while ((head != NULL) && 
             (!strcmp(head->value, "^")))
        {
          Node* reserv_ptr = pop(&head);
          strncat(*answer, reserv_ptr->value, 1);
          free(reserv_ptr->value);
          free(reserv_ptr);
        }
        push(&head, "^");
      }
    }
    if (exp[i] == ')') {
      while ((head != NULL) && (strcmp(head->value, "("))) {
        Node* reserv_ptr = pop(&head);
        strncat(*answer, reserv_ptr->value, 1);
        free(reserv_ptr->value);
        free(reserv_ptr);
      }
      Node* reserv_ptr = pop(&head);
      free(reserv_ptr->value);
      free(reserv_ptr);
      is_open = FALSE;
    }
    if (exp[i] == '[') {
      strncat(*answer, strchr(exp, '['), strlen(strchr(exp, '[')) - strlen(strchr(exp, ']')) + 1);
      memmove(exp, strchr(exp, ']') + 1, strlen(strchr(exp, ']')));
      i = -1;
    }
  }
  while (head != NULL) {
    Node* reserv_ptr = pop(&head);
    strncat(*answer, reserv_ptr->value, 1);
    free(reserv_ptr->value);
    free(reserv_ptr);
  }
  if (is_open) {
    return -1;
  }

  return 0;
}

void calculate(char** answer) {
  char* exp = *answer;
  Node_int* head = NULL;

  for (int i = 0; exp[i] != '\0'; i++) {
    if (exp[i] == '^') {
      Node_int* A = pop_int(&head);
      Node_int* B = pop_int(&head);
      int* result = NULL;
      int count = 0;

      count = calculate_intersection(A->value, A->count, B->value, B->count, &result);
      free(A->value);
      free(B->value);
      free(A);
      free(B);
      push_int(&head, result, count);
      free(result);
    }
    if (exp[i] == '\\') {
      Node_int* A = pop_int(&head);
      Node_int* B = pop_int(&head);
      int* result = NULL;
      int count = 0;

      count = calculate_difference(A->value, A->count, B->value, B->count, &result);
      free(A->value);
      free(B->value);
      free(A);
      free(B);
      push_int(&head, result, count);
      free(result);
    }
    if (exp[i] == 'U') {
      Node_int* A = pop_int(&head);
      Node_int* B = pop_int(&head);
      int* result = NULL;
      int count = 0;

      count = calculate_union(A->value, A->count, B->value, B->count, &result);
      free(A->value);
      free(B->value);
      free(A);
      free(B);
      push_int(&head, result, count);
      free(result);
    }
    if (exp[i] == '[') {
      char set[strlen(strchr(exp, '[')) - strlen(strchr(exp, ']')) + 2];
      memcpy(set, strchr(exp, '['), strlen(strchr(exp, '[')) - strlen(strchr(exp, ']') + 1));
      set[strlen(strchr(exp, '[')) - strlen(strchr(exp, ']')) + 1] = '\0';
      int count = 0;
      for (int i = 0; set[i] != '\0'; i++) {
        if (set[i] == ',') {
          count++;
        }
      }
      count++;
      int set_array[count];
      int reserv;
      if (strcmp(set, "[]") == 0) {
        set_array[0] = -1;
      } else {
        for (int i = 0, j = 0; i < count; j++) { 
          if (sscanf(set + j, "%d", &reserv)) {
            set_array[i] = reserv;
            for (; (set[j] != ',') && (set[j] != '\0'); j++){}
            i++;
          }
        }
      }
      push_int(&head, set_array, count);
      memmove(exp, strchr(exp, ']') + 1, strlen(strchr(exp, ']')));
      i = -1;
    }
  }

  Node_int* A = pop_int(&head);
  if (A != NULL) {
    printf("[");
    for(int i = 0 ; i < A->count - 1; i++) { 
     for(int j = 0 ; j < A->count - i - 1 ; j++) {  
         if(A->value[j] > A->value[j+1]) {           
            int tmp = A->value[j];
            A->value[j] = A->value[j+1];
            A->value[j+1] = tmp; 
         }
      }
    }
    if (A->value[0] != -1) {
      printf("%d", A->value[0]);
      for (int i = 1; i < A->count; i++) {
        printf(",%d", A->value[i]);
      }
    }
    printf("]");
    free(A->value);
    free(A);
  }

}

int calculate_intersection(int* A, int A_count, int* B, int B_count, int** result) {
  int count = 0;

  for (int i = 0; i < A_count; i++) {
    for (int j = 0; j < B_count; j++) {
      if (A[i] == B[j]) {
        count++;
        *result = (int*)realloc(*result, count * sizeof(int));
        (*result)[count - 1] = A[i];
      }
    }
  }  
  if (count == 0) {
    *result = (int*)malloc(sizeof(int));
    (*result)[0] = -1;
    count++;
  }

  return count;
}

int calculate_difference(int* A, int A_count, int* B, int B_count, int** result) { // B=12345;A=pust
  int count = 0;

  for (int i = 0; i < A_count; i++) {
    for (int j = 0; j < B_count; j++) {
      if (A[i] == B[j]) {
        count++;
        B[j] = -1;
      }
    }
  }
  if (((count == 0) && (B[0] == -1)) || (count == B_count)) {
    *result = (int*)malloc(sizeof(int));
    (*result)[0] = -1;
    count++;
    return 1;
  } else {
    *result = (int*)malloc((B_count - count) * sizeof(int));
    for (int i = 0, j = 0; i < B_count; i++) {
      if (B[i] != -1) {
        (*result)[j] = B[i];
        j++;
      } 
    }
  }

  return B_count - count;
}

int calculate_union(int* A, int A_count, int* B, int B_count, int** result) {
  int count = 0;

  for (int i = 0; i < A_count; i++) {
    for (int j = 0; j < B_count; j++) {
      if (A[i] == B[j]) {
        count++;
        *result = (int*)realloc(*result, count * sizeof(int));
        (*result)[count - 1] = A[i];
        A[i] = -1;
        B[j] = -1;
      }
    }
  }

  for (int i = 0; i < A_count; i++) {
    if (A[i] != -1) {
      count++;
      *result = (int*)realloc(*result, count * sizeof(int));
      (*result)[count - 1] = A[i];
    }
  }

  for (int i = 0; i < B_count; i++) {
    if (B[i] != -1) {
      count++;
      *result = (int*)realloc(*result, count * sizeof(int));
      (*result)[count - 1] = B[i];
    }
  }

  if (count == 0) {
    *result = (int*)malloc(sizeof(int));
    (*result)[0] = -1;
    count++;
  }

  return count;
}

void push(Node** head, char* value) {
  Node* p = (Node*)malloc(sizeof(Node));
  p->next = *head;
  p->value = (char*)malloc(sizeof(char) * (strlen(value) + 1));
  memcpy(p->value, value, strlen(value) + 1);
  *head = p;
}

Node* pop(Node** head) {
  Node* out = *head;
  *head = (*head)->next;

  return out;
}

void push_int(Node_int** head, int* value, int count) {
  Node_int* p = (Node_int*)malloc(sizeof(Node_int));
  p->next = *head;
  p->value = (int*)malloc(sizeof(int) * count);
  p->count = count;
  for (int i = 0; i < count; i++) {
      p->value[i] = value[i];
  }
  *head = p;
}

Node_int* pop_int(Node_int** head) {
  Node_int* out = *head;
  *head = (*head)->next;

  return out;
}
