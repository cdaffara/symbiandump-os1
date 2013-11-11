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
// Implementation of the Class Symbol for the elf2e32 tool
// @internalComponent
// @released
// 
//


#if !defined(_PL_SYMBOL_H_)
#define _PL_SYMBOL_H_

#include "pl_common.h"
#include "pl_sym_type.h"
#include <string>

#define UnAssignedOrdNum -1;

enum SymbolStatus {Matching,Missing,New};
/**
 * This class is shared among all that use the symbol information. To name  them,
 * DefFileHandler, UseCaseController, DSOHandler. To be finaslized  by
 * DefFileHandler.
 */
class Symbol
{

public:
	Symbol(char* aName,SymbolType aCodeDataType, char* aExportName=NULL, PLUINT32 aOrd = -1,\
		char* aComment=NULL, bool aR3Unused=false, bool aAbsent=false, PLUINT32 aSymbolSz = 0);

	Symbol(int symbolStatus,char *name,char *exportName,int ordinalNo,bool r3unused,bool absent,int symbolType,char *comment, PLUINT32 aSymbolSz = 0);

	Symbol(Symbol& aSymbol, SymbolType aCodeDataType, bool aAbsent);

	Symbol(Symbol& aSymbol);
	/*Symbol(char* aName,CDType aCodeDataType, PLUINT32	aOrd = UnAssignedOrdNum,\
		char* aComment = NULL, bool aAbsent = false);*/
	virtual ~Symbol();

	bool operator==(const Symbol* aSym) const;
	const char* SymbolName() const;
	const char* ExportName();
	PLUINT32 OrdNum() const;
	SymbolType CodeDataType();

	bool R3unused();
	bool Absent();
	void SetAbsent(bool aAbsent);
	char* Comment();
	int GetSymbolStatus();
	void SetOrdinal(PLUINT32 aOrdinalNum);
	void SetSymbolStatus(SymbolStatus aSymbolStatus);
	void SetSymbolName(char *aSymbolName);
	
	void SymbolName(char *aSymbolName);
	void Comment(char *aComment);
	void CodeDataType(SymbolType aType);
	void R3Unused(bool aR3Unused);
	void ExportName(char *aExportName);
	void SetSymbolSize(PLUINT32 aSz);
	PLUINT32 SymbolSize();

private:
	SymbolStatus iSymbolStatus;
	char*		iSymbolName;
	char*		iExportName;
	SymbolType	iSymbolType;
	PLUINT32	iOrdinalNumber;
	char*		iComment;
	bool		iAbsent;
	bool		iR3Unused;
	PLUINT32	iSize;
	
};




#endif // !defined(_PL_SYMBOL_H_)
