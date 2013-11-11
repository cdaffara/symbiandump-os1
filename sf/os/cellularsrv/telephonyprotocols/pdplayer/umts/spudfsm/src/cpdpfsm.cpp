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
// PDP FSM implementation
// @internalTechnology
// 
//




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpdpfsmTraces.h"
#endif

#include "tpdpstates.h"
#include "cpdpfsm.h"
#include "cpdpfsmfactory.h"
#include "eteldrivernmspace.h"


#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CPdpFsm* CPdpFsm::NewL(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput)
#else
	CPdpFsm* CPdpFsm::NewL(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput, MPdpFsmEventHandler& aPdpFsmEventHandler, SpudMan::TPdpContextType aContextType)
#endif
	{
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CPdpFsm* self = new(ELeave) CPdpFsm(aPdpId, aPdpFsmFactory, aEtelDriverInput);
#else
	CPdpFsm* self = new(ELeave) CPdpFsm(aPdpId, aPdpFsmFactory, aEtelDriverInput, aPdpFsmEventHandler, aContextType);
#endif
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPdpFsm::ConstructL()
	{
	iContextConfig.CreateL(PdpFsm::KContextConfigBufferSize);
	iContextConfig.SetMax();
	iContextConfig.FillZ();
	}



#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CPdpFsm::CPdpFsm(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput)
#else
CPdpFsm::CPdpFsm(TContextId aPdpId, CPdpFsmFactory * aPdpFsmFactory, REtelDriverInput * aEtelDriverInput, MPdpFsmEventHandler& aPdpFsmEventHandler,SpudMan::TPdpContextType aContextType)
:iPdpFsmEventHandler(&aPdpFsmEventHandler)
#endif
// NOTE: Both pointers are valid when called - see CPdpFsmFactory and CPdpFsmInterface. 
// No other caller is expected.
    {
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CTOR_1, "Constructor for Pdp ID %d", aPdpId);

	// save for later
	iPdpId = aPdpId;
	iPdpFsmFactory = aPdpFsmFactory;
	iEtelDriverInput = aEtelDriverInput;
	iContextType = aContextType;
	// we need a state
	iState = &iPdpFsmFactory->iStateInitialised;
    }
    

CPdpFsm::~CPdpFsm()
    {
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_DTOR_1, "Destructor for Pdp ID %d",iPdpId);
	iContextConfig.Close();
	iEtelDriverInput->DeletePdp(iPdpId);
	iSessionInfo.iSessionIds.Close();	
    }

	
TInt CPdpFsm::Input (const TInt aOperation, const TInt aParam)
    {
    OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_INPUT_1, "For context name [%S], State [%S], Operation [%S], Error Code [%d]", iDataChannelV2.iChannelId, iState->iName, *iState->LogOperation(*this, aOperation), aParam);	
	return iState->Input (*this, aOperation, aParam);
    }

#ifdef SYMBIAN_NETWORKING_UMTSR5
void CPdpFsm::Get(RPacketQoS::TQoSR5Requested& aParam)
{
	ASSERT(iQosRequested.ExtensionId() == TPacketDataConfigBase::KConfigRel5);
	aParam = iQosRequested.RequestedQoSR5();
}

void CPdpFsm::Get(RPacketQoS::TQoSR5Negotiated& aParam)
{
	ASSERT(iQosNegotiated.ExtensionId() == TPacketDataConfigBase::KConfigRel5);
	aParam = iQosNegotiated.NegotiatedQoSR5();
}

#else
// !SYMBIAN_NETWORKING_UMTSR5

void CPdpFsm::Get(RPacketQoS::TQoSR99_R4Requested& aParam)
    {
	aParam = iQosRequested.RequestedQoSR99_R4();
    }

void CPdpFsm::Get(RPacketQoS::TQoSR99_R4Negotiated& aParam)
    {
	aParam = iQosNegotiated.NegotiatedQoSR99_R4();
    }
    
#endif
// SYMBIAN_NETWORKING_UMTSR5


