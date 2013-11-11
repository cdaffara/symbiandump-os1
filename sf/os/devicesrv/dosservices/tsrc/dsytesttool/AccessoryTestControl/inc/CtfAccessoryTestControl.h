/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#ifndef __CTFACCESSORYTESTCONTROL_H__
#define __CTFACCESSORYTESTCONTROL_H__

#include <ctfstub.h>
#include <ctfremotestub.h>

#include "MtfAccessoryTestControl.h"
#include "TfAccessoryTestControlTypes.h"
#include "CtfTestControlObserver.h"

#include <AccessoryServer.h>
#include <AccessoryMode.h>
#include <AccessorySettings.h>
#include <AccessoryConnection.h>
#include <AccessoryControl.h>
#include <AccessoryAudioControl.h>
#include <AccessoryBTControl.h>
#include <AccessorySingleConnection.h>
#include <e32property.h>

#include <AccPolGenericID.h>
#include <AccPolGenericIDarray.h>
#include <AccConfigFileParser.h>

#include <AccModeSetting.h>
#include <AccModeSettingArray.h>
#include <AccSettingsDomainCRKeys.h>

#include <accpolobjecttypes.h>
#include <accpolaudiostreamformatcon.h>
#include <accpolaudiotopologyobjectcon.h>
#include <accpolvolumecontrol.h>
#include <accpolmutecontrol.h>
#include <accpolinputcontrol.h>

struct TTFAccessoryTestCaseStateControl;

const TInt KTFStubTypeAccessoryControl = 565;

class CTFTestControlObserver;

class CTFAccessoryTestControl : public CTFRemoteStub, public MTFAccessoryTestControl
    {
public:
    CTFAccessoryTestControl( void );
    virtual ~CTFAccessoryTestControl( void );        
    
private:
    CTFAccessoryTestControl( const CTFAccessoryTestControl& aStub );
    CTFAccessoryTestControl& operator=( const CTFAccessoryTestControl& aStub );
    
public:
    void InitializeL( void );
    
public:
    void CallAccessoryFunctionL( TTFAccessoryTestCaseStateControl& aParameter );
    TInt ParameterCount( TTFAccessoryFunction aFunction ) const;
    TBool ParameterBounds( TTFAccessoryFunction aFunction, TInt* aMin, TInt* aMax ) const;
    
    void CancelOutStandingRequest();
    
      void ResetAccessoryServer();
    
protected:  // Functions from base classes
                
    void DoCancel();
	void RunL();
	TInt RunError( TInt aError );
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
    
    void AccessoryNotification( TInt aResult, TInt aParameter);    	 

private:

    void CreateObjectPubSubL( TStreamConObjectType aobjectType, 
                              TInt64 aNumberOfObjects,
                              TStreamConObjectType aobjectType2,
                              TInt64 aNumberOfObjects2 );
                                                      
    void FillObjectL( CAccPolAudioStreamFormatCon* aContainer,
                      TStreamConObjectType aobjectType, 
                      TInt64 aNumberOfObjects );


    void CreateTopologyObjectsToPubSubL( TTFAccessoryTestCaseStateControl& aParameter );
    
    TInt TestValueRecordL();

    TInt TestBaseSerializationL();
    
	CTFTestControlObserver* CreateObserver();			
    
    void PublishAndSubscribeL(TTFAccessoryTestCaseStateControl& aParameter);
    
    void ResetAccessoryServices();
    
    TInt GetExistence(CAccPolSubblockNameArray* aNameArray,TDesC aCapability);
    
    void AccessoryConnectionOpenClose(TTFAccessoryFunction aCase, TInt* aRetval);
    
    void FindAndCheckRequestL( TInt* aRetval, TTFAccessoryTestCaseStateControl& aParameter );
    
    void AccessorySettingsOpenClose( TTFAccessoryFunction aCase, TInt* aRetval );
    
    void GetSeveralAccessoryModeSetting( TInt* aRetval );
    
    void SetSeveralAccessoryModeSetting( TInt* aRetval );
    
    void SetIntValue( TTFAccessoryTestCaseStateControl& aParameter, TInt *aRetval );
    
    void SelectionDialog( TTFAccessoryFunction aCase, TInt* aRetval );
    
    void CheckConnectedAccessory( TInt* aRetval );
    
    void CallTClassMethods( TInt* aRetval );
    
    void GetBoolAccessoryModeSetting( TTFAccessoryTestCaseStateControl& aParameter, TInt* aRetval );
    
    void GetIntAccessoryModeSetting( TTFAccessoryTestCaseStateControl& aParameter, TInt* aRetval );
    
    void GetDesAccessoryModeSetting( TTFAccessoryTestCaseStateControl& aParameter, TInt* aRetval );
    
    void Settings();
    
    void Policy();
    
    void CreateHdmiObjectsToPubSubL( TTFAccessoryTestCaseStateControl& aParameter );
    
    TInt CheckHdmiContainerObjectL();
    
    TInt CheckHdmiSinkObjectL();
    
    TInt CheckHdmiAudioFormatObjectL();
    
    TInt CheckHdmiLatencyObjectL();
    
    TInt CheckHdmiSpeakerAllocationObjectL();
    
    TInt CheckHdmiVideoFormatObjectL();
    
    void CreateHdmiObjectL( CAccPolObjectCon& aCon, THdmiConObjectType aType );
    
private:
	
	TAccPolGenericID     						    iGenericId[10];
    TAccPolGenericIDArray							iGenericIdArray;
    TAccAudioType 									iAudioType;
	friend class 									CTFTestControlObserver;   
	//TBTDevAddr 										iBtaddr;
	RAccessoryServer 								iAccessoryServer;   	
	RAccessoryAudioControl*	     					iAccessoryAudioControl;
    RAccessoryBTControl*        					iAccessoryBTControl;
    RAccessoryConnection*      						iAccessoryConnection;
    RAccessoryMode*	             					iAccessoryMode;
    RAccessorySettings*	         					iAccessorySettings;
    RAccessorySingleConnection*	 					iAccessorySingleConnection;
    RAccessoryControl*	          					iAccessoryControl;						
    TBool 											iInitialized;    
    //CAccConfigFileParser* 							iAccConfigFileParser;   
    RProperty	 									iSubscribe;    
    TSglQue<CTFTestControlObserver> 				iStack; 
    TSglQueIter<CTFTestControlObserver> 			iStackIter;
   	CTFTestControlObserver* 						iObserverItem;

   	CAccPolAudioStreamFormatCon* iStreamContainer;
   	CAccPolAudioTopologyObjectCon* iTopologyContainer;
   	CAccPolVolumeControl*                           iSpeakerRightVolume;
   	CAccPolVolumeControl*                           iSpeakerLeftVolume;
   	CAccPolMuteControl*                           iSpeakerMute;
   	CAccPolVolumeControl*                           iMicRightVolume;
   	CAccPolVolumeControl*                           iMicLeftVolume;   	
    };
#endif
