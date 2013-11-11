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
// Defines the RawConduit class - for linking the Proxy SAP to the ACL SAP
// 
//

#ifndef _RAWCONDUIT_H
#define _RAWCONDUIT_H

#include <es_prot.h>

class CBTProxySAP;
class CACLLink;

NONSHARABLE_CLASS(CACLRawConduit) : public CBase, public MSocketNotify
	{
public:
	static CACLRawConduit* NewL(CBTProxySAP& aParent);
	~CACLRawConduit();

	TInt Write(const TDesC8& aDesc,TUint aOptions);
	void GetData(TDes8& aData);

// from MSocketNotify
	virtual void NewData(TUint aCount);
	virtual void CanSend();
	virtual void ConnectComplete();
	virtual void ConnectComplete(const TDesC8& aConnectData);
    virtual void ConnectComplete(CServProviderBase& aSSP);
	virtual void ConnectComplete(CServProviderBase& aSSP,const TDesC8& aConnectData);
	virtual void CanClose(TDelete aDelete=EDelete);
	virtual void CanClose(const TDesC8& aDisconnectData,TDelete aDelete=EDelete);
	virtual void Error(TInt anError,TUint anOperationMask=EErrorAllOperations);
	virtual void Disconnect(void);
	virtual void Disconnect(TDesC8& aDisconnectData);
	virtual void IoctlComplete(TDesC8 *aBuf);
	virtual void NoBearer(const TDesC8& /*aConnectionInfo*/);
	virtual void Bearer(const TDesC8& aConnectionInfo);
private:
	void ConstructL();
	CACLRawConduit(CBTProxySAP& aProxySAP);
private:
	CBTProxySAP&	iParent;
	CACLLink*		iACLLink;
	HBufC8*			iReceiveBuf;
	TBool			iReady;
	};

#endif //#ifdef _RAWCONDUIT_H
