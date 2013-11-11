// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32std.h>
#include "server.h"
#include "inifile.h"

// uncomment so that if the wsini.ini file is not found on drive Wserv loaded from or drive Z: (if different)
// it will be searched for on drive C: (if different from Wserv's drive)
//#define LOAD_INI_FILE_FROM_DRIVE_Z_OR_C


GLREF_D CDebugLogBase *wsDebugLog;

_LIT(KDefaultSectionName,"DEFAULT");
_LIT(KCommentMarker,"//");
_LIT(KScreenSectionName,"SCREEN");
const TUint16 KNewSection('[');
const TUint16 KSpaceChar(' ');
const TUint16 KNewSection2(']');
const TInt KDefaultSectionNumber(0);
const TInt KDefaultScreenId(-1);


CIniFile::CIniFile()
	{}

CIniFile::~CIniFile()
	{
	FreeData();
	}

CIniSection * CIniFile::FindSection(TInt aScreen)
	{
	for (TInt sect = 0; sect < iSectionArray.Count(); ++sect)
		{
		if (iSectionArray[sect]->Screen() == aScreen)
			{
			return iSectionArray[sect];
			}
		}
	return NULL;
	}

CIniSection * CIniFile::FindSection(const TDesC& aName)
	{
	for (TInt sect = 0; sect < iSectionArray.Count(); ++sect)
		{
		if (iSectionArray[sect]->Screen() == KDefaultScreenId && !iSectionArray[sect]->Name().CompareF(aName))
			{
			return iSectionArray[sect];
			}
		}
	return NULL;
	}

/* Processes a .ini file entry section name.

 @return the corresponding index.
 @param aLine Input line of text from the .ini file, stripped of comments & excess whitespace.
 @leave KErrNoMemory
 */
CIniSection * CIniFile::AddOrFindIniSectionL(TPtr& aSectionName)
	{
	aSectionName.Trim();

	// DEFAULT section
	if (aSectionName.CompareF(KDefaultSectionName) == 0)
		{ 
		return iSectionArray[KDefaultSectionNumber];
		}

	// SCREENx section
	if (0 == aSectionName.FindF(KScreenSectionName))
		{
		TLex lex(aSectionName.Mid(KScreenSectionName().Length()));
		TInt screenNum;
		lex.SkipSpace();
		if (lex.Val(screenNum) == KErrNone)
			{
			return AddOrFindScreenSectionL(screenNum);
			}
		}
		
	// other section
	return AddOrFindNamedSectionL(aSectionName);
	}

CIniSection * CIniFile::CreateSectionL(TInt aScreen)
	{
	CIniSection* newSection = new (ELeave) CIniSection(aScreen);
	CleanupStack::PushL( newSection ) ;
	newSection->ConstructL() ;
	User::LeaveIfError(iSectionArray.Append(newSection));
	CleanupStack::Pop( newSection ) ;
	if (aScreen + 1 > iScreenCount)
		iScreenCount = aScreen + 1;
	return newSection;
	}

CIniSection * CIniFile::CreateSectionL(const TDesC& aName)
	{
	CIniSection* newSection = new (ELeave) CIniSection(KDefaultScreenId);
	CleanupStack::PushL( newSection ) ;
	newSection->ConstructL(aName) ;
	User::LeaveIfError(iSectionArray.Append(newSection));
	CleanupStack::Pop( newSection ) ;
	return newSection;
	}


void CIniFile::doConstructL(RFile &aFile)
	{
	TFileText textFile;
	textFile.Set(aFile);
	const TInt KMaxIniLine=256;
	TBuf<KMaxIniLine> readLine;
	TBool first=ETrue;

	// always have a [DEFAULT] section
	CIniSection * currentSection = CreateSectionL(KDefaultSectionName);

	FOREVER
		{
		TInt err=textFile.Read(readLine);
		if (err==KErrEof)
			break;
		User::LeaveIfError(err);

		if (readLine.Length()>0)
			{
			if (first && (readLine[0]==0xFFFE || readLine[0]==0xFEFF))
				readLine.Delete(0,1);
			
			// Comment marker "//" indicates the rest of the line should be discarded
			TInt commentStart = readLine.Find(KCommentMarker);
			if (commentStart != KErrNotFound)
				{
				readLine.Delete(commentStart, readLine.Length());
				}

			// compact unnecessary whitespace
			readLine.TrimAll();

			// anything left in buffer?
			if (readLine.Length() > 0)
				{
				// section name requires "[" and "]"
				if (readLine[0] == KNewSection && readLine.LocateReverse(KNewSection2) == readLine.Length() - 1)
					{
					TPtr nameText = readLine.MidTPtr(1, readLine.Length() - 2); // strip [ and ]
					currentSection = AddOrFindIniSectionL(nameText);
					}
				else
					{
					if (currentSection)
						{
						currentSection->AddVariableL(readLine);
						}
					}
				}
			first=EFalse;
			}
		}

	if (iScreenCount == 0)
		{
		iScreenCount = 1;
		}
	}

