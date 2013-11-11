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
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef CODECAPIVIDEORESOLVERUTILS_H
#define CODECAPIVIDEORESOLVERUTILS_H

#include <e32base.h>
#include <mdf/codecapiresolverutils.h>
class TPictureRateAndSize;

/**
 Used to parse the opaque data of the video codec plugins.
*/
class CCodecApiVideoOpaqueData : public CCodecApiOpaqueData
	{
public:
	IMPORT_C static CCodecApiVideoOpaqueData* NewL(const TDesC8& aOpaqueData);
	IMPORT_C static CCodecApiVideoOpaqueData* NewLC(const TDesC8& aOpaqueData);
	IMPORT_C const TSize& MaxPictureSize() const;
	IMPORT_C const TDesC8& Manufacturer() const;
	IMPORT_C const RArray<TPictureRateAndSize>& MaxPictureRates() const;
	virtual ~CCodecApiVideoOpaqueData();
protected:
	CCodecApiVideoOpaqueData(const TDesC8& aOpaqueData);
	void ConstructL();
	void ProcessTaggedDataL(const TDesC8& aTag, const TDesC8& aData);
	void SetPictureRateAndSizeL(const TDesC8& aData);
	void SetMaxPictureSizeL(const TDesC8& aData);
private:
	TSize 				iMaxPictureSize;
	RArray<TPictureRateAndSize>   iMaxPictureRates;
	HBufC8* 				iManufacturer;
	};

#endif  // CODECAPIVIDEORESOLVERUTILS_H


