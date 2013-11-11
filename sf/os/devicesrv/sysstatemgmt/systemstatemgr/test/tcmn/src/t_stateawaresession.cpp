// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
//Contributors:
//
// Description: Test wrapper for RSsmStateAwareSession

#include <test/datawrapper.h>
#include <ssm/ssmdomaindefs.h> 
#include <ssm/ssmsubstates.hrh>
#include <ssm/startupdomainpskeys.h>

#include "t_stateawaresession.h"

//
_LIT(KDomainId,                  "domainId"); 
_LIT(KAckNotify,                 "AckNotify%d");
_LIT(KAcknowledge,               "acknowledge");
_LIT(KExpectedState,             "expectedState"); 
_LIT(KError,                     "error");     

static TInt AckNotifyCounter = KErrNone;
//commands
_LIT(KCmdNewL,                                      "NewL");
_LIT(KCmdConnect,                                   "Connect");
_LIT(KCmdRequestStateNotification,                  "RequestStateNotification");
_LIT(KCmdAcknowledgeStateNotification,              "AcknowledgeStateNotification");
_LIT(KCmdClose,                                     "Close");
_LIT(KCmdState,                                     "State");
_LIT(KCmdRequestStateNotificationCancel,            "RequestStateNotificationCancel");
_LIT(KCmdAcknowledgeAndRequestStateNotification,    "AcknowledgeAndRequestStateNotification");
_LIT(KCmdDestructor,                                "~");

/**
 * Two phase constructor
 */
CTestRStateAwareSession* CTestRStateAwareSession::NewL()
    {
    CTestRStateAwareSession*  testStateAwareSession = new (ELeave) CTestRStateAwareSession();
    CleanupStack::PushL(testStateAwareSession);
    testStateAwareSession->ConstructL();
    CleanupStack::Pop(testStateAwareSession);
    return testStateAwareSession; 
    }

/**
* @return pointer to the object that the data wraps
*/
TAny*  CTestRStateAwareSession::GetObject()
    {
    return iSsmStateAwareSession;
    }

/**
 * SetObjectL
 */
void CTestRStateAwareSession::SetObjectL(TAny* aAny)
    {
    DoCleanup();
    iSsmStateAwareSession=static_cast<RSsmStateAwareSession*> (aAny);
    }

/**
 * DisownObjectL
 */
void CTestRStateAwareSession::DisownObjectL()
    {
    iSsmStateAwareSession=NULL;
    }

/**
 * Protected constructor. First phase construction
 */
CTestRStateAwareSession::CTestRStateAwareSession():
CDataWrapper()
    {
    }

/**
 * Protected constructor. Second phase construction
 */
void CTestRStateAwareSession::ConstructL()
    {
    iActiveNotifyOnChange = CActiveCallback::NewL(*this);
    }

/**
 * Destructor.
 */
CTestRStateAwareSession::~CTestRStateAwareSession()
    {
    DoCleanup();
    delete iActiveNotifyOnChange;
    }

/**
 * Process a command read from the ini file
 *
 * @param aCommand  the command to process
 * @param aSection      the entry in the ini file requiring the command to be processed
 *
 * @return ETrue if the command is processed
 */
TBool CTestRStateAwareSession::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
    {
    TBool retVal = ETrue;
    if (aCommand == KCmdNewL)
        {
        DoCmdNew();
        }
    else if (aCommand == KCmdConnect)
        {
        DoCmdConnect(aSection);
        }
    else if (aCommand == KCmdRequestStateNotification)
        {
        DoCmdRequestStateNotification(aAsyncErrorIndex);
        }
    else if (aCommand == KCmdAcknowledgeStateNotification)
        {
        DoCmdAcknowledgeStateNotification(aSection);
        }
    else if (aCommand == KCmdClose)
        {
        DoCmdClose();
        }
    else if (aCommand == KCmdState)
        {
        DoCmdState(aSection);
        }
    else if (aCommand == KCmdRequestStateNotificationCancel)
        {
        DoCmdRequestStateNotificationCancel();
        }
    else if (aCommand == KCmdAcknowledgeAndRequestStateNotification)
        {
        DoCmdAcknowledgeAndRequestStateNotification(aSection, aAsyncErrorIndex);
        }
    else if (aCommand == KCmdDestructor)
        {
        DoCmdDestructor();
        }
    else
        {
        retVal = EFalse;
        }    
   
    return retVal;
    }

/**
 * Creates RSSmStateAwareSession class instance
 */