CIniFile* CIniFile::NewL()
    {
    CIniFile* self = new (ELeave) CIniFile();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CIniFile::FreeData() 
	{
	iSectionArray.ResetAndDestroy() ;
	iScreenCount = 0;
	}

void errFreeData(TAny *aIniFile)
	{
	((CIniFile *)aIniFile)->FreeData();
	}

HBufC* IniFileSearchPathLC()
	{
	_LIT(KPath,"\\SYSTEM\\DATA\\");
	_LIT(KPathSep,";");
	const TInt KLengthPerPath = 2 + KPath().Length();
	// work out which drive Wserv loaded from
	RProcess self;
	TFileName myPath = self.FileName();
	TParsePtrC myDrive(myPath);
	TDriveUnit myDriveUnit(myDrive.Drive());
	// need extra buffer space for search paths for drives Z: or C: ?
#if defined(LOAD_INI_FILE_FROM_DRIVE_Z_OR_C)
	TInt numPaths = 2;
	if (myDriveUnit != EDriveZ && myDriveUnit != EDriveC)
		{
		numPaths += 1;
		}
#else
	TInt numPaths = 1;
	if (myDriveUnit != EDriveZ)
		{
		numPaths += 1;
		}
#endif
	HBufC* searchPath = HBufC::NewLC(numPaths * KLengthPerPath + (numPaths - 1) * KPathSep().Length());
	TPtr pPath(searchPath->Des());
	pPath.Append(myDrive.Drive());
	pPath.Append(KPath);
	if (myDriveUnit != EDriveZ)
		{
		pPath.Append(KPathSep);
		pPath.Append(TDriveUnit(EDriveZ).Name());
		pPath.Append(KPath);
		}
#if defined(LOAD_INI_FILE_FROM_DRIVE_Z_OR_C)
	if (myDriveUnit != EDriveC)
		{
		pPath.Append(KPathSep);
		pPath.Append(TDriveUnit(EDriveC).Name());
		pPath.Append(KPath);
		}
#endif
	return searchPath;
	}

void CIniFile::ConstructL()
	{
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.iObj.SetNotifyUser(EFalse);
	fs.PushL();
	HBufC* searchPath = IniFileSearchPathLC();
	_LIT(KFileName,"WSINI.INI");
	TFindFile findinifile(fs.iObj);
	TInt err=findinifile.FindByPath(KFileName,searchPath);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(searchPath);
 	TAutoClose<RFile> file;
	User::LeaveIfError(file.iObj.Open(fs.iObj,findinifile.File(),EFileStreamText|EFileRead));
	file.PushL();
	CleanupStack::PushL(TCleanupItem(errFreeData,this));
	doConstructL(file.iObj);
	CleanupStack::Pop(); // TCleanupItem
	file.Pop();
	fs.Pop();
	}

/* If the Section for the screen exists find the data, otherwise create a new data structure.

 @param aScreen Screen number
 @return index to section
 @leave KErrNoMemory
 */
CIniSection * CIniFile::AddOrFindScreenSectionL(TInt aScreen)
	{
	CIniSection * section = FindSection(aScreen);
	if (!section)
		section = CreateSectionL(aScreen);
	return section;
	}

/* If the Section exists find the data, otherwise create a new data structure.

 @param aName section name
 @return index to section
 @leave KErrNoMemory
 */
CIniSection * CIniFile::AddOrFindNamedSectionL(const TDesC& aName)
	{
	CIniSection * section = FindSection(aName);
	if (!section)
		section = CreateSectionL(aName);
	return section;
	}

TBool CIniFile::FindVar(const TDesC &aVarName, TPtrC &aResult)
	{
	return iSectionArray[KDefaultSectionNumber]->FindVar(aVarName, aResult);
	}


TBool CIniFile::FindVar(const TDesC &aVarName, TInt &aResult)
	{
	return iSectionArray[KDefaultSectionNumber]->FindVar(aVarName, aResult);
	}

TBool CIniFile::FindVar(const TDesC &aVarName)
//
// Used to simply detect the presence of the specified variable name
//
	{
	TPtrC ptr(NULL,0);
	return FindVar(aVarName, ptr);
	}

// FindVar in [SCREENx] sections
TBool CIniFile::FindVar( TInt aScreen, const TDesC &aVarName)
	{
	TPtrC ptr(NULL,0);
	return FindVar(aScreen, aVarName, ptr);
	}

TBool CIniFile::FindVar( TInt aScreen, const TDesC& aVarName, TPtrC &aResult )
	{
	CIniSection * section = FindSection(aScreen);
	TBool found = EFalse;
	if (section)
		found = section->FindVar(aVarName, aResult);
	if (!found)
		found = FindVar(aVarName, aResult);
	return found;
	}

TBool CIniFile::FindVar(TInt aScreen, const TDesC &aVarName, TInt &aResult)
	{
	CIniSection * section = FindSection(aScreen);
	TBool found = EFalse;
	if (section)
		found = section->FindVar(aVarName, aResult);
	if (!found)
		found = FindVar(aVarName, aResult);
	return found;
	}

// FindVar in named sections
TBool CIniFile::FindVar(const TDesC& aSectionName, const TDesC &aVarName)
	{
	TPtrC ptr(NULL,0);
	return FindVar(aSectionName, aVarName, ptr);
	}

TBool CIniFile::FindVar(const TDesC& aSectionName, const TDesC& aVarName, TPtrC &aResult )
	{
	CIniSection * section = FindSection(aSectionName);
	TBool found = EFalse;
	if (section)
		found = section->FindVar(aVarName, aResult);
	if (!found)
		found = FindVar(aVarName, aResult);
	return found;
	}

TBool CIniFile::FindVar(const TDesC& aSectionName, const TDesC &aVarName, TInt &aResult)
	{
	CIniSection * section = FindSection(aSectionName);
	TBool found = EFalse;
	if (section)
		found = section->FindVar(aVarName, aResult);
	if (!found)
		found = FindVar(aVarName, aResult);
	return found;
	}

TInt CIniFile::NumberOfScreens() const 
	{
  	return iScreenCount;
	}


// CIniSection.
// ini file structure is now in sections like this
//
// [DEFAULT]
// varname value
// varname2 value2
// [SCREEN0]
// screenvar value
// etc
//
// CIniSection represents a section - i.e. section name and content pairs.  
// Content pairs are as ini file was previously (so use same code)
// [default] section name is optional to support backwards compatibility
// if no sutable value is found in a [screenN] section the [default] section will be searched.


CIniSection::CIniSection(TInt aScreen) : iScreen(aScreen)
	{}

void CIniSection::ConstructL()
	{
	iPtrArray = new (ELeave) CArrayPtrFlat<TDesC>(8) ;
	}

void CIniSection::ConstructL(const TDesC& aName)
	{
	iName.CreateL(aName);
	ConstructL();
	}

CIniSection::~CIniSection()
	{
	iName.Close();
	iPtrArray->ResetAndDestroy() ;
	delete iPtrArray ;
	}

inline TInt CIniSection::Screen() const
	{ return iScreen; }

inline const TDesC& CIniSection::Name() const
	{ return iName; }

TBool CIniSection::FindVar( const TDesC& aVarName, TPtrC &aResult )
	{
	if (iPtrArray)
		{
		TInt index(KErrNotFound);
		
		if (FindVarName(aVarName, index))
			{
			TLex lex((*iPtrArray)[index]->Mid(aVarName.Length()));
			lex.SkipSpace();
			aResult.Set(lex.Remainder());

			if (wsDebugLog)
				{
				wsDebugLog->IniFileSettingRead(iScreen, aVarName, ETrue, aResult);
				}
			return(ETrue);
			}
		}

	if (wsDebugLog)
		{
		wsDebugLog->IniFileSettingRead(iScreen, aVarName, EFalse, KNullDesC);
		}
	return(EFalse);
	}

/*
 create a TPtrC with just the first word (variable name) in the given string
 */
TPtrC CIniSection::VarName(const TDesC& aVarString)
	{
	TInt varLength = aVarString.Locate(KSpaceChar);
	if (varLength == KErrNotFound)
		{
		varLength = aVarString.Length();
		}
	return aVarString.Left(varLength);
	}


TBool CIniSection::FindVar(const TDesC &aVarName, TInt &aResult)
	{
	TPtrC ptr(NULL,0);
	// do text Find
	if (FindVar(aVarName, ptr))
		{
		TLex lex(ptr);
		_LIT(HexFormatCheck,"0x");
		TPtrC hexPtr(HexFormatCheck);
		if(ptr.Left(2) != hexPtr)
			{
			if (lex.Val(aResult) == KErrNone)
				{
				return ETrue;
				}
			}
		else
			{
			lex.SkipAndMark(2); //To skip 0x in hex code
			if (lex.Val((TUint32&)aResult, EHex) == KErrNone)
				{
				return ETrue;
				}
			}
		}

	return EFalse;
	}


/*
 Find variable name in sorted array, using binary search
 @param aVarName variable name to search for, must have any space and variable value stripped.
 @param aIndex output index of matching or preceeding item
 @return
 */
TBool CIniSection::FindVarName(const TDesC& aVarName, TInt& aIndex)
	{
	// Binary Search
	// left is lowest index to include, right is highest index + 1;
	TInt left = 0;
	TInt right = iPtrArray->Count();

	while (right > left)
		{
		TInt middle = (left + right)>>1;
		// compare to start of variable string
		TPtrC cmpString = VarName(*(*iPtrArray)[middle]);
		TInt cmp = aVarName.CompareF(cmpString);

		if (cmp == 0)
			{
			aIndex = middle;
			return ETrue;
			}
		else if (cmp > 0)
			{
			left = middle + 1;
			}
		else
			{
			right = middle;
			}
		}

	aIndex = right;
	return EFalse;
	}

void CIniSection::AddVariableL(const TDesC& aNewVariable)
	{
	// use variable name only for search
	TPtrC varName = VarName(aNewVariable);
	TInt index(0);
	
	// ignore duplicate definitions
	if (0 == FindVarName(varName, index))
		{ // insert in sorted array
		HBufC* hbuf = aNewVariable.AllocLC() ;
		iPtrArray->InsertL(index, hbuf);
		CleanupStack::Pop(hbuf);
		}
	}

