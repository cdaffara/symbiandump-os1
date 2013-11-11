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
// Class used by psy test suites to do common tasks such as restarting LBS.
// 
//

#if (!defined __TE_LBSPSYUTILS__)
#define __TE_LBSPSYUTILS__

// Epoc includes
#include <e32base.h>
#include <e32cmn.h>
#include <flogger.h>
#include <f32file.h>

#include "lbsqualityprofile.h"
#include "LbsExtendModuleInfo.h"

const TInt KMaxInfoModules = 5;
// LBS includes
#include <lbs.h>

class CTe_SystemStarter
	{
public:	
	IMPORT_C CTe_SystemStarter(TUid iServerId);
	IMPORT_C ~CTe_SystemStarter();	
	
	IMPORT_C void RestartLbs_RootOnlyL(TBuf8<KMaxFileName>& aFirstExe);
	IMPORT_C void RestartLbs_NormalL(TBuf8<KMaxFileName>aFirstExe);
	IMPORT_C static void DefineLbsPropertiesL();
	IMPORT_C static void DeleteLbsPropertiesL();
	
private:
	static void InitializeQualityProfileInfoL();
	static void GetQualityProfileInfoL(RFs& aFs, RArray<TQualityProfile>& aQualityArray,
	                                      const TDesC& aFileName);

	static void GetModuleInfoL(RFs& aFs, TPositionModuleInfoExtended& aExtendedModuleInfo, 
	                                          const TDesC& aFileName);
	static void InitializeModuleInfoL();
	
private:
	TUid iServerId;
	//TBuf8<KMaxFileName> iFirstExe;	
	};
		
/* Portions Copyright (c) Symbian Software Ltd 2007.
 * Portions Copyright (c) 2005 Nokia * Nokia Core OS *
 * Save as expressly licensed to you by Symbian Software Ltd, all rights reserved.
 */


// LOGGING MACROS
#ifdef _DEBUG
_LIT( KLoggingFolder, "te_psy" );
_LIT( KLoggingFile, "log.txt" );
_LIT( KLoggingFullName, "c:\\logs\\te_psy\\log.txt");

#define CLEARTRACELOG \
    {\
    RFs fs; \
    User::LeaveIfError(fs.Connect());\
    CleanupClosePushL(fs);\
    CFileMan* fileMan=CFileMan::NewL(fs); \
    fileMan->Delete(KLoggingFullName); \
    delete fileMan; \
    CleanupStack::PopAndDestroy(&fs);\
    }
    
#define TRACETEXT( S1 )         { RFileLogger::Write( KLoggingFolder(),\
KLoggingFile(), EFileLoggingModeAppend, S1 ); }

#define TRACESTRING( S1 )       { _LIT( tempLogDes, S1 ); RFileLogger::Write(\
KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend, tempLogDes()); }

#define TRACESTRING2( S1, S2 )  { _LIT( tempLogDes, S1 ); RFileLogger::\
WriteFormat( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend,\
TRefByValue<const TDesC>( tempLogDes()), S2 ); }


#define TRACESTRING8_old( S1 )          { _LIT8( tempLogDes, S1 ); RFileLogger::\
Write( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend, tempLogDes());}

#define TRACESTRING8( S1 )          { RFileLogger::\
Write( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend, S1);}

#define TRACESTRING28( S1, S2 )     { _LIT8( tempLogDes, S1 ); RFileLogger::\
WriteFormat( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend,\
TRefByValue<const TDesC8>( tempLogDes()), S2 ); }

#define TRACESTRING38( S1, S2, S3 ) { _LIT8( tempLogDes, S1 ); RFileLogger::\
WriteFormat( KLoggingFolder(), KLoggingFile(), EFileLoggingModeAppend,\
TRefByValue<const TDesC8>( tempLogDes()), S2, S3 ); }


#else   // _DEBUG
#define CLEARTRACELOG 
#define TRACETEXT( S1 )
#define TRACESTRING( S1 )
#define TRACESTRING2( S1, S2 )
#define TRACESTRING8( S1 )
#define TRACESTRING28( S1, S2 )
#define TRACESTRING38( S1, S2, S3 )
#endif  // _DEBUG



#endif //__TE_LBSPSYUTILS__