void CPdpFsm::Get(TTFTInfo& aParam)
    {
	aParam = iTFT;
    }

void CPdpFsm::Get(TTFTOperationCode& aParam)
    {
	aParam = iTFTOperationCode;
    }

void CPdpFsm::Get(RPacketContext::TDataChannelV2& aParam)
    {
	aParam = iDataChannelV2;
    }

void CPdpFsm::Get(TPacketDataConfigBase& aParam)
	{
	TInt rel = GetContextConfigAs<TPacketDataConfigBase>().ExtensionId();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_GET_PACKETDATACONFIGBASE_1, "(TPacketDataConfigBase) for Packet Network [%d]", rel);

	if (rel == 0)
		{
    		// buffer has not been initialised yet
    		InitialiseContextConfig(aParam.ExtensionId());
    		}

	switch (rel)
    		{
    		case TPacketDataConfigBase::KConfigGPRS:
			Mem::Copy(&aParam, const_cast<TUint8*>(iContextConfig.Ptr()), sizeof(RPacketContext::TContextConfigGPRS));
			break;

    		case TPacketDataConfigBase::KConfigRel99Rel4:
			Mem::Copy(&aParam, const_cast<TUint8*>(iContextConfig.Ptr()), sizeof(RPacketContext::TContextConfigR99_R4));
        		break;

    		case TPacketDataConfigBase::KConfigRel5:
			Mem::Copy(&aParam, const_cast<TUint8*>(iContextConfig.Ptr()), sizeof(RPacketContext::TContextConfig_R5));
			break;
		}
	}

void CPdpFsm::Get(RPacketContext::TContextStatus& aParam)
    {
	aParam = iContextStatus;
    }


#ifdef SYMBIAN_NETWORKING_UMTSR5
void CPdpFsm::Set(const RPacketQoS::TQoSR5Requested& aParam)
{
	iQosRequested = aParam;
}

void CPdpFsm::Set(const RPacketQoS::TQoSR5Negotiated& aParam)
{
	iQosNegotiated = aParam;
}

#else
// !SYMBIAN_NETWORKING_UMTSR5


void CPdpFsm::Set(const RPacketQoS::TQoSR99_R4Requested& aParam)
    {
	iQosRequested = aParam;
    }


void CPdpFsm::Set(const RPacketQoS::TQoSR99_R4Negotiated& aParam)
    {
	iQosNegotiated = aParam;
    }


#endif 
// SYMBIAN_NETWORKING_UMTSR5


void CPdpFsm::Set(const TTFTInfo& aParam)
    {
	iTFT = aParam;
    }

void CPdpFsm::Set(const TTFTOperationCode& aParam)
    {
	iTFTOperationCode = aParam;
    }

void CPdpFsm::Set(const RPacketContext::TDataChannelV2& aParam)
    {
	iDataChannelV2 = aParam;
    }

void CPdpFsm::Set(const RPacketContext::TContextStatus& aParam)
    {
	iContextStatus = aParam;
    }

void CPdpFsm::Set(const TPacketDataConfigBase& aParam)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_SET_PACKETDATACONFIGBASE_1, "(TPacketDataConfigBase) for Packet Network [%d]", const_cast<TPacketDataConfigBase&>(aParam).ExtensionId());

	switch ((const_cast<TPacketDataConfigBase&>(aParam)).ExtensionId())
	    {
	    case TPacketDataConfigBase::KConfigGPRS:
	        Mem::Copy(const_cast<TUint8*>(iContextConfig.Ptr()), &aParam, sizeof(RPacketContext::TContextConfigGPRS));
	        break;
	        
	    case TPacketDataConfigBase::KConfigRel99Rel4:
	        Mem::Copy(const_cast<TUint8*>(iContextConfig.Ptr()), &aParam, sizeof(RPacketContext::TContextConfigR99_R4));
	        break;

	    case TPacketDataConfigBase::KConfigRel5:
	        Mem::Copy(const_cast<TUint8*>(iContextConfig.Ptr()), &aParam, sizeof(RPacketContext::TContextConfig_R5));
	        break;
	    }
    }

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	
void CPdpFsm::Set(MPdpFsmEventHandler& aPdpFsmEventHandler)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_SET_PDPFSMHANDLER_1, "(MPdpFsmEventHandler&)for context name [%S]", iDataChannelV2.iChannelId);
	
	if (iPdpFsmEventHandler)
    	{
    	iPdpFsmEventHandler->Event(KContextDeleteEvent, KErrAbort);
    	}
	iPdpFsmEventHandler = &aPdpFsmEventHandler;
    }
