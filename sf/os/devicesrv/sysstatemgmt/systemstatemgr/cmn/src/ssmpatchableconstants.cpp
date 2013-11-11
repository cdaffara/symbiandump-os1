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
// Description: Method to read patchable constant value.
//

#include <e32base.h>
#include <u32hal.h>
#include <e32svr.h>
#include <ssm/ssmpatchableconstants.h>

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
EXPORT_C TUint32 IsSsmGracefulShutdown()
	{
	TUint32 retVal = KSsmGracefulShutdown;

#ifdef __WINS__
	TUint32 valueOfKSsmGracefulShutdown = 0;
// KSsmGracefulShutdown is a ROM patchable constant, so need an emulator equivalent
// if WINS then read value from epoc.ini, requires licencees to set property in epoc.ini.
// Usage: In epoc.ini patchdata_ssmcmn_dll_KSsmGracefulShutdown 0xFFFFFFFF	(Can be any non-zero value)

	if (UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"patchdata_ssmcmn_dll_KSsmGracefulShutdown",&valueOfKSsmGracefulShutdown) == KErrNone)
		{
		retVal = valueOfKSsmGracefulShutdown;
		}
#endif // __WINS__
	return retVal;
	}
#endif // SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	
EXPORT_C TUint32 IsSsmGracefulOffline()
	{
	TUint32 retVal = KSsmGracefulOffline;

#ifdef __WINS__  
        // KSsmGracefulOffline is a ROM patchable constant, so need an emulator equivalent
        // if WINS then read value from epoc.ini, requires licencees to set property in epoc.ini. This value is 0xFFFFFFFF if feature is enabled.
        // Usage: In epoc.ini patchdata_KSsmGracefulOffline 0xFFFFFFFF (Can be any non-zero value)
        TUint32 readValueOfOffline = 0;
	    if( UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty, (TAny*)"patchdata_KSsmGracefulOffline", &readValueOfOffline) == KErrNone )
	        {
	        retVal = readValueOfOffline;
	        }
#endif // __WINS__
	return retVal;
	}
