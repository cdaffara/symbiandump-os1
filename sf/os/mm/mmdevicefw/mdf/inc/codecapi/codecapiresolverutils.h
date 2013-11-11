// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CODECAPIRESOLVERUTILS_H
#define CODECAPIRESOLVERUTILS_H

#include <e32base.h>

/**
 Used to parse the opaque data of codec plugins.
*/
class CCodecApiOpaqueData : public CBase
	{
public:
	IMPORT_C static CCodecApiOpaqueData* NewL(const TDesC8& aOpaqueData);
	IMPORT_C static CCodecApiOpaqueData* NewLC(const TDesC8& aOpaqueData);
	IMPORT_C TUid TypeUid() const;
	IMPORT_C const TDesC8& InputDataType() const;
	IMPORT_C const TDesC8& OutputDataType() const;	
	IMPORT_C TBool CompareInputDataType(const TDesC8& aDataType);
	IMPORT_C TBool CompareOutputDataType(const TDesC8& aDataType);
	virtual ~CCodecApiOpaqueData ();
protected:
	CCodecApiOpaqueData(const TDesC8& aOpaqueData);
	void ConstructL();
	void SetMediaUidL(const TDesC8& aData);
	void ParseTaggedDataL();
	virtual void ProcessTaggedDataL(const TDesC8& aTag, const TDesC8& aData);
	TUint ConvertTextToTUintL(const TDesC8& aData);
	void ConvertTextToUidL(const TDesC8& aData, TUid& aUid);
private:
	const TDesC8& 	iOpaqueData;
	TUid 			iTypeUid;
	HBufC8* 		iInputDataFormat;
	HBufC8* 		iOutputDataFormat;
	};

#endif  // CODECAPIRESOLVERUTILS_H
