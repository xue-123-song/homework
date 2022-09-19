/*
* Nane: Xue Songtao 
* LoginID: ics520030910034
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include "cachelab.h"

/* Trace parameters */
int s,S,E,v,b;
char t[500];

/* Status variables */
int curTime = 0;/* record current time */
int hits = 0,misses = 0,evictions = 0;

/* Cache Simulator Struct */
typedef struct cache_line {
    int valid;
    int accessTime;
    long tag;
} line_t;

typedef line_t* set_t;

set_t *sets;

/* Output usage */
void usage(char *program)
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", program);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
}

/* Parse command line */
void parse_cmd(int argc, char **argv)
{
    int opt;
    s = E = b = -1;
    memset(t,0,sizeof(t));

    while((opt = getopt(argc,argv,"hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'v':
                v = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
            case 't':
                strcpy(t,optarg);
                break;
            default:
                break;
        }
    }
    
    /* Wrong input */
    if (s == -1 || E == -1 || b == -1 || *t == 0) {
        printf("%s: Wrong command line argument\n", argv[0]);
        usage(argv[0]);
        exit(0);
    }
}

/* Initialize cache structure */
void init_cache()
{
    sets = (set_t *)malloc(S * sizeof(set_t));
    for(int i = 0;i < S;i++) {
        sets[i] = (line_t *)malloc(E * sizeof(line_t));
        for (int j = 0;j < E;j++) {
            sets[i][j].valid = 0;
            sets[i][j].accessTime = -1;
            sets[i][j].tag = -1;
        }
    }
}

/* Free cache structure */
void free_cache()
{
    for (int i = 0;i < S;i++)
        free(sets[i]);
    free(sets);
}

/* Update cache */
void update(long address)
{
    curTime++;
    int set_index = (address >> b) & ((1 << s) -1);
    long tag = (address >> (s + b));
    line_t *cur_line;
    /* Hit */
    for (int i = 0;i < E;i++) {
        cur_line = &sets[set_index][i];    
        if (cur_line->tag == tag && cur_line->valid == 1) {
            hits++;
            cur_line->accessTime = curTime;
            return;
        }
    }

    /* Miss */
    for (int i = 0;i < E;i++) {
        cur_line = &sets[set_index][i]; 
        if (cur_line->valid == 0) {
            misses++;
            cur_line->valid = 1;
            cur_line->tag = tag;
            cur_line->accessTime = curTime;
            return;
        }
    }

    /* Eviction */
    misses++;
    evictions++;
    int min_acctime = 0x7fffffff,line_rep = -1;
    for (int i = 0;i < E;i++) {
        cur_line = &sets[set_index][i]; 
        if (cur_line->accessTime < min_acctime) {
            min_acctime = cur_line->accessTime;
            line_rep = i;
        }
    }
    sets[set_index][line_rep].tag = tag;
    sets[set_index][line_rep].accessTime = curTime;
    sets[set_index][line_rep].valid = 1;
}

/* Simulate cache behavior */
void simulate()
{
    char op;
    long address;
    int res,size;
    FILE *file = fopen(t,"r");

    /* File does not exist */
    if (file == NULL) {
        printf("open error");
        exit(-1);
    }

    while(!feof(file)) {
        res  = fscanf(file,"%c %lx,%d", &op, &address, &size);
        if (res != 3)
            continue;
        if (v && (op == 'L' || op == 'M' || op == 'S'))
            printf("%c %lx,%d\n", op, address, size);
        switch(op) {
            case 'L':
                update(address);
                break;
            case 'S':
                update(address);
                break;
            case 'M':
                update(address);
                update(address);
                break;
        }
    }
    fclose(file);
    return;
}

int main(int argc,char* argv[])
{
    /* Lack argument */
    if (argc == 1) {
        usage(argv[0]);
        exit(0);
    }
    parse_cmd(argc,argv);
    S = 1 << s; /* S = 2 ^ s */
    init_cache();
    simulate();
    printSummary(hits, misses, evictions);
    free_cache();
    return 0;
}
