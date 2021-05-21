#include <stdlib.h>

#define mimeType struct _mimeType

struct _mimeType
{
	char *name;
	char *filter;
	char *action;
	mimeType *next;
};

class mimeTable
{
	public:
	 mimeTable();
	 mimeTable(char *sourceFile);
	 ~mimeTable();
	 bool loadFromFile(char *sourceFile);
	  
	 mimeType *getType(char *fileName);
	 bool addType(char *name, char *filter, char *action);
	
	private:
	 mimeType *firstType;
	 bool match(char *candidate, char *model);
	 bool startsWith(char *theString, char *beginning);
	 bool case_equal(char a, char b);
};


