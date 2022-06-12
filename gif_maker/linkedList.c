#include "linkedList.h"

/*
* This function will create new frame node to add to the list
* input: name, duration, path
* output: new FrameNode
*/
FrameNode* createFrame(char* name, unsigned int	duration, char* path)
{
	Frame* newFrame = (Frame*)malloc(sizeof(Frame));
	newFrame->name = malloc(sizeof(char) * strlen(name) + 1);
	strncpy(newFrame->name, name,strlen(name) + 1);

	newFrame->duration = duration;

	newFrame->path = malloc(sizeof(char) * strlen(path) + 1);
	strncpy(newFrame->path, path, strlen(path) + 1);

	FrameNode* newFrameNode = (FrameNode*)malloc(sizeof(FrameNode));
	newFrameNode->frame = newFrame;
	newFrameNode->next = NULL;

	return newFrameNode;
}
/*
* This function will add the frames to the head
* input: head frame, new frame
* output: none
*/
void addPhotoNode(FrameNode** head, FrameNode* newFrame)
{
	if (!*head)
	{
		*head = newFrame;
	}
	else
	{
		FrameNode* copyHead = *head;
		while (copyHead->next)
		{
			copyHead = copyHead->next;
		}
		copyHead->next = newFrame;
	}
}


/*
* This function will count how many nodes are in the linked list
* input: head
* output: num of nodes
*/
int countFrames(FrameNode** head)
{
	int count = 0;
	FrameNode* copyHead = *head;

	while (copyHead)
	{
		copyHead = copyHead->next;
		count++;
	}
	return count;
}


/*
* This function checks if a frame name already exitst.
* input: head
* output: found or not
*/
enum FOUND_FRAME findNameFrame(FrameNode* head, char* name)
{
	FrameNode* tmp = head;
	while (tmp)
	{
		if (!strcmp(name, tmp->frame->name))
			return FOUND;

		tmp = tmp->next;
	}
	return NOT_FOUND;
}

/*
* This function will remove a frame by the name of it
* input: head frame, name to remove
* output: if the frame found or not
*/
enum FOUND_FRAME removeFrameNode(FrameNode** head, char* name)
{
	FrameNode* tmp = *head, *prev = NULL;
	int foundNode = FALSE;

	while (tmp)
	{
		if (!strcmp(tmp->frame->name, name))  // finding if the input name exists in the list
		{
			foundNode = TRUE;
			break;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	if (foundNode) // if the name does exists
	{
		if (tmp == *head) // if the memory of the head equals to tmp
		{
			*head = (*head)->next;
		}
		else
		{
			prev->next = tmp->next;  
		}
		free(tmp);
	}

	return foundNode;
}

/*
* This function will change frame index
* input: head, new index
* output: trur/false if the frame founded
*/
enum FOUND_FRAME changeIndexFrame(FrameNode** head, char* name, int index)
{
	FrameNode* dstFrame = NULL, * prev = NULL, * curr = *head, *copyFrameCurr = NULL;
	int counter = 1, flag = NOT_FOUND, framesCount = countFrames(head);

	if ( index > framesCount || index < 1)
	{
		return NOT_FOUND;
	}
	else
	{
		copyFrameCurr = createFrame(name, curr->frame->duration, curr->frame->path); // creating new copy node  


		flag = removeFrameNode(head, name); // removing the node and checking if it exists
		
		if (flag == FOUND)
		{
			FrameNode* copyHead = *head;
			while (copyHead && (index != counter)) // finding the destination node by the index
			{
				++counter;
				dstFrame = copyHead;
				copyHead = copyHead->next;
			}
			if (counter == 1) // if the destination node is the head, change the destination to be the head
			{
				copyFrameCurr->next = *head;
				*head = copyFrameCurr;
			}
			else // else change the nodes
			{
				dstFrame->next = copyFrameCurr;
				copyFrameCurr->next = copyHead;
			}
		}
	}
	return flag; // returns if the name\index exists
}

/*
* This function will change a 1 frame duration
* input: head and name of the frame
* output: true\false if the change was made
*/
enum FOUND_FRAME changeDurationFrame(FrameNode** head, char* name, unsigned int newDuration)
{
	FrameNode* copyHead = *head;
	int flag = NOT_FOUND;

	while (copyHead)
	{
		if (!strcmp(copyHead->frame->name, name))
		{
			flag = FOUND;
			break;
		}
		copyHead = copyHead->next;
	}
	if (flag == FOUND)
	{
		copyHead->frame->duration = newDuration;
		return FOUND;
	}
	return NOT_FOUND;
}


/*
* This function changes all frames duration
* input: head, newDuration
* output: none
*/
void changeAllDurationFrames(FrameNode** head, unsigned int newDuration)
{
	FrameNode* copyHead = *head;
	while (copyHead)
	{
		copyHead->frame->duration = newDuration;
		copyHead = copyHead->next;
	}
}

/*
* This function prints the frames in the linked list with thier info
* input: head
* output: none
*/
void printLL(FrameNode* head)
{
	FrameNode* cpyHead = head;
	while (cpyHead)
	{
		printf("name: %s, path: %s, duration: %d\n", cpyHead->frame->name, cpyHead->frame->path, cpyHead->frame->duration);
		cpyHead = cpyHead->next;
	}
}
/*
* This function free the memory for the linked list *recursively*
* @parm head - the head of the linked list
* @output: none
*/
void freeLinkedList(FrameNode* head)
{
	FrameNode* tmp = NULL;
	while (head)
	{
		tmp = head;
		head = head->next;
		free(tmp->frame->name);
		free(tmp->frame->path);
		free(tmp);
	}
}

