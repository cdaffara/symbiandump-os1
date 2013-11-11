// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef REQHANDLER_H
#define REQHANDLER_H

#include <e32std.h>
#include <es_sock.h>
#include "sdpconsts.h"
#include "ServiceSearchVisitor.h"
#include "ExtractorVisitor.h"

static const TInt KSdpMaxReqHandlers = 7;

//class CSdpListener;
class CSdpConnection;
class TSdpPdu;

class SdpReqHandler
	{
public:
	static void HandleL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu);
	static TInt RunError(TInt aError, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu);
private:
	static void HandleServiceSearchL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu);
	static void HandleServiceAttributeL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu);
	static void HandleServiceSearchAttributeL(CSdpDatabase &aDatabase, const TSdpPdu &aReqPdu, TSdpPdu &aRespPdu);
	};


#endif
