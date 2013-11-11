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
// rrcgpsutcmodel - GPS UTC model encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __RRCGPSUTCMODEL_H__
#define __RRCGPSUTCMODEL_H__

#include <e32base.h>
#include "asn1per.h"

class RLbsAssistanceDataBuilderSet;

class CRrcGpsUTCModelContent : public CBase
	{
public:
	static CRrcGpsUTCModelContent* NewL();
	static CRrcGpsUTCModelContent* NewLC();
	~CRrcGpsUTCModelContent();

public:

	TBuf8<3> iA1;								//BIT STRING (SIZE (24)),
	TBuf8<4> iA0;								//BIT STRING (SIZE (32)),
	TBuf8<1> iTot;								//BIT STRING (SIZE (8)),
	TBuf8<1> iWnt;								//BIT STRING (SIZE (8)),
	TBuf8<1> iDeltatLS;							//BIT STRING (SIZE (8)),
	TBuf8<1> iWnLsf;							//BIT STRING (SIZE (8)),
	TBuf8<1> iDn;								//BIT STRING (SIZE (8)),
	TBuf8<1> iDeltatLSF;						//BIT STRING (SIZE (8))
	};


class CRrcGpsUTCModel : public CBase
	{
public:
	static CRrcGpsUTCModel* NewL(CRrcGpsUTCModelContent* aGpsUTCModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsUTCModel* NewLC(CRrcGpsUTCModelContent* aGpsUTCModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsUTCModel();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();

private:
	CRrcGpsUTCModel(CRrcGpsUTCModelContent* aGpsUTCModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	CRrcGpsUTCModelContent* iGpsUTCModelContent;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};

#endif // __RRCGPSUTCMODEL_H__
