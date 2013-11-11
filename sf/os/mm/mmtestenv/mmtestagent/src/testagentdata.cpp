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

#include <caf/caf.h>
#include <caf/attribute.h>
#include "testagentdata.h"
#include "testfileconstants.h"

using namespace ContentAccess;

CTestAgentData* CTestAgentData::NewL(const TDesC& aURI, TContentShareMode aMode)
	{
	CTestAgentData* self = NewLC(aURI, aMode);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentData* CTestAgentData::NewLC(const TDesC& aURI, TContentShareMode aMode)
	{
	CTestAgentData* self=new(ELeave) CTestAgentData();
	CleanupStack::PushL(self);
	self->ConstructL(aURI, aMode);
	return self;
	}

CTestAgentData::CTestAgentData()
	{
	}

CTestAgentData::~CTestAgentData()
	{
	// Tidy up RFile and RFs
	iFile.Close();
	iFs.Close();
	delete iRightsDatabase;
	}

void CTestAgentData::ConstructL(const TDesC& aURI, TContentShareMode aMode)
	{
	// It is at this point that the agent can open the target content
	// and cache the size. Evaluation of intent will be left until the
	// CData object is created.

	TUint mode = EFileStream | EFileRead;

	// setup share mode
	switch (aMode)
		{
	case EContentShareReadWrite :
		mode |= EFileShareAny;
		break;
	case EContentShareReadOnly :
		mode |= EFileShareReadersOnly;
		break;
	case EContentShareExclusive:
		mode |= EFileShareExclusive;
		break;
	default:
		// unknown, don't specifiy share mode
		break;
		}

	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFile.Open(iFs, aURI, mode));

	TBuf8<KDRMTestAgentHeaderLength> header;
	User::LeaveIfError(iFile.Read(header, KDRMTestAgentHeaderLength));


	if (!header.Compare(KDRMTestAgentHeader)==0)
		User::Leave(KErrNotSupported);

	TPckgBuf<TUint32> databaseIDPckg;

	User::LeaveIfError(iFile.Read(databaseIDPckg));
	iDatabaseID = databaseIDPckg();


	iDataOffset = KDRMTestAgentHeaderLength + sizeof(TUint32);
	User::LeaveIfError(iFile.Size(iDataSize));
	iDataSize -= iDataOffset;

	// seek to start of data in the file, ready to "read"
	TInt Pos = 0;
	Seek(ESeekStart, Pos);

	iRightsDatabase = CRightsDatabase::NewL();
	TBool rightsExist = iRightsDatabase->ExistID(iDatabaseID);
	if (!rightsExist)
		User::Leave(KErrCANoRights);
	}


void CTestAgentData::GetAttributeL(const CBitset& /*aQuerySet*/,
									        CBitset& aResponseSet)
	{
	// Test attributes
	aResponseSet.Set(EIsProtected);
	aResponseSet.Unset(EIsForwardable);
	aResponseSet.Unset(EIsModifyable);
	aResponseSet.Set(EIsCopyable);
	}

void CTestAgentData::DataSizeL(TInt &aSize)
	{
	aSize = iDataSize;
	}

TInt CTestAgentData::EvaluateIntent(TIntent aIntent)
	{
	if (aIntent != EPeek)
		{
		TInt32 count = 0;
		TRAPD(err, iRightsDatabase->GetRightsCountL(iDatabaseID, count));
		if (err == KErrNone)
			{
			if (count==0)
				return KErrCANoPermission;
			else
				return KErrNone;
			}
		else
			return err;
		}
	else
		return KErrNone;
	}

TInt CTestAgentData::ExecuteIntent(TIntent aIntent)
	{
	if ((aIntent != EPeek) && (aIntent != EStop)) 
		{
		TInt32 count = 0;
		TRAPD(err, iRightsDatabase->GetRightsCountL(iDatabaseID, count));
		if (err == KErrNone)
			{
			if (count>0)
				{
				TRAP(err, iRightsDatabase->DecrementRightsCountL(iDatabaseID));
				return err;
				}
			else if (count == 0)
				return KErrCANoPermission;
			else // if negative, no rights count is maintained
				return KErrNone;
			}
		else
			return err;
		}
	else
		return KErrNone;
	}

TInt CTestAgentData::Read(TDes8& aDes)
	{
	return iFile.Read(aDes);
	}

TInt CTestAgentData::Read(TDes8& aDes,TInt aLength)
	{
	return iFile.Read(aDes,aLength);
	}

void CTestAgentData::Read(TDes8& aDes,TRequestStatus& aStatus)
	{
	iFile.Read(aDes, aStatus);
	}

void CTestAgentData::Read(TDes8& aDes,
							 TInt aLength,
							 TRequestStatus& aStatus)
	{
	iFile.Read(aDes, aLength, aStatus);
	}

TInt CTestAgentData::Seek(TSeek aMode, TInt& aPos)
	{
	TInt rval;
	// Don't allow the user to seek into the OMA header, only around the data itself
	TInt offset;

	switch(aMode)
		{
	case ESeekAddress:
		// Unlikely for this case to be used
		if(aPos > iDataSize || aPos < 0)
			return KErrNotSupported;

		offset = iDataOffset + aPos;
		rval = iFile.Seek(aMode, offset);
		//  offset is populated with the ROM address, fill in aPos for the caller
		aPos=offset;
		break;

	case ESeekStart:
		if(aPos > iDataSize)
			aPos = iDataSize;
		if(aPos < 0)
			return KErrNotSupported;
		offset = iDataOffset + aPos;
		rval = iFile.Seek(aMode, offset);
		break;
	case ESeekCurrent:
		offset = aPos;
		rval = iFile.Seek(aMode, offset);
		// if we have done a seek before the start of the data
		// do another seek to get back to the start of the data
		if(offset < iDataOffset)
			{
			offset = iDataOffset;
			rval = iFile.Seek(ESeekStart, offset);
			aPos = 0;
			}
		else
			{
			aPos = offset - iDataOffset;
			}
		break;
	case ESeekEnd:
		// offsets are negative for ESeekEnd
		offset = aPos;
		if(offset < -iDataSize)
			offset = -iDataSize;
		rval = iFile.Seek(aMode, offset);
		aPos = offset - iDataOffset;
		break;
	default:
		return KErrNotSupported;
		}
	return rval;
	}

TInt CTestAgentData::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	return KErrNone;
	}

TInt CTestAgentData::GetAttribute(TInt aAttribute, TInt& aValue)
	{
	// Test attributes
	if (aAttribute == EIsProtected )
		{
			aValue = 1;
		}
	else if (aAttribute == EIsForwardable)
		{
			aValue = 0;
		}
	else if (aAttribute == EIsModifyable)
		{
			aValue = 0;
		}
	else if (aAttribute == EIsCopyable)
		{
			aValue =1;
		}
	return KErrNone;
	}

TInt CTestAgentData::GetAttributeSet(RAttributeSet& /*aAttributeSet*/)
	{
	return KErrNone;
	}

TInt CTestAgentData::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
	if(aAttribute == EMimeType)
		{
		TBuf8<64> mimeType;
		iRightsDatabase->GetMimeTypeL(iDatabaseID, mimeType);
		if (mimeType.Length()>0)
			{
			aValue.Copy(mimeType);
			return KErrNone;
			}

		else
			return KErrNotFound;
		}
	return KErrNone;
	}

TInt CTestAgentData::GetStringAttributeSet(RStringAttributeSet& /*aStringAttributeSet*/)
	{
	return KErrNone;
	}
