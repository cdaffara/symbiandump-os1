// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32std.h>
#include <bautils.h>
#include <barsc.h>
#include <centralrepository.h>

#include "BTGPSConstantsManager.h"
#include "BtGpsPsyPrivateCRKeys.h"

//Resource definitions
#include <nlabtgpspsyconstants.rsg>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
// Resource file name
_LIT(KResourceFileName, "\\resource\\nlabtgpspsyconstants.*");

// Minimum send delay
const TInt KMinimumSendDelay = 0;

// Maximum send delay
const TInt KMaximumSendDealy = 60000000; //1min

// Minimum user eqivalent range error
const TReal KMinimumUserEquivalentRangeError = 0.0;

// Maximum user eqivalent range error
const TReal KMaximumUserEquivalentRangeError = 1000.0;

// Minimum Number Of Test Nmea Sentences
const TInt KMinimumNumberOfTestNmeaSentences = 0;

// Maximum Number of Test Nmea Sentences
const TInt KMaximumNumberOfTestNmeaSentences = 100000;

// Minimum Delay Between Gps Init Messages
const TInt KMinimumDelayBetweenGpsInitMessages = 0;

// Maximum Delay Between Gps Init Messages
const TInt KMaximumDelayBetweenGpsInitMessages = 600000000; //10min

// Minimum Number Of Set Full Power Message
const TInt KMinimumNumberOfSetFullPowerMessage = 0;

// Maximum Number Of Set Full Power Message
const TInt KMaximumNumberOfSetFullPowerMessage = 100000;

// Minimum Delay For PNok Gps Detect 
const TInt KMinimumDelayForPNokGpsDetect = 0;

// Maximum Delay For PNok Gps Detect
const TInt KMaximumDelayForPNokGpsDetect = 600000000; //10min

// Minimum Inactivity Delay
const TInt KMinimumInactivityDelay = 0;

// Maximum Inactivity Delay
const TInt KMaximumInactivityDelay = 600000000; //10min

// Minimum Satellite Info Life Time
const TInt KMinimumSatelliteInfoLifeTime = 0;

// Maximum Satellite Info Life Time
const TInt KMaximumSatelliteInfoLifeTime = 600000000; //10min

// Minimum NMEA data buffer
const TInt KMinimumNmeaDataBuffer = 0;

