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
	int LRU_Index;
	char *data;
};

struct Trace {
	unsigned long address;
	char type;
	int size;
};


// in: line from input, out: type,size as ints, address as unsigned long
void getInstructInfo(struct Trace *trace, char *singleLine){
	char type;
	unsigned long address;
	int length;
	sscanf(singleLine, " %c %lx,%d", &type,&address,&length);
	trace->address=address;
	trace->size=length;
	trace->type=type;
}

//print out the trace info
void printTrace(struct Trace *trace) {
	printf("%c\t%lx\t%d\n",trace->type,trace->address,trace->size);
}

// in: cache, address, cache params
// action: tries to do load/store (since the data doesn't matter load and store are identical)
int doTheCache(struct Cache *cache, struct Trace *trace, int set_lim, int line_lim, int block_bits){
	unsigned long address = trace->address;
	int dsize = 8*sizeof(address);
	int set_num = (address << (dsize - (set_lim + block_bits))) >> (dsize - set_lim);  // get set_lim bits
	int tag_bits = address >> (set_lim + block_bits);   // get tag bits
	//printf("\naddr %lx dsize%d snum%d tag%d slim%d llim%d bbit%d\n", address,dsize,set_num,tag_bits,set_lim,line_lim,block_bits);
	// search by tag & check if valid
	int j, miss=1, replace=1;
	for (j=0; j < line_lim; j++){
		struct Line *thisLine = &(cache->sets[set_num].lines[j]);
		if(thisLine->valid == 1 && thisLine->tag == tag_bits) {
			// cache hit
			miss = 0;
			replace = 0;
			break;
		}
	}
  
	// "put" the data in the cache
	if (miss){
		// cache miss
		int LRU_count = 0;
		int LRU_place = 0;
		// replace if valid bit is 0...
		for(j = 0; j < line_lim; j++){
			struct Line *thisLine = &(cache->sets[set_num].lines[j]);
			if(thisLine->valid == 0){
				thisLine->valid = 1;
				thisLine->tag = tag_bits;
				thisLine->LRU_Index = 0;
				for (int k=0; k<line_lim; k++) {
					struct Line *otherLine = &(cache->sets[set_num].lines[k]);
					if (otherLine->valid==1) otherLine->LRU_Index++;
				}
				replace = 0;
				break;
			} else if (thisLine->LRU_Index > LRU_count) {
				LRU_count = thisLine->LRU_Index;
				LRU_place = j;
			}
		}
		// ...otherwise evict and replace
		if(replace){
			for(j = 0; j < line_lim; j++) cache->sets[set_num].lines[j].LRU_Index++;
			struct Line *thisLine = &(cache->sets[set_num].lines[LRU_place]);
			thisLine->tag = tag_bits;
			thisLine->LRU_Index = 0;
		}
	}
	return miss+replace;
}

int main(int argc, char *argv[]) {
	int set_lim=-1, line_lim=-1, block_bits=-1, help=0, verbose=0;
	char filename[32] = {0};
	if (argc < 9) {
		printf("not enough arguments\n");
		return -1;
	}
	for (int i=1;i<argc;i++) {
		char argbuf[4];
		char arg[4];
		strncpy(argbuf,argv[i],4);
		sscanf(argbuf,"-%s",arg);
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
	
	if (help) {
		printf("Usage: csim [-hv] -s <s> -e <E> -b <b> -t <tracefile>\n");
		printf("\t-h\t\tOptional help flag that displays this info\n");
		printf("\t-v\t\tOptional verbose flag that displays trace info\n");
		printf("\t-s <s>\t\tNumber of set index bits (number of sets is 2^s)\n");
		printf("\t-E <E>\t\tAssociativity (number of lines per set)\n");
		printf("\t-b <b>\t\tNumber of block bits (block size is 2^b)\n");
		printf("\t-t <tracefile>\tName of valgrind trace to display\n");
		return 0;
	}
		
	struct Cache *cache = malloc(sizeof(struct Cache));
	cache->sets = (struct Set *)malloc((1 << set_lim) * sizeof(struct Set));
	for (int i=0;i<set_lim;i++) {
  		cache->sets[i].lines = (struct Line *)malloc(line_lim * sizeof(struct Line));
		for (int j=0;j<line_lim;j++) {
			cache->sets[i].lines[j].valid=0;
		}
	}

	FILE *fPointer;
	fPointer = fopen(filename, "r");
	char singleLine[20];
	
	struct Trace *nextTrace = malloc(sizeof(struct Trace));
	int hits=0,misses=0,evicts=0;
	fgets(singleLine, 20, fPointer);
	do {
		getInstructInfo(nextTrace,singleLine);
		if (verbose) printf("%c %lx,%d ",nextTrace->type,nextTrace->address,nextTrace->size);
		int result;
		result = doTheCache(cache,nextTrace,set_lim,line_lim,block_bits);
		switch(result) {
			case 0:hits++;if(verbose)printf("hit ");break;
			case 1:misses++;if(verbose)printf("miss ");break;
			case 2:misses++;evicts++;if(verbose)printf("miss eviction ");break;
			default:if(verbose)printf("oops ");break;
		}
		if (nextTrace->type == 'M') {
			result = doTheCache(cache,nextTrace,set_lim,line_lim,block_bits);
			switch(result) {
				case 0:hits++;if (verbose)printf("hit ");break;
				case 1:misses++;if (verbose)printf("miss ");break;
				case 2:misses++;evicts++;if (verbose)printf("miss eviction ");break;
				default:if (verbose)printf("oops ");break;
			}
		}
		if (verbose) printf("\n");
		fgets(singleLine, 20, fPointer);
	} while(!feof(fPointer));
	fclose(fPointer);
	
	free(nextTrace);
	
	for (int i=0;i<set_lim;i++) {
		free(cache->sets[i].lines);
	}
	free(cache->sets);
	free(cache);
	
	printSummary(hits,misses,evicts);
	return 0;
}

