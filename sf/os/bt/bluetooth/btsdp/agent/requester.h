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

#ifndef BTSDPAGENTREQUESTER_H
#define BTSDPAGENTREQUESTER_H

#include <e32base.h>
#include <bttypes.h>
#include "ProtocolWrapper.h"


NONSHARABLE_CLASS(CSdpRequesterBase) : public CActive
	{
public:
	enum TConnectState
		{
		EDisconnected,
		ERequesting,
		EIdle,
		};

	~CSdpRequesterBase();
protected:
	CSdpRequesterBase(RSdpSession& aSdpSession, CSdpAgentEng& aParent);
	void NewRequestL(const TBTDevAddr& aRemDev);
	void RetrieveResponseL();
	virtual void IssueRequestL() =0;
	virtual void RequestCompleteL() =0;
	virtual void Reset();
	virtual void DoCancel();

private:
	void ConnectL();
	void Disconnect();
	void RunL();


private:
	// Base functionality
	TInt iRetryCount;
	RTimer iIdleTimer;
	// Response buffer
	HBufC8* iResponseHBuf;

protected:
	TConnectState iState;
	RSdpSession& iSdpSession;
	CSdpAgentEng& iParent;
	TBTDevAddr iRemoteAddress;
	RSdpAgent iRequester;
	// Ptr to the response buffer
	TInt iResultSize;
	TPtr8 iResponse;
	};


NONSHARABLE_CLASS(CSdpSearchRequester) : public CSdpRequesterBase
	{
public:
	static CSdpSearchRequester* NewL(RSdpSession& aSdpSession, CSdpAgentEng& aParent);
	~CSdpSearchRequester();

	void SearchRequestL(const TBTDevAddr& aRemoteDev,
						CSdpSearchPattern& aUUIDFilter,
						TUint16 aMaxCount, 
						const TDesC8& aContState);
private:
	CSdpSearchRequester(RSdpSession& aSdpSession, CSdpAgentEng& aParent);
	void ConstructL();

	// From CSdpRequesterBase;
	virtual void IssueRequestL();
	virtual void RequestCompleteL();
	TInt RunError(TInt aError);
	void DoCancel();

	void SendSearchRequest();
	void HandleSearchResponseL();
	void Reset();

private:
	// Current request
	//CSdpAttrValueDES* iUUIDList;
	CSdpSearchPattern* iUUIDFilter;
	TUint16 iMaxRecCount;
	TPtrC8 iContState;
	};



NONSHARABLE_CLASS(CSdpAttributeRequester) : public CSdpRequesterBase
	{
public:
	static CSdpAttributeRequester* NewL(RSdpSession& aSdpSession, CSdpAgentEng& aParent);
	~CSdpAttributeRequester();

	void AttributeRequestL(const TBTDevAddr& aRemoteDev,
						   TSdpServRecordHandle aHandle, 
						   TUint16 aMaxAttrByteCount,
						   CSdpAttrIdMatchList& aMatchList,
						   const TDesC8& aContState);

private:
	CSdpAttributeRequester(RSdpSession& aSdpSession, CSdpAgentEng& aParent);
	void ConstructL();

	// From CSdpRequesterBase;
	virtual void IssueRequestL();
	virtual void RequestCompleteL();
	TInt RunError(TInt aError);
	void DoCancel();

	void SendAttributeRequest();
	void HandleAttributeResponseL();
	void Reset();

private:
	// Current request
	TSdpServRecordHandle iHandle;
	TUint16 iMaxAttrByteCount;
	CSdpAttrIdMatchList* iMatchList;	
	TPtrC8 iContState;
	};

#endif
