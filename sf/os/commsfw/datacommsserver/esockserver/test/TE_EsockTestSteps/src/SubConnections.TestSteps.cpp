// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file SubConnections.TestSteps.cpp
*/


#include <nifman.h>
#include <cs_subconevents.h>

#include "SubConnections.TestSteps.h"
#include "SocketServer.TestSteps.h"
#include "Connections.TestSteps.h"


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestSbCnctn, "ESockTestSbCnctn");
#endif


// Create SubConnection
//---------------------

CCreateRSubConnectionStep::CCreateRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCreateRSubConnectionStep);
	}

TVerdict CCreateRSubConnectionStep::doTestStepPreambleL()
	{
	SetTestStepResult(EFail);

	if (iEsockTest==NULL)
	    iEsockTest = new (ELeave) CCEsockTestBase;

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TInt CCreateRSubConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();
	
    //try reading the next socket's name
    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1
        || iParams.iSubConnectionName.Length()==0)
            return KErrNotFound;

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCreateRSubConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CreateSubConnection(iParams.iSubConnectionName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not create subconnection (%S)."),&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Open SubConnection
//-------------------

COpenRSubConnectionStep::COpenRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KOpenRSubConnectionStep);
	}

TInt COpenRSubConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SubConnectionName, iParams.iSubConnectionName) != 1)
		|| (iParams.iSubConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    if (GetStringFromConfig(iSection,KTe_SocketServName,iParams.iSockServName)!=1)
        {
        INFO_PRINTF2(_L("%S: Socket server name missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)!=1)
        {
        INFO_PRINTF2(_L("%S: Connection name missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

	TPtrC subConnTypeName;
    if (GetStringFromConfig(iSection,KTe_SubConnectionTypeName,subConnTypeName)!=1)
        {
        INFO_PRINTF2(_L("%S: SubConnection type missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

    if (subConnTypeName.Compare(KTe_SubConnectionTypeAttach)==0)
	    { iParams.iSubConnType = RSubConnection::EAttachToDefault; }
    else if (subConnTypeName.Compare(KTe_SubConnectionTypeNew)==0)
	    { iParams.iSubConnType = RSubConnection::ECreateNew; }
    else
       {
       INFO_PRINTF3(_L("%S: SubConnection type (%S) not recognised."),&iParams.iSubConnectionName,&subConnTypeName);
       return KErrNotFound;
       }

	if (!GetIntFromConfig(iSection, KExpectedError, iExpectedError))  
       {
       	iExpectedError = KErrNone;
       }

       else
       {
       	INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);
       }

    // All ok if we got this far
    return KErrNone;
	}

TVerdict COpenRSubConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->OpenSubConnection(iParams);

   	if (error!=KErrNone)
          {
          INFO_PRINTF2(_L("Could not open subconnection (%S)."),&iParams.iSubConnectionName);
          INFO_PRINTF2(_L("Error: %d."),error);
          }

    if (error!=iExpectedError)
          {
          INFO_PRINTF3(_L("Returned error (%d) differs from expected (%d)."), error, iExpectedError);
          return EFail;
          }

	return EPass;
	}


// Open SubConnection (OOM)
//-------------------------

COpenRSubConnectionOOMStep::COpenRSubConnectionOOMStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KOpenRSubConnectionOOMStep);
	}

TInt COpenRSubConnectionOOMStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SubConnectionName, iParams.iSubConnectionName) != 1)
		|| (iParams.iSubConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    if (GetStringFromConfig(iSection,KTe_SocketServName,iParams.iSockServName)!=1)
        {
        INFO_PRINTF2(_L("%S: Socket server name missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

    if (GetStringFromConfig(iSection,KTe_ConnectionName,iParams.iConnectionName)!=1)
        {
        INFO_PRINTF2(_L("%S: Connection name missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

	TPtrC subConnTypeName;
    if (GetStringFromConfig(iSection,KTe_SubConnectionTypeName,subConnTypeName)!=1)
        {
        INFO_PRINTF2(_L("%S: SubConnection type missing."),&iParams.iSubConnectionName);
        return KErrNotFound;
        }

    if (subConnTypeName.Compare(KTe_SubConnectionTypeAttach)==0)
	    { iParams.iSubConnType = RSubConnection::EAttachToDefault; }
    else if (subConnTypeName.Compare(KTe_SubConnectionTypeNew)==0)
	    { iParams.iSubConnType = RSubConnection::ECreateNew; }
    else
       {
       INFO_PRINTF3(_L("%S: SubConnection type (%S) not recognised."),&iParams.iSubConnectionName,&subConnTypeName);
       return KErrNotFound;
       }

    // All ok if we got this far
    return KErrNone;
	}

TInt COpenRSubConnectionOOMStep::OpenSubConnectionOOM()
	{
#ifndef _DEBUG
	return KErrNone;
#else	


    //disable idle timers for the duration of the test - they occassionally kick in.
    RConnection* c = iEsockTest->iConns.Find(iParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;	
    TInt error = c->SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
    if (error != KErrNone)
        return error;
    
	error = KErrNoMemory;
	for (TInt j = 1; !iEsockTest->__DbgCheckFailNext(iParams.iSockServName); j++)
		{
		iEsockTest->__DbgFailNext(iParams.iSockServName, j);
		iEsockTest->CloseSubConnection(iParams.iSubConnectionName);
		error = iEsockTest->OpenSubConnection(iParams);
		INFO_PRINTF3(_L("RSubCbconnection::Open() returned (%d) in OOM loop (%d)!.."),error, j);
		}
	iEsockTest->__DbgFailNext(iParams.iSockServName, -1);
	iEsockTest->CloseSubConnection(iParams.iSubConnectionName);
	
	//re-enable idle timers.
    error = c->SetOpt(KCOLProvider, KConnDisableTimers, EFalse);
    
	return error;
	
#endif // _DEBUG
	}

TVerdict COpenRSubConnectionOOMStep::doSingleTestStep()
	{
    TInt error = OpenSubConnectionOOM();
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not open subconnection (%S)."),&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Close SubConnection
//--------------------

CCloseRSubConnectionStep::CCloseRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KCloseRSubConnectionStep);
	}

TInt CCloseRSubConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();
	
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SubConnectionName, iParams.iSubConnectionName) != 1)
		|| (iParams.iSubConnectionName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}
		
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CCloseRSubConnectionStep::doSingleTestStep()
	{
    TInt error = iEsockTest->CloseSubConnection(iParams.iSubConnectionName);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not close subconnection (%S)."),&iParams.iSubConnectionName);
        INFO_PRINTF2(_L("Error: %d."),error);
        return EFail;
        }
	return EPass;
	}


// Register for SubConnection Events
//----------------------------------

CRegisterForRSubConnectionEventsStep::CRegisterForRSubConnectionEventsStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
	SetTestStepName(KRegisterForRSubConnectionEventsStep);
	}

TInt CRegisterForRSubConnectionEventsStep::ConfigureFromIni()
	{
	//clean parameters from previous runs
	iParams.Reset();

    //try reading the next event container's name
    if (GetStringFromConfig(iSection,KTe_SubConnectionEventsName,iParams.iEventName)!=1
        || iParams.iEventName.Length()==0)
            return KErrNotFound;

    if (GetStringFromConfig(iSection,KTe_SubConnectionName,iParams.iSubConnectionName)!=1)
        {
        INFO_PRINTF2(_L("%S: Subconnection name missing."),&iParams.iEventName);
        return KErrNotFound;
        }

    TPtrC eventName;
    if (GetStringFromConfig(iSection,KTe_Register,eventName)==1)
        {
        //first check, maybe someone wants to register for all events?
        if (eventName.Compare(KTe_All)==0)
            {
            iParams.iAllEvents = ETrue;
            INFO_PRINTF2(_L("%S: Events to be registered: All"),&iParams.iEventName);
            return KErrNone;
            }
        //secondly, someone could be interested in the generic events only.. 
        if (eventName.Compare(KTe_Generic)==0)
            {
            iParams.iGenericEvents = ETrue;
            INFO_PRINTF2(_L("%S: Events to be registered: Generic"),&iParams.iEventName);
            return KErrNone;
            }
        }

    //thirdly..
    //go through the list of particular events to register.
    //there may be more than one expected event kind, register all of them
    TBool desciptionFound = ETrue;
    for (TInt idx = 0; desciptionFound; idx++)
        {
        TBuf<KMaxTestExecuteNameLength> fieldName(KTe_Register);
        fieldName.AppendNum(idx);
        if (GetStringFromConfig(iSection,fieldName,eventName)==1)
            {
            if (eventName.Compare(KTe_EventTypeParamsGranted)==0)
        	    { iParams.iEventMask |= KSubConGenericEventParamsGranted; }
            else if (eventName.Compare(KTe_EventTypeDataClientJoined)==0)
        	    { iParams.iEventMask |= KSubConGenericEventDataClientJoined; }
            else if (eventName.Compare(KTe_EventTypeDataClientLeft)==0)
        	    { iParams.iEventMask |= KSubConGenericEventDataClientLeft; }
            else if (eventName.Compare(KTe_EventTypeSubConDown)==0)
        	    { iParams.iEventMask |= KSubConGenericEventSubConDown; }
            else if (eventName.Compare(KTe_EventTypeParamsChanged)==0)
        	    { iParams.iEventMask |= KSubConGenericEventParamsChanged; }
            else if (eventName.Compare(KTe_EventTypeParamsRejected)==0)
        	    { iParams.iEventMask |= KSubConGenericEventParamsRejected; }
            else
               {
               INFO_PRINTF3(_L("%S: Event type (%S) not recognised."),&iParams.iEventName,&eventName);
               return KErrNotFound;
               }
            }
        else
            {
            //at least one event type must be present
            if (idx==0)
                {
                INFO_PRINTF2(_L("%S: Event type missing."),&iParams.iEventName);
                return KErrNotFound;
                }
            else
                {
                desciptionFound = EFalse;
                }
            }
        }

    INFO_PRINTF3(_L("%S: Events to be registered: 0x%x"),&iParams.iEventName,iParams.iEventMask);
    return KErrNone;
	}

TVerdict CRegisterForRSubConnectionEventsStep::doSingleTestStep()
	{
	TInt error = iEsockTest->RegisterForRSubConnectionEvent(iParams);
    if (error!=KErrNone)
        {
        INFO_PRINTF2(_L("Could not register for rsubconnection events (%S)."),&iParams.iEventName);
        INFO_PRINTF2(_L("Error: %d"),error);
        return EFail;
        }

	return EPass;
	}


// Check for SubConnection Events
//-------------------------------

CCheckRSubConnectionEventsStep::CCheckRSubConnectionEventsStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
    {
	SetTestStepName(KCheckRSubConnectionEventsStep);
	}

TInt CCheckRSubConnectionEventsStep::ConfigureFromIni()
	{
	iParams.Reset();

	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SubConnectionEventsName, iParams.iEventName) != 1)
		|| (iParams.iEventName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    //there may be more than one expected event kind, register all of them
    TInt desciptionFound = ETrue;
    for (TInt idx = 0; desciptionFound; idx++)
        {
        TBuf<KMaxTestExecuteNameLength> fieldName(KTe_Expected);
        fieldName.AppendNum(idx);

    	TPtrC eventName;
        if (GetStringFromConfig(iSection,fieldName,eventName)==1)
            {
            if (eventName.Compare(KTe_EventTypeParamsGranted)==0)
        	    { iParams.iEventMask |= KSubConGenericEventParamsGranted; }
            else if (eventName.Compare(KTe_EventTypeDataClientJoined)==0)
        	    { iParams.iEventMask |= KSubConGenericEventDataClientJoined; }
            else if (eventName.Compare(KTe_EventTypeDataClientLeft)==0)
        	    { iParams.iEventMask |= KSubConGenericEventDataClientLeft; }
            else if (eventName.Compare(KTe_EventTypeSubConDown)==0)
        	    { iParams.iEventMask |= KSubConGenericEventSubConDown; }
            else if (eventName.Compare(KTe_EventTypeParamsChanged)==0)
        	    { iParams.iEventMask |= KSubConGenericEventParamsChanged; }
            else if (eventName.Compare(KTe_EventTypeParamsRejected)==0)
        	    { iParams.iEventMask |= KSubConGenericEventParamsRejected; }
            else
               {
               INFO_PRINTF3(_L("%S: Event type (%S) not recognised."),&iParams.iEventName,&eventName);
               return KErrNotFound;
               }
            }
        else
            {
            //at least one event type must be present
            if (idx==0)
                {
                INFO_PRINTF2(_L("%S: Event name missing."),&iParams.iEventName);
                return KErrNotFound;
                }
            else
                {
                desciptionFound = EFalse;
                }
            }
            
        
  	    if (!GetIntFromConfig(iSection, KExpectedError, iExpectedError))
        	{
        	iExpectedError = KExpectedErrorNotUsed;
        	}
            
        }

    INFO_PRINTF3(_L("%S: Events to be expected: 0x%x"),&iParams.iEventName,iParams.iEventMask);
    
    if (iExpectedError != KExpectedErrorNotUsed)
		{
			INFO_PRINTF2(_L("Error to be expected: %d"),iExpectedError);	
		}
    
    // All ok if we got this far
    return KErrNone;
	}


TVerdict CCheckRSubConnectionEventsStep::doSingleTestStep()
	{
	// Need to test the initialisation of TNotificationEventBuf
	TNotificationEventBuf eventTest;

	if (eventTest.GroupId() != 0)
		{
		return EFail;
		}
	if (eventTest.Id() != 0)
		{
		return EFail;
		}

	TNotificationEventBuf* event = NULL;
    TInt err = KErrNone;

    
    TInt error = iEsockTest->ReceiveRSubConnectionEvent(event, iParams.iEventName);
    
  
  // CSubConNotificationEvent* setparaevent = CSubConNotificationEvent::NewL(*event); 	
    
    if (event == NULL)
        {
        INFO_PRINTF2(_L("%S: Did not receive any event!"),&iParams.iEventName);
        INFO_PRINTF2(_L("The error code returned was %d."),error);
        return EFail;
        }

	
	TInt eventId = event->Id();
	CSubConNotificationEvent* setparaevent; 
    if (eventId == KSubConGenericEventParamsGranted )
    	{
		setparaevent = CSubConNotificationEvent::NewL(*event);
		__ASSERT_DEBUG(setparaevent->IsGeneric(), User::Panic(KSpecAssert_ESockTestSbCnctn, 1));
        INFO_PRINTF1(_L("Received event: KSubConGenericEventParamsGranted"));
    	}
    else if (eventId == KSubConGenericEventDataClientJoined )
    	{
    	setparaevent = CSubConNotificationEvent::NewL(*event);
    	__ASSERT_DEBUG(setparaevent->IsGeneric(), User::Panic(KSpecAssert_ESockTestSbCnctn, 2));
        INFO_PRINTF1(_L("Received event: KSubConGenericEventDataClientJoined"));
    	}
    else if (eventId == KSubConGenericEventDataClientLeft )
    	{
    	setparaevent = CSubConNotificationEvent::NewL(*event);
    	__ASSERT_DEBUG(setparaevent->IsGeneric(), User::Panic(KSpecAssert_ESockTestSbCnctn, 3));
        INFO_PRINTF1(_L("Received event: KSubConGenericEventDataClientLeft"));
    	}
    else if (eventId == KSubConGenericEventSubConDown )
    	{        
    	setparaevent = CSubConNotificationEvent::NewL(*event); 
    	__ASSERT_DEBUG(setparaevent->IsGeneric(), User::Panic(KSpecAssert_ESockTestSbCnctn, 4));    
        INFO_PRINTF1(_L("Received event: KSubConGenericEventSubConDown"));
    	}
    else if (eventId == KSubConGenericEventParamsChanged )
    	{
    	setparaevent = CSubConNotificationEvent::NewL(*event);
    	__ASSERT_DEBUG(setparaevent->IsGeneric(), User::Panic(KSpecAssert_ESockTestSbCnctn, 5));
        INFO_PRINTF1(_L("Received event: KSubConGenericEventParamsChanged"));
    	}
    else if (eventId == KSubConGenericEventParamsRejected )
    	{    
    	setparaevent = CSubConNotificationEvent::NewL(*event);
    	__ASSERT_DEBUG(setparaevent->IsGeneric(), User::Panic(KSpecAssert_ESockTestSbCnctn, 6));
    	CSubConGenEventParamsRejected* eventreason = static_cast<CSubConGenEventParamsRejected*>(setparaevent);    		   	
    	err = eventreason->Error();    	
        INFO_PRINTF1(_L("Received event: KSubConGenericEventParamsRejected"));
    	}
    else
    	{
        INFO_PRINTF2(_L("Received event: Unknown (%d)"),eventId);
    	}  
    
    delete setparaevent;
    if ((eventId & iParams.iEventMask) == 0)
        {
        INFO_PRINTF2(_L("Did not receive an expected event with %S."),&iParams.iEventName);
        return EFail;
        }
        
        
    if(iExpectedError != err && iExpectedError < KExpectedErrorNotUsed)
    	{
    	INFO_PRINTF3(_L("Expected Error %d does not match returned Error %d"), iExpectedError, err);
    	return EFail;
    	}
    	
    else if (iExpectedError != KExpectedErrorNotUsed)
    	{
    	INFO_PRINTF3(_L("Expected Error %d matched returned Error %d"), iExpectedError, err);
    	}


	return EPass;
	}


// Control SubConnection
//----------------------

CControlRSubConnectionStep::CControlRSubConnectionStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KControlRSubConnectionStep);
	}

TInt CControlRSubConnectionStep::ConfigureFromIni()
	{
	iParams.Reset();

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CControlRSubConnectionStep::doSingleTestStep()
	{
	return TestStepResult();
	}


// Enumerate subconnections
//-------------------------

CEnumerateSubConnectionsStep::CEnumerateSubConnectionsStep(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KEnumerateSubConnectionsStep);
	}

TInt CEnumerateSubConnectionsStep::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_ConnectionName, iParams.iConnectionName) != 1)
		|| (iParams.iConnectionName.Length() == 0))
		{
		INFO_PRINTF2(KErrString_MissingConfigFileField, &KTe_ConnectionName);
		return KErrNotFound;
		}
		
    // Get the expected connection count to validate against
	if(GetIntFromConfig(iSection, KExpectedSubConnectionCount, iParams.iExpectedSubConnCount) != 1)
		{
		INFO_PRINTF2(KErrString_MissingConfigFileField, &KExpectedSubConnectionCount);
		return KErrNotFound;
		}
    
    // All ok if we got this far
    return KErrNone;
	}

TVerdict CEnumerateSubConnectionsStep::doSingleTestStep()
	{
	TUint subConnectionCount;
	
	// Find the connection to apply the enumerate call to
	RConnection* connection = iEsockTest->FindConnection(iParams.iConnectionName);

	// Enumerate the current connection count
	if(connection)
		{
		// Fetch the connection count
		TInt err = connection->EnumerateSubConnections(subConnectionCount);
		
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Error when enumerating connections (%d)."), &err);
			return EFail;
			}
		
		// Compare the connection count with that expected
		if (subConnectionCount == iParams.iExpectedSubConnCount)
			{
			INFO_PRINTF3(_L("Current subconnection count (%d) equal to that expected (%d)."), &subConnectionCount, &iParams.iExpectedSubConnCount);
			}
		else
			{
			INFO_PRINTF3(_L("Current subconnection count (%d) not equal to expected count. (%d)"), &subConnectionCount, &iParams.iExpectedSubConnCount);
			return EFail;
			}
		}
	else
		{
		INFO_PRINTF2(_L("Could not find connection (%S)."), &iParams.iConnectionName);
		return EFail;
		}

	// Test passed if we got this far
	return EPass;
	}



