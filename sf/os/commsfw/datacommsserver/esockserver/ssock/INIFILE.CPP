// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <es_ini.h>
#include <f32file.h>
#include <cfextras.h>


/**
@internalComponent
*/
class TFileListIter
	{
public:
	TFileListIter()
		{
		iList = 0;
		iIsCDir = EFalse;
		iIdx = 0;
		}
	void Set(const CDir* aList)
		{
		iList = static_cast<const void*>(aList);
	    iIdx = 0;
		iIsCDir = ETrue;
		}
	void Set(const CommsFW::COwnEntryList* aList)
		{
		iList = static_cast<const void*>(aList);
	    iIdx = 0;
		iIsCDir = EFalse;
		}
	const TEntry* operator++(TInt)
		{
		const TEntry* entry = NULL;
		if(iIsCDir)
			{
			const CDir* list = reinterpret_cast<const CDir*>(iList);
			if(list != NULL && iIdx < list->Count())
				{
				entry = &(*list)[iIdx++];
				}
			}
		else
			{
			const CommsFW::COwnEntryList* list = reinterpret_cast<const CommsFW::COwnEntryList*>(iList);
			if(list != NULL && iIdx < list->Count())
				{
				entry = &(*list)[iIdx++];
				}
			}
		return entry;
		}
private:
	const void* iList;
	TBool iIsCDir;
	TInt iIdx;
	};

const TInt KTokenSize=32;

// Warning !! redifinition required
const TInt32 KUidCommsProcess = 0x101F7989;

/*
@internalComponent
*/
enum TIniPanic
	{
	ESectionNameTooBig,
	EVarNameTooBig,
	};

/**
@internalComponent
*/
void Panic(TIniPanic aPanic)
	{
	_LIT(cESockIniData,"CESockIniData");
	User::Panic(cESockIniData,aPanic);
	}


EXPORT_C CESockIniData::CESockIniData() 
: iPtr(NULL,0)
	{
	}

EXPORT_C CESockIniData::CESockIniData(const CommsFW::COwnEntryList* aFileList) 
: iPtr(NULL,0),
  iFileList(aFileList)
  	{
	}

EXPORT_C CESockIniData::~CESockIniData()
	{
	delete (TText*)iPtr.Ptr();
	delete iToken;
	delete iName;
	}

EXPORT_C CESockIniData* CESockIniData::NewL(const TDesC& aName)
	{
	CESockIniData* p=new(ELeave) CESockIniData;
	CleanupStack::PushL(p);
	p->ConstructL(aName);
	CleanupStack::Pop();
	return p;
	}

EXPORT_C CESockIniData* CESockIniData::NewL(const CommsFW::COwnEntryList* aFileList)
	{
	CESockIniData* p=new(ELeave) CESockIniData(aFileList);
	CleanupStack::PushL(p);
	p->ConstructL(KNullDesC);
	CleanupStack::Pop();
	return p;
	}

