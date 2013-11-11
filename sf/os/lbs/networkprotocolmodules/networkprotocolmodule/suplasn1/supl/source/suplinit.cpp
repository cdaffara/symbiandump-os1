// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SUPL INIT - Implementation of SUPL INIT encoding & decoding.
// 
//

#include "suplinit.h"


/**
Implemetation of CSuplInitContent Class
*/
EXPORT_C CSuplInitContent* CSuplInitContent::NewL()
	{
	CSuplInitContent* self = CSuplInitContent::NewLC();
	CleanupStack::Pop(self);
	return self;							
	}

EXPORT_C CSuplInitContent* CSuplInitContent::NewLC()
	{
	CSuplInitContent* self = new (ELeave) CSuplInitContent();
	CleanupStack::PushL(self);
	return self;		
	}

CSuplInitContent::CSuplInitContent()
	{		
	}

EXPORT_C CSuplInitContent::~CSuplInitContent()
	{
	}

TBool CSuplInitContent::operator==(const CSuplInitContent& other) const
	{
	return (iPosMethod==other.iPosMethod &&
			iNotificationPresent==other.iNotificationPresent &&
			iSLPAddressPresent==other.iSLPAddressPresent &&
			iQopPresent==other.iQopPresent &&
			iSlpMode==other.iSlpMode &&
			iMacPresent==other.iMacPresent &&
			iKeyIdentityPresent==other.iKeyIdentityPresent);
	}
	
/**
Implemetation of CSuplInit Class
*/
CSuplInit* CSuplInit::NewLC(CSuplInitContent* aSuplInitContent)
	{
	CSuplInit* self = new (ELeave) CSuplInit(aSuplInitContent);
	CleanupStack::PushL(self);
	return self;
	};

CSuplInit* CSuplInit::NewL(CSuplInitContent* aSuplInitContent)
	{
	CSuplInit* self = CSuplInit::NewLC(aSuplInitContent);
	CleanupStack::Pop(self);
	return self;							
	};
		
CSuplInit::CSuplInit(CSuplInitContent* aSuplInitContent) 
: iSuplInitContent(aSuplInitContent)
	{
	}

CSuplInit::~CSuplInit()
	{
	}

void CSuplInit::ConstructL()
	{
	};


void CSuplInit::EncodeL(CAsn1PerEncoding& aPerEncoding)
	{	
	User::Leave(KErrNotSupported);
	}
		
void CSuplInit::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{		
	User::Leave(KErrNotSupported);
	}












