// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <bluetooth/logger.h>
#include <bluetoothav.h>
#include "gavdpinterface.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVLIB);
#endif

#ifdef _DEBUG
PANICCATEGORY("medcodec");
#endif

TAvdtpMediaCodecCapabilities::TAvdtpMediaCodecCapabilities(
					SymbianBluetoothAV::TBluetoothMediaType aMediaType,
					TBluetoothMediaCodecType aCodecType,
					TInt aLOSC)
: TAvdtpServiceCapability(EServiceCategoryMediaCodec, aLOSC),
iMediaType(aMediaType),
iMediaCodecType(aCodecType)
	{
	}

/**
Parse AVDTP protocol-domain buffer representing media codec capabilities into this
@param aPtr the buffer
@internalTechnology
*/	
EXPORT_C /*virtual*/ void TAvdtpMediaCodecCapabilities::Parse(const TDesC8& aPtr)
	{
	ASSERT_DEBUG(aPtr.Length()>=2);	// the parse broke if this is not true
	iMediaType = static_cast<SymbianBluetoothAV::TBluetoothMediaType>(aPtr[0]>>4);
	iMediaCodecType = static_cast<TBluetoothMediaCodecType>(aPtr[1]);
	}

/**
Allocate a concrete codec capability from an AVDTP-protocol domain buffer
*/
/*static*/ TAvdtpMediaCodecCapabilities* TAvdtpMediaCodecCapabilities::AllocCodecFromPDUL(const TDesC8& aBuf)
	{
	const TInt KMinMediaCodecLOSC = 2;
	
	if (aBuf.Length() < KMinMediaCodecLOSC)
		{
		User::Leave(KErrCorrupt);
		}
		
	using namespace SymbianBluetoothAV;

	TAvdtpMediaCodecCapabilities* res = NULL;
	
	static const TInt KMediaTypeOffset = 0;	
	TBluetoothMediaType type = static_cast
		<TBluetoothMediaType>(aBuf[KMediaTypeOffset]);
		
	static const TInt KMediaCodecTypeOffset = 1;	
	TBluetoothMediaCodecType codec = static_cast
		<TBluetoothMediaCodecType>(aBuf[KMediaCodecTypeOffset]);
	
	switch (codec)
		{
		// at present only support SBC
		// more useful for SymbianOS as nearly always want to mix anyway
		// which means decoding to PCM
		// must then re-encode to something, and that might as well be SBC as mandatory
		case EAudioCodecSBC:
			{
			res = new (ELeave) TSBCCodecCapabilities;
			}
			break;

		// to allow other codecs just arrange for a descriptor - clients don't
		// get the parsing done for free			
		// later could use ECOM plugins to support other codecs
		// other codecs can be added compile time in the style of the SBC codec
		default:
			{
			res = new (ELeave) TNonSBCCodecCapabilities(type, codec);
			}
		}
		
	return res;
	}

/**
Returns the media type
@return media type
@see SymbianBluetoothAV::TBluetoothMediaType
*/
EXPORT_C SymbianBluetoothAV::TBluetoothMediaType TAvdtpMediaCodecCapabilities::MediaType() const
	{
	return iMediaType;	
	}

/**
Returns the media codec type
@return media codec type
@see SymbianBluetoothAV::TBluetoothMediaType
*/
EXPORT_C TBluetoothMediaCodecType TAvdtpMediaCodecCapabilities::MediaCodecType() const
	{
	return iMediaCodecType;
	}

