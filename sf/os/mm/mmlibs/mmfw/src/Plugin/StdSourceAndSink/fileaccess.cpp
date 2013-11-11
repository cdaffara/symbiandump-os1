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
//

#include <f32file.h>
#include <f32file64.h>
#include <e32std.h>
#include <caf/attribute.h>
#include <caf/bitset.h>
#include <caf/content.h>
#include <caf/data.h>
using namespace ContentAccess;
#include <f32file.h>

#include "FileAccess.h"

CF32File::~CF32File()
	{
	if (!iFileHandle)
		iFile.Close();
	delete iFilePath;
	}

CF32File* CF32File::NewL(RFs& aSession, TDesC& aFilePath, TUint aMode) 
	{
	CF32File* self = new (ELeave) CF32File;
	CleanupStack::PushL(self);
	self->ConstructL(aSession, aFilePath, aMode);
	CleanupStack::Pop(self);
	return self;
	}
	

void CF32File::ConstructL(RFs& aSession, TDesC& aPath, TUint aFileMode)
	{
	iSession = &aSession;
	iFilePath = aPath.AllocL();
	TUint attributes = 0;
	TInt err = iSession->Att(*iFilePath, attributes);
	if (err == KErrNone)
		{
		if ( attributes & KEntryAttReadOnly )
			User::LeaveIfError(iFile.Open(*iSession, *iFilePath, EFileShareReadersOnly )) ;
		else //if ( !( attributes & KEntryAttReadOnly ) )
			User::LeaveIfError(iFile.Open(*iSession, *iFilePath, aFileMode)) ;
		}
	else if ((err == KErrNotFound) && (aFileMode & EFileWrite))
		{
		User::LeaveIfError(iFile.Create(*iSession, *iFilePath, aFileMode )) ;
		}
	else
		{
		User::Leave(err);
		}
	}
	

RFile& CF32File::FileL() 
	{
	return iFile;
	}
	
TInt CF32File::Data(CData*& /*aData*/)
	{
	return KErrNotSupported;
	}

TInt CF32File::Seek(TSeek aSeekMode, TInt aPos)
	{
	return iFile.Seek(aSeekMode, aPos);
	}

TInt CF32File::Read(TDes8& aDes,TInt aLength)
	{
	return iFile.Read(aDes, aLength);
	}

void CF32File::Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	iFile.Read(aDes, aLength, aStatus);
	}

TInt CF32File::Write(const TDesC8& aDes,TInt aLength)
	{
	return iFile.Write(aDes, aLength);
	}

void CF32File::Write(const TDesC8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	iFile.Write(aDes, aLength, aStatus);
	}

TInt CF32File::Size(TInt& aSize)
	{
	return iFile.Size(aSize);
	}

TInt CF32File::SetSize(TInt aSize)	
	{
	TInt err =  iFile.SetSize(aSize);
	if(err == KErrNone)
		iFileSize = aSize;
	else
		iFileSize = -1;

	return err;
	}

TInt CF32File::EvaluateIntent(TIntent /*aIntent*/) const
	{
	return KErrNone;
	}

		
TInt CF32File::ExecuteIntent(TIntent /*aIntent*/)
	{
	return KErrNone;
	}

TBool CF32File::IsProtected() const
	{
	return EFalse;
	}

TInt CF32File::Read(TInt aPosition, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	iFile.Read(aPosition, aDes, aLength, aStatus);
	return KErrNone;
	}

TInt CF32File::SetAgentProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	// not an error to set this if not supported, just wont do anything
	return KErrNone;
	}

CContentFile::~CContentFile()
	{
	delete iData;
  
  iLegacyFile.Close();
	iFile64.Close(); //For defect EASA-84ZC6J 
   	
	delete iFilePath;
	}
	

CContentFile* CContentFile::NewL(RFs& aSession, const TDesC& aFilePath, const TDesC& aUniqueId, TUint aMode, TBool aEnableUI) 
	{
	CContentFile* self = new (ELeave) CContentFile;
	CleanupStack::PushL(self);
	self->ConstructL(aSession, aFilePath, aUniqueId, aMode, aEnableUI);
	CleanupStack::Pop(self);
	return self;
	}
	
void CContentFile::ConstructL(RFs& aSession, const TDesC& aPath, const TDesC& aUniqueId, TUint /*aMode*/, TBool aEnableUI)
	{
	iSession = &aSession;
	iFilePath = aPath.AllocL();
	
	// Assume that we want the content to be shared and read-only.
	if (aUniqueId.Length() > 0)
		{
		iData = CData::NewL(TVirtualPathPtr(*iFilePath, aUniqueId), EContentShareReadWrite);
		}
	else
		{
		iData = CData::NewL(TVirtualPathPtr(*iFilePath), EContentShareReadWrite);
		}
	
	TInt err = iData->SetProperty(EAgentPropertyAgentUI, aEnableUI);
	if (err != KErrNone && err != KErrCANotSupported)
		{
		User::Leave(err);
		}
	User::LeaveIfError(iData->EvaluateIntent(EPeek));
	}

	

