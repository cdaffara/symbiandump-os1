// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Source file for the api for talking to the stub AGPS Module
// 
//

/**
 @file
 @internalComponent
 @released
*/

#include <e32base.h>
#include <e32property.h>
#include "LbsLocSourceTestChannel.h"
#include "lbsdevloggermacros.h"

//const TUid  KLbsTestAgpsManagerUid = { 0x1028226E };
const TUid  KLbsTestSuiteUid 	   = { 0x1028226D };
const TUint KChannelKeyFromModule  = 0x12340010;
//const TUint KChannelKeyToModule  = 0x12340011;



//
// Channel Interface
//

/** */
EXPORT_C RAgpsModuleTestChannel::RAgpsModuleTestChannel()
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::RAgpsModuleTestChannel()");
	}
	
/** */
EXPORT_C void RAgpsModuleTestChannel::InitialiseL()
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::InitialiseL()");
	__ASSERT_DEBUG(KLbsTestSuiteUid == RProcess().SecureId(), User::Invariant());
	
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);

	TInt err = RProperty::Define(KChannelKeyFromModule, 
								 RProperty::EInt,
								 readPolicy,
								 writePolicy);
	
	if ((KErrNone != err) && (KErrAlreadyExists != err))
		{
		User::LeaveIfError(err);
		}
	}
	
/** */
EXPORT_C void RAgpsModuleTestChannel::Shutdown()
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::Shutdown()");
	RProperty::Delete(KChannelKeyFromModule);
	}

/** */
EXPORT_C void RAgpsModuleTestChannel::OpenL()
	{	
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::OpenL()");
	User::LeaveIfError(iPropFromModule.Attach(KLbsTestSuiteUid, KChannelKeyFromModule));
	}
	
/** */
EXPORT_C void RAgpsModuleTestChannel::Close()
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::Close()");
	iPropFromModule.Close();
	}

/** */
EXPORT_C void RAgpsModuleTestChannel::Subscribe(TRequestStatus &aRequest)
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::Subscribe()");
	iPropFromModule.Subscribe(aRequest);
	}

/** */
EXPORT_C void RAgpsModuleTestChannel::Cancel()
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::Close()");
	iPropFromModule.Cancel();
	}

/** */
EXPORT_C TInt RAgpsModuleTestChannel::SetMsg(TTestMsgType aMsgType)
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::SetMsg()");
	TInt err;
	err = iPropFromModule.Set(aMsgType);
	__ASSERT_ALWAYS(err == KErrNone, User::Invariant());
	return err;
	}

/** */
EXPORT_C TInt RAgpsModuleTestChannel::GetMsg(TTestMsgType& aMsgType)
	{
	LBSLOG(ELogP1, "RAgpsModuleTestChannel::SetMsg()");
	TInt tmp;
	TInt err;
	
	err = iPropFromModule.Get(tmp);
	aMsgType = static_cast<TTestMsgType>(tmp);
	return err;
	}
