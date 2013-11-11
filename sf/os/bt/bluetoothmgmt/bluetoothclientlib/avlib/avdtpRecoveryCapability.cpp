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
PANICCATEGORY("recovcap");
#endif

EXPORT_C TAvdtpRecoveryCapabilities::TAvdtpRecoveryCapabilities()
: TAvdtpServiceCapability(EServiceCategoryRecovery, KServiceCatLOSCRecovery)
	{
	}

EXPORT_C /*virtual*/ TInt inline TAvdtpRecoveryCapabilities::AsProtocol(RBuf8& aBuffer) const
	{
	TInt ret = AddHeader(aBuffer);
	
	if (ret==KErrNone)
		{
		aBuffer.Append(static_cast<TUint8>(iRecoveryType));
		aBuffer.Append(static_cast<TUint8>(iMaximumRecoveryWindowSize));
		aBuffer.Append(static_cast<TUint8>(iMinimumRecoveryWindowSize));
		}
	return ret;	
	}
	
/*virtual*/ void TAvdtpRecoveryCapabilities::Parse(const TDesC8& aPtr)
	{
	ASSERT_DEBUG(aPtr.Length()>=3);	// the parse broke if this is not true
	iRecoveryType = static_cast<SymbianBluetoothAV::TAvdtpRecoveryType>(aPtr[0]);
	iMaximumRecoveryWindowSize = static_cast<TAvdtpRecoveryWindowSize>(aPtr[1]);
	iMinimumRecoveryWindowSize = static_cast<TAvdtpRecoveryWindowSize>(aPtr[2]);
	}

EXPORT_C SymbianBluetoothAV::TAvdtpRecoveryType TAvdtpRecoveryCapabilities::RecoveryType() const
	{
	return iRecoveryType;	
	}
	
EXPORT_C void TAvdtpRecoveryCapabilities::SetRecoveryType(SymbianBluetoothAV::TAvdtpRecoveryType aType)
	{
	iRecoveryType = aType;
	}
	
EXPORT_C TAvdtpRecoveryWindowSize TAvdtpRecoveryCapabilities::MaxWindowSize() const
	{
	return iMaximumRecoveryWindowSize;
	}
	
//This runs client-side - panic bad values, no return error needed
EXPORT_C void TAvdtpRecoveryCapabilities::SetMaxWindowSize(TAvdtpRecoveryWindowSize aSize)
	{
	iMaximumRecoveryWindowSize = aSize;
	}
	
	
EXPORT_C TAvdtpRecoveryWindowSize TAvdtpRecoveryCapabilities::MinWindowSize() const
	{
	return iMinimumRecoveryWindowSize;
	}
	
	
EXPORT_C void TAvdtpRecoveryCapabilities::SetMinWindowSize(TAvdtpRecoveryWindowSize aSize)
	{
	iMinimumRecoveryWindowSize=aSize;
	}

