/*
 ============================================================================
 Name        : OperatingSystem.c
 Author      : Hai Pham
 Version     :
 Copyright   : (c)
 Description : Page Fault in C, Ansi-style - Identify Number of Page Fault 
				using FIFO and LRU algorithms
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


const int ARRAY_SIZE = 256;


char *alg = NULL;
int * numbers = NULL;
int num_frame=0;
int size = 0;
int fault = 0;


/* ============================================================================
 * THIS IS THE IMPLEMENTATION OF DOUBLE LINKED LIST FOR LRU
 * ============================================================================
 */
struct node {
	int data;
	struct node * prev;
	struct node * next;
};

struct dlist {
	int num_elements;
	struct node* head;
	//struct node* tail;
};

//Initialize the list
struct dlist * init_list() {
	struct dlist * new_list = (struct dlist *)malloc(sizeof(struct dlist));
	new_list -> head = NULL;
	//new_list -> tail = NULL;
	new_list -> num_elements = 0;

	return new_list;
}

//Search an integer number in the list
struct node * search(struct dlist* this, int x) {
	struct node * current = this -> head;

	if (!current) return NULL;

	while (x != current -> data) {
		if (current -> next == NULL) return NULL;
			current = current -> next;
	}

	return current;
}

// Retreve the last element on the list
struct node * last(struct dlist* this) {
	struct node * current = this -> head;

	if (!current) return NULL;

	while (current -> next != NULL) {
		current = current -> next;
	}

	return current;
}


//Insert a new number into the list
void push(struct dlist* this, int x) {

	struct node* new = (struct node*)malloc(sizeof(struct node));

	//solve the new node
	new -> data = x;
	new -> prev = NULL;
	new -> next = this -> head;


	//solve the node pointed by list
	//only prev pointer affected
	if (this -> head != NULL) {
		this -> head -> prev = new;
	}

	//solve the list itself
	//only head pointer and num_elements affected
	this -> head = new;

	this -> num_elements++;
}

// Pop a number from a list
void pop(struct dlist* this, int x) {
	struct node * remove_item = search(this, x);

	if (remove_item == NULL) return;
	//solve the previous element
	//forward pointer -->
	if (remove_item -> prev != NULL) {
		remove_item -> prev -> next = remove_item -> next;
	} else {
		this -> head = remove_item -> next;
	}

	//backward pointer <--
	if (remove_item -> next != NULL) {
		remove_item -> next -> prev = remove_item -> prev;
	}

	//decrement the num_elements
	this -> num_elements--;
	free(remove_item);
}

//Print list
void print_list(struct dlist* this) {
	struct node *item = this -> head;

    printf("\tHead -> ");
	while (item) {
		printf("%d : ", item -> data);
		item = item -> next;
	}
	printf("NIL\n");
}
//============================================================================

/*
 * Read from input file line by line and
 * return an integer array
 */
int * process_file(FILE* file) {
	char line[10];
	int * nums = malloc(sizeof(int) * ARRAY_SIZE);
	int i=0;

	while(fgets(line, sizeof(line), file) != NULL) {
	    	sscanf (line, "%d", &nums[i]);
	    	printf ("%d\n", nums[i]);
	    	i++;
	}
	fclose(file);
	size = i;

	return nums;
}


/*
* LRU Implementation
* Using stack as double linked list
* Without circular tail
*/
int lru(int num_frame, int size, int * nums) {
	int i, count = 0;
	struct dlist* memstack = init_list();
	struct node* tempnode = (struct node*)malloc(sizeof(struct node));
	struct node* lastnode = (struct node*)malloc(sizeof(struct node));

/*
	for (i = 0; i < size; i++) {
		printf("%d\n", nums[i]);
	}
*/

	for (i = 0; i < size; i++) {

        printf("\ni = %d, currently processing: %d\n", i, nums[i]);

        // Scan thru the linked list
        // if meet then pop this number and push the equal value into
        // head of list
        tempnode = search(memstack, nums[i]);
        if (tempnode != NULL) {
            printf("MATCHED: %d. List before change is: ", nums[i]);
            print_list(memstack);

            //pop matched item
            // push it on top of the mem_list
            pop(memstack, tempnode -> data);
            push(memstack, nums[i]);

            printf("And changed to: ");
            print_list(memstack);
        } else

        // Else: check on whether there's available frame
        // if yes then push into head
        if (memstack -> num_elements < num_frame) {
            push(memstack, nums[i]);
            count++;
            printf("Fault, still available slot. ");
            printf("Push %d, num_elements = %d\n", nums[i], memstack -> num_elements);
            printf("List inserted is: ");
            print_list(memstack);
        }

        // else: no avail frame + no meet the number
        // pop the last item
        // push to the top
        else {
                lastnode = last(memstack);
                printf("Fault, frame is *FULL*. ");
                printf("Pop %d and Push %d, num_elements = %d\n", lastnode -> data, nums[i], memstack -> num_elements);

                //pop last node and push back on top
                pop(memstack, lastnode -> data);
                push(memstack, nums[i]);
                count++;

                printf("List updated is: ");
                print_list(memstack);
        }
    }
    free(memstack);
    free(tempnode);
    free(lastnode);
	return count;
}


