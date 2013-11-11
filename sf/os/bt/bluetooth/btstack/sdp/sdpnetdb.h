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

#ifndef SDPNETDB_H
#define SDPNETDB_H

#include <es_prot.h>
#include <bttypes.h>

class CSdpClient;
class CSdpProtocol;
class TSdpState;

/**
   The net db provider access point.
   This class forms the major interaction point with RNetDatabase classes
**/
NONSHARABLE_CLASS(CSdpNetDbProvider) : public CNetDBProvdBase
	{
friend class CSdpProtocol;
friend class CSdpClient;
public:
	// Creation
	static CSdpNetDbProvider* NewL(CSdpProtocol& aProtocol);
	~CSdpNetDbProvider();
	
	// CNetDBProvdBase functions
	void Query(TDes8& aBuffer);
	void Add(TDes8& aBuffer);
	void Remove(TDes8& aBuffer);
	void CancelCurrentOperation();
	// End CNetDBProvdBase functions
	
	// Notifications from the sdp client
	void ClientUp();
	void ClientDown();
	void QueryComplete(const TDesC8& aData);

    // Other functions
	void Error(TInt aErrorCode);
	//From CResolverProvdBase
	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);
private:
	// Creation
	CSdpNetDbProvider(CSdpProtocol& aProtocol);
	void ConstructL();

	// Query Dispatch
	void ConnectQuery();
	void ServiceQuery();
	void AttributeQuery();
	void EncodedQuery();
	void RetrieveResult();

	TInt ReturnResult(const TDesC8& aData);

	// From CNetDBProvdBase
	TInt CheckCapability(TUint32 aCapability, TUint aType);
	TInt CheckCapabilityPerPolicy(const TUint32 aCapability, const TUint aPolicyIndex) const;


private:
	//Data
	TBTDevAddr iRemoteDev;
	TDes8* iQueryBuffer;
	HBufC8* iResultBuf;

	// Links
	CSdpProtocol& iProtocol;
	CSdpClient*  iClient;
	// So we can go on a Que in the client
	TDblQueLink    iLink;
	//Mixin providing security checking, This is not an owned variable.
	MProvdSecurityChecker* iSecurityChecker;
	};

#endif
