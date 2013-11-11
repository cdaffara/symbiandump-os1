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

#ifndef __REQSTPROCESSOR_H
#define __REQSTPROCESSOR_H

#include <e32base.h>
#include <badesca.h>

class CEtelMngr;
class CReqstDoer;
class CRpsMsg;

class MReqstObserver
	{
public:
	virtual void ReqstCompleted() =0;
	};

//
// CRqstProcessorr.
// Creates the appropriate Request objects in order to deal with the request
//
// Not intended for derivation
class CReqstProcessor : public CBase, MReqstObserver
	{
public:
	static CReqstProcessor* NewL(CEtelMngr& aEtel);
	~CReqstProcessor();
 
	TInt ProcessL(CRpsMsg& aRqstMsg);
	
	//from MReqstObserver
	virtual void ReqstCompleted();
	
private:
	void ConstructL();
	CReqstProcessor(CEtelMngr& aEtel);
	void CleanupReqstDoer();
	
private:
	CReqstDoer*			iReqstDoer;
	CEtelMngr&			iEtel;	// Not owned
	};



#endif
