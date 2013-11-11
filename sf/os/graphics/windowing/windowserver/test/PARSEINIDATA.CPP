// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Parse the ini file
// 
//

/**
@file
@test
@internalComponent
*/

#include "PARSEINIDATA.H"
#include <f32file.h>
#include <e32std.h>

// Default directory to look for INI file
_LIT(KIniFileDir,"Z:\\System\\Data\\");

// Constant Value changed for DEF047130 fix
const TInt KTokenSize = 256;

enum TIniPanic
	{
	ESectionNameTooBig,
	EKeyNameTooBig,
	};

//
void Panic(TIniPanic aPanic)
	{
	_LIT(CIniData,"CIniData");
	User::Panic(CIniData,aPanic);
	}

/**
Constructor
*/
CIniData::CIniData() : iPtr(NULL,0)
	{
	__DECLARE_NAME(_S("CIniData"));
	}

/**
Destructor.
Frees the resources located in second-phase constructor
*/
CIniData::~CIniData()
	{
	delete (TText*)iPtr.Ptr();
	delete iToken;
	delete iName;
	}

/**
 Creates, and returns a pointer to CIniData object, leave on failure
 @param aName the name of the file which contains the ini data
 @param aDelimeter	Delimiter used in wsini.ini file between variable and value
 @return A pointer to the CiniData object
*/
CIniData* CIniData::NewL(const TDesC& aName, char aDelimiter)
	{
	CIniData* p = new(ELeave) CIniData;
	CleanupStack::PushL(p);
	p->ConstructL(aName, aDelimiter);
	CleanupStack::Pop();
	return p;
	}

/**
 Second-phase constructor.
 The function attempts to allocate a buffer and Read file's contents into iPtr
 @param aName the name of the file which contains the ini data
 @param aDelimeter	Delimiter used in wsini.ini file between variable and value
 @leave One of the system-wide error codes
*/
void CIniData::ConstructL(const TDesC& aName, char aDelimiter)
	{
	//Set delimiter
	iDelimiter = aDelimiter;
	
 	// Allocate space for token
	iToken=HBufC::NewL(KTokenSize+2);	// 2 extra chars for [tokenName]

	// Connect to file server
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	// Find file, given name
	TFindFile ff(fs.iObj);
	User::LeaveIfError(ff.FindByDir(aName, KIniFileDir));
	iName = ff.File().AllocL();

	// Open file
	TAutoClose<RFile> file;
	TInt size;
	User::LeaveIfError(file.iObj.Open(fs.iObj,*iName,EFileStreamText|EFileShareReadersOrWriters));
	file.PushL();

	// Get file size and read in
	User::LeaveIfError(file.iObj.Size(size));
	TText* data = (TText*)User::AllocL(size);
	iPtr.Set(data, size/sizeof(TText), size/sizeof(TText));
	TPtr8 dest((TUint8*)data, 0, size);
	User::LeaveIfError(file.iObj.Read(dest));
	TUint8* ptr = (TUint8*)data;

	//
	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if(size>=(TInt)sizeof(TText) && iPtr[0]==0xFEFF)
	{
		// UNICODE Text file so lose the FFFE
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, size/sizeof(TText)-1, size/sizeof(TText)-1);
	}
	else if(size)
	{
		// NON-UNICODE so convert to UNICODE
		TText* newdata = (TText*)User::AllocL(size*sizeof(TText));
		iPtr.Set(newdata, size, size);
		TInt i;
		for(i = 0 ; i<size ; ++i)
			iPtr[i] = ptr[i];
		delete data;
	}

	file.Pop();
	fs.Pop();
}

/**
Find a text value from given aKeyName regardless the section in the ini data file
@param aKeyName Key being searched for
@param aResult On return, contains the text result 
@return ETrue if found, otherwise EFalse
*/
TBool CIniData::FindVar(const TDesC& aKeyName, TPtrC& aResult)
	{
	// Call with no section, so starts at beginning
	if (FindVar((TDesC&)KNullDesC , aKeyName, aResult))
		return(ETrue);
	else
		return(EFalse);
	}

