// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Holds a list of all Muxers and listening SAP signal handlers.
// 
//

#include <bluetooth/logger.h>
#include "l2capSAPSignalHandler.h"
#include "l2capMuxController.h"
#include "l2capCommand.h"
#include "l2signalmgr.h"
#include "l2util.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

CL2CAPMuxController::~CL2CAPMuxController()
	{
	LOG_FUNC
	// Delete all L2Cap Muxers 
	TDblQueIter<CL2CAPMux> muxIter(iMuxers);
	CL2CAPMux* muxerPtr;
	while((muxerPtr = muxIter++) != NULL)
		{
		delete muxerPtr;
		}

	// Delete all the listening SAP Signal Handlers 
	TDblQueIter<CL2CapSAPSignalHandler> listeningSapIter(iListeningSapSignalHandlers);
	CL2CapSAPSignalHandler* sapSignalPtr;
	while((sapSignalPtr = listeningSapIter++) != NULL)
		{
		delete sapSignalPtr;
		}
		
	// Delete all the bound SAP Signal Handlers 
	TDblQueIter<CL2CapSAPSignalHandler> boundSapIter(iBoundSapSignalHandlers);
	while((sapSignalPtr = boundSapIter++) != NULL)
		{
		delete sapSignalPtr;
		}		
	}

	
CL2CAPMuxController::CL2CAPMuxController(CL2CAPProtocol& aProtocol)
	: 
	iMuxers(_FOFF(CL2CAPMux, iMuxControllerLink)),
	iBoundSapSignalHandlers(_FOFF(CL2CapSAPSignalHandler, iLink)),
	iListeningSapSignalHandlers(_FOFF(CL2CapSAPSignalHandler, iLink)),
	iProtocol(aProtocol)
	{
	LOG_FUNC
	}

// This method is called to register the signal handler owned by a cloned SAP to the
// appropriate Mux.  A Mux should always be available as this event has been triggered
// by receipt of an incoming connection.
TInt CL2CAPMuxController::AttachPassiveSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler,const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	// Find the muxer and register the SAP SH.
	TInt rerr = KErrNone;

	CL2CAPMux* mux = FindMuxer(aAddr);
	if(mux)
		{
		mux->RegisterSAPSignalHandler(aSAPSignalHandler);
		}
	else
		{
		rerr = KErrSessionClosed;
		}
	return rerr;
	}
	
// This method is called to register an active SAP signal handler to the appropriate Mux.
// If the Mux does not exist a new one will be allocated and tasked with establishing
// an ACL connection.	
TInt CL2CAPMuxController::AttachActiveSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler,const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	
	CL2CAPMux* mux = FindMuxer(aAddr);
	if(mux)
		{
		mux->RegisterSAPSignalHandler(aSAPSignalHandler);
		}
	else
		{
		TRAP(rerr, mux = CreateMuxerL(aAddr));
		if(rerr == KErrNone)
			{
			mux->RegisterSAPSignalHandler(aSAPSignalHandler);
			aSAPSignalHandler.ActiveConnectionRequest();
			}
		}
	return rerr;	
	}
	
	
// This method adds a listening SAP signal handler to the list of current listeners.	
void CL2CAPMuxController::AttachListeningSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler)
	{
	LOG_FUNC
	iListeningSapSignalHandlers.AddLast(aSAPSignalHandler);
	}

// This method adds a bound SAP signal handler to the bound list.
void CL2CAPMuxController::AttachBoundSignalHandler(CL2CapSAPSignalHandler& aSAPSignalHandler)
	{
	LOG_FUNC
	iBoundSapSignalHandlers.AddLast(aSAPSignalHandler);	
	}

TInt CL2CAPMuxController::BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSAP)
	{
	LOG_FUNC
	TInt retVal;
	CL2CAPMux* muxer = FindMuxer(aAddr);
	if(!muxer)
		{
		// If no error occurs, ownership of aSSP passes to the Mux;
		TRAP(retVal, muxer = CreateMuxerL(aAddr));	
		if(retVal == KErrNone)
			{
			TRAP(retVal, muxer->CompleteACLConnect(aSAP));
			}
		}
	else
		{
		retVal = KErrAlreadyExists;
		}

	return retVal;
	}
	
