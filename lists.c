#include "lists.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int asprintf(char **ret, const char *format, ...);

/* Free poll from dynamic memory and delete given pointer to poll */
void free_memory(Poll * p) {
  int i; 
  for (i =0; i < p->num_slots; i++){
    free(p->slot_labels[i]); 
  }
  free(p->slot_labels);

  Participant * part = p->participants;  
  while(part){
    if(part->comment){
       free(part->comment);
    } 
    if(part->availability){
      free(part->availability); 
    }
    Participant * part2 = part->next; 
    free(part); 
    part = part2;
  }
  free(p); 
}

/* Create a poll with this name and num_slots. 
 * Insert it into the list of polls whose head is pointed to by *head_ptr_add
 * Return 0 if successful 1 if a poll by this name already exists in this list.
 */
int insert_new_poll(char *name, int num_slots, Poll **head_ptr_add) {

  if (strlen(name) > MAX_NAME) {
    return 2;
  }

  Poll *poll = (Poll *)malloc(sizeof(Poll));
  if (poll == NULL){
    exit(1); 
  }
  poll->num_slots = num_slots;
  strcpy(poll->name, name);
  poll->slot_labels = NULL;
  poll->next = NULL;
  poll->participants = NULL;


  if (!head_ptr_add) {
    head_ptr_add = &poll;

  } else {
    Poll *iter = *head_ptr_add;

    if (iter) {
      while (iter->next) {
        if (strcmp(iter->name, name) == 0) {
          free_memory(poll);
          return 1; // duplicate name
        }
        iter = iter->next;
      }

      if (strcmp(iter->name, name) == 0) {
        free_memory(poll);
        return 1; // duplicate name
      }
      iter->next = poll;
    } else {
      *head_ptr_add = poll;
    }
    return 0;
  }
  return 0;
}

/* Return a pointer to the poll with this name in
 * this list starting with head. Return NULL if no such poll exists.
 */
Poll *find_poll(char *name, Poll *head) {
  Poll * iter = head; 
  while (iter){
    if (strcmp(name, iter->name) == 0){
      return iter;
    }
    iter = iter->next;
  }
  return NULL;
}

/* 
 *  Print the names of the current polls one per line.
 */
void print_polls(Poll *head) {
  Poll * iter = head; 
  while (iter){
    printf("%s\n", iter->name);
    iter = iter->next;
  }
}



/* Reset the labels for the poll by this poll_name to the strings in the
 * array slot_labels.
 * Return 1 if poll does not exist by this name. 
 * Return 2 if poll by this name does not match number of labels provided
 */
int configure_poll(char *poll_name, char **slot_labels, int num_labels, 
                   Poll *head_ptr) {
  Poll * iter = head_ptr; 
  while(iter){
    if (strcmp(poll_name, iter->name) == 0){

      if (iter->num_slots != num_labels) {
        return 2;
      }
      int i;
      iter->slot_labels = (char **)malloc(sizeof(char *) * num_labels);
      if (iter->slot_labels == NULL){
        exit(1); 
      } 
      for (i=0; i < num_labels ; i++){
        iter->slot_labels[i] = (char *) malloc(sizeof(char)*strlen(slot_labels[i]));
        if (iter->slot_labels == NULL){
        exit(1); 
        }
        strcpy(iter->slot_labels[i], slot_labels[i]);
      }
      return 0; 
    }
    iter = iter->next; 
  }
  return 1; 
}

 
/* Delete the poll by the name poll_name from the list at *head_ptr_add.
 * Update the head of the list as appropriate and free all dynamically 
 * allocated memory no longer used.
 * Return 0 if successful and 1 if poll by this name is not found in list. 
 */
int delete_poll(char *poll_name, Poll **head_ptr_add) {
  
  if (strcmp((*head_ptr_add)->name, poll_name) ==0){
    Poll * temp = *head_ptr_add; 
    head_ptr_add = &temp->next; 
    free_memory(temp);
    return 0;  
  }
  Poll * prev = NULL; 
  Poll * iter = *head_ptr_add; 
  while(iter){
    if (strcmp(iter->name, poll_name) == 0){
      prev->next = iter->next; 
      free_memory(iter);
      return 0;
    }
    prev = iter;
    iter = iter->next;
  
  }
  return 1; 
}

/* Add a participant with this part_name to the participant list for the poll
   with this poll_name in the list at head_pt. Duplicate participant names
   are not allowed. Set the availability of this participant to avail.
   Return: 0 on success 
           1 for poll does not exist with this name
           2 for participant by this name already in this poll
           3 for availibility string is wrong length for this poll. 
             Particpant not added
*/
int add_participant(char *part_name, char *poll_name, Poll *head_ptr, char* avail) {
  Poll * poll = head_ptr; 
  while (poll){
    if (strcmp(poll->name, poll_name) == 0){
      Participant * part = poll->participants; 
      while(part){
        if (strcmp(part->name, part_name)==0){
          return 2;
        }
        part = part->next;
      }

      if (strlen(avail) != poll->num_slots){
        return 3; 
      }
      Participant * newPart = (Participant *)malloc(sizeof(Participant)); 
      if (newPart == NULL){
        exit(1);
      }
      strcpy(newPart->name, part_name); 
      newPart->comment = NULL;
      newPart->availability = (char *)malloc(sizeof(char) * strlen(avail));
      if (newPart->availability == NULL){
        exit(1);
      } 
      strcpy(newPart->availability, avail);
      newPart->next = poll->participants; 
      poll->participants = newPart;
      return 0;   
    }

    poll = poll->next; 
  }

  return 1; 
}


