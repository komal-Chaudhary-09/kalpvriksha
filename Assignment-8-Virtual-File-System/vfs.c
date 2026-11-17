#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50

typedef struct FreeBlock {
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode {
    char name[MAX_NAME + 1];
    int isDirectory;
    struct FileNode *parent;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *child;
    int *blockPointers;
    int blockCount;
    int size;
} FileNode;

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];
FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;
int freeCount = 0;
FileNode *root = NULL;
FileNode *cwd = NULL;

void initSystem() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        FreeBlock *b = (FreeBlock *)malloc(sizeof(FreeBlock));
        b->index = i;
        b->next = NULL;
        b->prev = freeTail;
        if (freeHead == NULL) {
            freeHead = b;
        } else {
            freeTail->next = b;
        }
        freeTail = b;
        freeCount++;
    }
    root = (FileNode *)malloc(sizeof(FileNode));
    strcpy(root->name, "/");
    root->isDirectory = 1;
    root->parent = NULL;
    root->next = NULL;
    root->prev = NULL;
    root->child = NULL;
    root->blockPointers = NULL;
    root->blockCount = 0;
    root->size = 0;
    cwd = root;
}

int allocateBlock() {
    if (freeHead == NULL) {
        return -1;
    }
    FreeBlock *temp = freeHead;
    int idx = temp->index;
    freeHead = temp->next;
    if (freeHead != NULL) {
        freeHead->prev = NULL;
    } else {
        freeTail = NULL;
    }
    free(temp);
    freeCount--;
    return idx;
}

void freeBlock(int idx) {
    FreeBlock *b = (FreeBlock *)malloc(sizeof(FreeBlock));
    b->index = idx;
    b->next = NULL;
    b->prev = freeTail;
    if (freeTail == NULL) {
        freeHead = b;
    } else {
        freeTail->next = b;
    }
    freeTail = b;
    freeCount++;
}

FileNode *createNode(char *name, int isDir) {
    FileNode *n = (FileNode *)malloc(sizeof(FileNode));
    strcpy(n->name, name);
    n->isDirectory = isDir;
    n->parent = NULL;
    n->next = NULL;
    n->prev = NULL;
    n->child = NULL;
    n->blockPointers = NULL;
    n->blockCount = 0;
    n->size = 0;
    return n;
}

void insertChild(FileNode *dir, FileNode *node) {
    node->parent = dir;
    if (dir->child == NULL) {
        node->next = node;
        node->prev = node;
        dir->child = node;
    } else {
        FileNode *first = dir->child;
        FileNode *last = first->prev;
        last->next = node;
        node->prev = last;
        node->next = first;
        first->prev = node;
    }
}

void removeChild(FileNode *dir, FileNode *node) {
    if (node->next == node) {
        dir->child = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (dir->child == node) {
            dir->child = node->next;
        }
    }
    node->next = NULL;
    node->prev = NULL;
}

FileNode *findChild(FileNode *dir, char *name) {
    if (dir->child == NULL) {
        return NULL;
    }
    FileNode *temp = dir->child;
    do {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }
        temp = temp->next;
    } while (temp != dir->child);
    return NULL;
}

void releaseFileBlocks(FileNode *file) {
    for (int i = 0; i < file->blockCount; i++) {
        freeBlock(file->blockPointers[i]);
    }
    free(file->blockPointers);
    file->blockPointers = NULL;
    file->blockCount = 0;
    file->size = 0;
}

void freeAll(FileNode *node) {
    while (node->child != NULL) {
        FileNode *c = node->child;
        removeChild(node, c);
        freeAll(c);
    }
    if (node->blockPointers != NULL) {
        free(node->blockPointers);
    }
    free(node);
}

void printPath(FileNode *node) {
    if (node == root) {
        return;
    }
    printPath(node->parent);
    printf("/%s", node->name);
}

void cmdMkdir(char *name) {
    if (strlen(name) == 0 || strlen(name) > MAX_NAME) {
        printf("Invalid name (1 to 50 characters).\n");
        return;
    }
    if (findChild(cwd, name) != NULL) {
        printf("'%s' already exists.\n", name);
        return;
    }
    insertChild(cwd, createNode(name, 1));
    printf("Directory '%s' created successfully.\n", name);
}

void cmdCreate(char *name) {
    if (strlen(name) == 0 || strlen(name) > MAX_NAME) {
        printf("Invalid name (1 to 50 characters).\n");
        return;
    }
    if (findChild(cwd, name) != NULL) {
        printf("'%s' already exists.\n", name);
        return;
    }
    insertChild(cwd, createNode(name, 0));
    printf("File '%s' created successfully.\n", name);
}

