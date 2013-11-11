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

/**
 @file
 @internalComponent
*/

#ifndef REMCONIFDETAILS_H
#define REMCONIFDETAILS_H

#include <e32base.h>
#include <remcon/clienttype.h>

class CRemConInterfaceBase;
class RReadStream;
class RWriteStream;

NONSHARABLE_CLASS(CRemConInterfaceDetails) : public CBase
	{
public:
	IMPORT_C static CRemConInterfaceDetails* NewLC(TUid aUid, TRemConClientType aType, TBool aIsBulk, CRemConInterfaceBase* aInterface, const TDesC8& aFeatures);
	~CRemConInterfaceDetails();
	
	IMPORT_C TUid Uid() const;
	IMPORT_C TRemConClientType Type() const;
	IMPORT_C void GetRemConInterfaceFeaturesL(RArray<TUint>& aRemConInterfaceFeatures) const;
	IMPORT_C TBool IsBulk() const;
	
	IMPORT_C CRemConInterfaceBase*& Interface();
	
	IMPORT_C void DeleteFeatures();
	
	static CRemConInterfaceDetails* InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream);
	
private:
	CRemConInterfaceDetails(TUid aUid, TRemConClientType aType, TBool aIsBulk, CRemConInterfaceBase* aInterface);
	void ConstructL(const TDesC8& aFeatures);
	void AddFeatures(RBuf8& aFeatures);
	
private:
	CRemConInterfaceBase*	iInterface;
	
	TUid					iUid;
	TRemConClientType		iType;
	RBuf8					iFeatures;
	TBool					iBulk;
	};


NONSHARABLE_CLASS(CRemConInterfaceDetailsArray)
	: public CBase
	{
public:
	IMPORT_C static CRemConInterfaceDetailsArray* NewL();
	~CRemConInterfaceDetailsArray();
	
	IMPORT_C TArray<CRemConInterfaceDetails*> Array() const;
	
	IMPORT_C static CRemConInterfaceDetailsArray* InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream, TRemConClientType aTypeFilter = ERemConClientTypeUndefined);
	
	IMPORT_C void AppendL(CRemConInterfaceDetails* aDetails);
	
private:
	CRemConInterfaceDetailsArray();
	
private:
	RPointerArray<CRemConInterfaceDetails> iArray;
	};

#endif // REMCONIFDETAILS_H 
