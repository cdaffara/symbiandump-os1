// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef __GPRSQOS_H
#define __GPRSQOS_H

#include <etelqos.h>
#include "Gprscontext.h"

class CATGprsSetQoSProfile;
class CATGprsGetQoSProfile;
class CATIO;
class CATInit;
class CPhoneGlobals;


class CATGprsSetProfile;
class CATGprsGetProfile;
class CATGprsGetCapsProflie;
class CGprsQoS :public CSubSessionExtBase
/**
This class implements the RPacketQoS API.
CGprsQoS inherits from CSubSessionExtBase.
@internalComponent
*/
{
public:
	static	CGprsQoS* NewL(CGprsContext* aParent, TInt iCid, CATIO* aIo, CATInit*	aInit, CPhoneGlobals* aPhoneGlobals, TDes& aName);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

	RPacketQoS::TQoSGPRSRequested*	GetReqQoSProfile();
	void SetReqQoSProfile(const RPacketQoS::TQoSGPRSRequested* aQoSRequested);
	void SetNegQoSProfile(const RPacketQoS::TQoSGPRSNegotiated* aQoSNegotiated);

	inline CGprsContext* Parent(){return iParent;};
	TDes* GetProfileName();

protected:
	void	ConstructL(TDes& aName);
	CGprsQoS(CGprsContext* aParent, TInt iCid, CATIO* iIo, CATInit* iInit, CPhoneGlobals* iPhoneGlobals);
	virtual ~CGprsQoS();

private: 
	TInt SetProfile(const TTsyReqHandle aTsyReqHandle,const TDesC8* aReqProfile);
	TInt SetProfileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetProfile(const TTsyReqHandle aTsyReqHandle,TDes8* aNegProfile);
	TInt GetProfileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyProfileChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aNegProfile);
	TInt NotifyProfileChangedCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetProfileCaps(const TTsyReqHandle aTsyReqHandle, TDes8* aCapsProfile);
	TInt GetProfileCapsCancel(const TTsyReqHandle aTsyReqHandle);
private:
	TBool ShouldNotProcessIpcCall(TInt aIpc);
	TBool ShouldNotProcessIpcCallWhenComportIsLoaned(TInt aIpc);
private:
	TName							iQoSName;
	TInt							iCid; // context id relating the QoS profile to the context id.
	CATGprsSetProfile*				iATGprsSetProfile;		//< Pointer to context activation AT command object.
	CATGprsGetProfile*				iATGprsGetProfile;	//< Pointer to context deactivation AT command object.
	CATGprsGetCapsProflie*			iATGprsGetCapsProfile;			//< Pointer to context configuration AT command object.
	CGprsContext*					iParent;					//< Pointer to parent.
	CATIO*							iIo;						//< Pointer to lowever layer (C32) communication object.
	CATInit*						iInit;						//< Pointer to AT phone init.
	CPhoneGlobals*					iPhoneGlobals;				//< Pointer to global wide phone states.
	RPacketQoS::TQoSGPRSRequested	iQoSRequested;
	RPacketQoS::TQoSGPRSNegotiated	iQoSNegotiated;
};

#endif
