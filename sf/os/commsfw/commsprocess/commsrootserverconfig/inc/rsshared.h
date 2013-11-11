// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__RSSHARED_H__)
#define __RSSHARED_H__

#include <rscommon.h>

IMPORT_C TInt StartC32();		// Call this to start the Comms Process Rootserver
IMPORT_C TInt StartC32WithCMISuppressions(const TDesC& aCMISuppressionList);
IMPORT_C TInt StartC32Full();		// Call this to start the Comms Process Rootserver

IMPORT_C TInt WarmBootC32(const TDesC& aCMISuppressionList);

namespace RootServer
/** Namespace encapsulating the Rootserver components
@publishedPartner
@released
*/
{

/** The server name of the Rootserver in EKA2 case.
@internalComponent
*/
_LIT(KRootServerName, "!RootServer");


/** The Root Server library.
@internalComponent
*/
_LIT(KRootServerDLLName, "c32rootsrv.dll");


/** The example Comms Configurator.
@internalComponent
*/
_LIT(KCommC32StartExe, "C32start.exe");

/** Major version number of the Rootserver.
  @publishedPartner
  @released
  */
const TUint KRS32MajorVersionNumber=1;
  
/** Minor version number of the Rootserver
  @publishedPartner
  @released
 */
const TUint KRS32MinorVersionNumber=1;
  
/** Build version number of the Rootserver.
  @publishedPartner
  @released
 */
const TUint KRS32BuildVersionNumber=1;

/** Unique UID of the C32 Start Process.
@internalComponent
*/
const TInt32 KUidC32StartProcess=0x101F7988;

/** Key to which C32Start() publishes the number of modules that have died
@publishedPartner
@released
*/
const TUid KUidC32RootModuleDeathKey = {0x102045de};



/** Key to which C32Start() publishes the TRSStartState changes.
@publishedPartner
@released
*/
const TUid KUidC32StartPropertyKey = {0x102045dd};


const TUid KUidCommsProcess={0x101F7989};

/** Comms specific RootServer IPC numbers/messages
@internalComponent
*/
enum TC32RSMessages
	{
	RSSetMBufPoolSize = 101,
	RSAddMBufAllocInfo = 102,
	RSDbgFailNextMbuf = 103,
	RSDbgSetMbufPoolLimit = 104,
	RSDbgCheckMbuf = 105,
	RSDbgMbufFreeSpace = 106,
	RSDbgMbufTotalSpace = 107
	};



class TRSSetMBufMnrContainer
/** Parameters for startup information for the MBufManager
@internalTechnology
*/
	{
	public:
	TUint iInitSize;
	TUint iMaxSize;
	};


/** Descriptor Pckg for MBufManager startup paramters
@internalTechnology
*/
typedef TPckgBuf<TRSSetMBufMnrContainer> TRSSetMBufMnr;

/** Parameters for MBufManager's mbuf size alloc info
@internalTechnology
*/
class TRSAddMBufAllocInfoContainer
	{
	public:
	TUint iMBufSize;
	TUint iInitialAllocation;
	TUint iMinGrowth;
	TUint iGrowthThreshold;
#ifdef SYMBIAN_ZERO_COPY_NETWORKING
	TUint iPoolCeiling;
#endif // SYMBIAN_ZERO_COPY_NETWORKING
	};

/** Descriptor Pckg for MBufManager alloc size info paramters
@internalTechnology
*/
typedef TPckgBuf<TRSAddMBufAllocInfoContainer> TRSAddMBufAllocInfo;


} // namespace RootServer

#endif // __RSSHARED_H__

