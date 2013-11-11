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
#include "gavdpinterface.h" // for mux capability

/**TAvdtpServiceCapability() is for Symbian use only
@internalTechnology
*/
EXPORT_C TAvdtpServiceCapability::TAvdtpServiceCapability(TAvdtpServiceCategory aCategory, TInt aLOSC/*=KMaxTInt*/)
: iLOSC(aLOSC), iCategory(aCategory)
/*
	Base class c'tor
*/
	{
	}

/**Returns the category
@see TAvdtpServiceCategory
*/
EXPORT_C TAvdtpServiceCategory TAvdtpServiceCapability::Category() const
	{
	return iCategory;
	}

EXPORT_C /*virtual*/ void TAvdtpServiceCapability::Parse(const TDesC8&)
	{
	//This is the base implementation which is NULL as some caps
	//just have their header.
	}

/**AllocFromPDUL() is for Symbian use only
@internalTechnology
*/
EXPORT_C /*static*/ TAvdtpServiceCapability* TAvdtpServiceCapability::AllocFromPDUL(TAvdtpServiceCategory aCat,
																					const TDesC8& aDes)
	{
	TAvdtpServiceCapability* res=NULL;

	switch (aCat)
		{
		case EServiceCategoryMediaTransport:
			{
			res = new (ELeave) TAvdtpMediaTransportCapabilities;
			}
			break;
		case EServiceCategoryReporting:
			{
			res = new (ELeave) TAvdtpReportingCapabilities;
			}
			break;
		case EServiceCategoryRecovery:
			{
			res = new (ELeave) TAvdtpRecoveryCapabilities;
			}
			break;
		case EServiceCategoryContentProtection:
			{
			res = new (ELeave) TAvdtpContentProtectionCapabilities;
			}
			break;
		case EServiceCategoryHeaderCompression:
			{
			res = new (ELeave) TAvdtpHeaderCompressionCapabilities;	
			}
			break;
		case EServiceCategoryMultiplexing:
			{
			res = new (ELeave) TAvdtpMultiplexingCapability;
			}
			break;
		case EServiceCategoryMediaCodec:
			{
			res = TAvdtpMediaCodecCapabilities::AllocCodecFromPDUL(aDes);
			}
			break;
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	
	if (res)
		{
		// got a class into which it can parse itself from the protocol
		res->Parse(aDes); //non leaving
		}
	
	return res;
	}

	
/**AllocFromPckgL() is for Symbian use only
@internalTechnology
*/
EXPORT_C /*static*/ TAvdtpServiceCapability* TAvdtpServiceCapability::AllocFromPckgL(const TDesC8& aPackagedCap)
	{
	TInt len = aPackagedCap.Length();
	TAny* buf = User::AllocL(len);
	Mem::Copy(buf, aPackagedCap.Ptr(), len);
	return reinterpret_cast<TAvdtpServiceCapability*>(buf);
	}
	
	
TInt TAvdtpServiceCapability::AddHeader(RBuf8& aBuffer) const
/*
	Small helper to factor growing of buffer, and addition of a header
*/
	{
	// check to see if variable LOSC capabilities have updated LOSC
	__ASSERT_ALWAYS(iLOSC!=KMaxTInt, User::Invariant());
	
	TInt err = aBuffer.ReAlloc(aBuffer.Length()+iLOSC+KAvdtpServiceCapabilitiesHeaderLen);

	if (err==KErrNone)
		{
		aBuffer.Append(iCategory);
		aBuffer.Append(iLOSC);
		}
	
	return err;
	}

