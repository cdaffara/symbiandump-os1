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
// Description: Test wrapper for RSsmStateManager
// 
// 
//

#include <test/datawrapper.h>
#include <ssm/ssmstates.hrh>
#include <ssm/ssmsubstates.hrh>

#include "t_ssmstatemanager.h"

_LIT(KMainState,      "mainState");
_LIT(KSubState,       "subState");
_LIT(KReason,         "reason");  
_LIT(KAsync,          "async");

//commands
_LIT(KCmdNewL,                       "NewL");
_LIT(KCmdConnect,                    "Connect");
_LIT(KCmdClose,                      "Close");
_LIT(KCmdRequestStateTransition,     "RequestStateTransition");
_LIT(KCmdRequestStateTransitionCancel,  "RequestStateTransitionCancel");
_LIT(KCmdDestructor,                 "~");

/**
 * Two phase constructor
 */
CTestRSsmStateManager* CTestRSsmStateManager::NewL()
    {
    CTestRSsmStateManager*  testSsmStateManager = new (ELeave) CTestRSsmStateManager();
    CleanupStack::PushL(testSsmStateManager);
    testSsmStateManager->ConstructL();
    CleanupStack::Pop(testSsmStateManager);
    return testSsmStateManager; 
    }

/**
 * GetObject
 */
TAny*  CTestRSsmStateManager::GetObject()
    {
    return iSsmStateManager;
    }

/**
 * SetObjectL
 */
void CTestRSsmStateManager::SetObjectL(TAny* aAny)
    {
    DoCleanup();
    iSsmStateManager=static_cast<RSsmStateManager*> (aAny);
    }

/**
 * DisownObjectL
 */
void CTestRSsmStateManager::DisownObjectL()
    {
    iSsmStateManager = NULL;
    }

/**
 * Protected constructor. First phase construction
 */
CTestRSsmStateManager::CTestRSsmStateManager()
:CDataWrapper()
    {
    }

/**
 * Protected constructor. Second phase construction
 */
void CTestRSsmStateManager::ConstructL()
    {
    iActiveNotifyOnChange = CActiveCallback::NewL(*this);
    }

/**
 * Destructor.
 */
CTestRSsmStateManager::~CTestRSsmStateManager()
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
TBool CTestRSsmStateManager::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
    {
    TBool retVal = ETrue;
    if (aCommand == KCmdNewL)
        {
        DoCmdNew();
        }
    else if (aCommand == KCmdConnect)
        {
        DoCmdConnect();
        }
    else if (aCommand == KCmdRequestStateTransition )
        {
        DoCmdRequestStateTransition(aSection, aAsyncErrorIndex);
        }  
    else if (aCommand == KCmdRequestStateTransitionCancel )
        {
        DoCmdRequestStateTransitionCancel();
        }
    else if (aCommand == KCmdClose)
        {
        DoCmdClose();
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
 *  Creates RSSmStateAwareSession class instance
 */
void CTestRSsmStateManager::DoCmdNew()
    {
    INFO_PRINTF1(_L("Create RSsmStateManager instance"));
    DoCleanup();
    
    TRAPD( err, iSsmStateManager = new (ELeave)RSsmStateManager());
    if (err!=KErrNone)
        {
        ERR_PRINTF2(_L("new error %d"), err);
        SetError(err);
        }        
    }

/**
 * Close RSsmStateManager handle
 */
void CTestRSsmStateManager::DoCmdClose()
    {
    INFO_PRINTF1(_L("Close RSsmStateManager "));
    iSsmStateManager->Close();
    }

/**
 * Contains cleanup implementation
 */
void CTestRSsmStateManager::DoCleanup()
    {
    if(iSsmStateManager != NULL)
        {
        INFO_PRINTF1(_L("Deleting current RSsmStateManager"));
        delete iSsmStateManager;
        iSsmStateManager = NULL;
        }
    }

/** 
 * Destroy RSsmStateManager object
 */
void CTestRSsmStateManager::DoCmdDestructor()
    {
    INFO_PRINTF1(_L("Destroying the object"));
    DoCleanup();
    }

/**
 * Connects to Ssm State Manager
 */
void CTestRSsmStateManager::DoCmdConnect()
    {     
    TInt  err = iSsmStateManager->Connect();          
    if (KErrNone != err)
        {
        ERR_PRINTF2(_L("Connect() error %d"), err);
        SetError(err);   
        }    
    }

/**
 * RequestStateTransition in case of State transition request
 */
void CTestRSsmStateManager::DoCmdRequestStateTransition(const TDesC& aSection, const TInt aAsyncErrorIndex)
    {     
    TPtrC getMainState;
    TPtrC getSubStates;    
    if (!GetStringFromConfig(aSection, KMainState(), getMainState ))
        {
        //set default value if value is not provided in .ini file
        getMainState.Set( _L("ESsmNormal"));
        }
    if (!GetStringFromConfig(aSection, KSubState(), getSubStates ))
        {
        //set default value if value is not provided in .ini file
        getSubStates.Set(_L("ESsmNormalRfOffSubState"));
        }
    TUint16 mainState;
    TUint16 subState;
    if ( MapToMainState( getMainState, mainState )&& MapToSubState( getSubStates, subState ))
        {
        TInt reason;
        if (!GetIntFromConfig(aSection, KReason(), reason ))
            {
            //set default reason if value is not provided in .ini file
            reason = KErrNone;
            }    
        
        TSsmStateTransition stateTransition(mainState, subState, reason );
        //Call the API depending on async call or not
        TBool async;
        if (!GetBoolFromConfig(aSection, KAsync(), async ))
            {
            async = EFalse;
            }
        if (async)
            {
            iSsmStateManager->RequestStateTransition(stateTransition, iActiveNotifyOnChange->iStatus);
            iActiveNotifyOnChange->Activate(aAsyncErrorIndex);
            IncOutstanding();
            }
        else
            {
            TInt err = iSsmStateManager->RequestStateTransition(stateTransition); 
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("RequestStateTransition() failed with error %d"), err);
                SetError(err);
                }
            }        
        } 
    else
        {
        ERR_PRINTF1(_L(" Mapping state/substate failed%d"));
        SetBlockResult(EFail);
        }
    }

