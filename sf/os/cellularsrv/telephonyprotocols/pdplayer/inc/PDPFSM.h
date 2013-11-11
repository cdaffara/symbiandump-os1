/**
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file PDPFSM.H
 @internalComponent
*/

#ifndef PDPFSM_H
#define PDPFSM_H

#include <networking/pdpdef.h>
#include "pdpfsmnmspace.h"

class CPdpFsmFactory;

namespace SpudMan
{
	enum SpudManGuQoSSignal
	{
		EContextStart = 0x80,
		EContextCreate,
		EContextDelete,
		EContextActivate,
		ERegisterEventHandler,
		EContextSetEvents,
		EContextQoSSet,
		EContextModifyActive,
		EContextTFTModify,
		ENifSetDefaultQoS,
		EStartupPrimaryContextCreation,
		EMbmsSessionUpdate,
		EMbmsParameterUpdate,
		
		// sentinal
		ESpudManGuQoSSignalSentinal
	};

	enum SpudManSignal
	{
		ESetDefaultQoS = ESpudManGuQoSSignalSentinal,
		ECreatePrimaryPDPContext,
		ECreateSecondaryPDPContext,
		ESuspend,
		EResume,
		E1ryPdpActivate,
		ECancelContextCreate,
		ECreateMbmsPDPContext,
		
		EGetNegQoS,

		// sentinal
		ESpudManSignalSentinal
	};
	enum TPdpContextType
	{
		ENone,
		EPrimary,
		ESecondary,
		EMbms
	};
	
	
}


class MPdpFsmEventHandler
/**
Callback interface for PDPFsm to raise events.
Ignore Spud in the name. It's legacy artifact kept for
the old code to work.
*/
    {
public:
	virtual void Event(TInt aEvent, TInt aParam) = 0;
    };


class CPDPDefaultSubConnectionProvider;
class CPdpFsmInterface
/**
 * PDPSCPR implementation of the MPdpFsmInterface
 *
 * @internalComponent
 */
 {
 public:
	//-=========================================================
	// custom methods
	//-=========================================================	
    CPdpFsmInterface(CPDPDefaultSubConnectionProvider& aOwner);
	virtual ~CPdpFsmInterface();
	
	//-=========================================================
	// MPdpFsmInterface methods
	//-=========================================================	
	virtual void NewL(const TName& aName, TInt aUmtsRelease);
	void Open();
	void Close();
	
	TContextId NewFsmContextL(MPdpFsmEventHandler& aPdpFsmEventHandler, SpudMan::TPdpContextType aContextType);
	void       DeleteFsmContext(const TContextId aPdpId);
	TBool      FsmContextExists(const TContextId aPdpId);
	virtual TInt Input (const TContextId aPdpId, const TInt aOperation,const TInt aParam=KErrNone);

	virtual TInt Set(const TContextId aPdpId,const TTFTInfo& aParam);
	virtual TInt Set(const TContextId aPdpId,const TTFTOperationCode& aParam);
	virtual TInt Set(const TContextId aPdpId,const RPacketContext::TDataChannelV2& aParam);
	virtual TInt Set(TContextId aPdpId, const TPacketDataConfigBase& aParam);
	virtual TInt Set(const TContextId aPdpId,const RPacketContext::TContextStatus& aParam);
	virtual TInt Set(const TContextId aPdpId,const RPacketMbmsContext::TContextConfigMbmsV1& aParam);
	virtual void Set(const TContextId aPdpId,MPdpFsmEventHandler& aPdpFsmEventHandler);
	virtual TInt Set(const TContextId aPdpId,const TSessionOperatioInfo& aParam);

	virtual TInt Get(const TContextId aPdpId,TTFTInfo& aParam) const;
	virtual TInt Get(const TContextId aPdpId,TTFTOperationCode& aParam) const;
	virtual TInt Get(const TContextId aPdpId,RPacketContext::TDataChannelV2& aParam) const;
	virtual TInt Get(TContextId aPdpId, TPacketDataConfigBase& aParam) const;
	virtual TInt Get(const TContextId aPdpId,RPacketContext::TContextStatus& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketMbmsContext::TContextConfigMbmsV1& aParam) const;
	virtual TInt Get(TContextId aPdpId, TSessionOperatioInfo& aParam) const;
	
	virtual TInt Set(TContextId aPdpId,const RPacketQoS::TQoSR5Requested& aParam);
	virtual TInt Set(TContextId aPdpId,const RPacketQoS::TQoSR5Negotiated& aParam);
	virtual TInt Get(TContextId aPdpId,RPacketQoS::TQoSR5Requested& aParam) const;
	virtual TInt Get(TContextId aPdpId,RPacketQoS::TQoSR5Negotiated& aParam) const;

	virtual void Set(const RPacketService::TStatus aParam); // applies to all contexts
	virtual void Get(RPacketService::TStatus& aParam);// applies to all contexts

	virtual const TName& TsyName(void);

	inline TInt UmtsRelease()
		{
		return iUmtsRelease;
		}

	const TName& GetContextName(TContextId aPdpId) const;

 protected:
 private:
	// DO NOT provide implementations to these methods: Copying this object should be disabled
	CPdpFsmInterface(CPdpFsmInterface&);
	CPdpFsmInterface& operator= (CPdpFsmInterface& aInput);
	
 public:
 protected:
 private:
	// FSM Factory - churns out FSMs
	CPdpFsmFactory * iPdpFsmFactory;
    CPDPDefaultSubConnectionProvider& iDefaultSubConnProvd;
	RPacketService::TStatus iNetworkStatus;	
	TInt iRefCount;
	TInt iUmtsRelease;
};


#endif
// PDPFSM_H
