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

EXPORT_C TAvdtpContentProtectionCapabilities::TAvdtpContentProtectionCapabilities()
: TAvdtpServiceCapability(EServiceCategoryContentProtection),
KMinContentProtectionLOSC(2)
	{
	}

EXPORT_C TUint16 TAvdtpContentProtectionCapabilities::ContentProtectionType() const
	{
	return iCPType;
	}
	
EXPORT_C void TAvdtpContentProtectionCapabilities::SetContentProtectionType(TUint16 aCPType)
	{
	iCPType = aCPType;
	}
	
EXPORT_C const TDesC8& TAvdtpContentProtectionCapabilities::ContentProtectionData() const
	{
	return iCPData;
	}
	
EXPORT_C void TAvdtpContentProtectionCapabilities::SetContentProtectionData(const TDesC8& aCPData)
	{
	iCPData.Copy(aCPData);
	}
		
EXPORT_C /*virtual*/ TInt TAvdtpContentProtectionCapabilities::AsProtocol(RBuf8& aBuffer) const
	{
	// must set LOSC before adding header
	iLOSC = KMinContentProtectionLOSC + iCPData.Length(); // length of CPType+CPData
	
	TInt ret = AddHeader(aBuffer);
	if (ret == KErrNone)
		{
		aBuffer.Append(reinterpret_cast<const TUint8*>(&iCPType), KMinContentProtectionLOSC);
		aBuffer.Append(iCPData);
		}
	return ret;
	}

EXPORT_C void TAvdtpContentProtectionCapabilities::Parse(const TDesC8& aConfig)
	{
	iCPType = LittleEndian::Get16(aConfig.Ptr());
	iCPData = aConfig.Right(aConfig.Length()-2);
	}
	