void cmdWrite(char *name, char *data) {
    FileNode *file = findChild(cwd, name);
    if (file == NULL || file->isDirectory) {
        printf("File '%s' not found.\n", name);
        return;
    }
    int size = strlen(data);
    int needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (needed == 0) {
        needed = 0;
    }
    int available = freeCount + file->blockCount;
    if (needed > available) {
        printf("Write failed: disk is full.\n");
        return;
    }
    releaseFileBlocks(file);
    if (needed > 0) {
        file->blockPointers = (int *)malloc(needed * sizeof(int));
    }
    for (int i = 0; i < needed; i++) {
        int idx = allocateBlock();
        file->blockPointers[i] = idx;
        int start = i * BLOCK_SIZE;
        int len = size - start;
        if (len > BLOCK_SIZE) {
            len = BLOCK_SIZE;
        }
        memcpy(virtualDisk[idx], data + start, len);
    }
    file->blockCount = needed;
    file->size = size;
    printf("Data written successfully (size=%d bytes).\n", size);
}

void cmdRead(char *name) {
    FileNode *file = findChild(cwd, name);
    if (file == NULL || file->isDirectory) {
        printf("File '%s' not found.\n", name);
        return;
    }
    if (file->size == 0) {
        printf("(empty)\n");
        return;
    }
    int remaining = file->size;
    for (int i = 0; i < file->blockCount; i++) {
        int len = remaining;
        if (len > BLOCK_SIZE) {
            len = BLOCK_SIZE;
        }
        int idx = file->blockPointers[i];
        for (int j = 0; j < len; j++) {
            printf("%c", virtualDisk[idx][j]);
        }
        remaining = remaining - len;
    }
    printf("\n");
}

void cmdDelete(char *name) {
    FileNode *file = findChild(cwd, name);
    if (file == NULL || file->isDirectory) {
        printf("File '%s' not found.\n", name);
        return;
    }
    releaseFileBlocks(file);
    removeChild(cwd, file);
    free(file);
    printf("File deleted successfully.\n");
}

void cmdRmdir(char *name) {
    FileNode *dir = findChild(cwd, name);
    if (dir == NULL || !dir->isDirectory) {
        printf("Directory '%s' not found.\n", name);
        return;
    }
    if (dir->child != NULL) {
        printf("Directory is not empty.\n");
        return;
    }
    removeChild(cwd, dir);
    free(dir);
    printf("Directory removed successfully.\n");
}

void cmdLs() {
    if (cwd->child == NULL) {
        printf("(empty)\n");
        return;
    }
    FileNode *temp = cwd->child;
    do {
        if (temp->isDirectory) {
            printf("%s/\n", temp->name);
        } else {
            printf("%s\n", temp->name);
        }
        temp = temp->next;
    } while (temp != cwd->child);
}

void cmdCd(char *name) {
    if (strcmp(name, "..") == 0) {
        if (cwd->parent != NULL) {
            cwd = cwd->parent;
        }
    } else if (strcmp(name, "/") == 0) {
        cwd = root;
    } else {
        FileNode *dir = findChild(cwd, name);
        if (dir == NULL || !dir->isDirectory) {
            printf("Directory '%s' not found.\n", name);
            return;
        }
        cwd = dir;
    }
    printf("Moved to ");
    if (cwd == root) {
        printf("/");
    } else {
        printPath(cwd);
    }
    printf("\n");
}

void cmdPwd() {
    if (cwd == root) {
        printf("/");
    } else {
        printPath(cwd);
    }
    printf("\n");
}

void cmdDf() {
    int used = NUM_BLOCKS - freeCount;
    double percent = (used * 100.0) / NUM_BLOCKS;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", percent);
}

int main() {
    char line[10000];
    char cmd[20];
    char arg[100];

    initSystem();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    while (1) {
        if (cwd == root) {
            printf("/ > ");
        } else {
            printf("%s > ", cwd->name);
        }
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        line[strcspn(line, "\n")] = '\0';

        cmd[0] = '\0';
        arg[0] = '\0';
        sscanf(line, "%19s %99s", cmd, arg);

        if (strlen(cmd) == 0) {
            continue;
        }

        if (strcmp(cmd, "mkdir") == 0) {
            cmdMkdir(arg);
        } else if (strcmp(cmd, "create") == 0) {
            cmdCreate(arg);
        } else if (strcmp(cmd, "write") == 0) {
            char *first = strchr(line, '"');
            char *last = strrchr(line, '"');
            if (first == NULL || first == last) {
                printf("Usage: write <file> \"text\"\n");
            } else {
                *last = '\0';
                cmdWrite(arg, first + 1);
            }
        } else if (strcmp(cmd, "read") == 0) {
            cmdRead(arg);
        } else if (strcmp(cmd, "delete") == 0) {
            cmdDelete(arg);
        } else if (strcmp(cmd, "rmdir") == 0) {
            cmdRmdir(arg);
        } else if (strcmp(cmd, "ls") == 0) {
            cmdLs();
        } else if (strcmp(cmd, "cd") == 0) {
            cmdCd(arg);
        } else if (strcmp(cmd, "pwd") == 0) {
            cmdPwd();
        } else if (strcmp(cmd, "df") == 0) {
            cmdDf();
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    freeAll(root);
    while (freeHead != NULL) {
        FreeBlock *temp = freeHead;
        freeHead = freeHead->next;
        free(temp);
    }
    printf("Memory released. Exiting program...\n");
    return 0;
}