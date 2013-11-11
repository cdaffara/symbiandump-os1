// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _GAVDPINTERNAL_H
#define _GAVDPINTERNAL_H

#include <gavdp.h>
#include "gavdpinterface.h"
#include "e32debug.h"

class CGavdpHelper;

NONSHARABLE_CLASS(CGavdp) : public CBase
{
friend class CGavdpHelper;
friend class CGavdpDiscover;
friend class CGavdpStart;
friend class CGavdpGetCaps;
friend class CGavdpAbort;
friend class CGavdpSuspend;
friend class CGavdpSelectSEP;
friend class CGavdpConnector;
//friend class CGavdpPassiveSignallingConnector;
friend class CGavdpIndicator;
friend class CGavdpUPlaneConnector;

public:
	static CGavdp* NewL(MGavdpUser& aServiceUser, RSocketServ& aSocketServer, RGavdp& aHandle);
	~CGavdp();

	TInt Listen();

	void Cancel();
	
	void Connect(const TBTDevAddr& aRemoteAddr);
	void DiscoverRemoteSEPs();
	void GetRemoteSEPCapabilities(TSEID aSEID, const TAvdtpServiceCategories& aInterestedCategories);

	TInt RegisterSEP(TAvdtpSEPInfo& aInfo); // SEID will be written back

	TInt BeginConfiguringRemoteSEP(TSEID aRemoteSEID, TSEID aLocalSEID);
	TInt BeginConfiguringLocalSEP(TSEID aSEID);
	TInt AddSEPCapability(const TAvdtpServiceCapability& aCapability);
	void CommitSEPConfiguration();
	
	void StartStream(TSEID aSEID);
	void SuspendStream(TSEID aSEID);
	void AbortStream(TSEID aSEID);
	
	void SendSecurityControl(TSEID aSEID, const TDesC8& aSecurityData);
	
	TInt CreateBearerSockets(TSEID aSEID);

	inline RSocket&	AvdtpRequester(); // not available for use outside DLL because we define the inline in the cpp
	inline RSocket&	AvdtpIndicator(); // not available for use outside DLL because we define the inline in the cpp
	inline RSocketServ& AvdtpSession(); // not available for use outside DLL because we define the inline in the cpp
	
	TInt Shutdown();
private:
	CGavdp(MGavdpUser& aServiceUser, RSocketServ& aSocketServer, RGavdp& aHandle);
	void ConstructL();	
	void ConstructSocketsAndHelpersL();
	void DeAllocSocketsAndHelpers();
	void SendResponse(TInternalAvdtpAirIndication aIndication,
					  TInt aResult,
					  const TAny* aResponseData=NULL,
					  TInt aResponseDataLen=0);
	void ServiceComplete(TInt aResult); // cleans up helpers and shift RGavdp statemachine
	void BindSignallingL(const TBTDevAddr& aRemoteAddr);
	void BindBearersL(TSEID aSEID, TBool aRequireReporting, TBool aRequireRecovery);
	inline TBool CheckSEID(TSEID aSEID);
	void Error(TInt aError);
	void FatalError();
private:
	enum TGavdpState
		{
		EIdle,
		EConfiguringLocalSEP,
		EConfiguringRemoteSEP,
		EListening,
		EConnecting,
		EConnected,
		};
private:
	RSocketServ		iSocketServer;
	RSocket			iAvdtpRequester;
	RSocket			iAvdtpIndicator;
	CGavdpHelper*	iRequesterHelper;
	CGavdpHelper*	iIndicatorHelper;
	TGavdpState		iState;
	RSocketServ&	iClientSocketServer;
	MGavdpUser&		iServiceUser;
	TInt			iNumSEPsRegistered;
	RGavdp&			iHandle;
	};

/**
Faults - GAVDP itself has failed
**/	
enum TGavdpFault
	{
	EGavdpHelperTryingToUseNullGavdp,
	EGavdpUnexpectedAvdtpError,
	EGavdpHelperCompletingBadly,
	};
	
static void Panic(TGavdpPanic aPanic)
// Panic client due to programming error.
	{
	_LIT(KGavdpPanicName, "GAVDP Usage error");
	RDebug::Print(_L("%S panic=%d"), &KGavdpPanicName, aPanic);
	User::Panic(KGavdpPanicName, aPanic);
	}

static void Fault(TGavdpFault aFault)
// Panic due to GAVDP fault
	{
	_LIT(KGavdpFaultName, "GAVDP fault");
	User::Panic(KGavdpFaultName, aFault);
	}


