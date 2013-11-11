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
// implementation of etel driver input
// 
//

/**
 @file 
 @internalComponent
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "reteldriverinputTraces.h"
#endif

#include <e32def.h>
#include <e32std.h>

#include "ceteldriverfactory.h"
#include "ceteldrivercontext.h"
#include "reteldriverinput.h"

using namespace EtelDriver;

REtelDriverInput::REtelDriverInput()
: iDriverFactory(NULL)
	{
	}


REtelDriverInput::~REtelDriverInput()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_DTOR_1, "REtelDriverInput::~REtelDriverInput()");
	
	if (iDriverFactory)
	   {
	   delete iDriverFactory;
	   }	  
	}


/** requests to open etel driver

@param aPdpFsmInterface - reference to pdp FSM interface
*/
void REtelDriverInput::OpenL (CPdpFsmInterface& aPdpFsmInterface)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_OPENL_1, "REtelDriverInput::OpenL()");

	// internal check
	iDriverFactory = CEtelDriverFactory::NewL(aPdpFsmInterface);
	if (iDriverFactory)
		{
		iDriverFactory->InitL();
		}
	}


/** closes eTel driver and frees underlying resources
*/
void REtelDriverInput::Close()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CLOSE_1, "REtelDriverInput::Close()");
	
	// cancel all outstanding requests
	if (iDriverFactory)
		{
		CancelAllPdps();
		}

	delete iDriverFactory;
	iDriverFactory = NULL;
	}
	
/** requests to create new pdp context
	Note: this request will not open new primary or secondary context
		  But rather allocate necessary resources to do it.
		  Creation / deletion of eTel contexts should be done though Input method.

@param aPdpId - id of new pdp context
*/
void REtelDriverInput::CreatePdpL (TContextId aPdpId,SpudMan::TPdpContextType aContextType)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CREATEPDPL_1, "REtelDriverInput::CreatePdpL()");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CREATEPDPL_2, "pdp id : %d", aPdpId);
	
	ASSERT(iDriverFactory);
	iDriverFactory->CreatePdpL (aPdpId,aContextType );
	}
	
/** deletes pdp context

@param aPdpId - id of a pdp context
*/
void REtelDriverInput::DeletePdp (TContextId aPdpId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_DELETEPDP_1, "REtelDriverInput::DeletePdp()");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_DELETEPDP_2, "pdp id : %d", aPdpId);
	
	if(iDriverFactory)
    	{
    	iDriverFactory->FreePdp(aPdpId);	    
    	}
	}

/** requests to perform aOperation for aPdpId context

@param aPdpId - id of a pdp context
@param aOperation - etel driver operation
*/
void REtelDriverInput::Input(TContextId aPdpId, TEtelInput aOperation)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_INPUT_1, "REtelDriverInput::Input()");
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_INPUT_2, "REtelDriverInput::Input: pdp id : %d, operation %d", aPdpId, aOperation);
	ASSERT(iDriverFactory);
	
	iDriverFactory->Context(aPdpId).Input(aOperation);
	}

/** cancels last operation for aPdpId context 

@param aPdpId - id of a pdp context
*/
void REtelDriverInput::CancelPdp (TContextId aPdpId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CANCELPDP_1, "REtelDriverInput::CancelPdp()");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CANCELPDP_2, "REtelDriverInput::CancelPdp: pdp id : %d", aPdpId);
	ASSERT(iDriverFactory);
	
	iDriverFactory->Context(aPdpId).Cancel();
	}
	
/** starts network notifications for aPdpId context 

@param aPdpId - id of a pdp context
*/
void REtelDriverInput::StartPdpNotifications(TContextId aPdpId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_STARTPDPNOTIFICATIONS_1, "REtelDriverInput::StartPdpNotifications()");
	ASSERT(iDriverFactory);

	iDriverFactory->StartPdpNotifications(aPdpId);
	}
	
/** cancels network notifications for aPdpId context 

@param aPdpId - id of a pdp context
*/
void REtelDriverInput::CancelPdpNotifications (TContextId aPdpId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CANCELPDPNOTIFICATIONS_1, "REtelDriverInput::CancelPdpNotifications()");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CANCELPDPNOTIFICATIONS_2, "REtelDriverInput::CancelPdpNotifications: pdp id : %d", aPdpId);
	ASSERT(iDriverFactory);

	iDriverFactory->CancelPdpNotifications (aPdpId);
	}
	
/** cancels last operations for all created pdp contexts */
void REtelDriverInput::CancelAllPdps ()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CANCELALLPDPS_1, "REtelDriverInput::CancelAllPdps()");
	ASSERT(iDriverFactory);
	
	for(TContextId i = 0; i < static_cast<TContextId>(iDriverFactory->ContextCount()); i++)
		{
		if (iDriverFactory->HasContext(i))
		    {
    		iDriverFactory->Context(i).Cancel();
		    }
		}
	}

/** cancels all notifications for all pdp contexts */
void REtelDriverInput::CancelAllPdpNotifications ()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, RETELDRIVERINPUT_CANCELALLPDPNOTIFICATIONS_1, "REtelDriverInput::CancelAllPdpNotifications()");
	ASSERT(iDriverFactory);

	iDriverFactory->CancelAllPdpNotifications();
	}