EXPORT_C void CESockIniData::ConstructL(const TDesC& aName)
/**
Allocate a buffer and Read file's contents into iPtr
*/
	{
	
    iToken=HBufC::NewL(KTokenSize+2);	// 2 extra chars for [tokenName]
	
	// protocol_buf is used to holf the protocol list
	// and then the whole collated .ini file
	CBufFlat* protocol_buf=CBufFlat::NewL(4);
	CleanupStack::PushL(protocol_buf);
	TInt protocol_pos = 0;
	
	//data_buf is used to temporarily hold the .esk files'bodies
	CBufFlat* data_buf=CBufFlat::NewL(4);
	CleanupStack::PushL(data_buf);
	TInt data_pos = 0;
	
	TInt data_size =0;
	
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	TBuf<100> privatePath;
	TBool isC32Exe = (RThread().SecureId() == KUidCommsProcess);
 	if(isC32Exe)	// If it is C32Exe, uppend private path with ESock
        {
		privatePath = KEsockIniFileDir;
        }
	else
        {
		fs.iObj.PrivatePath(privatePath);
        }
	
	TFindFile ff(fs.iObj);
	TFileListIter iter;
	
	if(aName == ESOCK_INI_DATA || aName.Length() == 0) 
		{
		CDir* ownedFileList = NULL;
		if(iFileList)
			{
			iter.Set(iFileList);
			}
		else
			{
			// Have to build own file list
			if(ff.FindWildByDir(KEsockWildCard, privatePath, ownedFileList) == KErrNone)
				{
				CleanupStack::PushL(ownedFileList);
				iter.Set(ownedFileList);
				}
			}
		_LIT(KSockManMainSectionName,"sockman");
		_LIT(KProtocolListItemName,"protocols");
		TPtrC protocols;
		TInt err = KErrNone;
		
		_LIT(KTxtComma,",");
		_LIT(KNewLine,"\r\n");
		_LIT(KTxtIniFileStart,"[sockman] protocols= ");
		
		protocol_buf->InsertL(protocol_pos, (TAny*)(&KTxtIniFileStart)->Ptr(), (&KTxtIniFileStart)->Size());
		protocol_pos+= (&KTxtIniFileStart)->Size();

		TBool noBackupDirSearched(EFalse);
		while (err==KErrNone)                
			{
			TBool firstEntry = ETrue;
			while(const TEntry* entry = iter++)	//for (TInt i=0; i < iFileList->Count(); i++)    
				{
				TParse fullentry;
				fullentry.Set(entry->iName,& ff.File(),NULL);      
				TAutoClose<RFile> file;
				User::LeaveIfError(file.iObj.Open(fs.iObj,fullentry.FullName(),EFileStreamText|EFileRead));
				file.PushL();
				User::LeaveIfError(file.iObj.Size(data_size));

				// Following BR1123 we presume that all files are narrow-char only and widen them here
				__ASSERT_COMPILE(sizeof(TText) == 2);
				TUint8* rawBuff = (TUint8*) User::AllocLC(data_size * sizeof(TText));
				TPtr8 rawTPtr(rawBuff, data_size * sizeof(TText));
				User::LeaveIfError(file.iObj.Read(rawTPtr));

				TText* dstPtr = (TText*) rawBuff;
				for(TInt i = data_size - 1; i >= 0; --i)
					{
					dstPtr[i] = rawBuff[i];
					}
				iPtr.Set((TText*) rawBuff, data_size, data_size);
				CleanupStack::Pop();  // Get data off; destructor takes care of freeing iPtr
				
				if(FindVar(KSockManMainSectionName,KProtocolListItemName,protocols))
					{
					//Store the protocol in the protocol dynamic buffer
					if(firstEntry)
						{
						firstEntry = EFalse;
						}
					else
						{
						protocol_buf->ExpandL(protocol_pos, (&KTxtComma)->Size());
						protocol_buf->Write(protocol_pos,(TAny*)(&KTxtComma)->Ptr(), (&KTxtComma)->Size());
						protocol_pos+=(&KTxtComma)->Size();
						}
					protocol_buf->ExpandL(protocol_pos, protocols.Size());
					protocol_buf->Write(protocol_pos, protocols.Ptr(), protocols.Size());
					protocol_pos+=protocols.Size();
					}
				else
					{
					// Protocols line absent; whole of text is "trailer"
					protocols.Set(iPtr.Ptr(), 0);
					data_buf->ExpandL(data_pos, (&KNewLine)->Size());
					data_buf->Write(data_pos, (TAny*)(&KNewLine)->Ptr(), (&KNewLine)->Size());
					data_pos+=(&KNewLine)->Size();
					}
				
				TInt dif = (protocols.Ptr()+protocols.Length()) - iPtr.Ptr();
				
				//Store the rest of the file in the data dynamic buffer
				data_buf->ExpandL(data_pos, (data_size-dif)*sizeof(TText));
				data_buf->Write(data_pos, (protocols.Ptr()+protocols.Length()), (data_size-dif)*sizeof(TText));
				data_pos+= (data_size - dif)*sizeof(TText);
				
				//Delete temporary buffer
				delete (TText*)iPtr.Ptr();
				iPtr.Set(NULL,0,0);
				file.Pop();
				}
			
			if(ownedFileList)
				{
				CleanupStack::PopAndDestroy(ownedFileList);

				//Continue the search in the next drive in the search sequence
				err=ff.FindWild(ownedFileList); 
				
				if (err==KErrNone)
					{
					CleanupStack::PushL(ownedFileList);
					iter.Set(ownedFileList);
					protocol_buf->ExpandL(protocol_pos, (&KTxtComma)->Size());
					protocol_buf->Write(protocol_pos,(TAny*)(&KTxtComma)->Ptr(), (&KTxtComma)->Size());
					protocol_pos+=(&KTxtComma)->Size();
					}
				else if (err==KErrNoMemory)
					{
					User::Leave(err);
					}
				else if(err==KErrNotFound && !noBackupDirSearched)
					{
 					//It's time to look into nobackup directory
 					noBackupDirSearched = ETrue;
 					if(isC32Exe)	// If it is C32Exe, uppend private path with ESock
         				{
 						privatePath = KEsockNoBackupDir;
 						err = ff.FindWildByDir(KEsockWildCard, privatePath, ownedFileList);
 						if(err == KErrNone)
 							{
 							CleanupStack::PushL(ownedFileList);
 							iter.Set(ownedFileList);
 							}
         				}
 					}
				}
			else
			    {
			    err=KErrNotFound; // Break out of while loop
			    }
			}
		
		//Copy all the collated esk bodies into the protocol_buf after the protocol list
		protocol_buf->ExpandL(protocol_pos, data_buf->Size());
		protocol_buf->Write(protocol_pos, data_buf->Ptr(0), data_buf->Size());
		
		//Copy the all collated initialisation information into its final destination
		TText* newinifile = (TText*)User::AllocL(protocol_buf->Size());
		protocol_buf->Read(0, newinifile, protocol_buf->Size());
		
		data_size = protocol_buf->Size() / (TInt) sizeof(TText);
		iPtr.Set(newinifile, data_size, data_size);
		
		fs.Pop();
		}
	else
	// Deals with standard .ini file
		{
		
 		TInt err = KErrNone;
 		err = ff.FindByDir(aName, privatePath);
 		if(err!=KErrNone && isC32Exe)
 			{
 			privatePath = KEsockNoBackupDir;
 			User::LeaveIfError(ff.FindByDir(aName, privatePath));	
 			}
 				
		iName=ff.File().AllocL();
		
		TAutoClose<RFile> file;
		TInt size;
		User::LeaveIfError(file.iObj.Open(fs.iObj,*iName,EFileStreamText|EFileRead));
		file.PushL();
		
		User::LeaveIfError(file.iObj.Size(size));
		TText* data=(TText*)User::AllocL(size);
		iPtr.Set(data, size/(TInt)sizeof(TText), size/(TInt)sizeof(TText));
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
			iPtr.Set(data, size/(TInt)sizeof(TText)-1, size/(TInt)sizeof(TText)-1);
			}
		else if(size)
			{
			// NON-UNICODE so convert to UNICODE
			TText* newdata = (TText*)User::AllocL(size*sizeof(TText));
			iPtr.Set(newdata, size, size);
			TInt i;
			for(i=0 ; i<size ; ++i)
				iPtr[i]=ptr[i];
			delete data;
			}
		
		file.Pop();
		fs.Pop();
		
		}
	CleanupStack::PopAndDestroy(2); // remove protocol_buf - data_buf
	}

