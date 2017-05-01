#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define fbufsize 32

struct Cache {
	struct Set* sets;
};

struct Set {
	struct Line* lines;
};

struct Line {
	int valid;
	unsigned long tag;
	unsigned long LRU_Index;
	char *data;
};

struct Trace {
	unsigned long address;
	int type;
	int size;
};


// in: line from input, out: type,size as ints, address as unsigned long
void getInstructInfo(struct Trace *trace, char *singleLine){
	char type;
	unsigned long address;
	int length;
	sscanf(singleLine, " %c %lx,%d", &type,&address,&length);
	trace->address=address;
	trace->size=size;
	switch(type) {
		case 'L':trace->type=0;break;
		case 'S':trace->type=1;break;
		case 'M':trace->type=2;break;
		default:trace->type=-1;break;
	}
}

// in: cache
// action: loads data into cache
int cacheLoad(struct Cache *cache, unsigned long address, int set_lim, int line_lim, int block_bits){
	int dsize = sizeof(address);
	int set_num = (address << (dsize - (set_lim + block_bits))) >> (dsize - set_lim);  // get set_lim bits
	int tag_bits = address >> (set_lim + block_bits);   // get tag bits
  
	// search by tag & check if valid
	int j, hit, replace;
	hit = 0; replace = 0;
	for (j = 0; j < line_lim; j++){
		if(cache->sets[set_num].lines[j].valid == 1 && cache->sets[set_num].lines[j].tag == tag_bits){
			// cache hit
			// leave data there and report hit
			hit = 1;
		}
	}
  
	// put the data in the cache
	if (hit == 0){
		// cache miss
		// use address to put data into cache
		// replace if valid bit is 0 then if LRU is lowest
		for(j = 0; j < line_lim; j++){
			if(cache->sets[set_num].lines[j].valid == 0){
				cache->sets[set_num].lines[j].valid == 1;
				cache->sets[set_num].lines[j].tag == tag_bits;
				cache->sets[set_num].lines[j].data == block_bits;
				// make LRU index newest // if no value exists, set to 0 // search through and find greatest value and increase by 1
				for(j = 0; j < line_lim; j++){
					if(){
					}
				}
				replace = 1;
				break;
			}
		}
		if(replace == 0){
			for(j = 0; j < line_lim; j++){
				if(cache->sets[set_num].lines[j].LRU_Index <= LOWVAL) LOWVAL = LRU_Index;

			}
			for(j = 0; j < line_lim; j++){
				if(cache->sets[set_num].lines[j].LRU_Index == LOWVAL){
					cache->sets[set_num].lines[j].valid == 1;
					cache->sets[set_num].lines[j].tag == tag_bits;
					cache->sets[set_num].lines[j].data == block_bits;
					// make LRU index newest
				}
			}
		}
	}
	return hit;
}

// call to cache
// in: cache, trace
// action: calls cache
int callCache(struct Cache *cache, struct Trace *trace){
	return 0;
}

int main(int argc, char *argv[]) {
	int set_lim=-1, line_lim=-1, block_bits=-1, help=0, verbose=0;
	char filename[32] = {0};
	printf("%s\n", argv[1]);
	for (int i=1;i<argc;i++) {
		char argbuf[4];
		char arg[4];
		strncpy(argbuf,argv[i],4);
		int n = sscanf(argbuf,"-%s",arg);
		printf("X%s X%s X%d X%s X%d X%c\n",argv[i],argbuf,n,arg,i,*arg);
		if (*arg=='h' || (sizeof(arg)>1 && *(arg+1)=='h')) help=1;
		else if (*arg=='v') verbose=1;
		else if (*arg=='t') strncpy(filename,argv[++i],fbufsize-1);			
		else if (*arg=='s') set_lim = atoi(argv[++i]);
		else if (*arg=='E') line_lim = atoi(argv[++i]);
		else if (*arg=='b') block_bits = atoi(argv[++i]);
		else {
			printf("unknown argument\n");
			return -1;
		}
	}
	
	if (set_lim<0) {
		printf("Need set bits\n");
		return -1;
	} else if (line_lim<0) {
		printf("Need associativity\n");
		return -1;
	} else if (block_bits<0) {
  		printf("Need block bits\n");
		return -1;
	} else if (strlen(filename)==0) {
		printf("Need filename\n");
		return -1;
	}
		
	struct Cache *cache = malloc(sizeof(struct Cache));
	cache->sets = (struct Set *)malloc((1 << set_lim) * sizeof(struct Set));
	for (int i=0;i<set_lim;i++) {
  		cache->sets[i].lines = (struct Line *)malloc(line_lim * sizeof(struct Line));
		for (int j=0;j<line_lim;j++) {
			cache->sets[i].lines[j].data = (char *)malloc((1 << block_bits) * sizeof(char));
		}
	}

	FILE *fPointer;
	fPointer = fopen(filename, "r");
	char singleLine[20];
	
	struct Trace *nextTrace = malloc(sizeof(struct Trace));
	while(!feof(fPointer)) {
		fgets(singleLine, 20, fPointer);
		getInstructInfo(nextTrace,singleLine);
		
	}
	fclose(fPointer);

	printSummary(0,0,0);
	return 0;
}

