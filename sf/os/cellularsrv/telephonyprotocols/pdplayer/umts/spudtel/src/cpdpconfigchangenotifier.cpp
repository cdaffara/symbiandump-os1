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
// PDP config change notifier
// 
//

/**
 @file 
 @internalComponent
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cpdpconfigchangenotifierTraces.h"
#endif

#include <e32base.h>
#include <pcktcs.h>

#include "cpdpconfigchangenotifier.h"
#include "PDPFSM.h"
#include "pdpfsmnmspace.h"

/**
@param aId - pdp context id
@param aPacketContext - reference to etel packet context
@param aPdpFsmInterface - reference to pdp fsm interface
*/
CPdpConfigChangeNotifier* CPdpConfigChangeNotifier::NewL(TContextId aId, RPacketContext& aPacketContext, CPdpFsmInterface& aPdpFsmInterface)
	{
	CPdpConfigChangeNotifier* self = new (ELeave) CPdpConfigChangeNotifier(aId, aPacketContext, aPdpFsmInterface);
	CleanupStack::PushL(self);
	self->ConstructL();

	CleanupStack::Pop(self);
	
	return self;	
	}
/**
Initialise the scratch context buffer depending on the UMTS release obtained from 
PDP FSM interface.
*/
void CPdpConfigChangeNotifier::ConstructL()
	{
	TInt umtsRelease = iPdpFsmInterface.UmtsRelease();
	iScratchContext.CreateL(KScratchContextConfigSize);
	iScratchContext.SetMax();
	iScratchContext.FillZ();
     
    switch(umtsRelease)
		{
		case TPacketDataConfigBase::KConfigGPRS:
			{
			RPacketContext::TContextConfigGPRS tmp;
			Mem::Copy(const_cast<TUint8*>(iScratchContext.Ptr()), &tmp, sizeof(tmp));
			}
			break;

		case TPacketDataConfigBase::KConfigRel99Rel4:
			{
			RPacketContext::TContextConfigR99_R4 tmp;
			Mem::Copy(const_cast<TUint8*>(iScratchContext.Ptr()), &tmp, sizeof(tmp));
			}
			break;

		case TPacketDataConfigBase::KConfigRel5:
			{
			RPacketContext::TContextConfig_R5 tmp;
			Mem::Copy(const_cast<TUint8*>(iScratchContext.Ptr()), &tmp, sizeof(tmp));
			}
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	}



/**
@param aId - pdp context id
@param aPacketContext - reference to etel packet context
@param aPdpFsmInterface - reference to pdp fsm interface
*/
CPdpConfigChangeNotifier::CPdpConfigChangeNotifier(TContextId aId,
										RPacketContext& aPacketContext,
										CPdpFsmInterface& aPdpFsmInterface)
: CEtelDriverNotifier(aPdpFsmInterface),
  iId(aId),
  iPacketContext(aPacketContext)
	{
	}

CPdpConfigChangeNotifier::~CPdpConfigChangeNotifier()
	{
	Cancel();
	iScratchContext.Close();
	}

/** initiate notification */
void CPdpConfigChangeNotifier::Start()
	{
	if (!IsActive())
		{ 
		iPacketContext.NotifyConfigChanged(iStatus, iScratchContext);
		SetActive();
		}
	}

/** stops notification */	
void CPdpConfigChangeNotifier::DoCancel()
	{
	if(IsActive())
		{ 
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPCONFIGCHANGENOTIFIER_DOCANCEL_1, "CPdpConfigChangeNotifier::DoCancel EPacketContextNotifyConfigChanged");
		iPacketContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged); 
		}
	}

/** notifies pdp fsm 

@param aStatus - request status
*/
void CPdpConfigChangeNotifier::Notify(const TRequestStatus& aStatus)
	{
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPCONFIGCHANGENOTIFIER_NOTIFY_1, "Config Change Notification for Context [%S]", iPdpFsmInterface.GetContextName(iId));
	if(aStatus == KErrNone)
		{
		iPdpFsmInterface.Set(iId, GetScratchContextAs<TPacketDataConfigBase>());
		iPdpFsmInterface.Input(iId, PdpFsm::EConfigGPRSChangeNetwork);
		}
	else
		{ 
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPDPCONFIGCHANGENOTIFIER_NOTIFY_2, "CPdpConfigChangeNotifier::Notify(), error: %d", aStatus.Int());
		// Not all TSYs support RPacketContext::NotifyConfigChanged so we handle
		// KErrNotSupported silently (PDEF118981).
		ASSERT((aStatus == KErrCancel) || (aStatus == KErrNotSupported));
		}
	}



/**
@param aId - pdp context id
@param aPacketContext - reference to etel packet context
@param aPdpFsmInterface - reference to pdp fsm interface
*/
CMbmsPdpConfigChangeNotifier::CMbmsPdpConfigChangeNotifier(TContextId aId,
										RPacketMbmsContext& aPacketContext,
										CPdpFsmInterface& aPdpFsmInterface)
: CEtelDriverNotifier(aPdpFsmInterface),
  iId(aId),
  iMbmsPacketContext(aPacketContext),
  iConfigMbms(),
  iConfigMbmsPckg(iConfigMbms)
	{
	}

CMbmsPdpConfigChangeNotifier::~CMbmsPdpConfigChangeNotifier()
	{
	Cancel();
	}

/** initiate notification */
void CMbmsPdpConfigChangeNotifier::Start()
	{
	if (!IsActive())
		{ 
		iMbmsPacketContext.NotifyConfigChanged(iStatus, iConfigMbmsPckg); 
		SetActive();
		}
	}

/** stops notification */	
void CMbmsPdpConfigChangeNotifier::DoCancel()
	{
	if(IsActive())
		{ 
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSPDPCONFIGCHANGENOTIFIER_DOCANCEL_1, "CMbmsPdpConfigChangeNotifier::DoCancel EPacketContextNotifyConfigChanged");
		iMbmsPacketContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged); 
		}
	}

/** notifies pdp fsm 

@param aStatus - request status
*/
void CMbmsPdpConfigChangeNotifier::Notify(const TRequestStatus& aStatus)
	{
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSPDPCONFIGCHANGENOTIFIER_NOTIFY_1, "MBMS Config Change Notification for Context [%S]", iPdpFsmInterface.GetContextName(iId));
	if(aStatus == KErrNone)
		{
		iPdpFsmInterface.Set(iId, iConfigMbms);
		iPdpFsmInterface.Input(iId, PdpFsm::EConfigGPRSChangeNetwork,SpudMan::EMbms);
		//to be discussed...		
		}
	else
		{ 
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMBMSPDPCONFIGCHANGENOTIFIER_NOTIFY_2, "CMbmsPdpConfigChangeNotifier::Notify(), error: %d", aStatus.Int());
		// Not all TSYs support RPacketContext::NotifyConfigChanged so we handle
		// KErrNotSupported silently (PDEF118981).
		ASSERT((aStatus == KErrCancel) || (aStatus == KErrNotSupported));
		}
	}