EXPORT_C TBool CESockIniData::FindVar(const TDesC &aVarName, TPtrC &aResult) const
	{
	TInt pos=iPtr.Find(aVarName);
	if (pos==KErrNotFound)
		return(EFalse);
	TLex lex(iPtr.Mid(pos));
	lex.SkipCharacters();
	lex.SkipSpaceAndMark();		// Should be at the start of the data
	lex.SkipCharacters();
	aResult.Set(lex.MarkedToken().Ptr(),lex.MarkedToken().Length());
	return(ETrue);
	}

EXPORT_C TBool CESockIniData::FindVar(const TDesC &aVarName, TInt &aResult) const
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aVarName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
	}

EXPORT_C TBool CESockIniData::FindVar(const TDesC &aSection,const TDesC &aVarName,TPtrC &aResult) const
/**
Find a variable's value given a section name and a var name
*/
	{
	__ASSERT_DEBUG(aSection.Length()<=KTokenSize,Panic(ESectionNameTooBig));
	__ASSERT_DEBUG(aVarName.Length()<=KTokenSize,Panic(EVarNameTooBig));

	TPtr sectionToken=iToken->Des();
	_LIT(sectionTokenString,"[%S]");
	sectionToken.Format(sectionTokenString,&aSection);
	TInt sectionStart=iPtr.Find(sectionToken);
	if (sectionStart==KErrNotFound)
		return EFalse;
	TPtrC section=iPtr.Mid(sectionStart);
	sectionStart+=section.Find(TPtrC(_S("]")));
	if (sectionStart==KErrNotFound)
		return EFalse;
	sectionStart++;
	section.Set(iPtr.Mid(sectionStart));
	
	TInt sectionEnd=section.Find(TPtrC(_S("[")));
	if (sectionEnd==KErrNotFound)
		sectionEnd=iPtr.Length()-sectionStart;
	
	section.Set(iPtr.Mid(sectionStart,sectionEnd));
	TPtr varToken=iToken->Des();
	_LIT(varTokenString,"%S=");
	varToken.Format(varTokenString,&aVarName);
	TInt pos=section.Find(varToken);
	if (pos==KErrNotFound)
		return EFalse;
	TLex lex(section.Mid(pos));
	lex.SkipCharacters();
	lex.SkipSpaceAndMark();		// Should be at the start of the data
	lex.SkipCharacters();
	aResult.Set(lex.MarkedToken().Ptr(),lex.MarkedToken().Length());
	return(ETrue);

	}

