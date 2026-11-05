#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

struct MemoryBlock {
    int startAddress;
    int blockSize;
    int allocationId;      
    int isFree;       
};

char *memoryPool;
struct MemoryBlock *blockArray;
int blockCount = 0;
int blockCapacity = 4;
int totalPoolSize;
int nextAllocationId = 1;
int allocationStrategy = 1;   

int readInteger()
{
    char input[100];
    char *end;
    long value;

    if (fgets(input, sizeof(input), stdin) == NULL)
        exit(0);

    if (strchr(input, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
        return -1;
    }

    errno = 0;
    value = strtol(input, &end, 10);

    while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        end++;

    if (input == end || *end != '\0' || errno == ERANGE ||
        value < INT_MIN || value > INT_MAX)
        return -1;

    return (int)value;
}

int ensureBlockCapacity()
{
    if (blockCount < blockCapacity)
        return 1;
    struct MemoryBlock *temp = realloc(blockArray, blockCapacity * 2 * sizeof(struct MemoryBlock));
    if (temp == NULL)
        return 0;
    blockArray = temp;
    blockCapacity = blockCapacity * 2;
    return 1;
}

void insertBlock(int pos, int startAddress, int blockSize, int allocationId, int isFree)
{
    for (int i = blockCount; i > pos; i--)
        blockArray[i] = blockArray[i - 1];
    blockArray[pos].startAddress = startAddress;
    blockArray[pos].blockSize = blockSize;
    blockArray[pos].allocationId = allocationId;
    blockArray[pos].isFree = isFree;
    blockCount++;
}

void deleteBlock(int pos)
{
    for (int i = pos; i < blockCount - 1; i++)
        blockArray[i] = blockArray[i + 1];
    blockCount--;
}

void mergeAdjacentFreeBlocks()
{
    int i = 0;
    while (i < blockCount - 1) {
        if (blockArray[i].isFree && blockArray[i + 1].isFree) {
            blockArray[i].blockSize += blockArray[i + 1].blockSize;
            deleteBlock(i + 1);
        } else {
            i++;
        }
    }
}

int findFirstFit(int blockSize)
{
    for (int i = 0; i < blockCount; i++) {
        if (blockArray[i].isFree && blockArray[i].blockSize >= blockSize)
            return i;
    }
    return -1;
}

int findBestFit(int blockSize)
{
    int best = -1;
    for (int i = 0; i < blockCount; i++) {
        if (blockArray[i].isFree && blockArray[i].blockSize >= blockSize) {
            if (best == -1 || blockArray[i].blockSize < blockArray[best].blockSize)
                best = i;
        }
    }
    return best;
}

int findWorstFit(int blockSize)
{
    int worst = -1;
    for (int i = 0; i < blockCount; i++) {
        if (blockArray[i].isFree && blockArray[i].blockSize >= blockSize) {
            if (worst == -1 || blockArray[i].blockSize > blockArray[worst].blockSize)
                worst = i;
        }
    }
    return worst;
}

int findSuitableBlock(int blockSize)
{
    if (allocationStrategy == 2)
        return findBestFit(blockSize);
    if (allocationStrategy == 3)
        return findWorstFit(blockSize);
    return findFirstFit(blockSize);
}

int getTotalFreeMemory()
{
    int total = 0;
    for (int i = 0; i < blockCount; i++)
        if (blockArray[i].isFree)
            total += blockArray[i].blockSize;
    return total;
}

void displayMemoryStatistics()
{
    int allocBytes = 0, freeBytes = 0;
    int allocBlocks = 0, freeBlocks = 0;
    int largest = 0;

    for (int i = 0; i < blockCount; i++) {
        if (blockArray[i].isFree) {
            freeBytes += blockArray[i].blockSize;
            freeBlocks++;
            if (blockArray[i].blockSize > largest)
                largest = blockArray[i].blockSize;
        } else {
            allocBytes += blockArray[i].blockSize;
            allocBlocks++;
        }
    }

    printf("Total memory: %d bytes\n", totalPoolSize);
    printf("Allocated: %d bytes\n", allocBytes);
    printf("Free: %d bytes\n", freeBytes);
    printf("Allocated blocks: %d\n", allocBlocks);
    printf("Free blocks: %d\n", freeBlocks);
    printf("Largest free block: %d bytes\n", largest);
}

void displayMemoryMap()
{
    printf("Memory Map (%d bytes):\n", totalPoolSize);
    for (int i = 0; i < blockCount; i++) {
        if (blockArray[i].isFree)
            printf("[Free: %d bytes, Start: %d]\n", blockArray[i].blockSize, blockArray[i].startAddress);
        else
            printf("[Allocated #%d: %d bytes, Start: %d]\n",
                   blockArray[i].allocationId, blockArray[i].blockSize, blockArray[i].startAddress);
    }
}

void explainAllocationFailure(int blockSize)
{
    int freeBytes = getTotalFreeMemory();

    if (freeBytes >= blockSize) {
        printf("Allocation failed: no contiguous free block is large enough.\n");
        printf("External fragmentation: %d bytes are free in total but the request needs %d contiguous bytes.\n",
               freeBytes, blockSize);
        printf("Free block sizes:");
        for (int i = 0; i < blockCount; i++)
            if (blockArray[i].isFree)
                printf(" %d", blockArray[i].blockSize);
        printf("\n");
    } else {
        printf("Allocation failed: not enough free memory (%d requested, %d free).\n",
               blockSize, freeBytes);
    }
}

void allocateFromFreeBlock(int i, int blockSize, int allocationId)
{
    int rest = blockArray[i].blockSize - blockSize;
    int startAddress = blockArray[i].startAddress;

    blockArray[i].blockSize = blockSize;
    blockArray[i].allocationId = allocationId;
    blockArray[i].isFree = 0;

    if (rest > 0)
        insertBlock(i + 1, startAddress + blockSize, rest, 0, 1);
}

void allocateMemory(int blockSize)
{
    if (blockSize <= 0 || blockSize > totalPoolSize) {
        printf("Invalid size. It must be between 1 and %d.\n", totalPoolSize);
        return;
    }
    int i = findSuitableBlock(blockSize);
    if (i == -1) {
        explainAllocationFailure(blockSize);
        return;
    }

    if (blockArray[i].blockSize > blockSize && !ensureBlockCapacity()) {
        printf("Allocation failed: could not grow the block array.\n");
        return;
    }

    int allocationId = nextAllocationId;
    nextAllocationId++;
    allocateFromFreeBlock(i, blockSize, allocationId);

    for (int k = 0; k < blockSize; k++)
        memoryPool[blockArray[i].startAddress + k] = (allocationId + k) % 256;

    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n",
           allocationId, blockArray[i].startAddress, blockSize);
}

