// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EIRDATACODEC_H
#define EIRDATACODEC_H

#include <e32std.h>
#include <es_sock.h>

/**
@file
@internalTechnology
*/
/**
The state of the EIR data, either partial or complete
*/
enum TEIRDataCompleteness
	{
	EDataPartial = 0x1,
	EDataComplete = 0x2,
	};

const TUint16 KNameRecord8BitMaxLength=248;

/**
Extended Inquiry Response Data Types
*/
enum TExtendedInquiryResponseDataType
	{
// Invalid Data Type
	EEirInvalid 					= 0x00,
// Standard Data Types
	EEirFlags 						= 0x01,
	EEirUUID16Partial 				= 0x02,
	EEirUUID16Complete 				= 0x03,
	EEirUUID32Partial 				= 0x04,
	EEirUUID32Complete 				= 0x05,
	EEirUUID128Partial 				= 0x06,
	EEirUUID128Complete 			= 0x07,
	EEirLocalNamePartial 			= 0x08,
	EEirLocalNameComplete 			= 0x09,
	EEirTxPowerLevel 				= 0x0A,
// OOB Tags: Not used in EIR itself
	EEirOobClassOfDevice			= 0x0D,
	EEirOobSimplePairingHash		= 0x0E,
	EEirOobSimplePairingRandomizerR	= 0x0F,
// Vendor specific
	EEirVendorSpecific 				= 0xFF,
	};


class TNameRecord;
/**
@internalTechnology
Extended Inquiry Response Parser Class
This class takes a reference from inquiry result, which could hold both local name and Extended Inquiry Response. It provides API to parse and return the local name and data for any Extended Inquiry Response tag.
*/
NONSHARABLE_CLASS(TExtendedInquiryResponseDataCodec)
	{
public:
	IMPORT_C TExtendedInquiryResponseDataCodec();
	IMPORT_C explicit TExtendedInquiryResponseDataCodec(const TNameRecord& aNameRecord);
	IMPORT_C explicit TExtendedInquiryResponseDataCodec(TNameRecord& aNameRecord);
	IMPORT_C explicit TExtendedInquiryResponseDataCodec(const TDesC8& aDes);
	IMPORT_C explicit TExtendedInquiryResponseDataCodec(TDes8& aDes);
	IMPORT_C TInt GetData(TExtendedInquiryResponseDataType aDataType, TPtrC8& aDes) const;
	IMPORT_C TExtendedInquiryResponseDataType GetNextData(TPtrC8& aDes);
	IMPORT_C TBool IsDataTypePresent(TExtendedInquiryResponseDataType aDataType) const;
	IMPORT_C TInt GetDeviceName(TPtrC8& aName) const;
	IMPORT_C TInt SetDeviceName(const TPtrC8& aName, TBool iIsComplete);
	IMPORT_C void Set(const TNameRecord& aNameRecord);
	IMPORT_C void Set(TNameRecord& aNameRecord);
	IMPORT_C void Set(const TDesC8& aDes);
	IMPORT_C void Set(TDes8& aDes);
	IMPORT_C void Copy(TDesC8& aDes);
	IMPORT_C TInt DoSanityCheck(TDes8& aDes);

private:
	TUint16 ComputeSignificantLength(const TDesC8& aDes);
	TInt NextDataType(TInt aOffs) const;
	TBool IsValideDataType(TInt aDataType);

private:
	TPtr8			iEir;
	TInt			iOffset;
	TNameRecord*	iNameRecord;
	TUint32			iPadding1;
	TUint32			iPadding2;
	};

enum TEirCodecPanics
	{
	EEirBadDescriptor = 0,
	EEirBadState = 1,
	EEirCodecDataTooLarge = 2,
	};
_LIT(KEirCodecPanicName, "ExtendedInquiryResponseCodec");

#endif /*EIRDATACODEC_H*/
