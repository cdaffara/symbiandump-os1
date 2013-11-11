// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MOCKTELEPHONYAUDIOCONTROL_H
#define MOCKTELEPHONYAUDIOCONTROL_H

#include <ctsy/pluginapi/mtelephonyaudiocontrol.h>

class MTelephonyAudioControlCallback;

class TCallInfo
    {
    public:
        TCallInfo( const TName& aCallName, 
                   MTelephonyAudioControl::TTelephonyAudioControlCallType aCallType, 
                   TBool aEmergency, 
                   RMobileCall::TMobileCallDirection aCallDirection );
    
        TName& CallName(); 
        RMobileCall::TMobileCallStatus CallStatus();
        TBool Emergency(); 
        RMobileCall::TMobileCallDirection CallDirection();    
        void SetCallStatus( RMobileCall::TMobileCallStatus aCallStatus );
                   
    private:
        TName iCallName; 
        MTelephonyAudioControl::TTelephonyAudioControlCallType iCallType;
        TBool iEmergency; 
        RMobileCall::TMobileCallDirection iCallDirection;
        RMobileCall::TMobileCallStatus iCallStatus;
    };

class CMockTelephonyAudioControl : public CBase, public MTelephonyAudioControl
    {        
    public:    
        
        static CMockTelephonyAudioControl* NewL( MTelephonyAudioControlCallback* aTelephonyAudioControlCallback );
        
        ~CMockTelephonyAudioControl();
        
        void SetupTelephonyAudio( const TName& aCallName, 
                                  MTelephonyAudioControl::TTelephonyAudioControlCallType aCallType, 
                                  TBool aEmergency, 
                                  RMobileCall::TMobileCallDirection aCallDirection );
                               
        void TeardownTelephonyAudio( const TName& aCallName, TInt aError );
        
        void CallStateChange( const TName& aCallName, 
    		                  RMobileCall::TMobileCallStatus aCallStatus );
    
    private:
        CMockTelephonyAudioControl( MTelephonyAudioControlCallback* aTelephonyAudioControlCallback );
        void ConstructL();
    
    private:
        RArray<TCallInfo> iCallInfoArray;
        MTelephonyAudioControlCallback* iTelephonyAudioControlCallback;
    };

#endif // MOCKTELEPHONYAUDIOCONTROL_H

