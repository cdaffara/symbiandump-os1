/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef MMCLDD_H_
#define MMCLDD_H_

#include <mmc.h>
#include "LddAsyncRequest.h"

/**
 * LDD factory object for MMCSD
 *
 */
class DLddFactoryMmcCntrlInterface : public DLogicalDevice
	{
public:
	DLddFactoryMmcCntrlInterface();
	virtual ~DLddFactoryMmcCntrlInterface();
	virtual TInt Install();
	virtual void GetCaps(TDes8 &aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel); 	//overriding pure virtual
	};

	
/**
 * MMCSD LDD logical channel
 *
 */
class DLddMmcCntrlInterface : public DLogicalChannel , MAsyncRequestObserver
	{
public:
	DLddMmcCntrlInterface();
	~DLddMmcCntrlInterface();
protected:
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	virtual void HandleMsg(class TMessageBase *);
private:
	void DoCancel(TInt aReqNo);
	TInt DoControl(TInt aFunction, TAny* a1, TAny* a2);
	TInt MediaChangeDoControl(TInt aFunction, TAny* a1, TAny* a2);
	TInt PsuDoControl(TInt aFunction, TAny* a1, TAny* a2);
	TInt SocketDoControl(TInt aFunction, TAny* a1, TAny* a2);
	TInt StackDoControl(TInt aFunction, TAny* a1, TAny* a2);
	
	// async request methods
	TInt DoRequest(TInt aFunction, TRequestStatus* aStatus, TAny* a1, TAny* a2);
	TInt SocketDoRequest(TRequestStatus* aStatus, TAny* a1, TAny *a2, TInt aCommand);
	TInt StackConfigureEffectiveModes(TInt aEffectiveModesFunctionId, TAny* aParam);

	// FROM MAsyncRequestObserver
	virtual void Notify(DLddAsyncRequest& aRequest);

private:
	enum TMmcAsyncCmd {EMmcSesNone,EMmcSesReadBlk,EMmcSesWriteBlk, EMmcSesReadCSD, EMmcSesReadExtCSD, EMMcSocketPowerUp, EMMcStackPowerUp};	
	DMmcSessionAsyncRequest* CreateAsyncSessionRequest(TMmcAsyncCmd aCommand, TRequestStatus* aStatus);
	DBusEventRequest* CreateAsyncBusEventRequest(TMmcAsyncCmd aCommand, TRequestStatus* aStatus, TInt aUnit);

	
	TBool RequestStatusExists(TRequestStatus* aStatus);
	void ClearRequest(DLddAsyncRequest* aRequest);
	
private:
    enum TPanic {EReadDes,EWriteDes,EWriteTInt,EUnknownMmcSes};
    TInt iStackNum;
    DMMCPsu* iPsu;
    DMMCMediaChange* iMediaChange;
  	DMMCSocket* iSocket;
  	DMMCStack* iStack;
    TMMCard* iCard;

	DThread* iClient;
	TCSD iCSD;
	TExtendedCSD iExtendedCSD;
	TBool iIsSD;	
	RPointerArray<DLddAsyncRequest> iAsyncList;
	TMMCStackConfig iStackConfig;
	TInt			iUnit;
	};

#endif /*MMCLDD_H_*/
