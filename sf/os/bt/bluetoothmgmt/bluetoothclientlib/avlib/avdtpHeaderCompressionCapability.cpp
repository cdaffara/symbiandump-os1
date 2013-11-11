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
PANICCATEGORY("headcomp");
#endif

const TUint8 KBackChannelBitMask= 0x80;
const TUint8 KMediaBitMask		= 0x40;
const TUint8 KRecoveryBitMask	= 0x20;

EXPORT_C TAvdtpHeaderCompressionCapabilities::TAvdtpHeaderCompressionCapabilities()
: TAvdtpServiceCapability(EServiceCategoryHeaderCompression, KServiceCatLOSCHeaderCompression)
	{
	}
	
EXPORT_C /*virtual*/ TInt TAvdtpHeaderCompressionCapabilities::AsProtocol(RBuf8& aBuffer) const
	{
	TInt ret = AddHeader(aBuffer);
	
	if (ret==KErrNone)
		{
		TUint8 capabilityByte =0; // see fig8.52 AVDTP v1.0
		capabilityByte |= BackChannel() ? KBackChannelBitMask : 0;
		capabilityByte |= Media() ? KMediaBitMask : 0;
		capabilityByte |= Recovery() ? KRecoveryBitMask : 0;
		aBuffer.Append(capabilityByte);
		}
	return ret;	
	}

/*virtual*/	void TAvdtpHeaderCompressionCapabilities::Parse(const TDesC8& aPtr)
	{
	ASSERT_DEBUG(aPtr.Length()==1);	// the parse broke if this is not true
	iBackChannel = (aPtr[0] & KBackChannelBitMask) ? ETrue : EFalse;
	iMedia = (aPtr[0] & KMediaBitMask) ? ETrue : EFalse;
	iRecovery = (aPtr[0] & KRecoveryBitMask) ? ETrue : EFalse;
	}

	
EXPORT_C TBool TAvdtpHeaderCompressionCapabilities::Media() const
	{
	return iMedia;
	}
	
EXPORT_C TBool TAvdtpHeaderCompressionCapabilities::Recovery() const
	{
	return iRecovery;
	}
	
EXPORT_C TBool TAvdtpHeaderCompressionCapabilities::BackChannel() const
	{
	return iBackChannel;
	}
	
EXPORT_C void TAvdtpHeaderCompressionCapabilities::SetCapabilities(TBool aMedia,TBool aRecovery,TBool aBackChannel)
	{
	iMedia=aMedia;
	iRecovery=aRecovery;
	iBackChannel=aBackChannel;
	}