NONSHARABLE_CLASS(CGavdpHelper) : public CActive
	{
public:
	virtual void Begin() =0;
protected:
	CGavdpHelper(CGavdp& aGavdp, MGavdpUser& aUser);
	TInt RunError(TInt aError);
	inline RSocket& AvdtpRequester();
	inline RSocket& AvdtpIndicator();
	inline RSocketServ& AvdtpSession();
	void CheckFailedL();
	void DetachFromGavdp(TInt aError);
	inline CGavdp& Gavdp(); // not const because of return
	inline MGavdpUser& User(); // not const because of return
private:
	CGavdp*			iGavdp;	// NULL once helper s free-running
	MGavdpUser&		iUser;
	};
	
/**
Helper that enables indications to be raised by AVDTP
As a current imlpementation, this constantly queues ioctls
(AVDTP assists by queing them too)
@internalComponent
*/
NONSHARABLE_CLASS(CGavdpIndicator) : public CGavdpHelper
	{
public:
	static CGavdpIndicator* NewL(CGavdp& aGavdp, MGavdpUser& aUser);
	virtual void Begin();
	~CGavdpIndicator();
private:
	CGavdpIndicator(CGavdp& aGavdp, MGavdpUser& aUser);
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void CheckClientResult(TInt aResult);
private:
	TBuf8<KAvdtpMaxInternalIndicationSize> iIndication;
	};
	
	
NONSHARABLE_CLASS(CGavdpDiscover) : public CGavdpHelper
	{
public:
	static CGavdpDiscover* NewL(CGavdp& aGavdp, MGavdpUser& aUser);
	virtual void Begin();
	~CGavdpDiscover();
private:
	CGavdpDiscover(CGavdp& aGavdp, MGavdpUser& aUser);
	void RunL();
	void DoCancel();
private:
	TPckgBuf<TAvdtpInternalDiscoverConfirm>	iConfirm;
	};
	

NONSHARABLE_CLASS(CGavdpGetCaps) : public CGavdpHelper
	{
public:
	static CGavdpGetCaps* NewL(CGavdp& aGavdp,
				 			  MGavdpUser& aUser,
				 			  TSEID aSEID,
				 			  TAvdtpServiceCategories aInterestingCategories);
	virtual void Begin();
	~CGavdpGetCaps();
private:
	CGavdpGetCaps(CGavdp& aGavdp, MGavdpUser& aUser,
					TSEID aSEID, TAvdtpServiceCategories aInterestingCategories);
	void ConstructL();
	void RunL();
	void DoCancel();
private:
	HBufC8*					iSEPCapBuf; // general input and output from socket, hence not typed
	TPtr8					iSEPCapPtr;
	TSEID					iSEID;
	TAvdtpServiceCategories iInteresting;
	};
	

NONSHARABLE_CLASS(CGavdpAbort) : public CGavdpHelper
	{
public:
	static CGavdpAbort* NewL(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSEID);
	
	virtual void Begin();
	~CGavdpAbort();
private:
	CGavdpAbort(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSEID);
	void RunL();
	void DoCancel();
private:
	TPckgBuf<TSEID> iSEID;
	};


NONSHARABLE_CLASS(CGavdpStart) : public CGavdpHelper
	{
public:
	static CGavdpStart* NewL(CGavdp& aGavdp,
					  		  MGavdpUser& aUser,
					  		  TSEID aSeid);
					  		   			  
	virtual void Begin();
	~CGavdpStart();
private:
	CGavdpStart(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSeid);
	void RunL();
	void DoCancel();
private:			
	TPckgBuf<TSEID>	iSeidPckg;
	};


NONSHARABLE_CLASS(CGavdpSuspend) : public CGavdpHelper
	{
public:
	static CGavdpSuspend* NewL(CGavdp& aGavdp,
					  			  MGavdpUser& aUser,
					  			  TSEID aSeid);
					  		   			  
	virtual void Begin();
	~CGavdpSuspend();
private:
	CGavdpSuspend(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSeid);
	void RunL();
	void DoCancel();
private:			
	TPckgBuf<TSEID>	iSeidPckg;
	};

NONSHARABLE_CLASS(CGavdpSecurityControl) : public CGavdpHelper
	{
public:
	static CGavdpSecurityControl* NewL(CGavdp& aGavdp,
					  		  MGavdpUser& aUser,
					  		  TSEID aSeid,
					  		  const TDesC8& aSecurityControl);
					  		   			  
	virtual void Begin();
	~CGavdpSecurityControl();
private:
	CGavdpSecurityControl(CGavdp& aGavdp, MGavdpUser& aUser, TSEID aSeid,
					  		  const TDesC8& aSecurityControl);
	void RunL();
	void DoCancel();
private:			
	TPckgBuf<TAvdtpInternalSendSecurityControl>	iSecurityControlPckgBuf;
	};

