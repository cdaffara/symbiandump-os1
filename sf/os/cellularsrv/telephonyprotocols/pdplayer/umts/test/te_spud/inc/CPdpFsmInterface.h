/**
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalComponent
*/

// NOTE: This guard define is used to prevent the real PDPFSM.h from being included.
//       This file MUST be included first in test code
#ifndef PDPFSM_H
#define PDPFSM_H

#include <networking/pdpdef.h>
#include <networking/umtsnifcontrolif.h>
#include <etelpckt.h>
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
		
		EGetNegQoS,
		ECreateMbmsPDPContext,

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

/**
Dummy implementation of the MPdpFsmInterface that is seperated from the UmtsGprsSCPR
*/
class CPdpFsmInterface
   {
public:
	//-=========================================================
	// custom methods
	//-=========================================================	
    CPdpFsmInterface();
    ~CPdpFsmInterface();

	//-=========================================================
	// MPdpFsmInterface methods
	//-=========================================================	
	virtual void OpenL(TName& aName, TInt aUmtsRelease);
	virtual void Close();
	TContextId NewFsmContextL(MPdpFsmEventHandler& aPdpFsmEventHandler,SpudMan::TPdpContextType aContextType);
	virtual TInt Input (const TContextId aPdpId, const TInt aOperation, const TInt aParam=KErrNone);
   	
#ifdef SYMBIAN_NETWORKING_UMTSR5
	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR5Requested& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR5Negotiated& aParam);
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR5Requested& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR5Negotiated& aParam) const;

#else
// !SYMBIAN_NETWORKING_UMTSR5 

	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Requested& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketQoS::TQoSR99_R4Negotiated& aParam);
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR99_R4Requested& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketQoS::TQoSR99_R4Negotiated& aParam) const;
#endif 
// SYMBIAN_NETWORKING_UMTSR5 
	
	virtual TInt Set(TContextId aPdpId, const TTFTInfo& aParam);
	virtual TInt Set(TContextId aPdpId, const TTFTOperationCode& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketContext::TDataChannelV2& aParam);
	virtual TInt Set(TContextId aPdpId, const TPacketDataConfigBase& aParam);
	virtual TInt Set(TContextId aPdpId, const RPacketContext::TContextStatus& aParam);


	virtual TInt Get(TContextId aPdpId, TSessionOperatioInfo& aParam) const;
	virtual TInt Set(const TContextId aPdpId,const TSessionOperatioInfo& aParam);
	virtual TInt Get(TContextId aPdpId, RPacketMbmsContext::TContextConfigMbmsV1& aParam) const;
	virtual TInt Set(const TContextId aPdpId,const RPacketMbmsContext::TContextConfigMbmsV1& aParam);



	virtual TInt Get(TContextId aPdpId, TTFTInfo& aParam) const;
	virtual TInt Get(TContextId aPdpId, TTFTOperationCode& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketContext::TDataChannelV2& aParam) const;
	virtual TInt Get(TContextId aPdpId, TPacketDataConfigBase& aParam) const;
	virtual TInt Get(TContextId aPdpId, RPacketContext::TContextStatus& aParam) const;

	virtual void Set(const RPacketService::TStatus aParam); // applies to all contexts
	virtual void Get(RPacketService::TStatus& aParam);// applies to all contexts

	virtual const TName& TsyName(void);
	
	inline TInt UmtsRelease()
	    {
	    return iUmtsRelease;
	    }

	const TName& GetContextName(TContextId aPdpId);

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

	RPacketService::TStatus iNetworkStatus;	
	TInt iUmtsRelease;

};

#endif
// PDPFSM_H
