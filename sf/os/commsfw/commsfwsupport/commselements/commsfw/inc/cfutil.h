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

#ifndef __CFUTIL_H__
#define __CFUTIL_H__

#include <cfshared.h>
#include <cfmsgs.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cflog.h>
#endif
#include <elements/cfmacro.h>

namespace CommsFW 
//CommsFW namespace doxygen is supplied in cfmsgs.h. The namespace is @publishedPartner, @released
{
class TCFForwardMsg;
class CCFModuleChannelHandler:public CActive
/** Utility class to help holding a channelpair, listening for incoming messages
and sending responses for CPMs needing a quick way to implement support
for the basic messages Discover, Bind, Unbind and Shutdown.
All the virtuals have default bodies so only those that require special treatment
needs implementation.
@publishedPartner 
@released
*/
	{
	protected:
		IMPORT_C TInt Send(const CommsFW::TCFMessage &aMessage);
		IMPORT_C ~CCFModuleChannelHandler();
		IMPORT_C CCFModuleChannelHandler(TInt aPriority);
		IMPORT_C virtual void CFMessageDiscover(const CommsFW::TCFDiscoverMsg& aMessage);
		IMPORT_C virtual void CFMessageBind(const CommsFW::TCFBindMsg& aMessage);
		IMPORT_C virtual void CFMessageUnbind(const CommsFW::TCFUnbindMsg& aMessage);
		IMPORT_C virtual void CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage);
		IMPORT_C virtual void CFMessageForward(const CommsFW::TCFForwardMsg& aMessage);
		IMPORT_C virtual void CFMessageUnknown(const CommsFW::TCFMessage& aMessage);
		IMPORT_C virtual void CFChannelError(TInt aError);
		IMPORT_C void ConstructL(CommsFW::RCFChannel::TMsgQueues &aRxQueues,
			                     CommsFW::RCFChannel::TMsgQueues &aTxQueues);
	private:
		IMPORT_C virtual void DoCancel();
		IMPORT_C virtual void RunL();
	private:
		CommsFW::RCFChannelPair iChannelPair;
	}; // CCFModuleChannelHandler



IMPORT_C TInt ConvertVal(const TDesC8& aVar, TUint32& aResult);
IMPORT_C TInt ConvertVal(const TDesC8& aVar, TUint16& aResult);

} // CommsFW

#endif // __CFUTIL_H__


