// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__PSIONIR_H__)
#define __PSIONIR_H__

#include "ir_sock.h"
#include "e32base.h"

// eikirda can handle two kinds of put - file and data
enum TEikirdaTransferDataType {EData,EFile};

//**** Client must implement these notify callbacks ****
class MEikirdaSenderNotify
	{
public:
	virtual void PutProgressIndication(TInt aPercent)=0;
	virtual void AbortIndication()=0;
	virtual void ErrorIndication(TInt aError)=0;
	};

class MEikirdaReceiverNotify
	{
public:
	virtual void DiscoveryIndication(TDes& aDes)=0;
	virtual void ConnectIndication(TInt aErr,TDes& aDes)=0;
	virtual void DisconnectIndication()=0;
	virtual void AbortIndication()=0;
	virtual void PutRequestIndication(TDes& aDes)=0;
	virtual void PutProgressIndication(TInt aErr,TDes& aDes)=0;
	virtual void PutCompleteIndication(TDes& aDes)=0;
	virtual void ErrorIndication(TInt aErr)=0;
	};
//****												****

NONSHARABLE_CLASS(CEikirdaSenderBase) : public CBase
	{
public:
	virtual void ConstructL(MEikirdaSenderNotify *aClient)=0;
	virtual void Discover(TIrdaSockAddr& aSockAddr,THostName& aName,TRequestStatus& aStat)=0;
	virtual void Query(const TDes& aClass,const TDesC& aAttribute,TIASResponse& aResults,TRequestStatus& aStat)=0;
	virtual void Connect(TSockAddr& aAddr,TRequestStatus& aStat)=0;
	virtual void PutL(TEikirdaTransferDataType aType,const TDesC& aDesc,TRequestStatus& aStat)=0;
	virtual void Abort()=0;
	virtual void Disconnect(TRequestStatus& aStat)=0;
	virtual TInt SetOpt(TInt aOptionName,TInt aOptionLevel,const TDesC& aDesc)=0;
	virtual TInt GetOpt(TInt aOptionName,TInt aOptionLevel,TDes& aOption)=0;
	virtual void CompleteRequest(TInt aError)=0;
	// internal indications to be passed up to client side
	inline void PutProgressIndication(TInt aPercent) {if (iClient) iClient->PutProgressIndication(aPercent);};
	inline void ErrorIndication(TInt aError) {if (iClient) iClient->ErrorIndication(aError);};
protected:
	MEikirdaSenderNotify *iClient;
	};

NONSHARABLE_CLASS(CEikirdaReceiverBase) : public CBase
	{
public:
	virtual void ConstructL(MEikirdaReceiverNotify *aClient)=0;
	virtual void Start()=0;
	virtual void Stop()=0;
//protected:
	// internal indications to be passed up to client side
//	TInt DiscoveryIndication(TDes& aDes);
	TInt ConnectIndication(TInt aErr,TDes& aDes);
	inline void DisconnectIndication() {if (iClient) iClient->DisconnectIndication();};
	inline void PutRequestIndication(TDes& aDes) {if (iClient) iClient->PutRequestIndication(aDes);};
	TInt PutProgressIndication(TInt aErr,TDes& aDes);
	inline void PutCompleteIndication(TDes& aDes) {if (iClient) iClient->PutCompleteIndication(aDes);};
//protected:
	MEikirdaReceiverNotify *iClient;
	};

NONSHARABLE_CLASS( CEikirdaFactoryBase) : public CObject
	{
public:
	virtual CEikirdaSenderBase *NewEikirdaSenderL(MEikirdaSenderNotify *aClient)=0;
	virtual CEikirdaReceiverBase *NewEikirdaReceiverL(MEikirdaReceiverNotify *aClient)=0;
	virtual void ConstructL()=0;
	};

NONSHARABLE_CLASS(CEikirdaFactory) : public CEikirdaFactoryBase
    {
public:
	enum {KMajorVersion=1,KMinorVersion=0,KBuild=205};
	static CEikirdaFactory *NewL();
	virtual CEikirdaSenderBase *NewEikirdaSenderL(MEikirdaSenderNotify *aClient);
	virtual CEikirdaReceiverBase *NewEikirdaReceiverL(MEikirdaReceiverNotify *aClient);
	virtual void ConstructL();
	virtual ~CEikirdaFactory();	
private:
    CEikirdaFactory();
private:
	TVersion iVersion;
    };

//
// The remaining stuff should go into a private header file.
// Concrete instantiations
//
class CEikirdaSenderControl;
class CEikirdaReceiverControl;
NONSHARABLE_CLASS(CEikirdaSender) : public CEikirdaSenderBase
	{
public:
	virtual ~CEikirdaSender();
	static CEikirdaSenderBase *NewL(MEikirdaSenderNotify *aClient);
	// concrete instantiation of base class pure virtuals
	virtual void ConstructL(MEikirdaSenderNotify *aClient);
	virtual void Discover(TIrdaSockAddr& aAddr,THostName& aName,TRequestStatus& aStat);
	virtual void Query(const TDes& aClass,const TDesC& aAttribute,TIASResponse& aResults,TRequestStatus& aStat);
	virtual void Connect(TSockAddr& aAddr,TRequestStatus& aStat);
	virtual void PutL(TEikirdaTransferDataType aType,const TDesC& aDesc,TRequestStatus& aStat);
	virtual void Abort();
	virtual void Disconnect(TRequestStatus& aStat);
	virtual TInt SetOpt(TInt aOptionName,TInt aOptionLevel,const TDesC& aDesc);
	virtual TInt GetOpt(TInt aOptionName,TInt aOptionLevel,TDes& aOption);
	virtual void CompleteRequest(TInt aError);
private:
	CEikirdaSender();
private:
	TRequestStatus *iStatus;
	CEikirdaSenderControl *iSenderStation;
	};

NONSHARABLE_CLASS(CEikirdaReceiver) : public CEikirdaReceiverBase
	{
public:
	virtual ~CEikirdaReceiver();
	static CEikirdaReceiverBase *NewL(MEikirdaReceiverNotify *aClient);
	// concrete instantiation of base class pure virtuals
	virtual void Start();
	virtual void Stop();
private:
	CEikirdaReceiver();
	void ConstructL(MEikirdaReceiverNotify *aClient);
private:
	TRequestStatus iStatus;
	CEikirdaReceiverControl *iReceiverStation;
	};

#endif