void CTestRStateAwareSession::DoCmdNew()
    {
    INFO_PRINTF1(_L("Create RSsmStateAwareSession class instance"));
    DoCleanup();
    
    TRAPD(err, iSsmStateAwareSession = new (ELeave)RSsmStateAwareSession());
    
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("new error %d"), err);
        SetError(err);
        }      
    }

/**
 * Close RSsmStateAwareSession handle
 */
void CTestRStateAwareSession::DoCmdClose()
    {
    INFO_PRINTF1(_L("Close RSsmStateAwareSession"));
    iSsmStateAwareSession->Close();
    }

/**
 * Contains cleanup implementation
 */
void CTestRStateAwareSession::DoCleanup()
    {
    if(iSsmStateAwareSession != NULL)
        {
        INFO_PRINTF1(_L("Deleting current RSsmStateAwareSession"));
        delete iSsmStateAwareSession;
        iSsmStateAwareSession = NULL;
        }
    }

/**
 * Destroy RStateAwareSession object
 */
void CTestRStateAwareSession::DoCmdDestructor()
    {
    INFO_PRINTF1(_L("Destroying the RStateAwareSession object"));
    DoCleanup();
    }

/**
 * Connects a client to RStateAwareSession with Domain Id
 */
void CTestRStateAwareSession::DoCmdConnect(const TDesC& aSection)
    {
    TPtrC domainIdName;
    if (!GetStringFromConfig(aSection, KDomainId(), domainIdName))
        {
        //set default domainId if value is not provided in .ini file
        domainIdName.Set(_L("KSM2UiServicesDomain1"));        
        }
    TDmDomainId domainId;
    if(MapToDomainId(domainIdName, domainId ))
       {
       INFO_PRINTF2(_L(" Calling RSsmStateAwareSession->Connect with domain id %S"), &domainIdName );
       TInt  err = iSsmStateAwareSession->Connect(domainId);          
       if (KErrNone != err)
            {
            ERR_PRINTF2(_L("Connect() error %d"), err);
            SetError(err);  
            }       
       } 
    else
        {
        ERR_PRINTF2(_L("Could not map %S to domain Id"), &domainIdName );
        SetBlockResult(EFail);
        }
    }

/**
 * DoCmdRequestStateNotification 
 */
void CTestRStateAwareSession::DoCmdRequestStateNotification(const TInt aAsyncErrorIndex)
    {
    INFO_PRINTF2(_L("RequestStateNotification with Asyc Error Index: %d"), aAsyncErrorIndex);
    iSsmStateAwareSession->RequestStateNotification(iActiveNotifyOnChange->iStatus);
    iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
    IncOutstanding();    
    }

/**
 * DoCmdAcknowledgeStateNotification
 */
void CTestRStateAwareSession::DoCmdAcknowledgeStateNotification(const TDesC& aSection)
    {
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
   iSsmStateAwareSession->AcknowledgeStateNotification(error);    
    }

/**
 * DoCmdAcknowledgeAndRequestStateNotification
 */
