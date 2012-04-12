#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ----------------------------------------

 ccmd: a bash terminal utility for custom commands
 Copyright (C) 2012  Khaled Abdullah Khunaifer
 
 Version: 0.3 BETA
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

---------------------------------------- */

// developement notes:
// File: ~/.ccmd
// structure: every line is a custom command line
// line: |alias <ccmd>="<cmd>";|

// add ccmd, as an alias for cmd
void add (char * ccmd, char * cmd)
{
	// Open ccmd file
	FILE * file = fopen ( "~/.ccmd", "a" );
	
	if ( file != NULL )
	{
		fprintf(file, "alias %s=\"%s\";\n", ccmd, cmd);
		
		// Close file
		fclose ( file );
	}
	else
	{
		// error
		printf(" Error: an error occured opening ccmd file \n");
	}
}

// -----------------------------------

int lines(char * filename)
{
	FILE *f;
	char c;
	int lines = 0;
	
	f = fopen(filename, "r");
	
	if(f == NULL)
		return 0;
	
	while((c = fgetc(f)) != EOF)
		if(c == '\n')
			lines++;
	
	fclose(f);
	
	if(c != '\n')
		lines++;
	
	return lines;
}

// ------------------------------------------

char * substr2 (char * str, int start, int end)
{
	// trace: printf("substr('%s', %d, %d) \n",str,start,end);
	if ((end-start) < 1) return 0;
	char * t = (char *) malloc((end-start+1) * sizeof(char));
	strncpy ( t, str+start, (end-start) );
	return t;
}

int first (char * str, char ch)
{
	if (str == 0) return (-1);
	
	char * s = str;
	
	while (*s != '\0')
	{
		if (*s == ch) return (int)(s-str);
		s++;
	}
	
	return (-1);
}

int last (char * str, char ch)
{
	if (str == 0) return (-1);
	
	char * s = str;
	int i = (-1);
	
	while (*s != '\0')
	{
		if (*s == ch) i = (int)(s-str);
		s++;
	}
	
	return i;
}

// ------------------------------------------

// remove ccmd from the list
void rem (char * c)
{
	// Open ccmd file
	FILE * file = fopen ( "~/.ccmd", "r" );
	
	int i = 0;
	int L = lines("~/.ccmd");
	char *** M = malloc (sizeof (char**) * L);
	for (i = 0; i < L; i++) M[i] = malloc (sizeof (char*) * 2);
	
	// parse into matrix
	if ( file != NULL )
	{
		char line [4096] = "";
		char * line2 = 0;
		char * ccmd = 0;
		char * cmd = 0;
		
		while ( fgets ( line, sizeof line, file ) != NULL )
		{
			line2 = strstr(line, " "); // pass: |alias |
			ccmd = substr2(line2, 1, first(line2,'=')-1); // get: |<ccmd>|
			line2 = strstr(line2, "\""); // pass: |<ccmd>=|
			cmd  = substr2(line2, 1, last(line2,'\"')); // get: |<cmd>|
			
			// a recored is 2048 Bytes, 2KB
			M[i][0] = malloc (sizeof (char) * 512);
			strcpy(M[i][0], ccmd);
			M[i][1] = malloc (sizeof (char) * 1536);
			strcpy(M[i][1], cmd);
			
			i++;
		}
		
		// Close file
		fclose ( file );
	}
	else
	{
		// error
		printf(" Error: an error occured reading ccmd file \n");
		return;
	}
	
	// overwrite ccmd, without marked commands
	
	file = fopen ( "~/t.txt", "w" );
	
	if ( file != NULL )
	{
		int i = 0;
		
		while ( i < L )
		{
			if (M[i][0] == c)
			{
				fprintf(file, "alias %s=\"%s\";\n", M[i][0], M[i][1]);
			}
			i++;
		}
		
		// Close temp file
		fclose ( file );
		
		// replace original with temp
		if (remove("~/.ccmd") == 0)
		{
			if (rename("~/t.txt", "~/.ccmd") != 0)
			{
				printf("Error: new ccmd couldn't be placed, check t file");
				return;
			}
		}
		else
		{
			printf("Error: old ccmd file couldn't be deleted");
			return;
		}
	}
	else
	{
		// error
		printf(" Error: an error occured reading ccmd file \n");
		return;
	}
	
	printf("\n Custom command removed successfully \n\n");
}

// print list of ccmd's
void list ()
{
	// Open ccmd file
	FILE * file = fopen ( "~/.ccmd", "r" );
	
	char line [4096] = "";
	char * line2 = 0;
	char * ccmd = 0;
	char * cmd = 0;
	
	// parse and output
	if ( file != NULL )
	{
		while ( fgets ( line, sizeof line, file ) != NULL )
		{
			line2 = strstr(line, " "); // pass: |alias |
			ccmd = substr2(line2, 1, first(line2,'=')-1); // get: |<ccmd>|
			line2 = strstr(line2, "\""); // pass: |<ccmd>=|
			cmd  = substr2(line2, 1, last(line2,'\"')); // get: |<cmd>|
			
			printf ("%s : %s \n", ccmd, cmd);
		}
		
		// Close file
		fclose ( file );
	}
	else
	{
		// error
		printf(" Error: an error occured reading ccmd file \n");
	}
}


// -c --check if ccmd is installed (incomplete)
void check ()
{
	FILE * file = fopen ("~/.bashrc","r");
	
	// TODO: look for this line: |sh ~/.ccmd;| or |sh .ccmd;|
	
	fclose (file);
}

// -i --install ccmd script in .bashrc
void install ()
{
	FILE * file = fopen ("~/.bashrc","a");
	
	// append: |sh ~/.ccmd;| "\n" |clear;|
	
	if (file != 0)
	{
		fprintf (file, "\nsh ~/.ccmd;\nclear;");
	
		fclose (file);
	}
	else
	{
		printf("Error: .bashrc file couldn't be opened for writing, use sudo");
	}

}

// print general info, and use
void help ()
{
	printf("\n Custom Command Utility \n\n Use: ccmd -option <operand> \n\n Options: -a(dd) -r(emove) -l(ist) -i(nstall) -h(elp) -L(icense)\n\n note: in case of -i(nstall), you need to use sudo [ sudo ccmd -i ]\n\n");
}

void license ()
{
	printf("\n\n\tccmd  Copyright (C) 2012  Khaled Abdullah Khunaifer\n\tThis program comes with ABSOLUTELY NO WARRANTY; for how to use use `-h'.\n\tThis is free software, and you are welcome to redistribute it\n\tunder certain conditions; see license.txt for details.\n\n");
}

int main (int argc, char * argv[])
{	
	if (argc < 2)
	{
		help ();
	}
	else
	{
		switch (argv[1][1])
		{
			case 'a':
			//case "--add":
				add (argv[2], argv[3]);
				break;
			case 'r':
			//case "--remove":
				rem (argv[2]);
				break;
			case 'l':
			//case "--list":
				list ();
				break;
			case 'h':
			//case "--help":
				help ();
				break;
			case 'c':
			//case "--check":
				check ();
				break;
			case 'i':
			//case "--install":
				install ();
				break;
			case 'L':
			// case "--License"
				license ();
				break;
			default:
				printf("\n Invalid Option, use -h for --help \n\n");
		}
	}
	
	return 0;
}
