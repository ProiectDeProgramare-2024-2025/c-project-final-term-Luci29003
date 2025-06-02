#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define MAX_LISTS 10
#define MAX_TASKS 20
#define MAX_TEXT 100
#define FILE_NAME "todolist.txt"

typedef struct {
    char text[MAX_TEXT];
    int completed; // 0 - incomplete, 1 - completed
} Task;

typedef struct {
    char title[MAX_TEXT]; // e.g. "Tasks 01.06.2025"
    Task tasks[MAX_TASKS];
    int taskCount;
} ToDoList;

typedef struct {
    ToDoList lists[MAX_LISTS];
    int listCount;
} AppData;

// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Save data to file
void saveData(AppData *data) {
    FILE *f = fopen(FILE_NAME, "w");
    if (!f) {
        perror("Error saving data");
        return;
    }

    fprintf(f, "%d\n", data->listCount);
    for (int i = 0; i < data->listCount; i++) {
        fprintf(f, "%s\n", data->lists[i].title);
        fprintf(f, "%d\n", data->lists[i].taskCount);
        for (int j = 0; j < data->lists[i].taskCount; j++) {
            fprintf(f, "%d\n%s\n", data->lists[i].tasks[j].completed, data->lists[i].tasks[j].text);
        }
    }

    fclose(f);
}

// Load data from file
void loadData(AppData *data) {
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) {
        return; // nothing to load
    }

    fscanf(f, "%d\n", &data->listCount);
    for (int i = 0; i < data->listCount; i++) {
        fgets(data->lists[i].title, MAX_TEXT, f);
        data->lists[i].title[strcspn(data->lists[i].title, "\n")] = 0;

        fscanf(f, "%d\n", &data->lists[i].taskCount);
        for (int j = 0; j < data->lists[i].taskCount; j++) {
            fscanf(f, "%d\n", &data->lists[i].tasks[j].completed);
            fgets(data->lists[i].tasks[j].text, MAX_TEXT, f);
            data->lists[i].tasks[j].text[strcspn(data->lists[i].tasks[j].text, "\n")] = 0;
        }
    }

    fclose(f);
}

// Show all lists and tasks, with colors
void showAllLists(AppData *data) {
    if (data->listCount == 0) {
        printf("%sNo lists available.%s\n", YELLOW, RESET);
        return;
    }

    for (int i = 0; i < data->listCount; i++) {
        printf("%s-= %s =-%s\n", CYAN, data->lists[i].title, RESET);
        for (int j = 0; j < data->lists[i].taskCount; j++) {
            if (data->lists[i].tasks[j].completed) {
                printf("[%s✔%s] %s%s%s\n", GREEN, RESET, GREEN, data->lists[i].tasks[j].text, RESET);
            } else {
                printf("[%s %s] %s\n", RED, RESET, data->lists[i].tasks[j].text);
            }
        }
        printf("\n");
    }
}

// Add a new list
void addList(AppData *data) {
    if (data->listCount >= MAX_LISTS) {
        printf("%sToo many lists!%s\n", RED, RESET);
        return;
    }

    printf("Enter list title (e.g. Tasks 02.06.2025): ");
    fgets(data->lists[data->listCount].title, MAX_TEXT, stdin);
    data->lists[data->listCount].title[strcspn(data->lists[data->listCount].title, "\n")] = 0;
    data->lists[data->listCount].taskCount = 0;
    data->listCount++;

    printf("%sList created successfully.%s\n", GREEN, RESET);
}

// Add tasks to a list (multiple tasks)
void addTask(AppData *data) {
    if (data->listCount == 0) {
        printf("%sNo lists available. Please create one first.%s\n", YELLOW, RESET);
        return;
    }

    for (int i = 0; i < data->listCount; i++) {
        printf("%d. %s\n", i + 1, data->lists[i].title);
    }

    int listIndex;
    printf("Choose a list: ");
    scanf("%d", &listIndex);
    clearInputBuffer();
    listIndex--;

    if (listIndex < 0 || listIndex >= data->listCount) {
        printf("%sInvalid list. Please try again.%s\n", YELLOW, RESET);
        return;
    }

    ToDoList *list = &data->lists[listIndex];

    printf("Type tasks one by one. Press ENTER on empty line or type 0 to return to menu.\n");

    while (list->taskCount < MAX_TASKS) {
        printf("Task %d: ", list->taskCount + 1);
        char buffer[MAX_TEXT];
        fgets(buffer, MAX_TEXT, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) == 0 || strcmp(buffer, "0") == 0) {
            break;
        }

        strcpy(list->tasks[list->taskCount].text, buffer);
        list->tasks[list->taskCount].completed = 0;
        list->taskCount++;
    }

    if (list->taskCount == MAX_TASKS) {
        printf("%sReached maximum number of tasks for this list.%s\n", YELLOW, RESET);
    }
}

