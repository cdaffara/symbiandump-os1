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
 @internalComponent
 @released
*/

#ifndef __SYMBIAN_DEN_ROOTSERVERCHANNELHANDLER_H__
#define __SYMBIAN_DEN_ROOTSERVERCHANNELHANDLER_H__

#include <cfutil.h>
//#include "sd_thread.h"

namespace Den
/** Namespace for the Server Den
@internalComponent
@released
*/
{

/**
@class CWorkerRootServChannelHandler
The adapter responsible for communicating with the Root Server over the channel given with the
CommsFW::TCFModuleInfo structure delivered via the parameter to the main thread function.
*/
NONSHARABLE_CLASS(CWorkerRootServChannelHandler) : public CommsFW::CCFModuleChannelHandler
/**
@internalComponent
*/
	{
	typedef CommsFW::CCFModuleChannelHandler inherited;
public:
	IMPORT_C static CWorkerRootServChannelHandler* NewL(CommsFW::RCFChannel::TMsgQueues aRxQueues,
		CommsFW::RCFChannel::TMsgQueues aTxQueues, CCommonWorkerThread* aWorkerThread);
	IMPORT_C TInt Send(const CommsFW::TCFMessage& aMessage);
private:
	CWorkerRootServChannelHandler(CCommonWorkerThread* aWorkerThread);
	virtual void CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage);
	virtual void CFMessageDiscover(const CommsFW::TCFDiscoverMsg& aMessage);
	virtual void CFMessageBind(const CommsFW::TCFBindMsg& aMessage);
	virtual void CFMessageUnbind(const CommsFW::TCFUnbindMsg& aMessage);
	virtual void CFMessageForward(const CommsFW::TCFForwardMsg& aMessage);
private:
	/** Pointer back to the worker thread owning this instance. */
	CCommonWorkerThread* iWorkerThread;
	};

} // Den

#endif // __SYMBIAN_DEN_ROOTSERVERCHANNELHANDLER_H__