/**
Find a text value from given aKeyName and aSecName in the ini data file
@param aSectName Section containing key
@param aKeyName Key being searched for in aSectName
@param aResult On return, contains the text result 
@return ETrue if found, otherwise EFalse
*/
TBool CIniData::FindVar(const TDesC& aSectName,const TDesC& aKeyName,TPtrC& aResult)
	{

	__ASSERT_DEBUG(aSectName.Length()<=KTokenSize,Panic(ESectionNameTooBig));
	__ASSERT_DEBUG(aKeyName.Length()<=KTokenSize,Panic(EKeyNameTooBig));

	TInt posStartOfSection(0);
	TInt posEndOfSection(iPtr.Length()); // Default to the entire length of the ini data
	TPtrC SearchBuf;

	// If we have a section, set pos to section start
	TInt posI(0);	// Position in internal data Buffer
	if( aSectName.Length() > 0 )
		{
		TBool FoundSection(false);
		while ( ! FoundSection )
			{
			// Move search buffer to next area of interest
			SearchBuf.Set(iPtr.Mid(posI));

			// Make up token "[SECTIONNAME]", to search for
			TPtr sectionToken = iToken->Des();
			_LIT(sectionTokenFmtString,"[%S]");
			sectionToken.Format(sectionTokenFmtString,&aSectName);

			// Search for next occurrence of aSectName
			TInt posSB = SearchBuf.Find(sectionToken);

			if (posSB == KErrNotFound)
				return(EFalse);

			// Check this is at beginning of line (ie. non-commented)
			// ie. Check preceding char was LF
			if(posSB>0)
				{
				// Create a Buffer, starting one char before found subBuf
				TPtrC CharBefore(SearchBuf.Right(SearchBuf.Length()-posSB+1));
				// Check first char is end of prev
				if(CharBefore[0] == '\n')
					{
					FoundSection = ETrue;		// found
					posI = posI + posSB;
					}
				else
					{
					posI = posI + posSB + 1;	// try again
					}
				}
			else
				{
				FoundSection = ETrue;
				}

			}	// while ( ! FoundSection ) 

		// Set start of section, after section name, (incl '[' and ']')
		posStartOfSection = posI + aSectName.Length() + 2;

		// Set end of section, by finding begin of next section or end
		SearchBuf.Set(iPtr.Mid(posI));
		_LIT(nextSectionBuf,"\n[");
		TInt posSB = SearchBuf.Find(nextSectionBuf);
		if(posSB != KErrNotFound)
			{
			posEndOfSection = posI + posSB;
			}
		else
			{
			posEndOfSection = iPtr.Length();
			}

		}	// if( aSectName.Length() > 0 )

	// Look for key in ini file data Buffer
	posI = posStartOfSection;
	TBool FoundKey(false);
	while ( ! FoundKey )
		{
		// Search for next occurrence of aKeyName
		SearchBuf.Set(iPtr.Mid(posI,posEndOfSection-posI));
		TInt posSB = SearchBuf.Find(aKeyName);

		// If not found, return
		if (posSB == KErrNotFound)
			return(EFalse);

		// Check this is at beginning of line (ie. non-commented)
		// ie. Check preceding char was CR or LF
		if(posSB>0)
			{
			// Create a Buffer, starting one char before found subBuf
			TPtrC CharBefore(SearchBuf.Right(SearchBuf.Length()-posSB+1));
			// Check if the first char is end of prev and also check 
			// if the token found is not a substring of another string  
			TBool beginningOK = ((CharBefore[0] == '\n') || (CharBefore[0] == ' ') || (CharBefore[0] == '\t'));
			TBool endingOK = ((CharBefore[aKeyName.Length()+1] == iDelimiter) || (CharBefore[aKeyName.Length()+1] == ' ') || (CharBefore[aKeyName.Length()+1] == '\t'));
			if (beginningOK && endingOK)
				{
				FoundKey = ETrue;
				posI = posI + posSB;
				}
			else
				{
				posI = posI + posSB + 1;
				}
			}
		else
			{
			FoundKey = ETrue;
			}
		}	// while ( ! FoundKey )

	// Set pos, to just after iDelimiter sign
	SearchBuf.Set(iPtr.Mid(posI));
	TInt posSB = SearchBuf.Locate(iDelimiter);
	if(posSB==KErrNotFound)		// Illegal format, should flag this...
		return(EFalse);

	// Identify start and end of data (EOL or EOF)
	posI = posI + posSB + 1;	// 1 char after iDelimiter
	TInt posValStart = posI;
	TInt posValEnd;
	SearchBuf.Set(iPtr.Mid(posI));
	posSB = SearchBuf.Locate('\r');
	if(posSB != KErrNotFound)
		{
		posValEnd = posI + posSB;
		}
	else
		{
		posValEnd = iPtr.Length();
		}

	// Check we are still in the section requested
	if( aSectName.Length() > 0 )
		{
		if( posValEnd > posEndOfSection )
			{
			return(EFalse);
			}
		}
	// Parse Buffer from posn of key
	// Start one space after '='
	TLex lex(iPtr.Mid(posValStart, posValEnd-posValStart));
	lex.SkipSpaceAndMark();		// Should be at the start of the data
	aResult.Set(lex.MarkedToken().Ptr(),posValEnd-posValStart - lex.Offset() );
	return(ETrue);
	}

/**
Find an integer value from given aKeyName regardless the section in the ini data file
@param aKeyName Key being searched for
@param aResult On return, contains the TInt result 
@return ETrue if found, otherwise EFalse
*/
TBool CIniData::FindVar(const TDesC& aKeyName, TInt& aResult)
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aKeyName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult) == KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}

/**
Find an integer value from given aKeyName and aSecName in the ini data file
@param aSectName Section containing key
@param aKeyName Key being searched for  in aSectName
@param aResult On return, contains TInt result 
@return ETrue if found, otherwise EFalse
*/
TBool CIniData::FindVar(const TDesC& aSection,const TDesC& aKeyName,TInt& aResult)
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aSection,aKeyName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult) == KErrNone)
			return(ETrue);
		}
	return(EFalse);
}
