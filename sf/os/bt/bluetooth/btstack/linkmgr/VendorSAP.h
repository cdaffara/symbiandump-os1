// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _VENDORSAP_H
#define _VENDORSAP_H

#include <es_prot.h>
#include <bluetooth/hci/hciframe.h>

class CLinkMgrProtocol;

NONSHARABLE_CLASS(CVendorSAP) : public CServProviderBase
	{
public:
	static CVendorSAP* NewL(CLinkMgrProtocol& aProtocol);
	~CVendorSAP();

// from SAP
	virtual void Start();
	virtual void LocalName(TSockAddr& anAddr) const ;
	virtual TInt SetLocalName(TSockAddr& anAddr);
	virtual void RemName(TSockAddr& anAddr) const ;
	virtual TInt SetRemName(TSockAddr& anAddr);
	virtual TInt GetOption(TUint level,TUint name,TDes8& anOption)const ;
	virtual void Ioctl(TUint level,TUint name,TDes8* anOption);
	virtual void CancelIoctl(TUint aLevel,TUint aName);
	virtual TInt SetOption(TUint level,TUint name,const TDesC8 &anOption);
	virtual void ActiveOpen();
	virtual void ActiveOpen(const TDesC8& aConnectionData);
	virtual TInt PassiveOpen(TUint aQueSize);
	virtual TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData);
	virtual void Shutdown(TCloseType option);
	virtual void Shutdown(TCloseType option,const TDesC8& aDisconnectionData);
	virtual void AutoBind();
	TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* aAddr);
	void GetData(TDes8& aDesc,TUint options,TSockAddr* anAddr=NULL);

	// for HCIFacade
	void VendorCommandCompleteEvent(TInt aErr, const TDesC8* aEvent);
	void VendorDebugEvent(TInt aErr, const TDesC8* aEvent);
	//From CServProviderBase
	TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker);

protected:
	//Mixin providing security checking, This is not an owned variable.
	MProvdSecurityChecker* iSecurityChecker;

private:
	CVendorSAP(CLinkMgrProtocol& aProtocol);
	void NotifyIoctlComplete(TInt aErr, TUint aLevel, TUint aName, const TDesC8* aBuffer);

	TUint iIoctlName;
	TUint iIoctlLevel;
	
	// struct for queing the events
	struct TVendorDebugEvent
		{
		TBuf8<CHctlCommandFrame::KHCIMaxCommandLength> iEventBuf;	// max is per H1 spec
		TInt iEventError;
		TDblQueLink iEventLink;
		};
	
	TDblQue<TVendorDebugEvent>		iDebugEventQueue; //queue for vendor specific events
	CLinkMgrProtocol&				iProtocol;	


	TVendorDebugEvent* NewDebugEventData(const TDesC8& aBuf, TInt aError);
	void TryToCompleteVendorDebugEventIoctl();
	};

#endif // _VENDORSAP_H