/**
 * RequestStateTransitionCancel
 */
void CTestRSsmStateManager::DoCmdRequestStateTransitionCancel()
    {    
    iSsmStateManager->RequestStateTransitionCancel();
    }

/**
 Virtual RunL - Called on completion of an asynchronous command
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
 @see MTPActiveCallback
*/
void CTestRSsmStateManager::RunL(CActive* aActive, TInt aIndex)
    {
    INFO_PRINTF1(_L("RSsmStateManager->RunL()is called")); 
    if (aActive == iActiveNotifyOnChange)
        {
        TInt    err=iActiveNotifyOnChange->iStatus.Int();
        if ( err != KErrNone )
            {
            ERR_PRINTF2(_L("RunL Error %d"), err);
            SetAsyncError(aIndex, err);
            }
        }
    DecOutstanding();   
    }

/**
 Virtual DoCancel - Request to cancel the asynchronous command
 @param aActive Active Object that DoCancel has been called on
 @pre - N/A
 @post - N/A
 @leave system wide error code
 @see MTPActiveCallback
*/
void CTestRSsmStateManager::DoCancel(CActive* aActive, TInt aIndex)
    {
    INFO_PRINTF1(_L("DoCancel"));
    if( aActive ==  iActiveNotifyOnChange ) 
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
 * Helper method to Map defined MainState on ssmstates.hrh
 */
TBool CTestRSsmStateManager::MapToMainState( TPtrC& aGetMainState, TUint16& aMainState )
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

/**
 * Helper method to Map defined SubState in ssmsubstates.hrh..Need to be extended for other substates
 */
TBool CTestRSsmStateManager::MapToSubState( TPtrC& aGetSubState, TUint16& aSubState )
    {
    TInt ret = ETrue;
    if(!(aGetSubState.Compare(_L("ESsmNormalRfOffSubState"))))
        {
        aSubState = ESsmNormalRfOffSubState;
        }
    else if(!(aGetSubState.Compare(_L("ESsmNormalSubState"))))
        {
        aSubState = ESsmNormalSubState;
        }
    else if(!(aGetSubState.Compare(_L("ESsmNormalRfOnSubState"))))
        {
        aSubState = ESsmNormalRfOnSubState;
        }    
    else
        {
        ret = EFalse;
        }
    return ret;
    }
