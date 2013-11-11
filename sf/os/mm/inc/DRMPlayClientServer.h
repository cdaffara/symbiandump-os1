/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DRM Play ClientServer
*
*/

#ifndef __DRMPLAYCLIENTSERVER_H
#define __DRMPLAYCLIENTSERVER_H


#include <e32base.h>
#include <mmf/common/mmfstandardcustomcommands.h>



// server name
_LIT(KDRMPlayServerName,"DRMPlayServer");
_LIT(KDRMPlayServerImg,"DRMPlayServer");		// DLL/EXE name

const TUid KDRMPlayServerUid3={0x1020381D};  

const TUid KUidCustomInterfaceBuilder = {0x10200017}; // This should match \s60\mw\mmenhs\inc\CustomInterfaceBuilderTypes.h
const TUid KUidAudioOutput = {0x10200018}; // This should match \s60\mw\mmenhs\inc\AudioOutputMessageTypes.h

#ifdef __MYSERVER_NO_PROCESSES__
const TInt KDRMPlayServerStackSize=0x2000;			//  8KB
const TInt KDRMPlayServerInitHeapSize=0x1000;		//  4KB
const TInt KDRMPlayServerMaxHeapSize=0x1000000;		// 16MB
#endif


const TUint KDRMPlayServMajorVersionNumber=0;
const TUint KDRMPlayServMinorVersionNumber=1;
const TUint KDRMPlayServBuildVersionNumber=1;



// opcodes used in message passing between client and server
enum TDRMPlayServRqst
	{
	EDRMPlayServCreate,
	EDRMPlayServNewFilePlayerL,
	EDRMPlayServNewDesPlayerL,
	EDRMPlayServNewDesPlayerReadOnlyL,
	EDRMPlayServNewPlayerL,
	EDRMPlayServOpenFile,
	EDRMPlayServOpenFileByHandle,
	EDRMPlayServOpenFileBySource,
	EDRMPlayServOpenDes,
	EDRMPlayServOpenUrl,
	EDRMPlayServPlay,
	EDRMPlayServStop,
	EDRMPlayServSetVolume,
	EDRMPlayServSetRepeats,
	EDRMPlayServSetVolumeRamp,
	EDRMPlayServDuration,
	EDRMPlayServMaxVolume,
	EDRMPlayServPause,
	EDRMPlayServClose,
	EDRMPlayServGetPosition,
	EDRMPlayServSetPosition,
	EDRMPlayServSetPriority,
	EDRMPlayServGetVolume,
	EDRMPlayServGetNumberOfMetaDataEntries,
	EDRMPlayServGetMetaDataEntry,
	EDRMPlayServClearPlayWindow,
	EDRMPlayServSetPlayWindow,
	EDRMPlayServSetBalance,
	EDRMPlayServGetBalance,
	EDRMPlayServGetBitRate,
	EDRMPlayServRegisterForAudioLoadingNotification,
	EDRMPlayServGetAudioLoadingProgress,
	EDRMPlayServControllerImplementationInformation,
	EDRMPlayServCustomCommandSyncWithReturn,
	EDRMPlayServCustomCommandSyncWithoutReturn,
	EDRMPlayServCustomCommandAsyncWithReturnStep1,
	EDRMPlayServCustomCommandAsyncWithReturnStep2,
	EDRMPlayServCustomCommandAsyncWithoutReturnStep1,
	EDRMPlayServCustomCommandAsyncWithoutReturnStep2,
	EDRMPlayServSetPriorityPreference
	};

enum TDRMPlayServLeave
{
	ENonNumericString
};


enum TAsyncCallbackState
	{
	ELoadingStarted,
	ELoadingComplete,
	EInitComplete,
	EPlayComplete,
	ECustomCommandCompleteNoData,
	ECustomCommandComplete
};

typedef TPckgBuf<TAsyncCallbackState> TAsyncCallbackStatePckgBuf;
struct TErrorDurationStruct
	{
	TInt iError;
	TTimeIntervalMicroSeconds iDuration;
	};

typedef TPckgBuf<TErrorDurationStruct> TErrorDurationStructPckgBuf;

class TDRMCustomCommand
	{
	public:
		TDRMCustomCommand(){};
		~TDRMCustomCommand(){};
		TMMFMessageDestinationPckg iDestination; 
		TInt iFunction;
		TRequestStatus iRequestStatus;
		TAsyncCallbackStatePckgBuf 	iAsyncCallback;
	};

typedef TPckgBuf<TDRMCustomCommand>	TPckgCustomCommand;

struct TDataStruct
	{
	TBuf<100> iFileName;
	TBuf8<100> iData;
	TInt iPriority;
	TMdaPriorityPreference iPref;
	};
typedef TPckgBuf<TDataStruct> TDataStructPckgBuf;

struct TUrlStruct
	{
	TBuf<100> iUrl;
	TInt iIapId;
	TBuf8<100> iMimeType;
	};
	
class TPlayWindowStruct
	{
	public:
	TTimeIntervalMicroSeconds iPlayStart;
	TTimeIntervalMicroSeconds iPlayEnd;
	};	
	
typedef TPckgBuf<TPlayWindowStruct> TPlayWindowStructBuf;

class TSetPositionStruct
	{
	public:
	TTimeIntervalMicroSeconds iPosition;
	};	
	
typedef TPckgBuf<TSetPositionStruct> TSetPositionStructBuf;




typedef TPckgBuf<TUrlStruct> TUrlStructPckgBuf;

typedef TPckg<TDesC8> TPckgDesC8;
typedef TPckg<TDesC> TPckgDesC;
typedef TPckgBuf<TInt> TPckgBufTInt;
typedef TPckg<TTimeIntervalMicroSeconds> TPckgTTimeIntervalMicroSeconds;
typedef TPckgBuf<TTimeIntervalMicroSeconds> TPckgBufTTimeIntervalMicroSeconds;
typedef TPckgBuf<TPtr8> TPckgBufTDes8;
typedef TPckg<TRequestStatus> TPckgTRequestStatus;
typedef	TPckgBuf<TUint> TPckgBufTUint;
typedef	TPckgBuf<TMMSource> TPckgBufTMMSource;


#endif

