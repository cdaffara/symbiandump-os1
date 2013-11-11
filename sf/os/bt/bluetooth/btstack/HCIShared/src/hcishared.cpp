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
*/

#include <bluetooth/hci/hcishared.h>
#include "btfeatures.h"


enum TBTFeaturesPanic
	{
	ERangeTooLarge,
	};

void FeaturesPanic(TBTFeaturesPanic aPanic)
	{
	_LIT(KPanicText, "BTFeatures");
	User::Panic(KPanicText, aPanic);
	}

void Panic(THCISharePanic aPanic)
// Panic due to stack bug.
	{
	User::Panic(KHCISharedPanicName, aPanic);
	}

/** 
Constructor
Start with no requests to update the required configuration.
*/

EXPORT_C TBTFeatures::TBTFeatures()
	{
	Init(0);
	}

/** 
Constructor
Start with request to update the required configuration

@param aFeatures the required bluetooth features
*/

EXPORT_C TBTFeatures::TBTFeatures(TUint64 aFeatures)
	{
	Init(aFeatures);
	}


void TBTFeatures::Init(TUint64 aFeatures)
	{
	const TInt KMaxBit = 7;
	iFeatures.SetMax();
	for (TInt pos = 0; pos < iFeatures.MaxSize(); pos++)
		{
		TUint8 byte = (pos <= KMaxBit) ? (aFeatures >> (pos * 8)) & 0xff : 0;
		iFeatures[pos] = byte;
		}
	}

TUint32 TBTFeatures::Extract32BitsOffset(TInt aStart, TInt aEnd, TInt aOffset) const
	{
	return Extract32Bits(aStart, aEnd) << aOffset;
	}

TUint32 TBTFeatures::Extract32Bits(TInt aStart, TInt aEnd) const
	{
	__ASSERT_DEBUG(aEnd - aStart <= 32, FeaturesPanic(ERangeTooLarge));
	TUint32 result =0;
	TInt outOffset = 0;
	for (TInt bit = aStart; bit <= aEnd; bit++)
		{
		result |= (operator[](bit) ? 1<<outOffset : 0);
		outOffset++;
		}
	
	return result;
	}


/**
Subscript operator.

Return an individual bit from BT features

@param aOffset Object to compare to this.
@return ETrue if the individual bit is 1, otherwise EFalse returned
*/

EXPORT_C TBool TBTFeatures::operator[](TInt aOffset) const
	{
	TInt byte = aOffset / 8;
	TInt offset = aOffset - (byte * 8);
	return TBool(iFeatures[byte] & (1<<offset));
	}


/** 
Return current link mode set  

@return current link mode set 
*/

EXPORT_C TBTLinkModeSet TBTFeatures::LinkModes() const
	{
	return TBTLinkModeSet(Extract32Bits(ESupportedHoldModeBit, ESupportedParkModeBit));
	}


/** 
Return current SCO packets type

@return current SCO packets type 
*/
EXPORT_C TBTSCOPackets TBTFeatures::SCOPackets() const
	{
	return TBTSCOPackets(Extract32Bits(ESupportedSCOLinkBit, ESupportedHV3PacketsBit) * EPacketsHV1);
	}

/** 
Return current synchronous packet types (SCO and eSCO on v1.2).

@return current synchronous packet types (SCO and eSCO on v1.2).
*/
EXPORT_C TBTSyncPacketTypes TBTFeatures::SyncPackets() const
	{
	TBTSyncPacketTypes packets = Extract32Bits(ESupportedSCOLinkBit, ESupportedHV3PacketsBit) | Extract32BitsOffset(ESupportedExtendedSCOLinkBit, ESupportedEV5PacketsBit, 3);
	return packets;
	}

