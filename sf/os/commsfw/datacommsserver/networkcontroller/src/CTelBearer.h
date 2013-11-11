/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file CTelBearer.h
 @internalComponent
*/

#if !defined (__CTELBEARER_H__)
#define __CTELBEARER_H__

#include <comms-infras/cbearerbase.h>
#include <etelmm.h>
#include <etelpckt.h>
#include "sigstrgth.h"
#include "asynwrap.h"

_LIT(KTsyNameExtension,".tsy"); ///< KTsyNameExtension holds the extension for tsy.

NONSHARABLE_CLASS(CTelBearer) : public CBearerBase, public MAsyncRequestsObserver
/**
The class checks the availability of the telephony bearers (i.e. CSD and PSD (either GPRS or CDMA))

@internalComponent
*/
	{
public:
	virtual ~CTelBearer();
	static CTelBearer* NewLC(MBearerObserver* aObserver);
	static CTelBearer* NewL(MBearerObserver* aObserver);

	/** implementation of base class pure virtual */
	virtual TBool StartChecking();

	/** implementation of base class pure virtual */
	virtual TBool StartChecking(TBool aIsReconnect);

	/** implementation of base class pure virtual */
	virtual TBool StopChecking();

	/** implementation of base class pure virtual */
	virtual void Disconnect();

	/** override of base-class function */
	TInt SecondPhaseAvailability();

	/** implementation of MAsyncRequestObserver interface */
	void ETelAsyncRequestsComplete(TETelAsyncRequestData aRequestData);

protected:
	CTelBearer(MBearerObserver* aObserver);
	void ConstructL();

	/** implementation of base class pure virtual */
	virtual void AssignSupportedBearerSet();

private:
	void GetBearerSupportL();
	void OpenTelephonyL();
	void OpenPhoneL(RTelServer& aTelServer, const TDesC& aTsyName, RMobilePhone& aMmPhone);
	void CloseTelephony();
	void RetrievePhoneCapsL();
	void RetrieveNetworkModeL();
	void RetrievePacketSupportL();

	void CalculateAvailableBearerSet();
	TBool CsdAvailable() const;
	TBool PhoneAttachedAndRegistered() const;
	TBool MsClassSupportsPsd() const;

	void CheckBearerSupport();
	static TInt StartCheckingCb(TAny* aThisPtr);

private:
	
	/** State of ETEL server connection */
	enum TTelServerState
		{
		ENotConnected,
		EServerConnected,
		EPhoneModuleLoaded,
		EPhoneOpen,
		EPacketOpen
		};

private:
	CAsyncCallBack* iStartCheckingCb;
	TBool iIsReconnect;
	RTelServer iTelServer;
	TTelServerState iTelServerState;
	HBufC* iTsyName;
	RMobilePhone iMmPhone;
	CAgentSignalStrengthWatcher* iSigStrengthWatcher;
	RPhone::TCaps iPhoneCaps;
	TUint32 iPhoneMultimodeCaps;
	RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;
	RPacketService iPacket;
	RPacketService::TStatus  iPacketDataNetworkStatus;
	RPacketService::TAttachMode iPacketAttachMode;
	CASyncEtelRequestWrapper* iETelServerRequestsWrapper;
	RPacketService::TMSClass iMsClass;
	RPacketService::TRegistrationStatus iNetworkRegStatus;
	};

#endif // __CTELBEARER_H__

