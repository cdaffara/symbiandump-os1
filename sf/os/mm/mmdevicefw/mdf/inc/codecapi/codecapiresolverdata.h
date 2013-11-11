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

#ifndef CODECAPIRESOLVERDATA_H
#define CODECAPIRESOLVERDATA_H

#include <e32std.h>

/**
 Defines the type of matching that the resolver should perform 
*/
enum TCodecApiResolverMatchType
	{
	/**
	Resolver should match on input data format
	 */
	EMatchInputDataFormat,			
	/**
	Resolver should match on output data format
	 */
	EMatchOutputDataFormat,		
	/**
	Resolver should match on both input and output data format
	 */
	EMatchInputAndOutputDataFormat		
	};

/**
  Streaming class used to pass parameters from the Codec API to the resolver.
 */
class CCodecApiResolverData : public CBase
	{
public:
	IMPORT_C static CCodecApiResolverData* NewL();
	IMPORT_C static CCodecApiResolverData* NewL(const TDesC8& aPackage);
	IMPORT_C static CCodecApiResolverData* NewLC();
	IMPORT_C static CCodecApiResolverData* NewLC(const TDesC8& aPackage);
	IMPORT_C void SetMatchType(const TCodecApiResolverMatchType& aType);
	IMPORT_C void SetImplementationType(const TUid& aImplementationType);
	IMPORT_C void SetInputDataL(const TDesC8& aDataType);
	IMPORT_C void SetOutputDataL(const TDesC8& aDataType);
	IMPORT_C TCodecApiResolverMatchType MatchType() const;
	IMPORT_C TUid ImplementationType() const;
	IMPORT_C const TPtrC8 InputDataFormat() const;
	IMPORT_C const TPtrC8 OutputDataFormat() const;
	IMPORT_C HBufC8* NewPackLC() const;
	IMPORT_C void UnPackL(const TDesC8& aPackage);
	virtual ~CCodecApiResolverData();
private:
	void ConstructL(const TDesC8& aPackage);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	CCodecApiResolverData();
private:
	TCodecApiResolverMatchType	iMatchType;
	TUid 					iImplementationType;
	HBufC8* 				iInputDataFormat;
	HBufC8* 				iOutputDataFormat;
	}; 

#endif // CODECAPIRESOLVERDATA_H
