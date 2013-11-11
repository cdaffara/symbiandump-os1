// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ctsysystemstatepluginTraces.h"
#endif

#include "ctsysystemstateplugin.h"

CCtsySystemStatePlugin* CCtsySystemStatePlugin::NewL()
	{
	CCtsySystemStatePlugin* self = new (ELeave)CCtsySystemStatePlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); 	
	return self;
	}

CCtsySystemStatePlugin::CCtsySystemStatePlugin()
	{	
	}
	
void CCtsySystemStatePlugin::ConstructL()
	{	
	// create active object for do RF state changing  
	iAoChangeRfForEmergencyCall = new (ELeave) CChangeRfForEmergencyCall(*this, iSsmEmergencyCallRf);
	User::LeaveIfError(Connect());
	}

CCtsySystemStatePlugin::~CCtsySystemStatePlugin()
	{  
	if (iAoChangeRfForEmergencyCall)
		{
		delete iAoChangeRfForEmergencyCall;
		}
	}	
	

// ---------------------------------------------------------------------------
// CCtsySystemStatePlugin::Connect
// Try to connect to RSsmEmergencyCallRfAdaptation
// returns KErrNone if success, otherwise -- error code
// ---------------------------------------------------------------------------
TInt CCtsySystemStatePlugin::Connect()
	{	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCTSYSYSTEMSTATEPLUGIN_CONNECT_1, "CCtsySystemStatePlugin::Connect()");

	TInt error (KErrNone);
	if (!iConnected)	 
	{	 
        error = iSsmEmergencyCallRf.Connect();  
        if (KErrNone != error)
            {
            return error; 
            }
        iConnected = ETrue; 
        error = iSsmEmergencyCallRf.SetAsPriorityClient();
		if (KErrNone != error)
			{
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCTSYSYSTEMSTATEPLUGIN_CONNECT_2, "CCtsySystemStatePlugin::Connect()  - SetAsPriorityClient returned %d", error);
            error = KErrNone;
			}
	}	
	return error;
	}

// ---------------------------------------------------------------------------
// CCtsySystemStatePlugin::ActivateRfForEmergencyCall
// Calls  RSsmEmergencyCallRfAdaptation::ActivateRfForEmergencyCall
// returns KErrNotReady if there is no connection to CCtsySystemStatePlugin,
// otherwise -- error code returned from CCtsySystemStatePlugin
// ---------------------------------------------------------------------------
void CCtsySystemStatePlugin::ActivateRfForEmergencyCall(MCtsySsmPluginCallback* aSsmPluginCallback,
                                                        TCtsySsmCallbackData& aCallbackData)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCTSYSYSTEMSTATEPLUGIN_ACTIVATERFFOREMERGENCYCALL_1, "CCtsySystemStatePlugin::ActivateRfForEmergencyCall()" );

	iAoChangeRfForEmergencyCall->ActivateRfForEmergencyCall(aSsmPluginCallback, aCallbackData);	
	}
	
// ---------------------------------------------------------------------------
// CCtsySystemStatePlugin::DeactivateRfAfterEmergencyCall
// Calls  RSsmEmergencyCallRfAdaptation::DeactivateRfAfterEmergencyCall
// returns KErrNotReady if there is no connection to CCtsySystemStatePlugin,
// otherwise -- error code returned from CCtsySystemStatePlugin
// ---------------------------------------------------------------------------
void CCtsySystemStatePlugin::DeactivateRfAfterEmergencyCall()
	{	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCTSYSYSTEMSTATEPLUGIN_DEACTIVATERFAFTEREMERGENCYCALL_1, "CCtsySystemStatePlugin::DeactivateRfForEmergencyCall()" );
	//try to connect
	TInt err = Connect();
	if (err)
		{
	    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCTSYSYSTEMSTATEPLUGIN_DEACTIVATERFAFTEREMERGENCYCALL_2, "CCtsySystemStatePlugin::DeactivateRfAfterEmergencyCall() failed to connect %d", err);
		//do nothing
		return;
		}
				
	iAoChangeRfForEmergencyCall->DeactivateRfAfterEmergencyCall();
	}
	
// ---------------------------------------------------------------------------
// CCtsySystemStatePlugin::Close
// Close connection to RSsmEmergencyCallRfAdaptation
// ---------------------------------------------------------------------------	
void CCtsySystemStatePlugin::Close()
	{			
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCTSYSYSTEMSTATEPLUGIN_CLOSE_1, "CCtsySystemStatePlugin::Close()" );
	if (iConnected)	
		{	
		// if there is some request...
		if (CChangeRfForEmergencyCall::ENotBusy != iAoChangeRfForEmergencyCall->Busy())
			{
			// ... cancel it
			iSsmEmergencyCallRf.Cancel(); 			
			}
					
		iSsmEmergencyCallRf.Close(); 	
		iConnected = EFalse;				
		}
	}
	
	
	
	

