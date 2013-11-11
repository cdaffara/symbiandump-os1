// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#include <bluetooth/a2dpoptimisation.h>
#include <bluetooth/a2dpoptimisationparams.h>

#include <bluetooth/hciserverclient.h>
#include <bluetooth/hci/hciipc.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_A2DP_OPTIMISER);
#endif

EXPORT_C RA2dpOptimiser::RA2dpOptimiser()
	: iHCIServerSession(NULL)
	{
	}

/**
Opens the handle to the A2DP Optimisation interface.

@return KErrNone if successful, otherwise a standard Symbian error code
*/
EXPORT_C TInt RA2dpOptimiser::Open()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(!iHCIServerSession, PANIC(KA2dpOptimiserUtilsPanic, EHandleIsAlreadyOpen));
	
	TInt err = KErrNoMemory;
	iHCIServerSession = new RHCIServerSession;
	if(iHCIServerSession)
		{
		err = iHCIServerSession->Open(KHCIA2dpOptimserManagerUid);
		if(err != KErrNone)
			{
			delete iHCIServerSession;
			iHCIServerSession = NULL;
			}
		}
	return err;
	}

/**
Closes the handle to the A2DP Optimisation interface.

Note that it is safe to call this multiple times.
*/
EXPORT_C void RA2dpOptimiser::Close()
	{
    LOG_FUNC
	if(iHCIServerSession)
		{
		iHCIServerSession->Close();
		delete iHCIServerSession;
		iHCIServerSession = NULL;
		}
	}

/**
Attempt to optimise the ACL for A2DP streaming.

This method should return immediately without waiting for confirmation of optimisation
from the hardware as it will be called directly before streaming is required. This
method can also be called multiple times without having to remove the optimisation to
allow for changing streaming parameters.

The Peak Bandwidth argument is for the octets of A2DP data only, i.e. the overhead
of additional headers, L2CAP for example, should not be considered by the caller of this 
method. 

@param aBDAddr Address of the remote device recieving audio stream
@param aPeakBandwidth Peak Bandwidth of audio stream in octets per second
@param aAccessLatency Access Latency of audio stream in microseconds
*/
EXPORT_C void RA2dpOptimiser::OptimiseAclForAudioStreaming(const TBTDevAddr& aBDAddr, TUint aPeakBandwidth, TUint aAccessLatency)
	{
    LOG_FUNC
	__ASSERT_ALWAYS(iHCIServerSession, PANIC(KA2dpOptimiserUtilsPanic, EApiUsedWhenHandleIsNotOpen));	

	TPckgBuf<TA2dpOptimisationParams> args(TA2dpOptimisationParams(aBDAddr, aPeakBandwidth, aAccessLatency));
	static_cast<void>(iHCIServerSession->SendSync(EHCIA2dpOptimiseAcl, &args, NULL, 0));
	}


/**
Attempt to remove any A2DP optimisation that was previously set using 
OptimiseAclForAudioStreaming().

@param aBDAddr Address of the remote device recieving audio stream
*/
EXPORT_C void RA2dpOptimiser::RemoveAclOptimisation(const TBTDevAddr& aBDAddr)
	{
    LOG_FUNC
	__ASSERT_ALWAYS(iHCIServerSession, PANIC(KA2dpOptimiserUtilsPanic, EApiUsedWhenHandleIsNotOpen));
	
	TPckgBuf<TBTDevAddr> addrBuf(aBDAddr);
	static_cast<void>(iHCIServerSession->SendSync(EHCIA2dpRemoveAclOptimisation, &addrBuf, NULL, 0));
	}

//
// TA2dpOptimisationParams
//
EXPORT_C TA2dpOptimisationParams::TA2dpOptimisationParams(const TBTDevAddr& aBDAddr, TUint aPeakBandwidth, TUint aAccessLatency)
	: iBDAddr(aBDAddr), iPeakBandwidth(aPeakBandwidth), iAccessLatency(aAccessLatency)
	{
	LOG_FUNC
	}

EXPORT_C TA2dpOptimisationParams::TA2dpOptimisationParams()
	: iBDAddr(), iPeakBandwidth(0), iAccessLatency(0)
	{
	LOG_FUNC
	}

EXPORT_C TBTDevAddr TA2dpOptimisationParams::RemoteDeviceAddress() const
	{
	LOG_FUNC
	return iBDAddr;
	}

EXPORT_C TUint TA2dpOptimisationParams::PeakBandwidth() const
	{
	LOG_FUNC
	return iPeakBandwidth;
	}

EXPORT_C TUint TA2dpOptimisationParams::AccessLatency() const
	{
	LOG_FUNC
	return iAccessLatency;
	}

