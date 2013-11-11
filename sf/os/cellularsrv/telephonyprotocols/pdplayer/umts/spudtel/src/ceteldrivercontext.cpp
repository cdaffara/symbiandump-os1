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
// implementation of etel driver context class
// 
//

/**
 @file 
 @internalComponent
*/




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ceteldrivercontextTraces.h"
#endif

#include "ceteldrivercontext.h"
#include "ceteldriverfactory.h"


using namespace EtelDriver;
	

CEtelDriverContext* CEtelDriverContext::NewL(TContextId aId, SpudMan::TPdpContextType aContextType, CEtelDriverFactory& aFactory)
	{
        CEtelDriverContext* self  = new (ELeave) CEtelDriverContext(aId, aContextType, aFactory);
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(self);
        return self;
	}

void CEtelDriverContext::ConstructL()
	{
    iContextConfig.CreateL(PdpFsm::KContextConfigBufferSize);
	iContextConfig.SetMax();
	iContextConfig.FillZ();

	switch(PdpFsmInterface().UmtsRelease())
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
		default:
			User::Leave(KErrNotSupported);
			break;

		}	
	}

CEtelDriverContext::CEtelDriverContext (TContextId aId, SpudMan::TPdpContextType aContextType, CEtelDriverFactory& aFactory)
: CActive(CActive::EPriorityStandard),
  iId(aId),
  iContextType(aContextType),
  iCompletionStatus(KErrNone),
  iStrategyStep(MEtelDriverStrategy::EFinishStep),
  iStrategyId(ESentinelStrategy),
  iFactory(aFactory),
  iQosRequestedPckg(iQosRequested),
  iQosNegotiatedPckg(iQosNegotiated),

  iDataChannelV2(),
  iDataChannelV2Pckg(iDataChannelV2),
  iPcktMbmsSessionList(NULL),
  iContextConfigMbms(),
  iContextConfigMbmsPckg(iContextConfigMbms),
  iFilterV2(),
  iFilterV2Pckg(iFilterV2)
 	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_CTOR_1, "CEtelDriverContext::CEtelDriverContext");
	CActiveScheduler::Add(this);
  	}
CEtelDriverContext::~CEtelDriverContext()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DTOR_1, "CEtelDriverContext::~CEtelDriverContext()");
	Cancel(); // N.B. This cancels all outstanding operations on the context, including deletion!
	iContextConfig.Close();

	// Guarantees proper release of all handles.
	// If everything is closed already, this does no harm.
	iPacketQoS.Close(); // Close QoS first, a buggy TSY may not handle it the other way.
	iPacketContext.Close(); // At this point the reference count on the context is zero.
	// TSY should cleanly dispose of the context, if it had not done so already.
	
	// This is necessary only in a situtation where Spudman is destroyed while a strategy
	// on a context is outstanding: in this case deletion of SpudTel results in cancellation
	// of all outstanding operations, which is likely to result in handle leak.
	// Under other circumstances, the handles will be closed via an appropriate strategy.
	
	//delete if MBMS sessions structures.
	iMbmsPacketContext.Close();
	if (iMbmsSession)
		{
		delete iMbmsSession;
		delete iPcktMbmsSessionList;
		}
	iSessionInfo.iSessionIds.Close();	//RArray should be closed.
	
	}

/** initiates a new request 

@param aOperation - type of an etel driver request 

@return KErrInUse if pdp context has active strategy
*/
TInt CEtelDriverContext::Input (TEtelInput aOperation)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_INPUT_1, "CEtelDriverContext::Input()");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_INPUT_2,  "Operation %d", aOperation );
	
	if (MEtelDriverStrategy::EFinishStep != iStrategyStep)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_INPUT_3, "ERROR: Pdp context is in use, return %d", KErrInUse);
		// I'm still doing something
		ASSERT(EFalse); // shouldn't happen
		return KErrInUse;
		}
	
	iStrategyId = iFactory.StrategyId(aOperation);
	iStrategyStep = MEtelDriverStrategy::EStartStep;
	iCompletionStatus = KErrNone;
	
	SetActive();
	Strategy(iStrategyId).Next(*this, &iStatus);

	return KErrNone;
	}