/** 
Return current supported mode

@param aMode specified link modes to compare 
@return ETrue if specified link modes are supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsModeSupported(TBTLinkMode aMode) const
	{
	return LinkModes() & aMode;
	}

/** 
Return whether role switch is supported

@return ETrue if role switch is supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsRoleSwitchSupported() const
	{
	return operator[](ESupportedSwitchBit);
	}

/** 
Return whether encryption is supported

@return ETrue if encryption is supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsEncryptionSupported() const
	{
	return operator[](ESupportedEncryptionBit);
	}

/** 
Return whether Extended Inquiry Response is supported

@return ETrue if Extended Inquiry Response is supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsExtendedInquiryResponseSupported() const
	{
	return operator[](ESupportedExtendedInquiryResponseBit);
	}

/** 
Return whether Read signal strength indication with inquiry is supported

@return ETrue if Read signal strength indication with inquiry is supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsRssiWithInquiryResultsSupported() const
	{
	return operator[](ESupportedRSSIWithInquiryResultsBit);
	}

/** 
Return whether Secure Simple Pairing is supported

@return ETrue if Secure Simple Pairing is supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsSecureSimplePairingSupported() const
	{
	return operator[](ESupportedSecureSimplePairingBit);
	}

/** 
Return whether Encryption Pause and Resume is supported

@return ETrue if Encryption Pause and Resume is supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsEncryptionPauseResumeSupported() const
	{
	return operator[](ESupportedEncryptionPauseResumeBit);
	}

/** 
Return whether SCO link is supported

@return ETrue if SCO link is supported, otherwise EFalse returned
*/
EXPORT_C TBool TBTFeatures::IsSCOLinkSupported() const
	{
	return operator[](ESupportedSCOLinkBit);
	}

/** 
Return whether Extended SCO link is supported

@return ETrue if Extended SCO link is supported, otherwise EFalse returned
*/

EXPORT_C TBool TBTFeatures::IsExtendedSCOLinkSupported() const
	{
	return operator[](ESupportedExtendedSCOLinkBit);
	}



/** 
Indicate whether a low power mode is allowed

@param aMode Bluetooth SIG specified values for indicating link modes
@return ETrue if the indicated low power modes are allowed, otherwise return EFalse
*/

EXPORT_C TBool TLinkPolicy::IsModeAllowed(TBTLinkMode aMode) const
	{
	TBool allowed;

	switch (aMode)
		{
		case EHoldMode:
		case ESniffMode:
		case EParkMode:
			allowed = iLowPowerModePolicy & aMode;
			break;

		default:
			Panic(EBTUnknownLowPowerMode);
			allowed = EFalse;
		}
	
	return allowed;
	}

/** 
Indicate whether mode switch is allowed

@return ETrue if mode switch is allowed, otherwise return EFalse
*/

EXPORT_C TBool TLinkPolicy::IsSwitchAllowed() const
	{
	return iSwitchAllowed;
	}

/** 
Set permission or prohibition to a specific low power mode

@param aMode Bluetooth SIG specified values for indicating link modes
@param aAllowed the permission or prohibition to a specific low power mode
*/

EXPORT_C void TLinkPolicy::SetModeAllowed(TBTLinkMode aMode, TBool aAllowed)
	{
	switch (aMode)
		{
		case EHoldMode:
		case ESniffMode:
		case EParkMode:
			aAllowed ? iLowPowerModePolicy |= aMode : iLowPowerModePolicy &= ~aMode;
			break;

		default:
			Panic(EBTUnknownLowPowerMode);
		}
	}

/** 
Set permission to low power modes

@param aModesAllowed the low power modes to be allowed or not
*/

EXPORT_C void TLinkPolicy::SetModesAllowed(TUint8 aModesAllowed)
	{
	iLowPowerModePolicy = aModesAllowed;
	}

/** 
Set permission to low power modes switch

@param aAllowed the low power modes switch to be allowed or not
*/

EXPORT_C void TLinkPolicy::SetSwitchAllowed(TBool aAllowed)
	{
	iSwitchAllowed = aAllowed;
	}


/** 
Return current low power mode policy 

@return current low power mode policy
*/

