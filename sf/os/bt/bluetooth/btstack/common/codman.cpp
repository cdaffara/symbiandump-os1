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
// Class of Device (CoD) Service Manager.
// Manages the Service bit settings in the composite CoD word.
// The CoD is a 24 bit word, of which the upper eleven bits describe the Service class 
// profiles a Bluetooth device supports. Since these Services can be offered by transitory
// applications, we need to provide mechanisms that allow applications to set the Service
// bits accordingly. Bits 2 to 12 are the Service class and these are sub-divided into major 
// and minor classifications. Bits 0 and 1 are currently always zero.
// 23                  13 12      8  7         2 1 0
// |<--- Service class -->|<--- Device class --->
// [ . . . . . . . ][ . . . . . . . ][ . . . . . .0.0]
// The Services are identified by the following table.
// 23        13	
// v         v
// Service LimitedDiscoverableMode     00000000001 ......this is not used
// reserved                            00000000010
// reserved                            00000000100
// Service Positioning                 00000001000
// Service Networking                  00000010000
// Service Rendering                   00000100000
// Service Capturing                   00001000000
// Service ObjectTransfer              00010000000
// Service Audio                       00100000000
// Service Telephony                   01000000000
// Service Information                 10000000000
// Because the Symbian OS handles data in 32bit and 16bit sizes, the CoD word is always
// transported using an unsigned 32bit type. The MS byte is ignored. 
// In order to correctly process the Service bits, they are right-shifted 13 bit positions
// and then they are managed in a 16 bit type.
// Its feasible that multiple applications will offer the same service, therefore the 
// CoD Service Manager must keep a separate count (iServRefCount[]) of each Service bit set 
// and the bit is only removed from the CoD when the counter is decremented to zero.
// The practical implementation is that the CSM privately owns an array of eleven counters.
// Each SAP has an iCodServiceBits (16bit type), inherited from the base class which is a
// record of the Service bits set by that SAP. So the SAPs are know their individual Service
// bit settings and the CSM knows the totals.
// As a further precaution against mis-use, each SAP will be allowed one shot at setting
// the bits. This is controlled by assigning the msb (of iCodServiceBits) as a flag which
// will be TRUE when the Service bit(s) have been set. This is defined as KBTCodBitsRegdFlag.
// The action of the Publish and Suscribe interface (P&S) for writing the CoD remains, but is 
// now sub-ordinate to the SAP managed Service bits. The P&S execution path has been diverted
// from Linkmgr to the CSM. The P&S sets the CoD Device class bits as before, but its
// Service bit(s) are ORed with the SAP settings before writing the CoD.   
// 
//


#include <bluetooth/logger.h>
#include "codman.h"
#include "linkmgr.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_IN_CONN_LISTENER);
#endif

//------------------------------------------------------------------------//
//class CBTCodServiceMan
//------------------------------------------------------------------------//

void Panic(TBTCodPanic aPanic)
	{
	LOG_STATIC_FUNC
	User::Panic(KBTCodPanic, aPanic);
	}

CBTCodServiceMan* CBTCodServiceMan::NewL()
	{
#ifdef __FLOG_ACTIVE
	CONNECT_LOGGER
#endif
	LOG_STATIC_FUNC
	CBTCodServiceMan* self = new(ELeave) CBTCodServiceMan();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CBTCodServiceMan::CBTCodServiceMan()
	{
	LOG_FUNC
	iServRefCount.Reset();
	}

void CBTCodServiceMan::ConstructL()
	{
	LOG_FUNC
	}

CBTCodServiceMan::~CBTCodServiceMan()
	{
	LOG_FUNC
	__ASSERT_DEBUG(ResolveServiceBits()==NULL, Panic(EBTCodServiceBits));
#ifdef __FLOG_ACTIVE
	CLOSE_LOGGER
#endif
	}

void CBTCodServiceMan::Initialise()
	{
	LOG_FUNC
	iPandSCod = iLinkMgr->iLocalDeviceSettings.DeviceClass();	// iPandSCod now has valid CoD 
	}

TInt CBTCodServiceMan::RegisterCodService(TUint16 aCodService)
/*
   This method is called via the base class (CBluetoothSAP), when an L2CAPSAP 
   or RFCommSAP becomes active. It keeps a count of the Service bits being set.
*/
	{
	LOG_FUNC
	TInt err = KErrNone;
	TUint index = 0;
	
	while (aCodService != 0)
		{
		if (aCodService & KBTCodServiceLSB)
			{
			iServRefCount[index]++;
			}
		index++;
		aCodService >>= 1;
		}

	TRAP(err, WriteCoDL());
	return err;
	}

	
TInt CBTCodServiceMan::RemoveCodService(TUint16 aCodService)
	{
/*
   This method is called via the base class (CBluetoothSAP), when an L2CAPSAP 
   or RFCommSAP closes. It decrements the service bit counters and if any reach
   zero, the CoD will be written and delete the bit(s).
*/
	LOG_FUNC
	TInt err = KErrNone;
	TUint index = 0;
	TBool removeService = EFalse;
		
	while (aCodService != 0)
		{
		if (aCodService & KBTCodServiceLSB)
			{
			__ASSERT_DEBUG(iServRefCount[index] > 0, Panic(EBTCodBadCount));
			if(iServRefCount[index])
				{
				if (--iServRefCount[index] == 0)
					{
					removeService = ETrue;
					}
				}
			}
		aCodService >>= 1;
		index++;
		}

	if (removeService)
		{
		TRAP(err, WriteCoDL());
		}
	return err;
	}
	
	
TUint16 CBTCodServiceMan::ResolveServiceBits()
	{
/*
   This private method will sweep through the iServRefCounts and return
   a tally of the Service bit positions currently set.
*/
	LOG_FUNC
	TUint16 serviceBits = 0;

	for (TUint i=0; i<KBTMaxCodServices; i++)
		{
		if (iServRefCount[i])
			{
			serviceBits |= 1<<i;
			}
		}
	return serviceBits;
	}


TInt CBTCodServiceMan::PandSCodHandler(TUint32 aCod)
	{
	LOG_FUNC
	// The P&S interface wants to write the CoD value
	TInt err = KErrNone;

	aCod &= KBTCodMask;							// ensure upper unused byte is clear
	iPandSCod = aCod;							// save new P&S CoD word
	TRAP(err, WriteCoDL());
	return err;
	}


void CBTCodServiceMan::WriteCoDL()
	{
/*
   This private method is called whenever SAP services bits require updating.
   These bits are obtained from the iServRefCounts and ORed with P&S service
   settings before being written to the CoD.
*/
	LOG_FUNC
	TUint32 sapServiceBits;

	sapServiceBits = ResolveServiceBits();		// sweep through the iServRefCounts 
	sapServiceBits <<= KBTCodServiceOffset;		// re-position the service bits

	__ASSERT_ALWAYS(iLinkMgr, Panic(EBTCodBadLinkmgr));
	iLinkMgr->WriteClassOfDeviceL(sapServiceBits | iPandSCod);	// write Cod with managed Service bits 
	}
	