void CEtelDriverContext::RunL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_RUNL_1, "CEtelDriverContext::RunL()");
	ASSERT(iStrategyId < ESentinelStrategy);
	
	if(iStatus != KErrNone)
		{
		if(iCompletionStatus == KErrNone)
			{ 
			iCompletionStatus = iStatus; 
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_RUNL_2, "Last async request completed with error %d", iStatus.Int());
			}
		// Don't continue with the strategy for all cases except Delete
		// N.B.: deletion of a context has to be done till the very last step
		//	to ensure proper cleanup of resources.
		if(EContextDeleteStrategy != iStrategyId)
			{
			iStrategyStep = MEtelDriverStrategy::EFinishStep;
			}
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_RUNL_3, "Strategy is completed");
		}
	
	if(MEtelDriverStrategy::EFinishStep == iStrategyStep)
		{
		// we are done
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_RUNL_4, "Strategy is completed");
		Strategy(iStrategyId).NotifyFsm (*this, iCompletionStatus);
		}
	else
		{
		// continue with next step
		SetActive();
		Strategy(iStrategyId).Next(*this, &iStatus);
		}
	}

/** cancels last async request */  	
void CEtelDriverContext::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DOCANCEL_1, "CEtelDriverContext::DoCancel()");

	if(IsActive())
		{
		// delegate to strategy	
		Strategy(iStrategyId).CancelAsyncRequest(*this);
		}
	iStrategyStep = MEtelDriverStrategy::EFinishStep;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DOCANCEL_2, "Strategy is cancelled");
	}

/** accessor */
const TName& CEtelDriverContext::ExistingContextName() const 
	{ 
   	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_EXISTINGCONTEXTNAME_1, "CEtelDriverContext::ExistingContextName()");
	for (TContextId i = 0; i < static_cast<TContextId>(iFactory.ContextCount()); i++)
		{
        
		if (iFactory.HasContext(i) && iFactory.Context(i).Name().Size())
			{
			return iFactory.Context(i).Name(); 
			}
		}
		
	// Unacceptable situation: we didn't create a single context yet
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_EXISTINGCONTEXTNAME_2, "CEtelDriverContext::ExistingContextName - can't find existing context. return %S", iFactory.Context(0).Name());
	ASSERT(EFalse);
	return iFactory.Context(0).Name(); 
	}

/** accessor 

@return reference to etel RPhone
*/
RPhone& CEtelDriverContext::Phone() const
	{ 
	return iFactory.Phone(); 
	}

/** accessor 

@return reference to etel RPacketService
*/
RPacketService& CEtelDriverContext::PacketService() const
	{ 
	return iFactory.PacketService(); 
	}

/** accessor 

@return reference to etel driver strategy
*/
MEtelDriverStrategy& CEtelDriverContext::Strategy(TEtelDriverStrategy aId) const
	{ 
	return iFactory.Strategy(aId); 
	}

/** accessor 

@return reference to pdp fsm interface
*/
CPdpFsmInterface& CEtelDriverContext::PdpFsmInterface() const
	{ 
	return iFactory.PdpFsmInterface(); 
	}

/** start pdp notifications */
void CEtelDriverContext::StartNotifications() const
	{ 
	iFactory.StartPdpNotifications(iId); 
	}

/** stops pdp notifications */
void CEtelDriverContext::StopNotifications() const
	{ 
	iFactory.CancelPdpNotifications(iId); 
	}


