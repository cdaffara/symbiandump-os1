// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __REQSTPARSER_H
#define __REQSTPARSER_H

#include <e32base.h>
#include <badesca.h>

class CRequestHandler;
class CRpsMsg;
//
// CRequestParser.
// Parses and verifies a request msg from Master.
//
// Not intended for derivation
class CReqstParser : public CBase
	{
public:
	static TInt ExtractMsgLenL(const TDesC8& aHeaderAndLen);
	};


//
// CHandShakeReqstParser.
// Checks for handshake type requests and retrieves the requested data.
//
// Not intended for derivation
class CHandShakeReqstParser: public CBase
	{
public:
	static TBool ParseHSReqstL(CRequestHandler& aRqstHndlr, CRpsMsg& aReqstMsg, CDesCArrayFlat* aHSResponseDataArray);
	};
	
#endif
