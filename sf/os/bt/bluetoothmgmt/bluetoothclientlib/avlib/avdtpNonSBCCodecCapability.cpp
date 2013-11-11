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

#include <bluetoothav.h>
#include "gavdpinterface.h"

/**
Constructor
@param aMediaType The media type
@param aCodecType The codec type
*/	
EXPORT_C TNonSBCCodecCapabilities::TNonSBCCodecCapabilities(SymbianBluetoothAV::TBluetoothMediaType aMediaType,
															TBluetoothMediaCodecType aCodecType)
: TAvdtpMediaCodecCapabilities(aMediaType, aCodecType, KUnknownLOSC)
	{
	// LOSC set when SetCodecData called
	}
	
/**
@internalTechnology
*/	
EXPORT_C /*virtual*/ TInt TNonSBCCodecCapabilities::AsProtocol(RBuf8& aBuffer) const
	{
	using namespace SymbianBluetoothAV;

	TInt ret = AddHeader(aBuffer);
	if (ret == KErrNone)
		{		
		aBuffer.Append(static_cast<TChar>(iMediaType));
		aBuffer.Append(static_cast<TChar>(iMediaCodecType));

		aBuffer.Append(iCodecData);
		}
		
	return ret;

	}

/**
@internalTechnology
*/	
EXPORT_C /*virtual*/ void TNonSBCCodecCapabilities::Parse(const TDesC8& aPtr)
	{
	_LIT(KNonSBCCodecCapabilitiesPanic, "NonSBCConfigPanic");
	__ASSERT_ALWAYS(aPtr.Length()>=2, User::Panic(KNonSBCCodecCapabilitiesPanic, 0));
	
	// parse the media type and codec type via base-class
	TAvdtpMediaCodecCapabilities::Parse(aPtr);

	iCodecData = aPtr.Right(aPtr.Length()-KServiceCatLOSCMediaCodecMinimum);
	iLOSC = KServiceCatLOSCMediaCodecMinimum+iCodecData.Length();
	}

/**
Set codec settings transparently - used for arbitrary nonSBC codec
@param aCodecData descriptor containing arbitrary codec data
*/	
EXPORT_C void TNonSBCCodecCapabilities::SetCodecData(const TDesC8& aCodecData)
	{
	iCodecData = aCodecData;
	iLOSC = KServiceCatLOSCMediaCodecMinimum+aCodecData.Length();
	}
	
/**
Get codec settings transparently - used for arbitrary nonSBC codec
@return descriptor containing arbitrary codec data
*/	
EXPORT_C const TDesC8& TNonSBCCodecCapabilities::CodecData() const
	{
	return iCodecData;
	}