void releaseMemory(int allocationId)
{
    if (allocationId <= 0 || allocationId >= nextAllocationId) {
        printf("Unknown allocation ID.\n");
        return;
    }
    for (int i = 0; i < blockCount; i++) {
        if (!blockArray[i].isFree && blockArray[i].allocationId == allocationId) {
            blockArray[i].isFree = 1;
            blockArray[i].allocationId = 0;
            mergeAdjacentFreeBlocks();
            printf("Allocation released.\n");
            return;
        }
    }
    printf("Allocation %d is already free.\n", allocationId);
}

void resizeAllocation(int allocationId, int newSize)
{
    if (newSize <= 0 || newSize > totalPoolSize) {
        printf("Invalid size. It must be between 1 and %d.\n", totalPoolSize);
        return;
    }

    int i = -1;
    for (int k = 0; k < blockCount; k++) {
        if (!blockArray[k].isFree && blockArray[k].allocationId == allocationId)
            i = k;
    }
    if (i == -1) {
        printf("No allocated block with ID %d.\n", allocationId);
        return;
    }

    int oldSize = blockArray[i].blockSize;

    if (newSize == oldSize) {
        printf("Size is already %d, nothing to do.\n", oldSize);
        return;
    }

    if (newSize < oldSize) {
        int diff = oldSize - newSize;
        if (i + 1 < blockCount && blockArray[i + 1].isFree) {
            
            blockArray[i + 1].startAddress -= diff;
            blockArray[i + 1].blockSize += diff;
        } else {
            if (!ensureBlockCapacity()) {
                printf("Resize failed. Original allocation is unchanged.\n");
                return;
            }
            insertBlock(i + 1, blockArray[i].startAddress + newSize, diff, 0, 1);
        }
        blockArray[i].blockSize = newSize;
        printf("Allocation %d shrunk to %d bytes.\n", allocationId, newSize);
        return;
    }

    int need = newSize - oldSize;
    if (i + 1 < blockCount && blockArray[i + 1].isFree && blockArray[i + 1].blockSize >= need) {
        blockArray[i + 1].startAddress += need;
        blockArray[i + 1].blockSize -= need;
        blockArray[i].blockSize = newSize;
        if (blockArray[i + 1].blockSize == 0)
            deleteBlock(i + 1);
        printf("Allocation %d expanded in place to %d bytes.\n", allocationId, newSize);
        return;
    }

    int j = findSuitableBlock(newSize);
    if (j == -1) {
        printf("Resize failed. Original allocation %d is unchanged.\n", allocationId);
        explainAllocationFailure(newSize);
        return;
    }

    if (blockArray[j].blockSize > newSize && !ensureBlockCapacity()) {
        printf("Resize failed. Original allocation is unchanged.\n");
        return;
    }

    int oldStart = blockArray[i].startAddress;
    int newStart = blockArray[j].startAddress;

    memcpy(memoryPool + newStart, memoryPool + oldStart, oldSize);

    blockArray[i].isFree = 1;
    blockArray[i].allocationId = 0;
    allocateFromFreeBlock(j, newSize, allocationId);
    mergeAdjacentFreeBlocks();

    printf("Allocation %d moved from %d to %d, new size %d.\n",
           allocationId, oldStart, newStart, newSize);
}