#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
void CEtelDriverContext::DumpReqProfileParameters ()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_1,  "Requested Profile Parameters Dump - Context Id = %d", Id());
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_2,  "===========================================================");
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_3,  "ExtensionId = %d", iQosRequested.ExtensionId());
    
    if (iQosRequested.ExtensionId() != TPacketDataConfigBase::KConfigRel99Rel4
        && iQosRequested.ExtensionId() != TPacketDataConfigBase::KConfigRel5)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_4,  "Invalid/Unsupported ExtensionId");
        return;
        }

    if (iQosRequested.ExtensionId() == TPacketDataConfigBase::KConfigRel99Rel4
        || iQosRequested.ExtensionId() == TPacketDataConfigBase::KConfigRel5)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_5,  "[1]iReqTrafficClass = %d", iQosRequested.RequestedQoSR99_R4().iReqTrafficClass);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_6,  "[2]iMinTrafficClass = %d", iQosRequested.RequestedQoSR99_R4().iMinTrafficClass);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_7,  "[3]iReqDeliveryOrderReqd = %d", iQosRequested.RequestedQoSR99_R4().iReqDeliveryOrderReqd);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_8,  "[4]iMinDeliveryOrderReqd = %d", iQosRequested.RequestedQoSR99_R4().iMinDeliveryOrderReqd);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_9,  "[5]iReqDeliverErroneousSDU = %d", iQosRequested.RequestedQoSR99_R4().iReqDeliverErroneousSDU);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_10,  "[6]iMinDeliverErroneousSDU = %d", iQosRequested.RequestedQoSR99_R4().iMinDeliverErroneousSDU);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_11,  "[7]iReqMaxSDUSize = %d", iQosRequested.RequestedQoSR99_R4().iReqMaxSDUSize);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_12,  "[8]iMinAcceptableMaxSDUSize = %d", iQosRequested.RequestedQoSR99_R4().iMinAcceptableMaxSDUSize);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_13,  "[9]iReqMaxRate.iUplinkRate = %d", iQosRequested.RequestedQoSR99_R4().iReqMaxRate.iUplinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_14,  "[10]iReqMaxRate.iDownlinkRate = %d", iQosRequested.RequestedQoSR99_R4().iReqMaxRate.iDownlinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_15,  "[11]iMinAcceptableMaxRate.iUplinkRate = %d", iQosRequested.RequestedQoSR99_R4().iMinAcceptableMaxRate.iUplinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_16,  "[12]iMinAcceptableMaxRate.iDownlinkRate = %d", iQosRequested.RequestedQoSR99_R4().iMinAcceptableMaxRate.iDownlinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_17,  "[13]iReqBER = %d", iQosRequested.RequestedQoSR99_R4().iReqBER);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_18,  "[14]iMaxBER = %d", iQosRequested.RequestedQoSR99_R4().iMaxBER);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_19,  "[15]iReqSDUErrorRatio = %d", iQosRequested.RequestedQoSR99_R4().iReqSDUErrorRatio);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_20,  "[16]iMaxSDUErrorRatio = %d", iQosRequested.RequestedQoSR99_R4().iMaxSDUErrorRatio);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_21,  "[17]iReqTrafficHandlingPriority = %d", iQosRequested.RequestedQoSR99_R4().iReqTrafficHandlingPriority);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_22,  "[18]iReqTrafficHandlingPriority = %d", iQosRequested.RequestedQoSR99_R4().iMinTrafficHandlingPriority);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_23,  "[19]iReqTransferDelay = %d", iQosRequested.RequestedQoSR99_R4().iReqTransferDelay);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_24,  "[20]iMaxTransferDelay = %d", iQosRequested.RequestedQoSR99_R4().iMaxTransferDelay);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_25,  "[21]iReqGuaranteedRate.iUplinkRate = %d", iQosRequested.RequestedQoSR99_R4().iReqGuaranteedRate.iUplinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_26,  "[22]iReqGuaranteedRate.iDownlinkRate = %d", iQosRequested.RequestedQoSR99_R4().iReqGuaranteedRate.iDownlinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_27,  "[23]iMinGuaranteedRate.iUplinkRate = %d", iQosRequested.RequestedQoSR99_R4().iMinGuaranteedRate.iUplinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_28,  "[24]iMinGuaranteedRate.iDownlinkRate = %d", iQosRequested.RequestedQoSR99_R4().iMinGuaranteedRate.iDownlinkRate);
        }
    
