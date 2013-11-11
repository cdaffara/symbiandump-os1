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
// rrcgpsionosphericmodel - GPS ionospheric model encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __RRCGPSIONOSPHERICMODEL_H__
#define __RRCGPSIONOSPHERICMODEL_H__

#include <e32base.h>
#include "asn1per.h"


class RLbsAssistanceDataBuilderSet;
	
									



class CRrcGpsIonosphericModelContent : public CBase
	{	
public:
	static CRrcGpsIonosphericModelContent* NewL();
	static CRrcGpsIonosphericModelContent* NewLC();
	~CRrcGpsIonosphericModelContent();

private:
	CRrcGpsIonosphericModelContent();
	void ConstructL();	
	
public:
	TBuf8<1> iAlfa0;				// BIT STRING (SIZE (8)),
	TBuf8<1> iAlfa1;				// BIT STRING (SIZE (8)),
	TBuf8<1> iAlfa2;				// BIT STRING (SIZE (8)),
	TBuf8<1> iAlfa3;				// BIT STRING (SIZE (8)),
	TBuf8<1> iBeta0;				// BIT STRING (SIZE (8)),
	TBuf8<1> iBeta1;				// BIT STRING (SIZE (8)),
	TBuf8<1> iBeta2;				// BIT STRING (SIZE (8)),
	TBuf8<1> iBeta3;				// BIT STRING (SIZE (8))
	};


class CRrcGpsIonosphericModel : public CBase
	{
public:	
	static CRrcGpsIonosphericModel* NewL(CRrcGpsIonosphericModelContent* aIonosphericModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsIonosphericModel* NewLC(CRrcGpsIonosphericModelContent* aIonosphericModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsIonosphericModel();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();

private:
	CRrcGpsIonosphericModel(CRrcGpsIonosphericModelContent* aIonosphericModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	void ConstructL();

private:
	CRrcGpsIonosphericModelContent* iIonosphericModelContent;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};




#endif // __RRCGPSIONOSPHERICMODEL_H__