#endif  

void CPdpFsm::EtelInput (EtelDriver::TEtelInput aOperation)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_ETELINPUT_1, "for context name [%S] for ETEL input operation [%d]", iDataChannelV2.iChannelId, aOperation);	
	iEtelDriverInput->Input (iPdpId, aOperation);
    }

void CPdpFsm::EtelCancel (void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_ETELCANCEL_1, "for context id [%d]", iPdpId);	
	iEtelDriverInput->CancelPdp (iPdpId);
    }

void CPdpFsm::SpudInput (TInt aNotification, TInt aParam)
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_SPUDINPUT_1, "for context name [%S], Notification Event [%d], Error Code [%d]", iDataChannelV2.iChannelId, aNotification, aParam);   

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY	
	iPdpFsmFactory->SpudInput (iPdpId, aNotification, aParam);
#else
    ASSERT(iPdpFsmEventHandler);
    iPdpFsmEventHandler->Event(aNotification, aParam);
#endif
    }

// state change members

void CPdpFsm::ChangeStateToInitialised(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOINITIALISED_1, "for context id [%d]", iPdpId);	
	iState = &iPdpFsmFactory->iStateInitialised;
    }

void CPdpFsm::ChangeStateToOpeningPhone(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOOPENINGPHONE_1, "for context id [%d]", iPdpId);	
	iState = &iPdpFsmFactory->iStateOpeningPhone;
	}

void CPdpFsm::ChangeStateToCreatingPrimary(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCREATINGPRIMARY_1, "for context id [%d]", iPdpId);	
	iState = &iPdpFsmFactory->iStateCreatingPrimary;
    }

void CPdpFsm::ChangeStateToActivatingPrimary(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOACTIVATINGPRIMARY_1, "for context id [%d]", iPdpId);
	iState = &iPdpFsmFactory->iStateActivatingPrimary;
    }

void CPdpFsm::ChangeStateToCreatingSecondary(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCREATINGSECONDARY_1, "for context id [%d]", iPdpId);	
	iState = &iPdpFsmFactory->iStateCreatingSecondary;
    }

void CPdpFsm::ChangeStateToCreatedSecondary(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCREATEDSECONDARY_1, "for context id [%d]", iPdpId);	
	iState = &iPdpFsmFactory->iStateCreatedSecondary;
    }

void CPdpFsm::ChangeStateToSettingTFT(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOSETTINGTFT_1, "for context id [%d]", iPdpId);	
	iState = &iPdpFsmFactory->iStateSettingTFT;
    }

void CPdpFsm::ChangeStateToSettingQoS(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOSETTINGQOS_1, "for context id [%d]", iPdpId);
	iState = &iPdpFsmFactory->iStateSettingQoS;
    }

void CPdpFsm::ChangeStateToActivatingSecondary(void)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOACTIVATINGSECONDARY_1, "for context id [%d]", iPdpId);
	iState = &iPdpFsmFactory->iStateActivatingSecondary;
    }

void CPdpFsm::ChangeStateToOpen(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOOPEN_1, "for context name [%S]", iDataChannelV2.iChannelId);
	iState = &iPdpFsmFactory->iStateOpen;
    }

void CPdpFsm::ChangeStateToChangingQoS(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCHANGINGQOS_1, "for context name [%S]", iDataChannelV2.iChannelId);
	iState = &iPdpFsmFactory->iStateChangingQoS;
    }

