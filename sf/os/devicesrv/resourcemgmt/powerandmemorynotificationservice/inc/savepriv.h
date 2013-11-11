// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(SAVEPRIV_H)
#define SAVEPRIV_H


_LIT(__SHUTDOWN_SERVER_NAME,"!ShutdownServer");
_LIT(__TEST_SHUTDOWN_SERVER_NAME,"!TestShutdownServer");

/**
@internalTechnology
*/
const TInt KShutdownMajorVN			=6;
const TInt KShutdownMinorVN			=0;
const TInt KShutdownBuildVN			=1;
const TInt KShutdownMessageSlots	=1;

/**
@internalTechnology
*/
enum TSaveNotifierOpCode
	{
	TSaveOpCodeNotify,
	TSaveOpCodeNotifyCancel,
	TSaveOpCodeHandleError,
	TSaveOpCodePowerOff,        //Supported only in special test debug build.
	TSaveOpCodeQueryPowerState  //Supported only in special test debug build.
	};

#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
/**
Opcodes for handling shutdown event from ssm/ssmutilsrv/sup for shutdown.
This is required for sup (running in ssmutilsrv.exe) to interact with shutdownsrv.dll (running in shutdownsrvs.exe)

@internalTechnology
*/
enum TEventObserverAdaptorOpCode
	{
	EEventObsAdaptHandleShutdown = TSaveOpCodeQueryPowerState + 1,	//handle shutdown event
	EEventObsAdaptClientArrayCount,	//client array count
	EEventObsAdaptClientArray,		//client array
	EEventObsAdaptIsClientHung,		//is client hung
	EEventObsAdaptGetShutdownState	//information about the shutdown status
	};
#endif // SYMBIAN_SSM_GRACEFUL_SHUTDOWN

/**
@internalTechnology
*/
struct TErrorAndClientId
	{
	TInt iError;
	TUint iClientId;
	};

//
// TShutdownServerStart
//
/**
@internalTechnology
*/
class TShutdownServerStart
	{
public:
	TShutdownServerStart(TRequestStatus& aStatus);
	TPtrC AsCommand() const;
//
	TShutdownServerStart();
	TInt GetCommand();
	void SignalL();
private:
	TThreadId iId;
	TRequestStatus* iStatus;
	};


#endif
