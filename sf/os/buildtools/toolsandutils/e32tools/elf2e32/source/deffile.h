// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of the Class DefFile for the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef _DEF_FILE_
#define _DEF_FILE_

#include <list>

class Symbol;
typedef std::list <Symbol*>	SymbolList;

/**
Class for DEF File Handler.
@internalComponent
@released
*/
class DefFile
{
	public:
		DefFile():iSymbolList(NULL){};
		virtual ~DefFile();
		SymbolList* ReadDefFile(char *defFile);
		void WriteDefFile(char *fileName, SymbolList *newSymbolList);
		SymbolList* GetSymbolEntryList(char *defFile);
		SymbolList* GetSymbolEntryList() { return iSymbolList;}
	private:
		char *iFileName;
		SymbolList *iSymbolList;
		int GetFileSize(FILE * fptrDef);
		char* OpenDefFile(char *defFile);
		void ParseDefFile(char *defData);
		bool Tokenize(char* tokens, int aLineNum, Symbol& aSymbol);
};

//Different states while parsing a line in the def file.
enum DefStates
{
	EInitState = 0,
	ESymbolName = 1,
	EAtSymbol,
	EOrdinal,
	EOptionals,
	EComment,
	EFinalState,
	EInvalidState
};

#define CODESYM "CODE"
#define DATASYM "DATA"

/**
Class for parsing a line from the DEF File.
@internalComponent
@released
*/
class LineToken
{
public:
	LineToken(char* , int , char *, Symbol *aSym);
	char *iLine;
	Symbol *iSymbol;
	int iOffset;
	DefStates iState;
	
	char *iFileName;
	int iLineNum;

	bool Tokenize();
	void NextToken();
	void IncrOffset(int aOff);
	void SetState(DefStates aState);

	bool IsWhiteSpace(char *aStr, int&);
	bool IsPattern(char*, int&, int&);
	bool IsDigit(char*, int&);
	bool IsWord(char*, int&);
};
#endif
