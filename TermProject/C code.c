#include "DebugHdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MAX_LINES 100
#define LINE_LENGTH 80

int maxLines, maxCols;   
char **displayBuffer;    
int lineCount = 0;       
int currentLine = 0;     

typedef struct buf {
    char line[LINE_LENGTH];
    struct buf *next;
} Buffer;

Buffer *head = NULL;

void displayProgram() {
    clear();

    Buffer *current = head;
    int lineCount = 0;
    while (current != NULL) {
        mvprintw(lineCount, 0, "%s", current->line);
        current = current->next;
        lineCount++;
    }

    refresh();
}



void insertDirective() {
    // Prompt the user for input
    mvprintw(lineCount, 0, "Insert @var or @traces followed by a variable name: ");
    refresh();

    char userInput[LINE_LENGTH];
    mvgetstr(lineCount, strlen("Insert @var or @traces followed by a variable name: "), userInput);

    // Determine the prefix based on user input
    const char *prefix;
    if (strncmp(userInput, "@var ", 5) == 0 || strncmp(userInput, "@trace ", 7) == 0) {
        prefix = userInput;
    } else {
        mvprintw(lineCount + 1, 0, "Invalid input. Please use @var or @trace followed by a variable name.");
        refresh();
        return;
    }

    // Create a new line with the directive
    char *newLine = malloc(strlen(prefix) + 1);
    strcpy(newLine, prefix);

    // Find the last node in the buffer
    Buffer *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Create a new node for the modified line
    Buffer *newNode = (Buffer *)malloc(sizeof(Buffer));
    if (!newNode) {
        perror("Error allocating memory");
        return;
    }

    // Copy the new line to the new node
    strncpy(newNode->line, newLine, LINE_LENGTH);
    newNode->next = NULL;  // The new node will be added at the end, so its next is NULL

    // Add the new node to the end of the buffer
    current->next = newNode;

    // Refresh the display
    displayProgram();
}


void read_to_buffer(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    Buffer *current, *newNode;
    char line[LINE_LENGTH];
    while (fgets(line, sizeof(line), file) && (!head || head && lineCount < MAX_LINES)) {
        newNode = (Buffer *)malloc(sizeof(Buffer));
        if (!newNode) {
            perror("Error allocating memory");
            return;
        }
        strncpy(newNode->line, line, LINE_LENGTH);
        newNode->next = NULL;

        if (!head) {
            head = newNode;
            current = head;
        } else {
            current->next = newNode;
            current = newNode;
        }

        lineCount++;  // Increment lineCount after each line is read
    }

    fclose(file);
}



void add_to_buffer(int lineNum, const char *directive) {
    Buffer *current = head;
    int count = 0;

    while (current != NULL && count < lineNum) {
        count++;
        current = current->next;
    }

    if (current) {
        Buffer *newNode = (Buffer *)malloc(sizeof(Buffer));
        if (!newNode) {
            perror("Error allocating memory");
            return;
        }
        strncpy(newNode->line, current->line, LINE_LENGTH);
        newNode->next = NULL; 

        snprintf(current->line, LINE_LENGTH, "%s%s", directive, current->line);

        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}



void write_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    Buffer *current = head;
    while (current != NULL) {
        fputs(current->line, file);
        current = current->next;
    }

    fclose(file);
}



int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);
    
    read_to_buffer("./myprog.c");

    int ch;
    while ((ch = getch()) != 'x') {
        switch (ch) {
            case 'd':
                displayProgram();
                move(0, 0);
                break;
            case 'w':
                if (currentLine > 0) {
                    currentLine--;
                    move(currentLine, 0);
                }
                break;
            case 's':
                if (currentLine < lineCount - 1) {
                    currentLine++;
                    move(currentLine, 0);
                }
                break;
            case 'i':
                echo();
                insertDirective();
                noecho();
                break;
        }
        refresh();
    }
    endwin();
    
    write_to_file("./temp.txt");

    return 0;
}