// Maximum NMEA data buffer
const TInt KMaximumNmeaDataBuffer = 50*1024; //50k


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::NewL
// -----------------------------------------------------------------------------
CBTGPSConstantsManager* CBTGPSConstantsManager::NewL()
    {
    CBTGPSConstantsManager* self = new (ELeave) CBTGPSConstantsManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::~CBTGPSConstantsManager
// -----------------------------------------------------------------------------
CBTGPSConstantsManager::~CBTGPSConstantsManager()
    {
    delete iBatteryLowDialogText;
	delete iBatteryFullDialogText;
    delete iExtAntennaConnectedDialogText;
    delete iExtAntennaDisconnectedDialogText;
    delete iExtPowerConnectedDialogText;
    delete iExtPowerDisconnectedDialogText;
    }

// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSConstantsManager::ConstructL()
    {
    //File session for resource file
    RFs fileSession;

    //Open file session
    User::LeaveIfError(fileSession.Connect());
    CleanupClosePushL(fileSession);

    TFileName* localisedRscName = GetLocalisableResourceNameL(fileSession);
    CleanupStack::PushL(localisedRscName);

	//Construct RResourceFile
	RResourceFile localisationResourceFile;
    localisationResourceFile.OpenL(fileSession, *localisedRscName);
    CleanupClosePushL(localisationResourceFile);

    //Check the signature
    localisationResourceFile.ConfirmSignatureL(0);
    
    //Read constants from resource file
    iBatteryLowDialogText = GetTextL(
        localisationResourceFile, 
        R_BATTERY_LOW);
		
    iBatteryFullDialogText = GetTextL(
        localisationResourceFile, 
        R_BATTERY_FULL);
		
    iExtAntennaConnectedDialogText = GetTextL(
        localisationResourceFile, 
        R_EXT_ANTENNA_CONNECTED);
		
    iExtAntennaDisconnectedDialogText = GetTextL(
        localisationResourceFile, 
        R_EXT_ANTENNA_DISCONNECTED);
		
    iExtPowerConnectedDialogText = GetTextL(
        localisationResourceFile, 
        R_EXT_POWER_CONNECTED);
		
    iExtPowerDisconnectedDialogText = GetTextL(
        localisationResourceFile,
        R_EXT_POWER_DISCONNECTED);
        
    //Get int constant
    GetIntL();
    
    CleanupStack::PopAndDestroy(&localisationResourceFile);
    CleanupStack::PopAndDestroy(localisedRscName);
    CleanupStack::PopAndDestroy(&fileSession);
    
    }

// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::CBTGPSConstantsManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSConstantsManager::CBTGPSConstantsManager()
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::GetLocalisableResourceNameL
// -----------------------------------------------------------------------------
TFileName* CBTGPSConstantsManager::GetLocalisableResourceNameL(RFs& aFs)
    {
	TFileName* fileName = new (ELeave) TFileName;
	CleanupStack::PushL(fileName);
	
	TFindFile findFile(aFs);
	CDir* dir;
	User::LeaveIfError(findFile.FindWildByDir(
	    KResourceFileName, 
	    KNullDesC(), 
	    dir));
	CleanupStack::PushL(dir);
	TParse fileNameParser;
	fileNameParser.Set(findFile.File(), NULL, NULL);
	
	fileName->Copy(fileNameParser.DriveAndPath());
	fileName->Append((*dir)[0].iName);
	CleanupStack::PopAndDestroy(dir);
	
	BaflUtils::NearestLanguageFile(aFs, *fileName);
	
	CleanupStack::Pop(fileName);
	return fileName;
    }

// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::GetIntL
// -----------------------------------------------------------------------------
void CBTGPSConstantsManager::GetIntL()
    {
    //Read constants from CenRep
    CRepository* settings = CRepository::NewL(KCRUidBtGpsPsy );
    CleanupStack::PushL(settings);
    
    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyMsgSendDelay, 
        iSendDelay));
    CheckRangeL(iSendDelay,KMinimumSendDelay,KMaximumSendDealy);

    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyUserEquivalentRangeError, 
        iUserEquivalentRangeError));
    CheckRangeL(
        iUserEquivalentRangeError,
        KMinimumUserEquivalentRangeError,
        KMaximumUserEquivalentRangeError);
    
    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyNumTestNmeaSentences, 
        iNumberOfTestNmeaSentences));
    CheckRangeL(
        iNumberOfTestNmeaSentences,
        KMinimumNumberOfTestNmeaSentences,
        KMaximumNumberOfTestNmeaSentences);

    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyNormalGpsInitMsgDelay, 
        iDelayBetweenGpsInitMessages));
    CheckRangeL(iDelayBetweenGpsInitMessages,
        KMinimumDelayBetweenGpsInitMessages,
        KMaximumDelayBetweenGpsInitMessages);

    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyNumSetFullPowerMsg, 
        iNumberOfSetFullPowerMessage));
    CheckRangeL(iNumberOfSetFullPowerMessage,
        KMinimumNumberOfSetFullPowerMessage,
        KMaximumNumberOfSetFullPowerMessage);
    
    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyPnokPollingDelay, 
        iDelayForPNokGpsDetect));
    CheckRangeL(iDelayForPNokGpsDetect,
        KMinimumDelayForPNokGpsDetect,
        KMaximumDelayForPNokGpsDetect);
    
    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyInactiveDealy, 
        iInactivityDelay));
    CheckRangeL(iInactivityDelay,
        KMinimumInactivityDelay,
        KMaximumInactivityDelay);
    
    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyUtilisePowerSaving, 
        iUtilisePowerSaving));
    CheckRangeL(iUtilisePowerSaving, 0, 1);
    
    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsySateInfoLifeTime, 
        iSatelliteInfoLifeTime));
    CheckRangeL(iSatelliteInfoLifeTime, 
        KMinimumSatelliteInfoLifeTime,
        KMaximumSatelliteInfoLifeTime);
        
    User::LeaveIfError(settings->Get(
        KBluetoothGpsPsyNmeaBufferSize,
        iNmeaBufferSize));
    CheckRangeL(iNmeaBufferSize,
        KMinimumNmeaDataBuffer,
        KMaximumNmeaDataBuffer);
        
    CleanupStack::PopAndDestroy(settings);
    
    }
            
// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::GetTextL
// -----------------------------------------------------------------------------
HBufC* CBTGPSConstantsManager::GetTextL(
            RResourceFile& aResourceFile, 
            TInt aId)
    {
    HBufC8* buffer = aResourceFile.AllocReadLC(aId);
    HBufC* retBuf;
    
    //Resource parser
    TResourceReader resourceReader;
    resourceReader.SetBuffer(buffer);
    
    retBuf = resourceReader.ReadHBufCL();
    User::LeaveIfNull(retBuf);
    
    CleanupStack::PopAndDestroy(buffer);
    
    return retBuf;
    }

// -----------------------------------------------------------------------------
// CBTGPSConstantsManager::CheckRangeL
// Checks that the aValue is in range [aMin,aMax]
// -----------------------------------------------------------------------------
//
template<class T>
void CBTGPSConstantsManager::CheckRangeL(T aValue, T aMin, T aMax)
    {
    if ( aValue < aMin )
        {
        User::Leave(KErrUnderflow);
        }

    if ( aValue > aMax )
        {
        User::Leave(KErrOverflow);
        }
    }


//  End of File



