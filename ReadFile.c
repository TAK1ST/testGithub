#include <stdio.h>
int main()
{
	FILE * filePointer = NULL;
	char buffer[100];
	filePointer = fopen("file.txt","r");
	
	if(filePointer == NULL)
	{
		printf("Could not open file\n");
		return 1;	
	}	
//	for (int i = 0; i < 10; i++) printf("%d",i);
	while (fscanf(filePointer,"%c", buffer)!=EOF) printf("%c\n",buffer);
	fclose(filePointer);
	return 0;
}