EXPORT_C TBool CESockIniData::FindVar(const TDesC &aSection,const TDesC &aVarName,TInt &aResult) const
	{
	TPtrC ptr(NULL,0);
	if (FindVar(aSection,aVarName,ptr))
		{
		TLex lex(ptr);
		if (lex.Val(aResult)==KErrNone)
			return(ETrue);
		}
	return(EFalse);
}

EXPORT_C TInt CESockIniData::WriteVar(const TDesC& aSection,const TDesC& aVarName,TInt aValue)
/**
Changes the string associated with a token
*/
	{
	TBuf<32> buf;
	buf.Num(aValue);
	return WriteVar(aSection, aVarName, buf);
	}

EXPORT_C TInt CESockIniData::WriteVar(const TDesC& aSection,const TDesC& aVarName,const TDesC& aValue)
/**
Changes the string associated with a token
*/
	{
	// First find the variable - this gives us a descriptor into the
	// ini data giving the bound of the item that has gto be replaced.
	TPtrC ptr;
	
	if (!FindVar(aSection, aVarName, ptr))
		return KErrNotFound;
	
	TInt pos = ptr.Ptr()-iPtr.Ptr();
	TInt size = (iPtr.Length()+aValue.Length()-ptr.Length())*sizeof(TText);

	if (size>iPtr.MaxLength())
		{
		TText* newdata = (TText*)User::ReAlloc((TUint8*)iPtr.Ptr(), size); 
		if (newdata == 0)
			return KErrNoMemory;

		iPtr.Set(newdata, iPtr.Length(), size/(TInt)sizeof(TText));
		}
	
	iPtr.Replace(pos, ptr.Length(), aValue);
	return KErrNone;
	}

EXPORT_C void CESockIniData::CommitL()
	{
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	TAutoClose<RFile> file;
	User::LeaveIfError(file.iObj.Replace(fs.iObj,*iName,EFileStreamText|EFileWrite));
	file.PushL();

	TPtrC8 ptrc8((TUint8*)iPtr.Ptr(), iPtr.Size());
	User::LeaveIfError(file.iObj.Write(ptrc8));

	file.Pop();
	fs.Pop();
	}

