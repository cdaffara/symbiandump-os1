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
// Base class to the various states required in Spud FSM. Each real state is derived from
// this base class. The bass class is never instanciated but it's base functionality is 
// used for default handling in the most part.
// 
//

/**
 @file 
 @internalComponent
*/
 


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstateTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "cpdpfsmfactory.h"
#include "tpdpstate.h"
#include "pdpfsmnmspace.h"
#include "reteldriverinput.h"
#include "PDPFSM.h"

// Literals for TPdpState::LogOperation

_LIT(KPdpFsm_EPhoneOpened, "PdpFsm::EPhoneOpened");
_LIT(KPdpFsm_EPhoneOpenedFailed, "PdpFsm::EPhoneOpenedFailed");
_LIT(KPdpFsm_EQoSSet, "PdpFsm::EQoSSet");
_LIT(KPdpFsm_EQoSSetFailed, "PdpFsm::EQoSSetFailed");
_LIT(KPdpFsm_ETftSet, "PdpFsm::ETftSet");
_LIT(KPdpFsm_ETftSetFailed, "PdpFsm::ETftSetFailed");
_LIT(KPdpFsm_ETftChanged, "PdpFsm::ETftChanged");
_LIT(KPdpFsm_ETftChangedFailed, "PdpFsm::ETftChangedFailed");
_LIT(KPdpFsm_E1ryPdpContextCreated, "PdpFsm::E1ryPdpContextCreated");
_LIT(KPdpFsm_E1ryPdpContextCreatedFailed, "PdpFsm::E1ryPdpContextCreatedFailed");
_LIT(KPdpFsm_E2ryPdpContextCreated, "PdpFsm::E2ryPdpContextCreated");
_LIT(KPdpFsm_E2ryPdpContextCreatedFailed, "PdpFsm::E2ryPdpContextCreatedFailed");
_LIT(KPdpFsm_EPdpActivated, "PdpFsm::EPdpActivated");
_LIT(KPdpFsm_EPdpActivatedFailed, "PdpFsm::EPdpActivatedFailed");
_LIT(KPdpFsm_EContextDeleted, "PdpFsm::EContextDeleted");
_LIT(KPdpFsm_EContextDeletedFailed, "PdpFsm::EContextDeletedFailed");
_LIT(KPdpFsm_EQoSProfileChangeNetwork, "PdpFsm::EQoSProfileChangeNetwork");
_LIT(KPdpFsm_EConfigGPRSChangeNetwork, "PdpFsm::EConfigGPRSChangeNetwork");
_LIT(KPdpFsm_EPacketStatusChangeNetwork, "PdpFsm::EPacketStatusChangeNetwork");
_LIT(KPdpFsm_EPdpContextModified, "PdpFsm::EPdpContextModified");
_LIT(KPdpFsm_EPdpContextModifiedFailed, "PdpFsm::EPdpContextModifiedFailed");
_LIT(KRPacketContext_EStatusUnknown, "RPacketContext::EStatusUnknown");
_LIT(KRPacketContext_EStatusInactive, "RPacketContext::EStatusInactive");
_LIT(KRPacketContext_EStatusActivating, "RPacketContext::EStatusActivating");
_LIT(KRPacketContext_EStatusActive, "RPacketContext::EStatusActive");
_LIT(KRPacketContext_EStatusDeactivating, "RPacketContext::EStatusDeactivating");
_LIT(KRPacketContext_EStatusSuspended, "RPacketContext::EStatusSuspended");
_LIT(KRPacketContext_EStatusDeleted, "RPacketContext::EStatusDeleted");
_LIT(KEContextStatusChangeNetwork_Unknown, "EContextStatusChangeNetwork_Unknown");
_LIT(KRPacketService_EStatusUnattached, "RPacketService::EStatusUnattached");
_LIT(KRPacketService_EStatusAttached, "RPacketService::EStatusAttached");
_LIT(KRPacketService_EStatusActive, "RPacketService::EStatusActive");
_LIT(KRPacketService_EStatusSuspended, "RPacketService::EStatusSuspended");
_LIT(KEServiceStatusChangeNetwork_Unknown, "EServiceStatusChangeNetwork_Unknown");
_LIT(KSpudMan_EContextCreate, "SpudMan::EContextCreate");
_LIT(KSpudMan_EContextDelete, "SpudMan::EContextDelete");
_LIT(KSpudMan_EContextActivate, "SpudMan::EContextActivate");
_LIT(KSpudMan_ERegisterEventHandler, "SpudMan::ERegisterEventHandler");
_LIT(KSpudMan_EContextSetEvents, "SpudMan::EContextSetEvents");
_LIT(KSpudMan_EContextQoSSet, "SpudMan::EContextQoSSet");
_LIT(KSpudMan_EContextModifyActive, "SpudMan::EContextModifyActive");
_LIT(KSpudMan_EContextTFTModify, "SpudMan::EContextTFTModify");
_LIT(KSpudMan_ENifSetDefaultQoS, "SpudMan::ENifSetDefaultQoS");
_LIT(KSpudMan_EStartupPrimaryContextCreation, "SpudMan::EStartupPrimaryContextCreation");
_LIT(KSpudMan_ESetDefaultQoS, "SpudMan::ESetDefaultQoS");
_LIT(KSpudMan_ECreatePrimaryPDPContext, "SpudMan::ECreatePrimaryPDPContext");
_LIT(KSpudMan_ECreateSecondaryPDPContext, "SpudMan::ECreateSecondaryPDPContext");
_LIT(KSpudMan_ESuspend, "SpudMan::ESuspend");
_LIT(KSpudMan_EResume, "SpudMan::EResume");
_LIT(KSpudMan_E1ryPdpActivate, "SpudMan::E1ryPdpActivate");
_LIT(KSpudMan_ECancelContextCreate, "SpudMan::ECancelContextCreate");
_LIT(KSpudMan_EGetNegQoS, "SpudMan::EGetNegQoS");
_LIT(KUnknown, "Unknown");


