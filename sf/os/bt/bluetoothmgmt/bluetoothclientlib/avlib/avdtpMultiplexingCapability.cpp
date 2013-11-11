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
// avdtpMediaMultiplexing.cpp
// 
//

/**
 @file
 @internalTechnology
*/

#include <bluetoothav.h>
#include "gavdpinterface.h"


EXPORT_C void TAvdtpMultiplexingCapability::Parse(const TDesC8& aPtr)
	{
	// The following code uses a TUint8* as a TTSID* and TTCID*.  If the
	// size of either of these were to change (they are currently defined 
	// as TUint8) this could cause alignment errors on hardware.
	__ASSERT_COMPILE(sizeof(TTSID) == 1);
	__ASSERT_COMPILE(sizeof(TTCID) == 1);

	// Maxlength of iCID's iSID's plus 1 for iFrag and 1 for array size/last element adjustment
	TInt maxLength = iSIDs.Count()+iCIDs.Count()+2; 
	__ASSERT_DEBUG(aPtr.Length()>2 && aPtr.Length()<maxLength, Panic(EInvalidCapabilityDataLength));	// the parse broke if this is less than 3, 
		// but if there are 8 or more bytes we could overflow the iSIDs & iCIDs arrays.
	static const TUint8 KFragOffset = 7;
	iFrag = static_cast<TBool>(aPtr[0]>>KFragOffset);
	
	// parsing here means we have to leave it up to the user of this class
	// to have found configuration before determining semantics of the array
	// of IDs, the air configuration alone is *impossible* to tell what the 
	// second entry is (either reporting or recovery)
	TInt arrayIndex=0;
	for (TInt packetIndex=1; packetIndex<=aPtr.Length()-sizeof(TTCID); packetIndex+=sizeof(TTSID)+sizeof(TTCID))
		{
		iSIDs[arrayIndex] = *static_cast<const TTSID*>(aPtr.Mid(packetIndex, sizeof(TTSID)).Ptr());
		iCIDs[arrayIndex] = *static_cast<const TTCID*>(aPtr.Mid(packetIndex+sizeof(TTSID), sizeof(TTCID)).Ptr());
		arrayIndex++;
		}
	}

EXPORT_C /*virtual*/ TInt TAvdtpMultiplexingCapability::AsProtocol(RBuf8& aBuffer) const
	{
	TInt losc = 3; // min - frag octet + media ids

	__ASSERT_DEBUG(iCIDs[0]!=KInvalidTCID && iSIDs[0]!=KInvalidTSID, /*Panic(EAVDTPBadMuxConfiguration)*/User::Invariant());
	
	// tsid and tcid must either both be valid or not
	__ASSERT_DEBUG((iCIDs[1]!=KInvalidTCID && iSIDs[1]!=KInvalidTSID) ||
					   (iCIDs[1]==KInvalidTCID && iSIDs[1]==KInvalidTSID), /*Panic(EAVDTPBadMuxConfiguration)*/User::Invariant());
	
	// tsid and tcid must either both be valid or not
	__ASSERT_DEBUG((iCIDs[2]!=KInvalidTCID && iSIDs[2]!=KInvalidTSID) ||
				   (iCIDs[2]==KInvalidTCID && iSIDs[2]==KInvalidTSID), /*Panic(EAVDTPBadMuxConfiguration)*/User::Invariant());	

	// check to see how many entries will be made - can just use say CID to test
	if (iCIDs[1]!=KInvalidTCID)
		{
		losc+=2;
		}
		
	if (iCIDs[2]!=KInvalidTCID)
		{
		losc+=2;
		}
		
	// set LOSC for building header
	iLOSC=losc;		
	TInt ret = AddHeader(aBuffer);

	if (ret==KErrNone)
		{
		aBuffer.Append(iFrag ? 0x80 : 0x00);
		
		//only append in set ones - must always have media
		for (TInt arrayIndex=0; arrayIndex<=iCIDs.Count()-1; arrayIndex++)
			{
			// test either CID, or SID for whether to put in capability
			if (iCIDs[arrayIndex]!=KInvalidTCID)
				{
				aBuffer.Append(reinterpret_cast<const TUint8*>(&iSIDs[arrayIndex]),sizeof(TTSID));
				aBuffer.Append(reinterpret_cast<const TUint8*>(&iCIDs[arrayIndex]),sizeof(TTCID));
				}
			}
		}	

	return ret;
	}
	

EXPORT_C void TAvdtpMultiplexingCapability::Reset()
	{
	for (TInt arrayIndex=0; arrayIndex<=iCIDs.Count()-1; arrayIndex++)
		{
		iSIDs[arrayIndex] = KInvalidTSID;
		iCIDs[arrayIndex] = KInvalidTCID;
		}
	}

	
