// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Basic MMTSY Voice and Echo server tests.
// 
//

/**
 @file
*/

#ifndef TE_Voice_H__
#define TE_Voice_H__

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>

#include "TE_VoiceTestStepBase.h"

//------------------------------------------------------------------ 

/**
 * This class substitutes function TestLineInfoLL() from T_Voce
 * 
*/
class CTestLineInfo : public CTestVoiceStep
	{
public:
	CTestLineInfo() ;
	~CTestLineInfo(){} ;
	virtual enum TVerdict doTestStepL();
	
	} ;
//------------------------------------------------------------------

/**
 * This class substitutes function TestPhoneInfoL() from T_Voce
 *
*/
class CTestPhoneInfo : public CTestVoiceStep
	{
public:
	CTestPhoneInfo() ;
	~CTestPhoneInfo(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestLineStatusIncomingCallL() from T_Voce
 *
*/
class CTestLineStatusIncomingCall : public CTestVoiceStep
	{
public:
	CTestLineStatusIncomingCall() ;
	~CTestLineStatusIncomingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestLineStatusOutgoingCallL() from T_Voce
 *
*/
class CTestLineStatusOutgoingCall : public CTestVoiceStep
	{
public:
	CTestLineStatusOutgoingCall() ;
	~CTestLineStatusOutgoingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;



//------------------------------------------------------------------

/**
 * This class substitutes function TestLineNotificationsIncomingCallL() from T_Voce
 *
*/
class CTestLineNotificationsIncomingCall : public CTestVoiceStep
	{
public:
	CTestLineNotificationsIncomingCall() ;
	~CTestLineNotificationsIncomingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestLineNotificationsOutgoingCallL() from T_Voce
 *
*/
class CTestLineNotificationsOutgoingCall : public CTestVoiceStep
	{
public:
	CTestLineNotificationsOutgoingCall() ;
	~CTestLineNotificationsOutgoingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestLineCancelsL() from T_Voce
 *
*/
class CTestLineCancels : public CTestVoiceStep
	{
public:
	CTestLineCancels() ;
	~CTestLineCancels(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestCallInfoL() from T_Voce
 *
*/
class CTestCallInfo : public CTestVoiceStep
	{
public:
	CTestCallInfo() ;
	~CTestCallInfo(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestCallNotificationsIncomingCallL() from T_Voce
 *
*/
class CTestCallNotificationsIncomingCall : public CTestVoiceStep
	{
public:
	CTestCallNotificationsIncomingCall() ;
	~CTestCallNotificationsIncomingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestCallNotificationsOutgoingCallL() from T_Voce
 *
*/
class CTestCallNotificationsOutgoingCall : public CTestVoiceStep
	{
public:
	CTestCallNotificationsOutgoingCall() ;
	~CTestCallNotificationsOutgoingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestCallStatusIncomingCallL() from T_Voce
 *
*/
class CTestCallStatusIncomingCall : public CTestVoiceStep
	{
public:
	CTestCallStatusIncomingCall() ;
	~CTestCallStatusIncomingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestCallStatusOutgoingCallL() from T_Voce
 *
*/
class CTestCallStatusOutgoingCall : public CTestVoiceStep
	{
public:
	CTestCallStatusOutgoingCall() ;
	~CTestCallStatusOutgoingCall(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestCallCancelsL() from T_Voce
 *
*/
class CTestCallCancels : public CTestVoiceStep
	{
public:
	CTestCallCancels() ;
	~CTestCallCancels(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestMoreCallInfoL() from T_Voce
 *
*/
class CTestMoreCallInfo : public CTestVoiceStep
	{
public:
	CTestMoreCallInfo() ;
	~CTestMoreCallInfo(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------

/**
 * This class substitutes function TestCloseCallWhenActiveL() from T_Voce
 *
*/
class CTestCloseCallWhenActive : public CTestVoiceStep
	{
public:
	CTestCloseCallWhenActive() ;
	~CTestCloseCallWhenActive(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------
//------------------------------------------------------------------

//SUBSTITUTE CLASS REPLACEMENT FOR DEFECT FIX DEF010965 
//Class was missing when ported from T_Voice
/**
* This class substitutes function TestLineCapabilitiesL() from T_Voce
*
*/
class CTestLineCapabilities : public CTestVoiceStep
	{
public:
	CTestLineCapabilities() ;
	~CTestLineCapabilities(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------
//------------------------------------------------------------------

/**
 * This class substitutes function TestCancelDialRequestL() from T_Voce
 *
*/
class CTestCancelDialRequest : public CTestVoiceStep
	{
public:
	CTestCancelDialRequest() ;
	~CTestCancelDialRequest(){} ;
	virtual enum TVerdict doTestStepL();

	} ;

//------------------------------------------------------------------


#endif// TE_Voice_H__
