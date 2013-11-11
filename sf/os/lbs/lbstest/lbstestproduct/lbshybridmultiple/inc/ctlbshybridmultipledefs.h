/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CT_LBS_HYBRID_MULTIPLE_DEFS_H__
#define __CT_LBS_HYBRID_MULTIPLE_DEFS_H__

/** ID of the different protocol modules used in the test */
const TUid KPm1RequestUid = {0x10285AB7};	//Extended Hybrid PM 1
//const TUid KRequestUid = {0x10282293};	//Extended Hybrid PM 2		

/** Enum for the privacy request call back messages*/
enum TPrivacyControllerTestMessages
	{
	EProcessNetworkLocationRequest = 3000,
	EProcessNetworkPositionUpdate,
	EProcessRequestComplete
	};

/** Enum for the Self Location messages*/
enum TSelfLocateTestMessages
	{
	ERequestSelfLocation = 4000,
	ENotifyPositionUpdate = 5000,
	EGetLastKnownLocation
	};

/** Enum for the X3P messages*/
enum TX3PTestMessages
	{
	ETransmitPosition = 6000,
	EX3PRefPositionUpdate = 7000,
	EX3PPositionUpdate
	};

/** */
enum TCombinedTestMessages
	{
	ENetTestMsgEndSession = 1100,
	ENetTestMsgProcessSessionCompleteError 
	};

/** 
 */
class MHybridMultipleTestCallBack
	{
public:
	virtual void StopTest(TInt aError) = 0;
	virtual void LogTestStatement(const TDesC& aMessage) = 0;
	virtual void SequenceFinished() = 0;
	virtual void SignalCheckForNewSequences() = 0;
	};

#endif //__CT_LBS_HYBRID_MULTIPLE_DEFS_H__
