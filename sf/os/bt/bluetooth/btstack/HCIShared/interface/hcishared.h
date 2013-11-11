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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HCISHARED_H
#define HCISHARED_H

#include <bttypes.h>

/** HCIShared Panic code
 */
_LIT(KHCISharedPanicName, "HCIShared Panic");

enum THCISharePanic
	{
	EBTUnknownLowPowerMode		=0
	};

/** Size of featureset
 */
const TInt KBTHCIFeaturesBytes = 8;

NONSHARABLE_CLASS(TLinkPolicy)
/**
	The following class captures all the link policy settings
**/
	{
public:
	IMPORT_C TBool IsModeAllowed(TBTLinkMode aMode) const;
	IMPORT_C TBool IsSwitchAllowed() const;

	IMPORT_C void SetModeAllowed(TBTLinkMode, TBool aAllowed);
	IMPORT_C void SetModesAllowed(TUint8 aModesAllowed);
	IMPORT_C void SetSwitchAllowed(TBool aAllowed);

	IMPORT_C TUint8 LowPowerModePolicy() const;

	IMPORT_C TUint8 LinkPolicy() const;

private:
	TUint8 iLowPowerModePolicy;
	TBool	iSwitchAllowed;
	};

NONSHARABLE_CLASS(TBTBasebandLinkState)
	{
public:
	enum TLinkState
		{
		ELinkDown,
		ELinkPending,
		ELinkUp,
		};

	IMPORT_C TBTBasebandLinkState();

	IMPORT_C void SetLinkState(TLinkState aState);
	IMPORT_C void SetAuthenticated(TBool aAuthenticated);
	IMPORT_C void SetEncrypted(TBool aEncrypted);
	IMPORT_C void SetLinkMode(TBTLinkMode aMode);
	IMPORT_C void SetLinkRole(TBTBasebandRole aRole);
	IMPORT_C void SetPacketTypes(TUint16 aPacketTypes);
	IMPORT_C void SetMaxSlots(TUint8 aMaxSlots);

	IMPORT_C TLinkState LinkState() const;
	IMPORT_C TBool Authenticated() const;
	IMPORT_C TBool Encrypted() const;
	IMPORT_C TBTLinkMode LinkMode() const;
	IMPORT_C TBTBasebandRole LinkRole() const;
	IMPORT_C TUint16 PacketTypes() const;
	IMPORT_C TUint8 MaxSlots() const;

protected:
	TLinkState		iLinkState;
	TBool			iAuthenticated;
	TBool			iEncrypted;
	TBTLinkMode		iMode;
	TBTBasebandRole iRole;
	TUint16			iPacketTypes;
	TUint8			iMaxSlots;
	};


/**
Handle the local and remote supported features bitmasks.  Provides utility
functions to extract items of interest, and individual bits.
*/
NONSHARABLE_CLASS(TBTFeatures)
	{
	public:
		IMPORT_C explicit TBTFeatures();
		IMPORT_C explicit TBTFeatures(TUint64 aFeatures);
		IMPORT_C TBool operator[](TInt aOffset) const;
		
		IMPORT_C TBTLinkModeSet LinkModes() const;
		IMPORT_C TBTSCOPackets SCOPackets() const;
		IMPORT_C TBTSyncPacketTypes SyncPackets() const;
		
		IMPORT_C TBool IsEncryptionSupported() const;
		IMPORT_C TBool IsRoleSwitchSupported() const;
		IMPORT_C TBool IsSecureSimplePairingSupported() const;
		IMPORT_C TBool IsModeSupported(TBTLinkMode aMode) const;
		IMPORT_C TBool IsEncryptionPauseResumeSupported() const;
		IMPORT_C TBool IsExtendedInquiryResponseSupported() const;
		IMPORT_C TBool IsRssiWithInquiryResultsSupported() const;
		IMPORT_C TBool IsSCOLinkSupported() const;
		IMPORT_C TBool IsExtendedSCOLinkSupported() const;
	
	private:
		void Init(TUint64 aFeatures);
		TUint32 Extract32Bits(TInt aStart, TInt aEnd) const;
		TUint32 Extract32BitsOffset(TInt aStart, TInt aEnd, TInt aOffset) const;
		
	private:
		TBuf8<KBTHCIFeaturesBytes> iFeatures;
	};
#endif // HCISHARED_H