void CTestRStateAwareSession::DoCmdAcknowledgeAndRequestStateNotification(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {
    TInt error;
    if (!GetIntFromConfig(aSection, KError(), error ))
        {
        //set default error value if value is not provided in .ini file
        error = KErrNone;
        }
    iSsmStateAwareSession->AcknowledgeAndRequestStateNotification(error, iActiveNotifyOnChange->iStatus );
    iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
    IncOutstanding();        
    }

/**
 * DoCmdRequestStateNotificationCancel
 */
void CTestRStateAwareSession::DoCmdRequestStateNotificationCancel()
    {
    INFO_PRINTF1(_L("RequestStateNotificationCancel is called "));
    iSsmStateAwareSession->RequestStateNotificationCancel();
    }

/** 
 * DoCmdState()
 */
void CTestRStateAwareSession::DoCmdState(const TDesC& aSection)
    {   
    TSsmState state;
    state = iSsmStateAwareSession->State();
    INFO_PRINTF3(_L("Call to State() returned MainState %d : Substate %d"),state.MainState(),state.SubState());
    TPtrC readExpectedState;
    // Comparision will be done only when value is read from the ini file...other wise expected value is ignored
    if (GetStringFromConfig(aSection, KExpectedState(), readExpectedState ))
        {
        TUint16 expectedState;
        if (MapToMainState(readExpectedState, expectedState))
            {
            if(expectedState!=state.MainState())
                {
                ERR_PRINTF3(_L("Expected State is %d : Actual State is %d"), expectedState,state.MainState() );
                SetBlockResult(EFail);
                }
            }
        else
            {
            ERR_PRINTF2(_L("Could not map %S to any state"), &readExpectedState );
            SetBlockResult(EFail);
            }
        }
    }

/**
 Virtual RunL - Called on completion of an asynchronous command
 @see MTPActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CTestRStateAwareSession::RunL(CActive* aActive, TInt aIndex)
    {
    TInt error = KErrNone;  
    if (aActive == iActiveNotifyOnChange)
        {
        error = iActiveNotifyOnChange->iStatus.Int();
        if (error == KErrNone)
            {            
            TSsmState currentState = iSsmStateAwareSession->State(); 
            // This component only responds to Offline state events.
            if (currentState.MainState() == ESsmNormal && currentState.SubState() == ESsmNormalRfOffSubState )
                {
                TInt globalSystemState;
                error =RProperty::Get(KPSUidStartup, KPSGlobalSystemState, globalSystemState);
                const TInt ackErr = ( error != KErrNone || globalSystemState == ESwStateNormalRfOff )? KErrGeneral : KErrNone;
                //Acknowledge the state change 
                INFO_PRINTF2(_L("RunL() called with AsyncIndex %d"), aIndex);
                TBuf<KMaxTestExecuteCommandLength>  tempStore;
                tempStore.Format(KAckNotify(),++AckNotifyCounter);
                TBool acknowledge = ETrue;
                GetBoolFromConfig(tempStore,KAcknowledge(),acknowledge);
                //Acknowledge with success/failure
                if (acknowledge)
                    {
                    iSsmStateAwareSession->AcknowledgeStateNotification(ackErr);
                    }
                }                  
            }
        else
            {
            ERR_PRINTF2(_L("RunL Error %d"), error);
            SetAsyncError(aIndex, error);
            }       
        }
    DecOutstanding();
    }

/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @see - MTPActiveCallback
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
*/
void CTestRStateAwareSession::DoCancel(CActive* aActive, TInt aIndex)
    {
    INFO_PRINTF1(_L("DoCancel"));
    if(aActive ==  iActiveNotifyOnChange) 
        {
        TInt err =  iActiveNotifyOnChange->iStatus.Int(); 
        if (err!=KErrNone)
            {
            ERR_PRINTF2(_L("DoCancel Error %d"), err);
            SetAsyncError(aIndex, err);
            }
        }   
    DecOutstanding();   
    }
/**
 * Helper method to map the DomainIds. This can be extendable for other domain Ids
 */
TBool CTestRStateAwareSession::MapToDomainId(TPtrC& aDomainIdName, TDmDomainId& aDomainId)
    {
    TBool ret = ETrue;
    if (!(aDomainIdName.Compare(_L("KSM2UiServicesDomain1"))))
        {
        aDomainId = KSM2UiServicesDomain1;
        }
    else if (!(aDomainIdName.Compare(_L("KSM2HALDomain2"))))
        {
        aDomainId = KSM2HALDomain2;
        }
    else if (!(aDomainIdName.Compare(_L("KSM2HALDomain3"))))
        {
        aDomainId = KSM2HALDomain3;  
        }
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain1"))))
        {
        aDomainId = KSM2AppServicesDomain1;  
        }
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain2"))))
        {
        aDomainId = KSM2AppServicesDomain2;  
        }
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain3"))))
        {
        aDomainId = KSM2AppServicesDomain3;  
        } 
    else if (!(aDomainIdName.Compare(_L("KSM2AppServicesDomain4"))))
        {
        aDomainId = KSM2AppServicesDomain4;  
        } 
    else 
        {
        ret = EFalse;
        }   
    return ret;
    }

/**
 * Helper method to Map defined MainState on ssmstates.hrh
 */
TBool CTestRStateAwareSession::MapToMainState( TPtrC& aGetMainState, TUint16& aMainState )
    {
    TInt ret = ETrue;
    if(!(aGetMainState.Compare(_L("ESsmStartup"))))
        {
        aMainState = ESsmStartup;
        }
    else if(!(aGetMainState.Compare(_L("ESsmNormal"))))
        {
        aMainState = ESsmNormal;
        }    
    else if(!(aGetMainState.Compare(_L("ESsmShutdown"))))
        {
        aMainState = ESsmShutdown;
        }
    else if(!(aGetMainState.Compare(_L("ESsmFail"))))
        {
        aMainState = ESsmFail;
        }
    else if(!(aGetMainState.Compare(_L("ESsmRestore"))))
        {
        aMainState = ESsmRestore;
        }
    else if(!(aGetMainState.Compare(_L("ESsmEmergencyCallsOnly"))))
        {
        aMainState = ESsmEmergencyCallsOnly;
        }    
    else
        {
        ret = EFalse;
        }
    return ret;
    }

