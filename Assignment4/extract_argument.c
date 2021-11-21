//////////////////////////////////////////////////////////////////////
// get argument function
//
// return true if ok and false if error or no further argument
// line .. the command line
// argn .. the to extracted argument beginning with 0
// result .. should be a static char array of 1000 bytes
//
// it respects quotation marks!!
// its also not much tested
// 
// 
//////////////////////////////////////////////////////////////////////
#include <string.h>

char get_argument(char* line, int argn, char* result)
	{
	//firstly: remove all spaces at the front
	char temp[1000];
	int start_space = 1;
	for (int i = 0, u = 0; i <= strlen(line); i++)
		if (line[i] == ' ' && start_space == 1) continue;
		else 
			{
			temp[u++] = line[i]; 
			start_space = 0;
			}
	//now remove an double or tripple spaces
	char temp2[1000];
	int space_on = 0;
	for (int i = 0, u = 0; i <= strlen(temp); i++)
		{
		if (space_on == 1 && temp[i] == ' ') continue;
		temp2[u++] = temp[i];
		if (temp[i] == ' ') space_on = 1;
		else space_on = 0;
		}
	//finally extract the arguments
	int start, end;
	start = end = 0;
	int count = 0;
	int quote = 0;
	for (int i = 0; i <= strlen(temp2); i++)
		if (temp2[i] == '\"') quote = !quote;
		else if (quote == 0 &&(temp2[i] == ' ' || temp2[i] == 0))
			{
			end = i;
			if (argn == count)
				{
				int length = end - start;
				strncpy(result, temp2 + start, length);
				result[length] = 0;
				return 1;
				}
			start = end + 1;
			count++;
			}
	return 0;
	}