TPdpState::TPdpState()
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
	: iName()
#endif		
{
}

TInt TPdpState::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt /*aErrorCode*/)
{
	if (aOperation == PdpFsm::EContextDeleted || aOperation == SpudMan::EContextDelete ||
		(aOperation == PdpFsm::EContextStatusChangeNetwork && aFsm.iContextStatus == RPacketContext::EStatusDeleted))
	{
		aFsm.ChangeStateToClosing();
		EtelDriverCancel (aFsm);
		EtelDriverInput(aFsm, EtelDriver::EContextDelete);
		return KErrNone;
	}

	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATE_INPUT_1, "!!! Unhandled Operation (%S)!!!", *(LogOperation (aFsm, aOperation)));
	return KErrGeneral;
}

void TPdpState::EtelDriverInput (CPdpFsm& aFsm, EtelDriver::TEtelInput aOperation)
{
	aFsm.EtelInput (aOperation);
}

void TPdpState::EtelDriverCancel (CPdpFsm& aFsm)
{
	aFsm.EtelCancel();
}

void TPdpState::SpudManNotify (CPdpFsm& aFsm, TInt aNotification, TInt aParam)
{
	aFsm.SpudInput (aNotification, aParam);
}

const TDesC * TPdpState::LogOperation (CPdpFsm& aFsm, TInt aOperation)
{
	switch (aOperation)
	{
	// ETel notifications
	case PdpFsm::EPhoneOpened:
		return &KPdpFsm_EPhoneOpened;
	case PdpFsm::EPhoneOpenedFailed:
		return &KPdpFsm_EPhoneOpenedFailed;
	case PdpFsm::EQoSSet:
		return &KPdpFsm_EQoSSet;
	case SpudMan::EGetNegQoS:
		return &KSpudMan_EGetNegQoS;
	case PdpFsm::EQoSSetFailed:
		return &KPdpFsm_EQoSSetFailed;
	case PdpFsm::ETftSet:
		return &KPdpFsm_ETftSet;
	case PdpFsm::ETftSetFailed:
		return &KPdpFsm_ETftSetFailed;
	case PdpFsm::ETftChanged:
		return &KPdpFsm_ETftChanged;
	case PdpFsm::ETftChangedFailed:
		return &KPdpFsm_ETftChangedFailed;
	case PdpFsm::E1ryPdpContextCreated:
		return &KPdpFsm_E1ryPdpContextCreated;
	case PdpFsm::E1ryPdpContextCreatedFailed:
		return &KPdpFsm_E1ryPdpContextCreatedFailed;
	case PdpFsm::E2ryPdpContextCreated:
		return &KPdpFsm_E2ryPdpContextCreated;
	case PdpFsm::E2ryPdpContextCreatedFailed:
		return &KPdpFsm_E2ryPdpContextCreatedFailed;
	case PdpFsm::EPdpActivated:
		return &KPdpFsm_EPdpActivated;
	case PdpFsm::EPdpActivatedFailed:
		return &KPdpFsm_EPdpActivatedFailed;
	case PdpFsm::EContextDeleted:
		return &KPdpFsm_EContextDeleted;
	case PdpFsm::EContextDeletedFailed:
		return &KPdpFsm_EContextDeletedFailed;
	case PdpFsm::EPdpContextModified:
		return &KPdpFsm_EPdpContextModified;
	case PdpFsm::EPdpContextModifiedFailed:
		return &KPdpFsm_EPdpContextModifiedFailed;
		
//		
//	// network signals
	case PdpFsm::EQoSProfileChangeNetwork:
		//RPacketQoS::TQoSR99_R4Negotiated
		return &KPdpFsm_EQoSProfileChangeNetwork;
	case PdpFsm::EConfigGPRSChangeNetwork:
		//RPacketContext::TContextConfigGPRS
		return &KPdpFsm_EConfigGPRSChangeNetwork;
	case PdpFsm::EContextStatusChangeNetwork:
		switch (aFsm.iContextStatus)
		{
		case RPacketContext::EStatusUnknown:
			return &KRPacketContext_EStatusUnknown;
		case RPacketContext::EStatusInactive:
			return &KRPacketContext_EStatusInactive;
		case RPacketContext::EStatusActivating:
			return &KRPacketContext_EStatusActivating;
		case RPacketContext::EStatusActive:
			return &KRPacketContext_EStatusActive;
		case RPacketContext::EStatusDeactivating:
			return &KRPacketContext_EStatusDeactivating;
		case RPacketContext::EStatusSuspended:
			return &KRPacketContext_EStatusSuspended;
		case RPacketContext::EStatusDeleted:
			return &KRPacketContext_EStatusDeleted;
		}
		return &KEContextStatusChangeNetwork_Unknown;
	case PdpFsm::EServiceStatusChangeNetwork:
		{
		RPacketService::TStatus ns;
		
			aFsm.iPdpFsmFactory->iPdpFsmInterface->Get(ns);

			switch (ns)
			{
			case RPacketService::EStatusUnattached:
				return &KRPacketService_EStatusUnattached;
			case RPacketService::EStatusAttached:
				return &KRPacketService_EStatusAttached;
			case RPacketService::EStatusActive:	
				return &KRPacketService_EStatusActive;
			case RPacketService::EStatusSuspended:
				return &KRPacketService_EStatusSuspended;
			}
		}
		return &KEServiceStatusChangeNetwork_Unknown;
	case PdpFsm::EPacketStatusChangeNetwork: // not used in eteldriver
		return &KPdpFsm_EPacketStatusChangeNetwork;
		
	// spudman signals
	case SpudMan::EContextCreate:
		return &KSpudMan_EContextCreate;
	case SpudMan::EContextDelete:
		return &KSpudMan_EContextDelete;
	case SpudMan::EContextActivate:
		return &KSpudMan_EContextActivate;
	case SpudMan::ERegisterEventHandler:
		return &KSpudMan_ERegisterEventHandler;
	case SpudMan::EContextSetEvents:
		return &KSpudMan_EContextSetEvents;
	case SpudMan::EContextQoSSet:
		return &KSpudMan_EContextQoSSet;
	case SpudMan::EContextModifyActive:
		return &KSpudMan_EContextModifyActive;
	case SpudMan::EContextTFTModify:
		return &KSpudMan_EContextTFTModify;
	case SpudMan::ENifSetDefaultQoS:
		return &KSpudMan_ENifSetDefaultQoS;
	case SpudMan::EStartupPrimaryContextCreation:
		return &KSpudMan_EStartupPrimaryContextCreation;

	case SpudMan::ESetDefaultQoS:
		return &KSpudMan_ESetDefaultQoS;
	case SpudMan::ECreatePrimaryPDPContext:
		return &KSpudMan_ECreatePrimaryPDPContext;
	case SpudMan::ECreateSecondaryPDPContext:
		return &KSpudMan_ECreateSecondaryPDPContext;
	case SpudMan::ESuspend:
		return &KSpudMan_ESuspend;
	case SpudMan::EResume:
		return &KSpudMan_EResume;
	case SpudMan::E1ryPdpActivate:
		return &KSpudMan_E1ryPdpActivate;
	case SpudMan::ECancelContextCreate:
		return &KSpudMan_ECancelContextCreate;
				
	default:
		return &KUnknown;
	}

	
}