TInt CContentFile::EvaluateIntent(TIntent aIntent) const
	{
	ASSERT(iData);
	return iData->EvaluateIntent(aIntent);
	}

		
TInt CContentFile::ExecuteIntent(TIntent aIntent)
	{
	ASSERT(iData);
	return iData->ExecuteIntent(aIntent);
	}

TBool CContentFile::IsProtected() const
	{
	ASSERT(iData);
	TInt value = 0;
	TInt err =iData->GetAttribute(EIsProtected, value);
	return (err == KErrNone && value);
	}

TInt CContentFile::SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
	{
	ASSERT(iData);
	return iData->SetProperty(aProperty, aValue);
	}

TInt CContentFile::Seek(TSeek aSeekMode, TInt aPos)
	{
	ASSERT(iData);
	return iData->Seek(aSeekMode, aPos);
	}

TInt CContentFile::Read(TDes8& aDes,TInt aLength)
	{
	ASSERT(iData);
	return iData->Read(aDes,aLength);
	}

void CContentFile::Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	ASSERT(iData);
	iData->Read(aDes, aLength, aStatus);
	}

TInt CContentFile::Write(const TDesC8& /*aDes*/,TInt /*aLength*/)
	{
	return KErrAccessDenied;
	}
void CContentFile::Write(const TDesC8& /*aDes*/, TInt /*aLength*/, TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrAccessDenied);
	}

TInt CContentFile::SetSize(TInt /*aSize*/)
	{
	// Only a source is implemented, hence this cannot be allowed
	return KErrAccessDenied;
	}


// Get the size of file
// this method opens a new, read-only, RFile the first time this method is called

TInt CContentFile::Size(TInt& aSize)
	{
	ASSERT(iData);
	TRAPD(err, iData->DataSizeL(aSize));
	return err;
	}

/**
 * return a RFile for the legacy RFile method
 * this method opens a new, read-only, RFile the first time this method is called
 * @internalTechnology
 * @return Reference to RFile handle to current file
 */
RFile& CContentFile::FileL() 
	{
	if (!iLegacyFileOpen)
		{//For defect EASA-84ZC6J 
  		TInt error = iLegacyFile.Open(*iSession, *iFilePath, EFileRead | EFileStream | EFileShareReadersOrWriters);
		  if ( error == KErrTooBig )
	        {
	         User::LeaveIfError(iFile64.Open(*iSession, *iFilePath, EFileRead | EFileStream | EFileShareReadersOrWriters));
	         iLegacyFileOpen = ETrue;
	         return iFile64;
	        }
	    
	    else if (error == KErrNone)
	       {
	       iLegacyFileOpen = ETrue;
	       return iLegacyFile;
	        }
	    
	    else 
	        {
	        User::Leave(error);
	        }
		}
	if(iLegacyFile.SubSessionHandle())
	    {
	    return iLegacyFile;
	    }
	else
	    {
	    return iFile64;
	    }
}
	
TInt CContentFile::Data(CData*& aData)
	{
	if (iData==NULL)
		{
		return KErrNotReady;
		}
	else
		{
		aData = iData;
		return KErrNone;
		}
	}
	
CF32File* CF32File::NewL(RFile& aFile) 
	{
	CF32File* self = new (ELeave) CF32File;
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	CleanupStack::Pop(self);
	return self;
	}
	
void CF32File::ConstructL(RFile& aFile)
	{
	iFile = aFile;
	iFileHandle = ETrue;
	}

CContentFile* CContentFile::NewL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI) 
	{
	CContentFile* self = new (ELeave) CContentFile;
	CleanupStack::PushL(self);
	self->ConstructL(aFile, aUniqueId, aEnableUI);
	CleanupStack::Pop(self);
	return self;
	}

void CContentFile::ConstructL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI)
	{
	iData = CData::NewL(aFile, aUniqueId);
	TInt err = iData->SetProperty(EAgentPropertyAgentUI, aEnableUI);
	if (err != KErrNone && err != KErrCANotSupported)
		{
		User::Leave(err);
		}
	User::LeaveIfError(iData->EvaluateIntent(EPeek));
	}

TInt CContentFile::Read(TInt aPosition, TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
	{
	return iData->Read(aPosition, aDes, aLength, aStatus);
	}
