#include "mimeTable.h"
#include <stdio.h>
#include <fstream>


mimeTable::mimeTable()
{
	firstType = NULL;
	addType("Unknown Type","*"," ");
}

mimeTable::mimeTable(char *sourceFile)
{
	firstType = NULL;
	addType("Unknown Type","*"," ");
	loadFromFile(sourceFile);
}

mimeTable::~mimeTable()
{
	mimeType *cursor;
	while(cursor != NULL)
	{
		firstType = cursor;
		free(cursor->name);
		free(cursor->filter);
		free(cursor->action);
		cursor = cursor->next;
		free(firstType);
	}
}

bool mimeTable::loadFromFile(char *sourceFile)
{
	FILE *typesFile;
	char tempName[1024];
	char tempFilter[1024];
	char tempAction[1024];
	char *cursor;

	typesFile = fopen(sourceFile, "r");
	if(typesFile == NULL)
		return false;
		
	while(fgets(tempName, 1024, typesFile))
	{
		cursor = tempName;
		if((*cursor != 9) && (*cursor != '\n') && (*cursor != ' ') && (*cursor != '#'))
			{
			while(*cursor != 9)
				cursor++;
			*cursor = 0;
			cursor++;

			while(*cursor == 9)
				cursor++;
		
			strcpy(tempFilter, cursor);
			cursor = tempFilter;
		
			while(*cursor != 9)
				cursor++;
			*cursor = 0;
			cursor++;

			while(*cursor == 9)
				cursor++;
			
			strcpy(tempAction, cursor);
			cursor = tempAction;		
		
			while(*cursor != '\n')
				cursor++;
			*cursor = 0;

			addType(tempName, tempFilter, tempAction);
			}

	}


	return true;
}

mimeType *mimeTable::getType(char *fileName)
{
	mimeType *currentType;
	currentType = firstType;
	
	while(!match(fileName,currentType->filter))
	{
		currentType = currentType->next;
	}
	return(currentType);
}


bool mimeTable::match(char *candidate, char *model)
{
	int i;
	char nextPattern[1024];
	/* This function has to be fixed & tested as much as possible
	*/
	
	if(!strcmp(model, "*"))
			return(true);
	while(*model != '*' && *model !=0)
	{
		if(!case_equal(*candidate, *model))
			return false;
		model++;
		candidate++;
	}
	while(*model != 0)
	{
		model++;
		i=0;
		while((*model != '*') && (*model != 0))
		{
			nextPattern[i] = *model;
			model++;
			i++;
		}
		nextPattern[i]=0;
	
		while((!startsWith(candidate,nextPattern)) && (*candidate!= 0))
		{
			candidate ++;
		}
		if(*candidate == 0)
			return false;
		
		i = strlen(nextPattern);
		candidate += i;
	}		
		
	if(*candidate == 0)
	{
		return true;
	}
		else
	{
		return false;
	}	
}

bool mimeTable::startsWith(char *theString, char *beginning)
{
	while(case_equal(*theString, *beginning) && (*beginning !=0))
	{
		theString++;
		beginning++;
	}
	return (*beginning == 0);
}		


bool mimeTable::case_equal(char a, char b)
	{
	if((a <= 'Z') && (a >= 'A'))
	{
		a += 'a' - 'A'; 
	}

	if((b <= 'Z') && (b >= 'A'))
	{
		b += 'a' - 'A'; 
	}
	return (a == b);
	
}

void mimeTable::addType(char *name, char *filter, char *action)
{
	mimeType *temp;
	temp = new mimeType;
	temp->name = (char *)malloc(strlen(name)*sizeof(char));
	temp->filter = (char *)malloc(strlen(filter)*sizeof(char));
	temp->action = (char *)malloc(strlen(action)*sizeof(char));
	
	strcpy(temp->name,name);
	strcpy(temp->filter, filter);
	strcpy(temp->action, action);
	
	temp->next = firstType;
	firstType = temp;
}









