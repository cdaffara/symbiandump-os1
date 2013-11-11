/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This file contains the header file of the CXXXStub class.
*
*/


#ifndef __CREMCONCONTROL_H__
#define __CREMCONCONTROL_H__

#include <RemConInterfaceSelector.h>
#include <RemConPocTargetObserver.h> 
#include <RemConPocTarget.h> 
#include <RemConSynchronizationTarget.h>
#include <RemConSynchronizationTargetObserver.h>

#include <RemConCallHandlingTarget.h>
#include <RemConCallHandlingTargetObserver.h>

#include <RemConCoreApiTargetObserver.h>
#include <RemConCoreApiTarget.h>
#include <RemConExtensionApi.h>


#include "ctfstub.h"
#include "ctfremotestub.h"

#include "remcontypes.h"
#include <e32property.h> 
#include "mremconcontrol.h"
#include "remconpoc.h" 
#include "remconsync.h" 
#include "remconcall.h"
#include "remconcore.h" 
#include "remconpspchobserver.h"

struct TRemConControlTestCaseState;
class MRemConControl;

const TInt KTFStubTypeRemConControl = 570;

class CRemConControl : public MRemConControl, public CTFRemoteStub 
    {
public:
    CRemConControl( void );
    
    virtual ~CRemConControl( void );

private:
    CRemConControl( const CRemConControl& aStub );
    CRemConControl& operator=( const CRemConControl& aStub );
    
public:
    void InitializeL( void );
    
public:
    void CallApiFunctionL( TRemConControlTestCaseState& aParameter );
    
    void DeleteInstances();
    
    void SwapToNormalHandlerL();

    /**
    * From CTFRemoteStub
    *
    * Processes a message and completes it. It is not necessary to 
    * complete the message immediately.
    */
    void ProcessMessageAndCompleteL( const RMessage2& aMessage );

    /**
    * From CTFRemoteStub
    *
    * Notifies the stub that the asynchronous message passed to
    * ProcessMessageAndCompleteL has been completed with given
    * status. This can be used to complete the test case.
    */
    void MessageCompleteL( TInt aResult );
	
	void NotifyKeyEvent( TInt aEvent, TInt aParameter ) ;
	
    void CompleteKeyEventResponse ( TInt aEvent, TInt aParam, TInt aError); 
	 
	void ResetEventResult();
	
private:
    void Finalize( void );
	
private:
		
	CRemConPoc*    iPocTarget;
    CRemConCore*   iCoreTarget;
    CRemConSync*   iSyncTarget;
    CRemConCall*   iCallTarget;
    
    TInt iKeyEvent;
    TInt iParameter; 
    TInt iError;
    
 
    CRemConPSPCHObserver* iCmdReceiverFromStub;

	RProperty iSubscribe;
	TBool iEventReceived;
	TBool iCompleteResponseReceived;
	
	CRemConInterfaceSelector* iTargetInterfaceSelector;
	
	RProcess iAudioClientProc;
	
    };

#endif	// __CXXXSTUB_H__
