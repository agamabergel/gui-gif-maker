#ifndef LINKEDLISTH
#define LINKEDLISTH

#define CV_IGNORE_DEBUG_BUILD_GUARD
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE !FALSE

enum FOUND_FRAME { NOT_FOUND = 0, FOUND };

#define FALSE 0
#define TRUE !FALSE

// Frame struct
typedef struct Frame
{
	char* name;
	unsigned int	duration;
	char* path;
} Frame;


// Link (node) struct
typedef struct FrameNode
{
	Frame* frame;
	struct FrameNode* next;
} FrameNode;


FrameNode* createFrame(char* name, unsigned int	duration, char* path);
void addPhotoNode(FrameNode** head, FrameNode* newFrame);
enum FOUND_FRAME removeFrameNode(FrameNode** head, char* name);
int countFrames(FrameNode** head);
enum FOUND_FRAME changeIndexFrame(FrameNode** head, char* name, int index);
enum FOUND_FRAME changeDurationFrame(FrameNode** head, char* name, unsigned int newDuration);
void changeAllDurationFrames(FrameNode** head, unsigned int newDuration);
void printLL(FrameNode* head);
void freeLinkedList(FrameNode* head);
enum FOUND_FRAME findNameFrame(FrameNode* head, char* name);


#endif