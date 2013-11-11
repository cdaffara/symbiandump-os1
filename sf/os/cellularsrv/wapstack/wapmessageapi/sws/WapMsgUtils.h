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
// Utilities 
// 
//

#ifndef __CSWSWAPMSGUTILS_H__ 
#define __CSWSWAPMSGUTILS_H__

#include <e32base.h>
#include <e32std.h>
#include <es_sock.h>
#include <wapmessage.h>

const TInt KWapStackMaxDatagramSize = 4000;
const TInt KWapStackNorminalDatagramSize = 4000;



class CSWSWapMsgUtils : public CBase
/** 
Provide a set of static function which to be used by SWS. Contains the functions to build a
specific socket address from given parameters, analyse a specific socket address, and to
determine the SMS bearer type, GSM.
@internalComponent
@released
@since v8.0
*/
	{
public:
	// Returns the current local address, a null pointer must be passed in, buffer ownership is given to the client
	static TInt GetLocalAddress(HBufC8*& aLocalHost);
	static void BuildAddrL(TSockAddr& aAddr, Wap::TBearer aBearer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort);
	static void AnalyseAddrL(TSockAddr& aAddr, Wap::TBearer aBearer, TDes8& aRemoteHost, Wap::TPort& aRemotePort);

	};

#endif //__CSWSWAPMSGUTILS_H__