/* Add a comment from the participant with this part_name to the poll with
 * this poll_name. Replace existing comment if one exists. 
 * Return values:
 *                                                                                                                                          
 *    0 on success
 *    1 no poll by this name
 *    2 no participant by this name for this poll
 */
int add_comment(char *part_name, char *poll_name, char *comment, Poll *head_ptr) {
  Poll * poll = head_ptr; 
  while(poll){ 
    if (strcmp(poll_name, poll->name) == 0){
      Participant * part = poll->participants;
      while(part){
        if (strcmp(part->name, part_name) ==0){
          if (part->comment){
            free(part->comment); 
          }
          part->comment = (char *)malloc(sizeof(char)*strlen(comment));
          if (part->comment == NULL){
            exit(1);
          } 
          strcpy(part->comment, comment); 
          return 0; 
        }
        part = part->next;
      }
      return 2;
    }
    poll = poll->next; 
  }
  
  return 1;
}

/* Add availabilty for the participant with this part_name to the poll with
 * this poll_name. Return values:
 *    0 success
 *    1 no poll by this name
 *    2 no participant by this name for this poll
 *    3 avail string is incorrect size for this poll 
 */
int update_availability(char *part_name, char *poll_name, char *avail, 
          Poll *head_ptr) {
  Poll * poll = head_ptr; 
  while(poll){
    if (strcmp(poll_name, poll->name) == 0){
      if (strlen(avail) != poll->num_slots){
        return 3;      
      }
      Participant * part = poll->participants; 
      while (part){
        if (strcmp(part->name, part_name) == 0){
          strcpy(part->availability, avail);
          return 0; 
        }
        part = part->next; 
      }
      return 2;
    }
    poll = poll->next; 
  }
  return 1;
}

/*  Return pointer to participant with this name from this poll or
 *  NULL if no such participant exists.
 */
Participant *find_part(char *name, Poll *poll) {
  Participant * part = poll->participants; 
  while(part){
    if (strcmp(part->name, name) == 0){
      return part; 
    }
    part = part->next;
  }
  // while(poll->participants != NULL){
  //   if (strcmp(poll->participants->name,name)== 0){
  //     return poll->participants;}
  //   poll->participants = poll->participants->next;}
 
  return NULL;
}
    

/* For the poll by the name poll_name from the list at head_ptr,
 * prints the name, number of slots and each label and each participant.
 * For each participant, prints name and availability.
 * Prints the summary of votes returned by the poll_summary function.
 * See assignment handout for formatting example.
 * Return 0 if successful and 1 if poll by this name is not found in list. 
 */
int print_poll_info(char *poll_name, Poll *head_ptr) {
  Poll * poll = head_ptr; 
  while(poll){
    if (strcmp(poll->name, poll_name) ==0){
      printf("%s %d \n", poll->name, poll->num_slots);
      Participant * part = poll->participants; 
      while (part){
        printf ("Participant : %s %s \n", part->name, part->availability);
        if (part->comment){
          printf("  %s \n", part->comment); 
        } 
        part = part->next;
      }

      char * summary = poll_summary(poll);
      printf("%s\n", summary);
      return 0;  
    }
    poll = poll->next;
  }
  return 1;
}


/* Builds and returns a string for this poll that summarizes the answers.
 * Summary information includes the total number of Y,N and M votes
 * for each time slot. See an example in the assignment handout.
 */
char *poll_summary(Poll *poll) { 

  int n = poll->num_slots;
  int yes[n];
  int no[n];
  int maybe[n];
  int i;
  for (i = 0; i < n; i++) {
    yes[i] = 0;
    no[i] = 0;
    maybe[i] = 0;
  }

  Participant *part = poll->participants;
  while (part) {
    for (i = 0; i < n; i++) {
      if (part->availability[i] == 'Y') {
        yes[i]++;
      } else if (part->availability[i] == 'N') {
        no[i]++;
      } else if (part->availability[i] == 'M') {
        maybe[i]++;
      }
    }
    part = part->next;
  }

  char *title = "\nAvailability summary:\n";
  int count =0;
  count += strlen(title); 
  
  for (i = 0; i < n ; i++)
  {
    count += strlen(poll->slot_labels[i]);
    count += 10; 
    count += sizeof(yes[i]);
    count += 3; 
    count += sizeof(no[i]); 
    count += 3; 
    count += sizeof(maybe[i]);
    count += 2;
  }

  char *result = (char *)malloc(sizeof(char) * count); // TODO; find required size first
  if (result == NULL){
    exit(1); 
  }
  strcpy(result, title);

  for (i = 0; i < n; i++) {
    char *line = NULL;
    asprintf(&line, "%s \t Y:%d N:%d M:%d\n", poll->slot_labels[i], yes[i], no[i], maybe[i]);
    strcat(result, line);
  }
  return result;

}