// Mark tasks as done (multiple)
void markTaskDone(AppData *data) {
    if (data->listCount == 0) {
        printf("%sNo lists available.%s\n", YELLOW, RESET);
        return;
    }

    for (int i = 0; i < data->listCount; i++) {
        printf("%d. %s\n", i + 1, data->lists[i].title);
    }

    int listIndex;
    printf("Select a list: ");
    scanf("%d", &listIndex);
    clearInputBuffer();
    listIndex--;

    if (listIndex < 0 || listIndex >= data->listCount) {
        printf("%sInvalid list. Please try again.%s\n", YELLOW, RESET);
        return;
    }

    ToDoList *list = &data->lists[listIndex];

    if (list->taskCount == 0) {
        printf("%sThis list contains no tasks.%s\n", YELLOW, RESET);
        return;
    }

    printf("Tasks in list '%s':\n", list->title);
    for (int i = 0; i < list->taskCount; i++) {
        if (list->tasks[i].completed) {
            printf("%d. [%s✔%s] %s%s%s\n", i + 1, GREEN, RESET, GREEN, list->tasks[i].text, RESET);
        } else {
            printf("%d. [%s %s] %s\n", i + 1, RED, RESET, list->tasks[i].text);
        }
    }

    printf("Enter the numbers of tasks to mark as completed (e.g. 1 3 5). Enter 0 to cancel:\n> ");

    char input[256];
    fgets(input, sizeof(input), stdin);

    if (strcmp(input, "0\n") == 0) return;

    char *token = strtok(input, " ");
    while (token != NULL) {
        int num = atoi(token);
        if (num > 0 && num <= list->taskCount) {
            list->tasks[num - 1].completed = 1;
        }
        token = strtok(NULL, " ");
    }

    // Check if all tasks are done, then delete the list
    int allDone = 1;
    for (int i = 0; i < list->taskCount; i++) {
        if (!list->tasks[i].completed) {
            allDone = 0;
            break;
        }
    }

    if (allDone) {
        printf("%sAll tasks completed. List '%s' will be deleted.%s\n", GREEN, list->title, RESET);
        for (int i = listIndex; i < data->listCount - 1; i++) {
            data->lists[i] = data->lists[i + 1];
        }
        data->listCount--;
    }
}

int main() {
    AppData data = {0};

    // Create file if not exists
    FILE* f = fopen(FILE_NAME, "a");
    if (f) fclose(f);

    loadData(&data);

    int opt;
    do {
        system("clear"); // clear screen on macOS/Linux
        printf("\n%s=== TO-DO MENU ===%s\n", CYAN, RESET);
        printf("1. %sCreate a list%s\n", BOLD, RESET);
        printf("2. %sAdd task to a list%s\n", BOLD, RESET);
        printf("3. %sView lists and tasks%s\n", BOLD, RESET);
        printf("4. %sMark tasks as done%s\n", BOLD, RESET);
        printf("5. %sExit and save%s\n", BOLD, RESET);
        printf("Choose an option: ");
        scanf("%d", &opt);
        clearInputBuffer();

        switch (opt) {
            case 1: addList(&data); break;
            case 2: addTask(&data); break;
            case 3: showAllLists(&data); break;
            case 4: markTaskDone(&data); break;
            case 5: saveData(&data); printf("%sData saved. Goodbye!%s\n", GREEN, RESET); break;
            default: printf("%sInvalid option.%s\n", YELLOW, RESET);
        }

        if (opt != 5) {
            printf("\nPress Enter to continue...");
            getchar();
        }
    } while (opt != 5);

    return 0;
}


