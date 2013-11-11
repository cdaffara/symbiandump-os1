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
// rrcgpsbadsatlist - GPS Bad Sat List encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __RRCGPSBADSATLIST_H__
#define __RRCGPSBADSATLIST_H__

#include <e32base.h>
#include "asn1per.h"

class RLbsAssistanceDataBuilderSet;


class CRrcGpsBadSatListContent : public CBase
	{
public:
	static CRrcGpsBadSatListContent* NewL();
	static CRrcGpsBadSatListContent* NewLC();
	~CRrcGpsBadSatListContent();
public:
	TUint8 iBadSatList[16];
	TUint8 iNumberOfBadSat;
	};


class CRrcGpsBadSatList : public CBase
	{
public:
	static CRrcGpsBadSatList* NewL(CRrcGpsBadSatListContent* aGpsBadSatListContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsBadSatList* NewLC(CRrcGpsBadSatListContent* aGpsBadSatListContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsBadSatList();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();

private:
	CRrcGpsBadSatList(CRrcGpsBadSatListContent* aGpsBadSatListContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	CRrcGpsBadSatListContent* iBadSatListContent;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};

#endif // __RRCGPSBADSATLIST_H__
