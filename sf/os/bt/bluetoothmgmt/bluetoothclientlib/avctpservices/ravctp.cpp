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
//

/**
 @file
 @internalTechnology
*/
 
#include <bluetooth/logger.h>
#include <avctpservices.h>
#include "avctpbody.h"
#include "avctpserviceutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP_SERVICES);
#endif

using namespace SymbianAvctp;

#define ASSERT_OPEN __ASSERT_ALWAYS(iBody, Panic(EAvctpRAvctpNotOpen));
#define ASSERT_CLOSED __ASSERT_ALWAYS(!iBody, Panic(EAvctpRAvctpAlreadyOpen));


EXPORT_C RAvctp::RAvctp()
	{
	LOG_FUNC
	iBody = NULL;
	}

EXPORT_C TInt RAvctp::Open(MAvctpEventNotify& aNotify, TPid aPid, MAvctpChannel*& aPrimaryChannel)
	{
	LOG_FUNC
	ASSERT_CLOSED
	
	TRAPD(err, iBody = CAvctpBody::NewL(aNotify, aPid, aPrimaryChannel));
	return err;
	}
	
EXPORT_C void RAvctp::Close(TCloseType aImmediacy)
	{
	LOG_FUNC
	
	if (iBody)
		{
		iBody->Close(aImmediacy); // This tells iBody to delete itself
		iBody = NULL;
		}
	}
	
EXPORT_C void RAvctp::CloseGracefully()
	{
	LOG_FUNC
	ASSERT_OPEN
	
	iBody->CloseGracefully(); // This tells iBody to delete itself
	}
		

/**
 * This is separated out from RAvctp::Open so that the client can choose to run RAvctp in two-thread mode
 * in which the "second channel" Send and receive service provide in AVCTP Services is installed
 * on another thread if the client so chooses.
 *
 * */
EXPORT_C TInt RAvctp::InstallSecondaryChannel(MAvctpEventNotify& aSecondChannelObserverWillyNilly, MAvctpChannel*& aSecondaryChannel)
	{
	LOG_FUNC
	ASSERT_OPEN
	
	return iBody->InstallSecondaryChannel(aSecondChannelObserverWillyNilly, aSecondaryChannel);
	}

EXPORT_C void RAvctp::UninstallSecondaryChannel(TCloseType aImmediacy)
	{
	LOG_FUNC
	ASSERT_OPEN
	
	iBody->UninstallSecondaryChannel(aImmediacy);
	}

// EOF
