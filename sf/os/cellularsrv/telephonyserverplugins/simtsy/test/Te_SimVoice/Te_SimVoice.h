
// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic  tests.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_SimVoice_H__
#define TE_SimVoice_H__

#include "Te_SimVoiceTestStepBase.h"

#define SIM_TSY

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineStatusIncommingCallL() from T_SimVoice
 * 
*/
class CTestLineStatusIncommingCall : public CSimVoiceTestStepBase
	{
public:
	CTestLineStatusIncommingCall() ;
	~CTestLineStatusIncommingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineNotificationsIncommingCallL() from T_SimVoice
 * 
*/
class CTestLineNotificationsIncommingCall : public CSimVoiceTestStepBase
	{
public:
	CTestLineNotificationsIncommingCall() ;
	~CTestLineNotificationsIncommingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallNotificationsIncommingCallL() from T_SimVoice
 * 
*/
class CTestCallNotificationsIncommingCall : public CSimVoiceTestStepBase
	{
public:
	CTestCallNotificationsIncommingCall() ;
	~CTestCallNotificationsIncommingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallNotificationsOutgoingCallL() from T_SimVoice
 * 
*/
class CTestCallNotificationsOutgoingCall : public CSimVoiceTestStepBase
	{
public:
	CTestCallNotificationsOutgoingCall() ;
	~CTestCallNotificationsOutgoingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineNotificationsOutgoingCallL() from T_SimVoice
 * 
*/
class CTestLineNotificationsOutgoingCall : public CSimVoiceTestStepBase
	{
public:
	CTestLineNotificationsOutgoingCall() ;
	~CTestLineNotificationsOutgoingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineStatusOutgoingCallL() from T_SimVoice
 * 
*/
class CTestLineStatusOutgoingCall : public CSimVoiceTestStepBase
	{
public:
	CTestLineStatusOutgoingCall() ;
	~CTestLineStatusOutgoingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallStatusOutgoingCallL() from T_SimVoice
 * 
*/
class CTestCallStatusOutgoingCall : public CSimVoiceTestStepBase
	{
public:
	CTestCallStatusOutgoingCall() ;
	~CTestCallStatusOutgoingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallStatusIncommingCallL() from T_SimVoice
 * 
*/
class CTestCallStatusIncommingCall : public CSimVoiceTestStepBase
	{
public:
	CTestCallStatusIncommingCall() ;
	~CTestCallStatusIncommingCall(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineInfoL() from T_SimVoice
 * 
*/
class CTestLineInfo : public CSimVoiceTestStepBase
	{
public:
	CTestLineInfo() ;
	~CTestLineInfo(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallCancelsL() from T_SimVoice
 * 
*/
class CTestCallCancels : public CSimVoiceTestStepBase
	{
public:
	CTestCallCancels() ;
	~CTestCallCancels(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineCancelsL() from T_SimVoice
 * 
*/
class CTestLineCancels : public CSimVoiceTestStepBase
	{
public:
	CTestLineCancels() ;
	~CTestLineCancels(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestMoreCallInfoL() from T_SimVoice
 * 
*/
class CTestMoreCallInfo : public CSimVoiceTestStepBase
	{
public:
	CTestMoreCallInfo() ;
	~CTestMoreCallInfo(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestCallInfoL() from T_SimVoice
 * 
*/
class CTestCallInfo : public CSimVoiceTestStepBase
	{
public:
	CTestCallInfo() ;
	~CTestCallInfo(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestPhoneInfoL() from T_SimVoice
 * 
*/
class CTestPhoneInfo : public CSimVoiceTestStepBase
	{
public:
	CTestPhoneInfo() ;
	~CTestPhoneInfo(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestDtmfL() from T_SimVoice
 * 
 */
class CTestDtmf : public CSimVoiceTestStepBase
	{
public:
	CTestDtmf() ;
	~CTestDtmf(){} ;
	virtual TVerdict doTestStepL();
	
	} ;

/**
 * This class tests call duration
 * 
 */

class CTestCallDuration : public CSimVoiceTestStepBase  
{
public:
	virtual TVerdict doTestStepL();
	TVerdict RunTestL(TBool aBRingOut);
	CTestCallDuration();
	virtual ~CTestCallDuration();

private:
	TBool RingOrDialL(TBool aBRingOut);
};

/**
 * This class tests remote hangup
 * 
 */
class CTestRemoteHangup : public CSimVoiceTestStepBase
	{
public:
	CTestRemoteHangup() ;
	virtual TVerdict doTestStepL();
	} ;

#endif// TE_SimVoice_H__