CL2CAPMux* CL2CAPMuxController::FindMuxer(const TBTDevAddr& aAddr)
/**
	Find muxer by BT address.
	Lookup this address to find if there's a L2CAP Mux associated
    with it
**/
	{
	LOG_FUNC
	TDblQueIter<CL2CAPMux> muxIter(iMuxers);
	muxIter.SetToFirst();
	CL2CAPMux* muxerPtr = NULL;

	while((muxerPtr = muxIter++) != NULL)
		{
		if(muxerPtr->RemoteBTAddr() == aAddr)
			{
			break;
			}
		}
	return muxerPtr;
	}	
	
CL2CAPMux* CL2CAPMuxController::CreateMuxerL(const TBTDevAddr& aAddr)
/**
   Create a new L2CAP Mux.
   
   The muxer returned is owned by this CL2CAPMuxController -- it
   does not hand over ownership of the mux to the caller.

   Note that this function does not check (except in debug) that there
   is no existing muxer.

**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(!FindMuxer(aAddr), Panic(EL2CAPMuxAlreadyExists));
	
	CL2CAPMux* muxer = CL2CAPMux::NewL(*this, aAddr);

	iMuxers.AddFirst(*muxer);
	return muxer;
	}
	
CL2CapSAPSignalHandler* CL2CAPMuxController::FindListeningSignalHandler(TL2CAPPort aPort)
	{
	LOG_FUNC
	/** Searches the listening Q for a SAP listening at PSM: aPort
	@return the listening SAP found, or zero (NULL) if not found.
	**/

	TDblQueIter<CL2CapSAPSignalHandler> sapIter(iListeningSapSignalHandlers);
	CL2CapSAPSignalHandler* sapSignalPtr;

	while((sapSignalPtr = sapIter++) != NULL)
		{
		if(sapSignalPtr->LocalPort() == aPort)
			return sapSignalPtr;
		}

	return NULL;
	}

CL2CapSAPSignalHandler* CL2CAPMuxController::FindIdleSignalHandler(TL2CAPPort aPort)
	{
	LOG_FUNC
	/** Searches the bound Q for a SAP bound to PSM: aPort and if it doesn't find one it
	    then searches the listening Q.
	@return the idle SAP found, or zero (NULL) if not found.
	**/

	TDblQueIter<CL2CapSAPSignalHandler> sapIter(iBoundSapSignalHandlers);
	CL2CapSAPSignalHandler* sapSignalPtr;
	
	while((sapSignalPtr = sapIter++) != NULL)
		{
		if(sapSignalPtr->LocalPort() == aPort)
			return sapSignalPtr;
		}

	return FindListeningSignalHandler(aPort);
	}

void CL2CAPMuxController::UpdateMuxerPriorities()
	{
	LOG_FUNC
	TDblQueIter<CL2CAPMux> muxIter(iMuxers);
	CL2CAPMux* muxerPtr;

	// Lowest muxer priority.
	TInt muxerPriority = EBaseMuxerPriority;
	
	// This starting value is equivalent to one or all of the channels
	// open on a Mux being of priority low.
	TInt currentSumMuxerPriorities = 0;
	TInt nextSumMuxerPriorities;
	TInt sum;
	
	while(currentSumMuxerPriorities != KMaxTInt)
		{
		nextSumMuxerPriorities = KMaxTInt;

		muxIter.SetToFirst();
		while((muxerPtr = muxIter++) != NULL)		
			{
			sum = muxerPtr->GetSumMuxerChannelPriorities();
			if(sum == currentSumMuxerPriorities)
				{
				muxerPtr->MuxerPriorityUpdate(muxerPriority);
				}
			
			if(sum > currentSumMuxerPriorities && sum < nextSumMuxerPriorities)
				{
				nextSumMuxerPriorities = sum;
				}
			}
		currentSumMuxerPriorities = nextSumMuxerPriorities;
		muxerPriority++;
		}
	}

TInt CL2CAPMuxController::FindFreeUserPSM(TL2CAPPort& aPSM)
	{
	LOG_FUNC
	TInt rerr = KErrL2CAPNoMorePSMs;
	aPSM = KL2CapDynamicPSMStart;

	// The PSM value will eventually wrap round if a free PSM
	// can't be found.
	while(aPSM >= KL2CapDynamicPSMStart)
		{
		if(FindIdleSignalHandler(aPSM) == NULL)
			{
			// There is not currently a SAP listening on this
			// PSM, so this will be chosen.
			rerr = KErrNone;
			break;
			}
		aPSM+=2;
		}

	return rerr;
	}

