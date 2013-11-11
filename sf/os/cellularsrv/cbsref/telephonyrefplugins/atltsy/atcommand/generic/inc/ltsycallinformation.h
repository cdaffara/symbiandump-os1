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
// Contributors:
//
// Description:
// This contains TLtsyCallInformation which store baseband related call information
// 

#ifndef LTSYCALLINFORMATION_H_
#define LTSYCALLINFORMATION_H_

//system include
#include <e32base.h>
#include <etelmm.h>

//user include

class TLtsyCallInformation
	{
public:
	/**
	 * 
	 * 
	 */
	TLtsyCallInformation();
	
	/**
	 *Call id is used or not 
	 * 
	 */
	enum TCallIdUsedInfo
		{
		EUsed,
		ENotUsed
		};
	
	/**
	 * Direction of the call
	 * 
	 */
	enum TDirectionCall
		{
		EUnknownDirection = -1,	//Don't know
		EMoCall = 0, 	//Mobile originated(MO) call
		EMtCall = 1   	//Moblie teminated(MT) call
		};

	/**
	 * State of the call
	 * 
	 */
	enum TCallState
		{
		EUnknownState = -1,   //Don't know call state
		EActiveCall = 0,	  //Active call
		EHeldCall = 1,        //Hold call
		EDialingCall = 2,     //Dialing Call(MO call)
		EAlertingCall = 3,    //Alerting Call(MO call)
		EInComingCall = 4,    //InComing call(MT call)
		EWaitingCall = 5      //Waiting call(MT call)
		};
	
	/**
	 * Teleservice
	 * 
	 */
	enum TCallMode
		{
		EVoiceCall  = 0,
		EDataCall   = 1,
		EFaxCall    = 2,
		EUnKnowCall = 9
		};	
	
	/**
	 * Multiparty(Conference)
	 * 
	 */
	enum TConferenceCall
		{
		EUnknownIsConference = -1, //Don't know
		ENotConference = 0,		   //Not conference
		EIsConference  = 1         //Conference
		};
	
public:
	/**
	 * 
	 */
	void SetCallId(TInt aCallId);
	
	/**
	 * 
	 * 
	 */
	TInt GetCallId() const;
	
	void SetCallIdIsUsedInfo(TCallIdUsedInfo aUsedInfo);
	TCallIdUsedInfo GetCallIdIsUsedInfo() const;
	
	void SetCallDirection(TDirectionCall aDirection);
	TDirectionCall GetCallDirection() const;
	
	void SetCallState(TCallState aCallState);
	TCallState GetCallState() const;
	
	void SetCallMode(TCallMode aCallMode);
	TCallMode GetCallMode() const;
	
	void SetConferenceCall(TConferenceCall aConferenceCall);
	TConferenceCall GetConferenceCall() const;
	
	void SetMobileTelNum(const TDesC8& aTelNum);
	const TDesC8& GetMobileTelNum() const;
	
	void SetAddressType(TInt aAddressType);
	TInt GetAddressType() const;
	
	void SetMobileName(const TDesC8& aMobileName);
	const TDesC8& GetMobileName() const;
	
	void SetEmergencyCallFlag(TBool aIsEmergency);
	TBool GetEmergencyCallFlag() const;
	
private:
	
	/**
	 * Call id
	 * 
	 */
	TInt iCallId;
	
	/**
	 * Call id used info
	 * 
	 */
	TCallIdUsedInfo iUsedInfo;
	
	/**
	 * Direction of the call
	 * 
	 */
	TDirectionCall iDirection;
	
	/**
	 * State of the call
	 * 
	 */
	TCallState iCallState;
	
	/**
	 * Teleservice
	 * 
	 */
	TCallMode iCallMode;
	
	/**
	 * Multiparty(Conference)
	 * 
	 */
	TConferenceCall iIsConference;
	
	/**
	 * Telephone num
	 * 
	 */
	TBuf8<RMobilePhone::KMaxMobileTelNumberSize> iTelNum; 
	
	/**
	 * 
	 * 
	 */
	TInt iAddressType;
	
	/**
	 * Moblie name
	 * 
	 */
	TBuf8<RMobilePhone::KMaxMobileNameSize> iMobileName;
	
	/**
	 * Emergency call
	 * 
	 */
	TBool iIsEmergencyCall;
	
	};

#endif /*LTSYCALLINFORMATION_H_*/
