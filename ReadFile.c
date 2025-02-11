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
	
	while (fscanf(filePointer,"%s", buffer)!=EOF) printf("%s",buffer);
	fclose(filePointer);
	printf("Read file successful");
	return 0;
}
