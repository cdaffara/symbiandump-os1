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

#ifndef __REQSTHANDLER_H
#define __REQSTHANDLER_H

#include <e32base.h>
#include <badesca.h>

class CListener;
class CReqstParser;
class CRespBuilder;
class CReqstProcessor;
class CEtelMngr;


// Not intended for derivation
class CRequestHandler : public CBase
	{
public:
	static CRequestHandler* NewL();
	~CRequestHandler();
	
	void WaitForRemoteConnectionL();
	
	// Handles reqst received from Master. Called by CListener.
	const TDesC8& HandleRequestL(const TDesC8& aReqst);
	
	CEtelMngr* EtelMngr() 
		{return iEtelMngr;};
	
private:
	void ConstructL();
	
protected:
	CRequestHandler();
	
private:
	CListener* 			iListener;
	CReqstParser* 		iParser;
	CRespBuilder*		iRespBuilder;
	CReqstProcessor*	iReqstProcessor;
	CEtelMngr*			iEtelMngr;
	
private:
	HBufC8*			iResponseBuf;  // Holds the response msg to be sent to master
	};



#endif
