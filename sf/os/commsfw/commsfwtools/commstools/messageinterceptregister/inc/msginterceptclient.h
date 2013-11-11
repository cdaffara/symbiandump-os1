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
 @internalTechnology
*/

#ifndef __ELEMENTS_MSGINTERCEPTCLIENT_H__
#define __ELEMENTS_MSGINTERCEPTCLIENT_H__

#include <elements/nm_signatures.h>
#include <elements/sd_apiextensionclient.h>

class RMessageInterceptor : public Den::RApiExtSessionBase
	{
public:
	IMPORT_C void Close();

protected:
	RBuf8 iReqAndRespEventBuf;
	};

class RMessageInterceptorFM : public RMessageInterceptor
	{
public:
	IMPORT_C TInt Open(Den::RExtendableSessionBase& aExtendableInterface);
	IMPORT_C TInt EnableFallibleMode(TInt aCount);
	IMPORT_C void ConfigureFallibleMessagesL(RArray<Messages::TNodeSignal::TMessageId>& aArray/*, TAction& aAction = TDefaultAction*/);
	IMPORT_C TBool CheckFinished();
	};

class RMessageInterceptorPM : public RMessageInterceptor
	{
public:
	IMPORT_C TInt Open(Den::RExtendableSessionBase& aExtendableInterface);

	// pattern control
	//IMPORT_C TInt Enable();
	//IMPORT_C void Disable();
	};

#endif // __ELEMENTS_MSGINTERCEPTCLIENT_H__