/*
* FIFO Implementation
*/
int fifo(int num_frame, int size, int * nums) {
	int i, count = 0;
	struct dlist* memstack = init_list();
	struct node* tempnode = (struct node*)malloc(sizeof(struct node));
	struct node* lastnode = (struct node*)malloc(sizeof(struct node));

/*
	for (i = 0; i < size; i++) {
		printf("%d\n", nums[i]);
	}
*/

	for (i = 0; i < size; i++) {

        printf("\ni = %d, currently processing: %d\n", i, nums[i]);

        // Scan thru the linked list
        // if meet then do nothing
        tempnode = search(memstack, nums[i]);
        if (tempnode != NULL) {
            printf("MATCHED: %d. List is: ", nums[i]);
            print_list(memstack);

        } else

        // Else: check on whether there's available frame
        // if yes then push into head
        if (memstack -> num_elements < num_frame) {
            push(memstack, nums[i]);
            count++;
            printf("Fault, still available slot. ");
            printf("Push %d, num_elements = %d\n", nums[i], memstack -> num_elements);
            printf("List inserted is: ");
            print_list(memstack);
        }

        // else: no avail frame + no meet the number
        // pop the last item
        // push new element to the head
        else {
                lastnode = last(memstack);
                printf("Fault, frame is *FULL*. ");
                printf("Pop %d and Push %d, num_elements = %d\n", lastnode -> data, nums[i], memstack -> num_elements);

                //pop last node and push back on top
                pop(memstack, lastnode -> data);
                push(memstack, nums[i]);
                count++;

                printf("List updated is: ");
                print_list(memstack);
        }
    }
    free(memstack);
    free(tempnode);
    free(lastnode);
	return count;
}

/*============================================
 * MAIN PROGRAM
 =============================================
 */
int main(int argc, char *argv[]) {



	FILE *myFile = NULL;
	char * algorithm = NULL;


	/*
	 * DEAL WITH INPUT Parameters
	 */
	if(argc == 4){
		// read 1st parameter and check
		algorithm = argv[1];
		printf("Algorithm input is %s\n", algorithm);

		if (strcmp(algorithm, "LRU")==0 || strcmp(algorithm, "FIFO")==0) {

			// continue to read 2nd parameter
			myFile = fopen(argv[2], "rt");
	        if (myFile == NULL){
	            printf("Problem opening file %s, make sure correct "
	            			"path name is given.\n", argv[1]);
	            printf("Exiting now...\n");
	            return 0;
	        } else {
	        	// advance with 3rd parameter
	        	num_frame = atoi(argv[3]);
	        	if (num_frame <= 0 || num_frame > 7) {
	        		printf("Error on 3rd argument, must be either in [1,2,..,7]\n");
	        		printf("Exiting now...\n");
	        		return -1;
	        	}
	        }

		} else {
			printf("Error on input, please input either "
						"FIFO or LRU as first parameter\n");
			printf("Exiting now...\n");
			return -1;
        }
    }
    else {
        //myFile = stdin;
    	printf("Problem with input arguments, make sure correct "
    	            		"3 args: FIFO/LRT filename numberofpage\n");
    	printf("Exiting now...\n");
    	return -1;
    }

    /* Read each line in the file and
     * convert it into integers
     */
	numbers = process_file(myFile);

    //Double check the read-in file
    printf("Algorithm input is %s\n", algorithm);
    printf("Array size is %d\n", size);
    printf("Number of page is %d\n", num_frame);


    /*
     * Process the logic program
     */
    if (strcmp(algorithm, "LRU")==0) {
    	fault = lru(num_frame, size, numbers);
    } else
    if (strcmp(algorithm, "FIFO")==0) {
        fault = fifo(num_frame, size, numbers);
    }

    printf("\n\n\tNumber of page fault is %d\n", fault);


    /*
     * Free out memory and exit
     */
    free(numbers);
    free(alg);
    return 0;


}