// ---------------------------------------------------------------------------	
//  Active object for changing RF State
// ---------------------------------------------------------------------------	
CChangeRfForEmergencyCall::CChangeRfForEmergencyCall(CCtsySystemStatePlugin& aCtsySystemStatePlugin, 
													 RSsmEmergencyCallRfAdaptation& aSsmEmergencyCallRf) :
			                                         CActive(EPriorityStandard),
			                                         iCtsySystemStatePlugin(aCtsySystemStatePlugin), 
			                                         iSsmEmergencyCallRf(aSsmEmergencyCallRf)
   {
   CActiveScheduler::Add(this);
   }

// ---------------------------------------------------------------------------  
//  Destructor for Active object
// ---------------------------------------------------------------------------	
CChangeRfForEmergencyCall::~CChangeRfForEmergencyCall()
		{
		Cancel();
		}
	
// ---------------------------------------------------------------------------  
//  Active object ActivateRfForEmergencyCall
// ---------------------------------------------------------------------------  
void CChangeRfForEmergencyCall::ActivateRfForEmergencyCall(MCtsySsmPluginCallback* aSsmPluginCallback,
														   TCtsySsmCallbackData& aCallbackData)
   {	
   OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCHANGERFFOREMERGENCYCALL_ACTIVATERFFOREMERGENCYCALL_1, "CChangeRfForEmergencyCall::ActivateRfForEmergencyCall()" );

   if (EDeactivateBusy == iBusy)
      {
      // cancel any request
	  Cancel();
	  iBusy = ENotBusy;				
      }
   else if (EActivateBusy == iBusy)
      {
      if (aSsmPluginCallback)
          {
		  aSsmPluginCallback->SsmPluginCallback(KErrServerBusy, aCallbackData);
          }
	  return;
      }
		
   // store pointer to callback interface
   iSsmPluginCallback = aSsmPluginCallback;
		
   // copy data for callback
   iCallbackData.iReqHandle = aCallbackData.iReqHandle;		
   RMobileENStore::TEmergencyNumber* number;
   aCallbackData.iNumberPackage.UnPackData(&number);				
   iCallbackData.iNumberPackage.PackData(number);
	
   iSsmEmergencyCallRf.ActivateRfForEmergencyCall(iStatus);
   iBusy = EActivateBusy;
	
   SetActive();
   }
	
// ---------------------------------------------------------------------------  
//  Active object DeactivateRfAfterEmergencyCall
// --------------------------------------------------------------------------- 
void CChangeRfForEmergencyCall::DeactivateRfAfterEmergencyCall()
   {	
   OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCHANGERFFOREMERGENCYCALL_DEACTIVATERFAFTEREMERGENCYCALL_1, "CChangeRfForEmergencyCall::DeactivateRfAfterEmergencyCall()" );

		// if we've already sent request  
		if (EActivateBusy == iBusy)
			{
			// cancel any request
			Cancel();
			iBusy = ENotBusy;
			iSsmPluginCallback  = NULL; // to prevent calling of callback (but I think there shoul be Cancel)		
			return;			
			}
		else if (EDeactivateBusy == iBusy)							
			{
			// do nothing
			return;
			}
			
		iSsmEmergencyCallRf.DeactivateRfForEmergencyCall(iStatus);
		iBusy = EDeactivateBusy;
	
		SetActive();		
		}
// ---------------------------------------------------------------------------  
//  Active object Busy method
// --------------------------------------------------------------------------- 		
TBool CChangeRfForEmergencyCall::Busy()
   {	
   return iBusy;
   }

// ---------------------------------------------------------------------------  
//  RunL for Active object
// ---------------------------------------------------------------------------  	
void CChangeRfForEmergencyCall::RunL()
   {	
   iBusy = ENotBusy;		
   
   // close SSM
   if (iSsmPluginCallback)
      {
	  // just call callback interface and pass into it error code
	  iSsmPluginCallback->SsmPluginCallback(iStatus.Int(), iCallbackData);
      iSsmPluginCallback = NULL;	
      }	
   }

// ---------------------------------------------------------------------------  
//  Cancel for Active object
// --------------------------------------------------------------------------- 		
void CChangeRfForEmergencyCall::DoCancel()
   {
   iSsmEmergencyCallRf.Cancel(); 
   iBusy = ENotBusy;
   }

