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
// PDP FSM header
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef CPDPFSM_H
#define CPDPFSM_H

#include <e32base.h>
#include <etelmm.h>
#include <etelqos.h>
#include <networking/umtsnifcontrolif.h>
#include "reteldriverinput.h"
#include "pdpfsmnmspace.h"
#include "PDPFSM.h"
#include "tpdpstates.h"

class CPdpFsmFactory;
class RPacketQoS;

#ifdef SYMBIAN_NETWORKING_UMTSR5
class TQoSR5Requested;
class TQoSR5Negotiated; 
#else
class TQoSR99_R4Requested;
class TQoSR99_R4Negotiated;

#endif 
// SYMBIAN_NETWORKING_UMTSR5
class TFTInfo;
class TFTOperationCode;
class CPdpFsm : public CBase
{
	friend class TPdpState;
	friend class TPdpStateInitialised;
	friend class TPdpStateOpeningPhone;
	friend class TPdpStateCreatingPrimary;
	friend class TPdpStateActivatingPrimary;
	friend class TPdpStateCreatingSecondary;
	friend class TPdpStateCreatedSecondary;
	friend class TPdpStateSettingQoS;
	friend class TPdpStateSettingTFT;
	friend class TPdpStateActivatingSecondary;
	friend class TPdpStateOpen;
	friend class TPdpStateChangingQoS;
	friend class TPdpStateChangingTFT;
	friend class TPdpStateGettingNegQoS;
	friend class TPdpStateModifyingActive;
	friend class TPdpStateSuspended;
	friend class TPdpStateClosing;
	friend class TPdpStateStopping;
	friend class TPdpStateCreatingMbms;
	friend class TPdpStateActivatingMbms;
	friend class TPdpStateCreatedMbms;
	
public:
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	static CPdpFsm* NewL(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput);
#else
	static CPdpFsm* NewL(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput, MPdpFsmEventHandler& aPdpFsmEventHandler, SpudMan::TPdpContextType aContextType);
#endif
	
	~CPdpFsm();

	TInt Input (const TInt aOperation, const TInt aParam);
	

#ifdef SYMBIAN_NETWORKING_UMTSR5
	void Get(RPacketQoS::TQoSR5Requested& aParam);
	void Get(RPacketQoS::TQoSR5Negotiated& aParam);
	void Set(const RPacketQoS::TQoSR5Requested& aParam);
	void Set(const RPacketQoS::TQoSR5Negotiated& aParam);

#else
	void Get(RPacketQoS::TQoSR99_R4Requested& aParam);
	void Get(RPacketQoS::TQoSR99_R4Negotiated& aParam);
	void Set(const RPacketQoS::TQoSR99_R4Requested& aParam);
	void Set(const RPacketQoS::TQoSR99_R4Negotiated& aParam);
#endif 
// SYMBIAN_NETWORKING_UMTSR5

	void Get(TTFTInfo& aParam);
	void Get(TTFTOperationCode& aParam);
	void Get(RPacketContext::TDataChannelV2& aParam);
	void Get(TPacketDataConfigBase& aParam);
	void Get(RPacketContext::TContextStatus& aParam);
 	
	void Set(const TTFTInfo& aParam);
	void Set(const TTFTOperationCode& aParam);
	void Set(const RPacketContext::TDataChannelV2& aParam);
	void Set(const TPacketDataConfigBase& aParam);

	//mbms set parameters
	void Set(const RPacketMbmsContext::TContextConfigMbmsV1& aParam);
	void Get(RPacketMbmsContext::TContextConfigMbmsV1& aParam);
	void Set(const TSessionOperatioInfo& aParam );
	void Get(TSessionOperatioInfo& aParam);

	void Set(const RPacketContext::TContextStatus& aParam);

    void EtelInput (EtelDriver::TEtelInput aOperation);
	void EtelCancel (void);
    void SpudInput (TInt aNotification, TInt aParam);
    
    const RPacketContext::TDataChannelV2& GetDataChannelV2()
        {
        return iDataChannelV2;
        }

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	
	void Set(MPdpFsmEventHandler& aPdpFsmEventHandler);
#endif    
    
protected:
private:

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CPdpFsm(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput);
#else
	CPdpFsm(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput, MPdpFsmEventHandler& aPdpFsmEventHandler, SpudMan::TPdpContextType aContextType);
#endif
	void ConstructL();
	
	void ChangeStateToInitialised(void);
	void ChangeStateToOpeningPhone(void);
	void ChangeStateToCreatingPrimary(void);
	void ChangeStateToActivatingPrimary(void);
	void ChangeStateToCreatingSecondary(void);
	void ChangeStateToCreatedSecondary(void);
	void ChangeStateToSettingQoS(void);
	void ChangeStateToSettingTFT(void);
	void ChangeStateToActivatingSecondary(void);
	void ChangeStateToOpen(void);
	void ChangeStateToChangingQoS(void);
	void ChangeStateToChangingTFT(void);
	void ChangeStateToGettingNegQoS(void);
	void ChangeStateToModifingActive(void);
	void ChangeStateToSuspended(void);
	void ChangeStateToClosing(void);
	void ChangeStateToStopping(void);
	void ChangeStateToCreatingMbms(void);
	void ChangeStateToActivatingMbms(void);
	void ChangeStateToCreatedMbms(void);

	void InitialiseContextConfig(TUint32 aConfigRel);
	
    template<class CONTEXTTYPE>
	CONTEXTTYPE& GetContextConfigAs()
	    {
	    __ASSERT_COMPILE(PdpFsm::KContextConfigBufferSize >= sizeof(CONTEXTTYPE));
	    return *const_cast<CONTEXTTYPE*>(reinterpret_cast<const CONTEXTTYPE*>(iContextConfig.Ptr()));
	    }


public:
protected:
private:
	
	// local data cache for this context
	TRequestedProfileBuffer iQosRequested;
	TNegotiatedProfileBuffer iQosNegotiated;

	RPacketContext::TDataChannelV2     iDataChannelV2;
	RPacketContext::TContextStatus     iContextStatus;
	TTFTInfo 						   iTFT;
	TTFTOperationCode				   iTFTOperationCode;
	RBuf8 iContextConfig;
	RPacketMbmsContext::TContextConfigMbmsV1 iMbms;
	TSessionOperatioInfo			 	iSessionInfo;	
	CPdpFsmFactory   * iPdpFsmFactory; // need stuff from  factory
	REtelDriverInput * iEtelDriverInput;
	
	TPdpState * iState; // current state
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	
	MPdpFsmEventHandler* iPdpFsmEventHandler;
#endif
	TContextId  iPdpId; // our instance PDP context id - fixed when allocateed

	SpudMan::TPdpContextType	 iContextType;
};



#endif // CPDPFSM_H



