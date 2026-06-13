#include <stdio.h>
#include <stdint.h>
#define BUFFER_SIZE 8

// Ring Buffer Structure 
typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    uint8_t head;     // Next write position
    uint8_t tail;     // Next read position
    uint8_t count;    // Number of bytes currently stored
} RingBuffer;

// Function Prototypes 
void initBuffer(RingBuffer *rb);
int isFull(RingBuffer *rb);
int isEmpty(RingBuffer *rb);
int writeBuffer(RingBuffer *rb, uint8_t data);
int readBuffer(RingBuffer *rb, uint8_t *data);

// Initialize buffer to empty state 
void initBuffer(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

// Check whether buffer is full 
int isFull(RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE);
}

// Check whether buffer is empty
int isEmpty(RingBuffer *rb)
{
    return (rb->count == 0);
}

// Write one byte into the buffer
int writeBuffer(RingBuffer *rb, uint8_t data)
{
    //Do not overwrite unread data
    if (isFull(rb))
    {
        return -1;
    }

    rb->buffer[rb->head] = data;

    /*
    => BUFFER_SIZE = 8 (power of 2)
    => Using '& (BUFFER_SIZE - 1)' is faster than '% BUFFER_SIZE' on many MCUs because it avoids division instructions.
    */
    rb->head = (rb->head + 1) & (BUFFER_SIZE - 1);
    rb->count++;
    return 0;
}

// Read one byte from the buffer
int readBuffer(RingBuffer *rb, uint8_t *data)
{
    // No data available
    if (isEmpty(rb))
    {
        return -1;
    }

    *data = rb->buffer[rb->tail];

    /* Wrap around to beginning when end of buffer is reached */
    rb->tail = (rb->tail + 1) & (BUFFER_SIZE - 1);
    rb->count--;
    return 0;
}

int main(void)
{
    RingBuffer rb;
    uint8_t value;
    initBuffer(&rb);

    // Data for Step 1
    uint8_t data1[8] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48
    };

    for (int i = 0; i < 8; i++)
    {
        if (writeBuffer(&rb, data1[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)", data1[i], rb.count);
            
            if (isFull(&rb))
            {
                printf(" FULL");
            }
            printf("\n");
        }
    }

    if (writeBuffer(&rb, 0x99) == -1)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }

    for (int i = 0; i < 3; i++)
    {
        if (readBuffer(&rb, &value) == 0)
        {
            printf("[READ] -> 0x%02X (count=%d)\n", value, rb.count);
        }
    }
   
    uint8_t data2[3] = {0x49, 0x4A, 0x4B};

    for (int i = 0; i < 3; i++)
    {
        if (writeBuffer(&rb, data2[i]) == 0)
        {
            printf("[WRITE] 0x%02X -> OK (count=%d)\n", data2[i], rb.count);
        }
    }
    
    while (!isEmpty(&rb))
    {
        if (readBuffer(&rb, &value) == 0)
        {
            printf("[READ] -> 0x%02X (count=%d)\n", value, rb.count);
        }
    }

    if (readBuffer(&rb, &value) == -1)
    {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}