EXPORT_C TUint8 TLinkPolicy::LowPowerModePolicy() const
	{
	return iLowPowerModePolicy;
	}

/** 
Return current link policy  

@return current link policy 
*/

EXPORT_C TUint8 TLinkPolicy::LinkPolicy() const
	{
	return static_cast<TUint8>(iSwitchAllowed + (iLowPowerModePolicy << 1));
	}


/** 
Constructor
Start with default configuration
*/
EXPORT_C TBTBasebandLinkState::TBTBasebandLinkState()
 :	iLinkState(ELinkDown), 
	iAuthenticated(EFalse), 
	iEncrypted(EFalse),
	iMode(EActiveMode),
	iRole(ERoleUnknown),
	iPacketTypes(EAnyPacket),
	iMaxSlots(5)
	{
	}

/** 
Set the link state

@param aState link state values 
*/
EXPORT_C void TBTBasebandLinkState::SetLinkState(TLinkState aState)		
	{ 
 	iLinkState = aState; 
	}

/** 
Set the authentication state

@param aAuthenticated TBool value indicates whether authentication is allowed or not
*/
EXPORT_C void TBTBasebandLinkState::SetAuthenticated(TBool aAuthenticated)
	{ 
 	iAuthenticated = aAuthenticated;
	}

/** 
Set the encryption state

@param aEncrypted TBool value indicates whether encryption is allowed or not
*/
EXPORT_C void TBTBasebandLinkState::SetEncrypted(TBool aEncrypted)
	{ 
 	iEncrypted = aEncrypted;
	}

/** 
Set the link mode

@param aMode TBool value indicates the BT link mode
*/
EXPORT_C void TBTBasebandLinkState::SetLinkMode(TBTLinkMode aMode)			
	{ 
 	iMode = aMode;
	}

/** 
Set the link role

@param aRole Bluetooth SIG specified values for specification of (piconet) role
*/
EXPORT_C void TBTBasebandLinkState::SetLinkRole(TBTBasebandRole aRole)		
	{ 
 	iRole = aRole;
	}

/** 
Set the packet types

@param aPacketTypes Bluetooth SIG specified packet types
*/
EXPORT_C void TBTBasebandLinkState::SetPacketTypes(TUint16 aPacketTypes)	
	{ 
 	iPacketTypes = aPacketTypes;
	}

/** 
Set the maximum types

@param aMaxSlots LMP maximum slot
*/
EXPORT_C void TBTBasebandLinkState::SetMaxSlots(TUint8 aMaxSlots)				
	{ 
 	iMaxSlots = aMaxSlots;
	}

/** 
Return current baseband link state 

@return current baseband link state  
*/
EXPORT_C TBTBasebandLinkState::TLinkState TBTBasebandLinkState::LinkState() const				
	{ 
	return iLinkState;
	}

/** 
Return current authentication status 

@return current authentication status  
*/
EXPORT_C TBool TBTBasebandLinkState::Authenticated() const					
	{ 
	return iAuthenticated;
	}

/** 
Return current encryption status 

@return current encryption status  
*/
EXPORT_C TBool TBTBasebandLinkState::Encrypted() const						
	{ 
	return iEncrypted;
	}

/** 
Return current baseband link state 

@return current baseband link state 
*/
EXPORT_C TBTLinkMode TBTBasebandLinkState::LinkMode() const				
	{ 
	return iMode;
	}

/** 
Return current baseband link role 

@return current baseband link role 
*/
EXPORT_C TBTBasebandRole TBTBasebandLinkState::LinkRole() const			
	{ 
	return iRole;
	}

/** 
Return current baseband packet types 

@return current baseband packet types 
*/
EXPORT_C TUint16 TBTBasebandLinkState::PacketTypes() const					
	{ 
	return iPacketTypes;
	}

/** 
Return current baseband max slots 

@return current baseband max slots 
*/
EXPORT_C TUint8 TBTBasebandLinkState::MaxSlots() const						
	{ 
	return iMaxSlots;
	}