void CPdpFsm::ChangeStateToChangingTFT(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCHANGINGTFT_1, "for context name [%S]", iDataChannelV2.iChannelId);
	iState = &iPdpFsmFactory->iStateChangingTFT;
    }

void CPdpFsm::ChangeStateToGettingNegQoS(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOGETTINGNEGQOS_1, "for context name [%S]", iDataChannelV2.iChannelId);
	iState = &iPdpFsmFactory->iStateGettingNegQoS;
    }

void CPdpFsm::ChangeStateToModifingActive(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOMODIFINGACTIVE_1, "for context name [%S]", iDataChannelV2.iChannelId);	
	iState = &iPdpFsmFactory->iStateModifyingActive;
	}

void CPdpFsm::ChangeStateToSuspended(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOSUSPENDED_1, "for context name [%S]", iDataChannelV2.iChannelId);
	iState = &iPdpFsmFactory->iStateSuspended;
    }

void CPdpFsm::ChangeStateToClosing(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCLOSING_1, "for context name [%S]", iDataChannelV2.iChannelId);
	iState = &iPdpFsmFactory->iStateClosing;
    }

void CPdpFsm::ChangeStateToStopping(void)
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOSTOPPING_1, "for context name [%S]", iDataChannelV2.iChannelId);
	iState = &iPdpFsmFactory->iStateStopping;
    }
    
void CPdpFsm::ChangeStateToCreatingMbms(void)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCREATINGMBMS_1, "for context id [%d]", iPdpId);
	iState = &iPdpFsmFactory->iStateCreatingMbms;
	}
	
void CPdpFsm::ChangeStateToActivatingMbms(void)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOACTIVATINGMBMS_1, "for context id [%d]", iPdpId);
	iState = &iPdpFsmFactory->iStateActivatingMbms;
	}

void CPdpFsm::ChangeStateToCreatedMbms(void)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_CHANGESTATETOCREATEDMBMS_1, "for context id [%d]", iPdpId);
	iState = &iPdpFsmFactory->iStateCreatedMbms;
	}


	
void CPdpFsm::Get(RPacketMbmsContext::TContextConfigMbmsV1& aParam)
    {
	aParam = iMbms;
    }
    
    
void CPdpFsm::Set(const RPacketMbmsContext::TContextConfigMbmsV1& aParam)
    {
	iMbms = aParam ;
    }
    
    
void CPdpFsm::Get(TSessionOperatioInfo& aParam)
    {
	aParam = iSessionInfo;
    }
    
    
void CPdpFsm::Set(const TSessionOperatioInfo& aParam)
    {
	iSessionInfo = aParam ;
    }    

void CPdpFsm::InitialiseContextConfig(TUint32 aConfigRel)
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPFSM_INITIALISECONTEXTCONFIG_1, "for context id [%d] of Packet Network [%d]", iPdpId, (TInt)aConfigRel);

    ASSERT(aConfigRel == TPacketDataConfigBase::KConfigGPRS
        || aConfigRel == TPacketDataConfigBase::KConfigRel99Rel4
        || aConfigRel == TPacketDataConfigBase::KConfigRel5);
    
    switch(aConfigRel)
        {
        case TPacketDataConfigBase::KConfigGPRS:
            {
            RPacketContext::TContextConfigGPRS tmp;
            Mem::Copy(const_cast<TUint8*>(iContextConfig.Ptr()), &tmp, sizeof(tmp));
            }
            break;
            
        case TPacketDataConfigBase::KConfigRel99Rel4:
            {
            RPacketContext::TContextConfigR99_R4 tmp;
            Mem::Copy(const_cast<TUint8*>(iContextConfig.Ptr()), &tmp, sizeof(tmp));
            }
            break;
            
        case TPacketDataConfigBase::KConfigRel5:
            {
            RPacketContext::TContextConfig_R5 tmp;
            Mem::Copy(const_cast<TUint8*>(iContextConfig.Ptr()), &tmp, sizeof(tmp));
            }
            break;
        }
    }


