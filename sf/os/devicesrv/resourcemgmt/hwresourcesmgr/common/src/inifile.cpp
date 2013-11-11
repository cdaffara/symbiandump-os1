// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the class CIniFile for accessing ini file.
// 
//

/**
 @file
*/

#include "inifile.h"
#include <f32file.h>

CIniFile::CIniFile() 
 :	iPtr(NULL,0)
	{
	}

EXPORT_C CIniFile::~CIniFile()
	{
	delete (TText*)iPtr.Ptr();
	delete iToken;
	delete iName;
	}
	
EXPORT_C CIniFile* CIniFile::NewL(const TDesC& aName, const TDesC& aPath)
/**
 * Factory function for CIniFile.which passes in the path of the file and its name.
 *
 * @param aName The name of the ini file to be used, e.g. "TRPAT.INI".
 */
	{
	CIniFile* p = new(ELeave) CIniFile();
	CleanupStack::PushL(p);
	p->ConstructL(aName, aPath);
	CleanupStack::Pop(p);

	return p;
	}

void CIniFile::ConstructL(const TDesC& aName, const TDesC& aPath)
/**
 * Allocate a buffer and Read file's contents into iPtr
 *
 */
	{
    iToken = HBufC::NewL(KTokenSize+2);	// 2 extra chars for []

	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	TFindFile ff(fs.iObj);

	// For security reasons ini files are in the private directory for the
	// comms-infras/rootserver (C32exe) process.
	TBuf<KMaxPath> privatePath;
	
	privatePath = aPath;
	User::LeaveIfError(ff.FindByDir(aName, privatePath));
	
	iName=ff.File().AllocL();

	TAutoClose<RFile> file;
	TInt size;
	User::LeaveIfError(file.iObj.Open(fs.iObj,*iName,EFileStreamText|EFileRead|EFileShareReadersOnly));
	file.PushL();

	User::LeaveIfError(file.iObj.Size(size));


	TText* data = REINTERPRET_CAST(TText*, User::AllocL(size));
	iPtr.Set(data, size/sizeof(TText), size/sizeof(TText));
	TPtr8 dest(REINTERPRET_CAST(TUint8*,data), 0, size);
	User::LeaveIfError(file.iObj.Read(dest)); 

	TUint8* ptr = REINTERPRET_CAST(TUint8*,data);

	//
	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if(size>= STATIC_CAST(TInt,sizeof(TText)) && iPtr[0]==0xFEFF)
   		{
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, size/sizeof(TText)-1, size/sizeof(TText)-1);
		}
	else if(size)
		{
		TText* newdata = REINTERPRET_CAST(TText*,
			                              User::AllocL(size*sizeof(TText)));
		iPtr.Set(newdata, size, size);
		TInt i;
		for(i=0 ; i<size ; ++i)
			iPtr[i]=ptr[i];
		delete data;
		}

	file.Pop();
	fs.Pop();
	}

EXPORT_C TBool CIniFile::FindVar(const TDesC &aSection,
						const TDesC &aVarName,
						TPtrC &aResult)
//
// Find a variable's value given a section name and a var name
//
	{
	__ASSERT_DEBUG(aSection.Length()<=KTokenSize, User::Panic(_L("CIniFile"),ESectionNameTooBig));
	__ASSERT_DEBUG(aVarName.Length()<=KTokenSize,User::Panic(_L("CIniFile"),EVarNameTooBig));

	TPtr sectionToken=iToken->Des();
	_LIT(KSectionTokenString,"[%S]");
	sectionToken.Format(KSectionTokenString,&aSection);
	TInt sectionStart=iPtr.Find(sectionToken);
	TInt ret = ETrue;
	if (sectionStart==KErrNotFound)
		ret = EFalse;
	else
		{		
		TPtrC section=iPtr.Mid(sectionStart);
		sectionStart+=section.Find(TPtrC(_S("]")));
		if (sectionStart==KErrNotFound)
			ret = EFalse;
		else
			{
			sectionStart++;
			section.Set(iPtr.Mid(sectionStart));
			
			TInt sectionEnd=section.Find(TPtrC(_S("[")));
			if (sectionEnd==KErrNotFound)
				sectionEnd=iPtr.Length()-sectionStart;
			else
				sectionEnd--;
			section.Set(iPtr.Mid(sectionStart,sectionEnd));
			TPtr varToken=iToken->Des();
			_LIT(KVarTokenString1,"%S=");
			_LIT(KVarTokenString2,"%S =");
			varToken.Format(KVarTokenString1,&aVarName);
			TInt pos=section.Find(varToken);

			if (pos == KErrNotFound)
				{
				varToken.Format(KVarTokenString2,&aVarName);
				pos=section.Find(varToken);
				}

			if (pos==KErrNotFound)
				{
				ret = EFalse;
				}
			else
				{
				// 'lex' points at the start of the data
				TPtrC lex(section.Mid(pos));
				TInt startpos = lex.Locate(TChar('='));
				startpos++; // startpos points immediately after the =.
				while ( TChar(lex[startpos]).IsSpace() )
					startpos++; // skip to start of data
				TInt endpos = lex.Locate(TChar('\n')); // assumes \n is after =.
				if ( endpos == KErrNotFound ) // may not be \n on last line
					endpos = lex.Length() + 1;
				aResult.Set(lex.Mid(startpos).Ptr(),endpos-startpos-1);
				}
			}
		}

	return ret;
	}

//
// End of file
