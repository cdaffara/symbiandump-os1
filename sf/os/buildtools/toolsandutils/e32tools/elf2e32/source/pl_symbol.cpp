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

#include "pl_symbol.h"


/**
This constructor sets the symbol members.
@internalComponent
@released
*/
Symbol::Symbol(char* aName, SymbolType aCodeDataType, char* aExportName, PLUINT32	aOrd, \
			   char* aComment , bool aR3Unused, bool aAbsent, PLUINT32 aSz) :\
			   iSymbolName(aName), iExportName(aExportName),iSymbolType(aCodeDataType),\
			    iOrdinalNumber(aOrd),iComment(aComment),iAbsent(aAbsent), iR3Unused(aR3Unused), \
	        	iSize (aSz) 
{

}

/**
This constructor sets the symbol members.
@internalComponent
@released
*/
Symbol::Symbol(Symbol& aSymbol, SymbolType aCodeDataType, bool aAbsent)
			   : iExportName(NULL), \
		iSymbolType(aCodeDataType),iComment(NULL), iR3Unused(false), iAbsent(aAbsent) 
{
	iSymbolName = new char[strlen(aSymbol.SymbolName()) + 1];
	strcpy(iSymbolName, aSymbol.SymbolName());
	iOrdinalNumber = aSymbol.OrdNum();
}

/**
This copy constructor copies the symbol members from the input symbol.
@param aSymbol - The symbol from which the members are to be copied.
@internalComponent
@released
*/
Symbol::Symbol(Symbol& aSymbol)
{
	memcpy(this, &aSymbol, sizeof(aSymbol));
	
	iSymbolName = new char[strlen(aSymbol.SymbolName()) + 1];
	strcpy(iSymbolName, aSymbol.SymbolName());
	
	if(aSymbol.Comment())
	{
		iComment = new char[strlen(aSymbol.Comment()) + 1];
		strcpy(iComment, aSymbol.Comment());
	}

	if(aSymbol.ExportName())
	{
		iExportName = new char[strlen(aSymbol.ExportName()) + 1];
		strcpy(iExportName, aSymbol.ExportName());
	}
}

/**
This constructor sets the symbol members.
@internalComponent
@released
*/
Symbol::Symbol(int symbolStatus,char *name,char *exportName,int ordinalNo,bool r3unused,bool absent,int symbolType,char *comment, PLUINT32 aSz)\
	:iSize (aSz)
{
	if(symbolStatus==0)
	{
		iSymbolStatus=Matching;
	}
	else if(symbolStatus==1)
	{
		iSymbolStatus=Missing;
	}
	else
	{
		iSymbolStatus=New;
	}
	iSymbolName=name;
	iExportName=exportName;
	iOrdinalNumber=ordinalNo;
	iR3Unused=r3unused;
	iAbsent=absent;
	if(symbolType==0)
	{
		iSymbolType=SymbolTypeCode;
	}
	else if(symbolType==1)
	{
		iSymbolType=SymbolTypeData;
	}
	else
	{
		iSymbolType=SymbolTypeNotDefined;
	}
	iComment=comment;
}

/**
This destructor frees the symbol members.
@internalComponent
@released
*/
Symbol::~Symbol()
{
		delete [] iSymbolName;
		delete [] iExportName;
		delete [] iComment;
}

/**
This function sets the symbol name.
@param aSymbolName - The symbol name
@internalComponent
@released
*/
void Symbol::SetSymbolName(char *aSymbolName) 
{
	iSymbolName = new char[strlen(aSymbolName)+1];
	strcpy(iSymbolName, aSymbolName);
}

/**
This function compares the symbol for equality.
@param aSym - The symbol that is compared with this symbol
Return - It returns true if the 2 symbols are equal.
@internalComponent
@released
*/
bool Symbol::operator==(const Symbol* aSym) const {
	if(strcmp(iSymbolName, aSym->iSymbolName))
		return false;
	if( iSymbolType != aSym->iSymbolType )
		return false;

	return true;
}

/**
This function returns the symbol name.
@internalComponent
@released
*/
const char* Symbol::SymbolName() const {
	return iSymbolName;
}

/**
This function returns the aliased symbol name.
@internalComponent
@released
*/
const char* Symbol::ExportName() {
	 return iExportName;
}

/**
This function returns the ordinal number of the symbol.
@internalComponent
@released
*/
PLUINT32 Symbol::OrdNum() const {
	 return iOrdinalNumber;
}

/**
This function returns if the symbol is code or a data symbol.
@internalComponent
@released
*/
SymbolType Symbol::CodeDataType() { 
	return iSymbolType;
}

/**
This function returns if r3unused is true.
@internalComponent
@released
*/
bool Symbol::R3unused() { 
	return iR3Unused;
}

/**
This function returns if the symbol is marked absent in the def file.
@internalComponent
@released
*/
bool Symbol::Absent() { 
	return iAbsent;
}

/**
This function sets the symbol to be absent.
@param aAbsent - bool value
@internalComponent
@released
*/
void Symbol::SetAbsent(bool aAbsent) { 
	iAbsent = aAbsent;
}

/**
This function returns the comment against this def file.
@internalComponent
@released
*/
char* Symbol::Comment() { 
	return iComment;
}

/**
This function returns the symbol is a matching/missing/new symbol in the def file.
@internalComponent
@released
*/
int Symbol::GetSymbolStatus() { 
	return  iSymbolStatus;
}

/**
This function sets the ordinal number for this symbol.
@internalComponent
@released
*/
void Symbol::SetOrdinal(PLUINT32 aOrdinalNum) {
	iOrdinalNumber=aOrdinalNum;
}

/**
This function sets the status of the symbol i.e., whether it is 
a matching/missing/new symbol.
@internalComponent
@released
*/
void Symbol::SetSymbolStatus(SymbolStatus aSymbolStatus) {
	iSymbolStatus = aSymbolStatus;
}

/**
This function sets the symbol name.
@param aSymbolName - Symbol Name
@internalComponent
@released
*/
void Symbol::SymbolName(char *aSymbolName)
{ 
	iSymbolName = aSymbolName;
}
/**
This function sets the export name of the symbol.
@param aComment - aExportName
@internalComponent
@released
*/
void Symbol::ExportName(char *aExportName)
{ 
	iExportName = aExportName;
}

/**
This function sets the comment against the symbol.
@param aComment - aComment
@internalComponent
@released
*/
void Symbol::Comment(char *aComment)
{ 
	iComment = aComment;
}

/**
This function sets the symbol type if it is Code or Data symbol.
@param aType - Symbol Type
@internalComponent
@released
*/
void Symbol::CodeDataType(SymbolType aType) 
{
	iSymbolType = aType;
}

/**
This function sets if R3Unused is true for this symbol.
@param aR3Unused - bool value
@internalComponent
@released
*/
void Symbol::R3Unused(bool aR3Unused)
{
	iR3Unused = aR3Unused;
}

/**
This function sets if R3Unused is true for this symbol.
@param aSize - size of the symbol
@internalComponent
@released
*/
void Symbol::SetSymbolSize(PLUINT32 aSize){
	iSize = aSize;
}

/**
This function gets the size of this symbol.
@internalComponent
@released
*/
PLUINT32 Symbol::SymbolSize(){
	return iSize;
}
