#pragma once
#ifndef __COMMANDPARSE_H_INCLUDED__
#define __COMMANDPARSE_H_INCLUDED__
#include <windows.h>
#include <string>
#define VALIDATE_COMMAND( s, c ) \
	if (_strnicmp( s + 1, c, strlen( c ) ) != 0) \
		return false;
#define HANDLE_ARGUMENT( a, b ) \
	if (a[1] == b)
/**
 * GetCommandArgs
 *	@param1		: The command to parse for arguments.
 *	@param2		: Array of strings to store the found arguments.
 *	@returns	: Number of arguments found within the command.
 *
 * This function mimics the *nix command 'strsep' to locate and
 * parse strings with inline-quotes.
 *
 * Thanks to:
 *		http://stackoverflow.com/questions/9659697/parse-string-into-array-based-on-spaces-or-double-quotes-strings
 *
 * Modified for Ashita needs with plugin commands.
 *
 */
int __stdcall GetCommandArgs( const char* pszCommand, std::string* args )
{
	enum {
		NONE,
		IN_WORD,
		IN_STRING,
	} state = NONE;

	char	szCurrentArgument[ 255 ] = { 0 };
	char	szCurrentChar	= 0;
	char	*pszStart		= 0;
	int		nArgCount		= 0;

	for (const char* p = pszCommand; *p != '\0' || state != NONE; ++p)
	{
		szCurrentChar = (unsigned char)*p;
		switch (state)
		{
		case NONE:
			{
				if (isspace( szCurrentChar ))
					continue;
				if (szCurrentChar == '"')
				{
					state = IN_STRING;
					pszStart = (char*)( p + 1 );
					continue;
				}

				state = IN_WORD;
				pszStart = (char*)p;
				continue;
			}

		case IN_STRING:
			if (szCurrentChar == '"')
			{
				strncpy( szCurrentArgument, pszStart, p - pszStart );
				args[nArgCount] = std::string( szCurrentArgument ); 
				nArgCount++;

				state = NONE;
			}
			continue;

		case IN_WORD:
			if (isspace( szCurrentChar ) || *++p == '\0')
			{
				strncpy(szCurrentArgument, pszStart, p - pszStart );
				args[nArgCount] = std::string( szCurrentArgument );
				nArgCount++;

				state = NONE;
			}
			*--p;
			continue;
		}
	}

	return nArgCount;
}

#endif // __COMMANDPARSE_H_INCLUDED__
