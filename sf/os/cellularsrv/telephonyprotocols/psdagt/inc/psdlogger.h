/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header for PSD-specific logging class
* 
*
*/



/**
 @file PsdLogger.h
 @internalComponent
*/

#ifndef __PSDLOGGER_H__
#define __PSDLOGGER_H__


#include <comms-infras/eventlogger.h>
#include <etelpckt.h>
#include <comms-infras/cagentsmbase.h>

const TInt KRegularUpdateInterval = 60000000;	///< 1 minute in microseconds

class MPsdEnv;
class MAgentObserver;

class CDataLogger: public CBase
/**
Class responsible for logging data volume transfer details to the log file

@internalComponent
*/
	{
public:
	static CDataLogger* NewL(MPsdEnv* aPsdSM,MAgentNotify* aObserver);
	~CDataLogger();
	void Start();
	void StopL();
private:
	CDataLogger(MPsdEnv* aPsdSM,MAgentNotify* aObserver);
	TBool IsVolumeChanged(RPacketContext::TDataVolume& aNewVolume) const;
	static TInt Update(TAny* aContext);
	void RegularUpdateL();
	void LogData();
	void ConstructL();
private:
	MPsdEnv* iSM;
	MAgentNotify* iSMObserver; ///< for access to Notification, Agt->Nif
	CPeriodic* iRegularUpdater;
	RPacketContext::TDataVolume iVolumeTransferred;
	};


#endif
