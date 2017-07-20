/**
 *  Author: Simon Redman
 *  Github: www.github.com/Sompom
 *  Date:   20 July 2017
 *
 *  Description: These methods and test code are examples of indirect pointer
 *      manipulation on linked lists as described here:
 *      https://grisha.org/blog/2013/04/02/linus-on-understanding-pointers/
 *
 * The key point about the line count is not runtime, but that the indirect
 * implementation has less room for bugs by removing one special case the
 * programmer could forget or could mess up
 * (Though there is a very tiny runtime speedup in the indirect case)
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list_entry
{
  int value;
  struct list_entry* next;
} list_entry_t;

/**
 * Append to the global list by an indirect pointer
 *
 * Number of C code lines: 7
 * Number of assembly instructions (GCC v4.4.7, -O2): 24
 *
 * @param value The value to apppend
 * @param indirect_start Pointer to the head of the list to append to
 */ 
void append_indirect(int value, list_entry_t** indirect_start)
{
  list_entry_t** indirect;
  list_entry_t* new;

  indirect = indirect_start;

  new = malloc(1 * sizeof(list_entry_t));
  new->value = value;
  new->next = NULL;

  while (!(*indirect == NULL))
  {
    indirect = &((*indirect)->next);
  };

  *indirect = new;
}

/**
 * Append to the list using a direct pointer
 *
 * Note that direct_start is itself an indirect pointer because otherwise
 * we would have to play annoying games with return values
 *
 * Number of C code lines: 11
 * Number of assembly instructions (GCC v4.4.7, -O2): 30
 *
 * @param value The value to append
 * @param direct_start head of the list to append to
 */
void append_direct(int value, list_entry_t** direct_start)
{
  list_entry_t* direct;
  list_entry_t* prev;
  list_entry_t* new;

  direct = *direct_start;
  prev = NULL;

  new = malloc(1 * sizeof(list_entry_t));
  new->value = value;
  new->next = NULL;

  while (!(direct == NULL))
  {
    prev = direct;
    direct = direct->next; 
  }

  if (prev == NULL)
  {
    *direct_start = new;
  } else
  {
    prev->next = new;
  }
}

/**
 *  Use a function pointer to indirect the call to append
 *  This could also be an inline function
 */
void(*append)(int, void*) = (void*)append_indirect;

/**
 * Simple main method to test that the list is sensible
 */
int main (int argc, char** argv)
{
  size_t index;
  size_t iteration;
  int num_itr;
  int output;
  list_entry_t* list;
  list_entry_t* list_unordered;

  if (argc < 3)
  {
    printf("Usage:\n");
    printf("First argument: String to insert into the linked list");
    printf("Second argument: Number of test iterations to run");
    printf("Third argument: If exists, silence output");
    return -1;
  }

  num_itr = atoi(argv[2]);
  if (num_itr < 1)
  {
    printf("Number of iterations must be an integer greater than 0");
    return -1;
  }

  output = !(argc > 3);

  if (output)
  {
    if (append == (void*)append_indirect)
    {
      printf("Using indirect append\n");
    } else if (append == (void*)append_direct)
    {
      printf("Using direct append\n");
    }
  }

  for (iteration = 0; iteration < num_itr; iteration ++)
  {
    list_unordered = NULL;

    for (index = 0; index < strlen(argv[1]); index ++)
    {
      append(argv[1][index] - '0', &list_unordered);
    }

    list = list_unordered;

    while (!(list == NULL))
    {
      list_entry_t* prev;
      if (output)
      {
        printf("Iteration: %d, %p: Value: %d, Next: %p\n", iteration, list, list->value, list->next);
      }
      prev = list;
      list = list->next;
      free(prev);
    }
  }

  return 0;
}