void selectAllocationStrategy()
{
    int s = 0;
    while (s < 1 || s > 3) {
        printf("Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ");
        s = readInteger();
        if (s < 1 || s > 3)
            printf("Invalid choice.\n");
    }
    allocationStrategy = s;
    if (s == 1)
        printf("Strategy set to First Fit.\n");
    else if (s == 2)
        printf("Strategy set to Best Fit.\n");
    else
        printf("Strategy set to Worst Fit.\n");
}

int main()
{
    printf("Enter memory pool size: ");
    totalPoolSize = readInteger();
    while (totalPoolSize < 1 || totalPoolSize > 1000000) {
        printf("Invalid size. Enter a value between 1 and 1000000: ");
        totalPoolSize = readInteger();
    }

    memoryPool = malloc(totalPoolSize);
    blockArray = malloc(blockCapacity * sizeof(struct MemoryBlock));
    if (memoryPool == NULL || blockArray == NULL) {
        printf("Memory allocation failed.\n");
        free(memoryPool);
        free(blockArray);
        return 1;
    }

    blockArray[0].startAddress = 0;
    blockArray[0].blockSize = totalPoolSize;
    blockArray[0].allocationId = 0;
    blockArray[0].isFree = 1;
    blockCount = 1;

    selectAllocationStrategy();

    int choice = 0;
    while (choice != 7) {
        printf("\nMenu:\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");
        printf("Choice: ");
        choice = readInteger();

        if (choice == 1) {
            printf("Requested size: ");
            int blockSize = readInteger();
            allocateMemory(blockSize);
        } else if (choice == 2) {
            printf("Allocation ID: ");
            int allocationId = readInteger();
            releaseMemory(allocationId);
        } else if (choice == 3) {
            printf("Allocation ID: ");
            int allocationId = readInteger();
            printf("New size: ");
            int blockSize = readInteger();
            resizeAllocation(allocationId, blockSize);
        } else if (choice == 4) {
            displayMemoryMap();
        } else if (choice == 5) {
            displayMemoryStatistics();
        } else if (choice == 6) {
            selectAllocationStrategy();
        } else if (choice != 7) {
            printf("Invalid choice.\n");
        }
    }

    free(memoryPool);
    free(blockArray);
    return 0;
}