#ifdef SYMBIAN_NETWORKING_UMTSR5
    if (iQosRequested.ExtensionId() == TPacketDataConfigBase::KConfigRel5)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_29,  "[25]iSignallingIndication = %d", iQosRequested.RequestedQoSR5().iSignallingIndication);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPREQPROFILEPARAMETERS_30,  "[26]iSourceStatisticsDescriptor = %d", iQosRequested.RequestedQoSR5().iSourceStatisticsDescriptor);
        }
#endif        
    }



void CEtelDriverContext::DumpNegProfileParameters ()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_1,  "Negotiated Profile Parameters Dump - Context Id = %d", Id());
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_2,  "===========================================================");
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_3,  "ExtensionId = %d", iQosNegotiated.ExtensionId());
    
    if (iQosNegotiated.ExtensionId() != TPacketDataConfigBase::KConfigRel99Rel4
        && iQosNegotiated.ExtensionId() != TPacketDataConfigBase::KConfigRel5)
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_4,  "Invalid/Unsupported ExtensionId");
        return;
        }

    if (iQosNegotiated.ExtensionId() == TPacketDataConfigBase::KConfigRel99Rel4
        || iQosNegotiated.ExtensionId() == TPacketDataConfigBase::KConfigRel5)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_5,  "[2]iTrafficClass = %d", iQosNegotiated.NegotiatedQoSR99_R4().iTrafficClass);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_6,  "[3]iDeliveryOrderReqd = %d", iQosNegotiated.NegotiatedQoSR99_R4().iDeliveryOrderReqd);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_7,  "[4]iDeliverErroneousSDU = %d", iQosNegotiated.NegotiatedQoSR99_R4().iDeliverErroneousSDU);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_8,  "[5]iMaxSDUSize = %d", iQosNegotiated.NegotiatedQoSR99_R4().iMaxSDUSize);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_9,  "[6]iBER = %d", iQosNegotiated.NegotiatedQoSR99_R4().iBER);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_10,  "[7]iSDUErrorRatio = %d", iQosNegotiated.NegotiatedQoSR99_R4().iSDUErrorRatio);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_11,  "[8]iTrafficHandlingPriority = %d", iQosNegotiated.NegotiatedQoSR99_R4().iTrafficHandlingPriority);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_12,  "[9]iTransferDelay = %d", iQosNegotiated.NegotiatedQoSR99_R4().iTransferDelay);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_13,  "[10]iGuaranteedRate.iUplinkRate = %d", iQosNegotiated.NegotiatedQoSR99_R4().iGuaranteedRate.iUplinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_14,  "[11]iGuaranteedRate.iDownlinkRate = %d", iQosNegotiated.NegotiatedQoSR99_R4().iGuaranteedRate.iDownlinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_15,  "[12]iMaxRate.iUplinkRate = %d", iQosNegotiated.NegotiatedQoSR99_R4().iMaxRate.iUplinkRate);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_16,  "[13]iMaxRate.iDownlinkRate = %d", iQosNegotiated.NegotiatedQoSR99_R4().iMaxRate.iDownlinkRate);
        }

    
#ifdef SYMBIAN_NETWORKING_UMTSR5
    if (iQosNegotiated.ExtensionId() == TPacketDataConfigBase::KConfigRel5)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_17,  "[14]iSignallingIndication = %d", iQosNegotiated.NegotiatedQoSR5().iSignallingIndication);
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CETELDRIVERCONTEXT_DUMPNEGPROFILEPARAMETERS_18,  "[15]iSourceStatisticsDescriptor = %d", iQosNegotiated.NegotiatedQoSR5().iSourceStatisticsDescriptor);
        }
#endif
    }
    
#endif
// _DEBUG