NONSHARABLE_CLASS(CGavdpSelectSEP) : public CGavdpHelper
	{
public:
	static CGavdpSelectSEP* NewL(CGavdp& aGavdp, MGavdpUser& aUser);
	
	virtual void Begin();
	~CGavdpSelectSEP();
private:
	CGavdpSelectSEP(CGavdp& aGavdp, MGavdpUser& aUser);
	void RunL();
	void DoCancel();
private:
	TPckgBuf<TAvdtpConfigReject> iRejDataBuf; // in case we get rejected by peer
	};


NONSHARABLE_CLASS(CGavdpConnector) : public CGavdpHelper
	{
public:
	static CGavdpConnector* NewL(CGavdp& aGavdp, MGavdpUser& aUser, const TBTDevAddr& aAddr, TBool aPassive = EFalse);
	
	virtual void Begin();
	~CGavdpConnector();
private:
	CGavdpConnector(CGavdp& aGavdp, MGavdpUser& aUser, const TBTDevAddr& aAddr, TBool aPassive);
	void RunL();
	void DoCancel();
private:
	TAvdtpSockAddr	iSocketAddress;
	TBool			iIsPassiveConnector;	//true when listening
	};

class CGavdpUPlaneConnector;

NONSHARABLE_CLASS(CSocketConnector) : public CActive
	{
public:
	static CSocketConnector* NewL(CGavdpUPlaneConnector& aParent,
									RSocketServ& aSocketServer,
									const TAvdtpSockAddr& aSockAddr);
	inline RSocket& Socket(); // return type => non-const method
	inline const TAvdtpSockAddr& Address() const;
	virtual void RunL();
	virtual void DoCancel();
	~CSocketConnector();
	void Begin();
private:
	CSocketConnector(CGavdpUPlaneConnector& aParent,
									RSocketServ& aSocketServer,
									const TAvdtpSockAddr& aSockAddr);
	void ConstructL();
private:
	CGavdpUPlaneConnector& iParent;
	RSocket iSocket;
	RSocketServ& iSocketServer;
	TAvdtpSockAddr iSockAddr;
	};
	
inline RSocket& CSocketConnector::Socket()
	{
	return iSocket;
	}
	
inline const TAvdtpSockAddr& CSocketConnector::Address() const
	{
	return iSockAddr;
	}

	
NONSHARABLE_CLASS(CGavdpUPlaneConnector) : public CGavdpHelper
	{
public:
	static CGavdpUPlaneConnector* NewL(CGavdp& aGavdp, MGavdpUser& aUser,
								 	   const TAvdtpSockAddr& aAddr,
									   TBool aRequireReporting,
									   TBool aRequireRecovery,
						  			   RSocketServ& aClientSession);
	
	virtual void Begin();
	void ConnectComplete(TInt aErr);
	~CGavdpUPlaneConnector();
private:
	CGavdpUPlaneConnector(CGavdp& aGavdp,
						  MGavdpUser& aUser,
						  const TAvdtpSockAddr& aAddr,
						  TBool aRequireReporting,
						  TBool aRequireRecovery,
						  RSocketServ& aClientSession);
	void RunL();
	void DoConnectComplete(TInt aErr);
	void DoCancel();
	void AllBearersReadyL();
	void ConstructL(TBool aRequireReporting, TBool aRequireRecovery);
private:
	TFixedArray<CSocketConnector*,ENumStreamTypes> iSocketConnectors;
	RSocketServ&	iClientSession;
	TInt			iNumBearersRequired;
	TAvdtpSockAddr	iAddress; // just has remote BT Addr and SEID
	};
	
inline RSocket& CGavdp::AvdtpRequester()
	{
	return iAvdtpRequester;
	}

inline RSocket& CGavdp::AvdtpIndicator()
	{
	return iAvdtpIndicator;
	}
	
inline RSocketServ& CGavdp::AvdtpSession()
	{
	return iSocketServer;
	}

inline RSocket& CGavdpHelper::AvdtpRequester()
	{
	return Gavdp().AvdtpRequester();
	}

inline RSocket& CGavdpHelper::AvdtpIndicator()
	{
	return Gavdp().AvdtpIndicator();
	}
	
inline RSocketServ& CGavdpHelper::AvdtpSession()
	{
	return Gavdp().AvdtpSession();
	}
		
inline CGavdp& CGavdpHelper::Gavdp()
	{
	// free running helpers cannot interact with GAVDP
	__ASSERT_ALWAYS(iGavdp, Fault(EGavdpHelperTryingToUseNullGavdp));
	return *iGavdp;
	}
	
#endif // _GAVDPINTERNAL_H

