#include <stdio.h>
#include <stdlib.h>

#include "processlib.h"
#include "test_util.h"
#include "windowManager.h"

#define MAX_BLOCKS 4096
#define MAX_MEMORY 1024 * 4096 * 0.8  //Should be around 80% of memory managed by the MM

typedef struct MM_rq {
    void *address;
    uint32_t size;
} mm_rq;

void test_mm() {
    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;

    while (1) {
        rq = 0;
        total = 0;

        // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < MAX_MEMORY) {
            mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
            mm_rqs[rq].address = malloc(mm_rqs[rq].size);  // TODO: Port this call as required
            //TODO: check if NULL
            total += mm_rqs[rq].size;
            rq++;
        }
        //printf("Reserve");
        //busy_wait(1000000000);
        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address != NULL)
                memset(mm_rqs[i].address, i, mm_rqs[i].size);  // TODO: Port this call as required

        // Check
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address != NULL)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
                    printf("ERROR!\n");  // TODO: Port this call as required

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address != NULL)
                free(mm_rqs[i].address);  // TODO: Port this call as required

        //printf("Libere");
        // busy_wait(1000000000);
    }
}
