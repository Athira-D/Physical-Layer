
#define INT 0
#define STRING 1
#define FLOAT 2
#define EQ 101
#define LE 102
#define LT 103
#define GE 104
#define GT 105
#define NE 106 //if considered
#define RST 100 //reset op.
#define PRJCT 107

typedef struct fnode{
	char str[16];
	int ival;
	struct fnode *right;
	struct fnode *next;
} fnode;

typedef struct Condition{
	char attr[16];
	int op;
	char value[16];
} Condition;
