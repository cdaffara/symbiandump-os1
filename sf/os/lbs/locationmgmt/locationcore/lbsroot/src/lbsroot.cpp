// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Global entry point for the LBS Network Request Handler
// 
//

/**
 @file
 @internalTechnology
 @released
*/
#include <e32base.h>
#include <e32std.h>
#include <e32test.h>
#include <e32def.h>
#include <bautils.h>
#include <f32file.h>
#include <bacline.h> // For CCommandLineArguments
#include <centralrepository.h>
#ifdef SYMBIAN_FEATURE_MANAGER
	#include <featdiscovery.h>
	#include <featureuids.h>
#endif
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "lbsdevloggermacros.h"
#include "LbsInternalInterface.h"
#include "lbsnetinternalapi.h"
#include "lbsassistancedatacacheapi.h"
#include "lbsngnlmint.h"
#include "lbsreffnpint.h"
#include "lbsnetregstatusint.h"
#include "lbsroot.h"
#include "lbsrootcenrepdefs.h"
#include "lbsmoduleinfoparser.h"
#include "tserverstartparams.h"
#include "cserverlaunch.h"
#include "cprocesslaunch.h"
#include "csecureprocessasbase.h"
#include "LbsExtendModuleInfo.h"
#include "lbsqualityprofile.h"
#include "lbssuplpushprops.h"
#include "lbspositioningstatusprops.h"
#if defined(_DEBUG)
#include "LbsOomTest.h"
#endif


// Maximum number of times that LbsRoot will try to re-start the system
// if a process panics when it is running.
//
// Note: LbsRoot will not try to re-start the Lbs system if a process panics
//       while it is starting up. Instead it will return an error code to the 
//       process that requested the startup.
const TInt KMaxSystemStartRetries = 3;

// Time to wait for each process to close down
const TTimeIntervalMicroSeconds32 KLbsProcessCloseTimeout = 10000000;

const TInt KSecToMicroSecTransfer = 1000*1000;

// Locations and paths used when parsing .ini files
_LIT(KLbsDir, "lbs");
_LIT(KModuleIniDir, "locmods");
_LIT(KIniWildCard, "*.ini");
_LIT(KQualityProfileIniName, "lbsprofile.ini");


//
// Server Framework boilerplate code
//

CBase* CSecureProcessASBase::CreateRootObjectL()
	{
	CLbsSystem* lbsSystem = CLbsSystem::NewL();
	return lbsSystem;
	}

TInt CSecureProcessASBase::EntryPointL(CBase* /*aBaseObj*/)
	{
	// EntryPointL not used by manager
	return KErrNone;
	}
	
// must be static	
void CSecureProcessASBase::Final()
	{
	}

//
// CLbsSystem
//

void CLbsSystem::GetModuleInfoL(RFs& aFs, TPositionModuleInfoExtended& aExtendedModuleInfo, 
                                          const TDesC& aFileName)
    {
	LBSLOG(ELogP1, "CLbsSystem::GetModuleInfoL()\n");
    TPositionQuality posQuality;
    
    // Check the file is present.
	TUint att;
	TInt err = aFs.Att(aFileName, att);
	if(err != KErrNone)
		{
		LBSLOG_ERR(ELogP3, "Leave in CLbsSystem::GetModuleInfoL() - file not found\n");
		User::Leave(err);
		}

    CLbsIniFileReader* fileReader = CLbsIniFileReader::NewL(aFs, aFileName);
	CleanupStack::PushL(fileReader);

	fileReader->FindNextSection();

    fileReader->ReadPosQualityCriteria(posQuality);

    TPositionModuleInfo tempPositionModuleInfo;
   	TUint8* endAddr2 = Mem::Copy(&aExtendedModuleInfo.iPositionModuleInfoBytes[0], 
   								&tempPositionModuleInfo, 
   								sizeof(TPositionModuleInfo));

    
    TPositionModuleInfo* posModeInfo =  (TPositionModuleInfo*) &aExtendedModuleInfo.iPositionModuleInfoBytes[0];
    
    posModeInfo->SetPositionQuality(posQuality);
 
    fileReader->ReadModuleInfoL(*posModeInfo);
    fileReader->ReadPlugInUid(aExtendedModuleInfo.iDataSourcePluginId);
    fileReader->ReadExecutableNameL(aExtendedModuleInfo.iExecutableName);
    fileReader->ReadDeviceCapabilities(aExtendedModuleInfo.iDeviceGpsModeCapabilities);
 
    aExtendedModuleInfo.iModuleId = posModeInfo->ModuleId();

	CleanupStack::PopAndDestroy(fileReader);
    }
    
void CLbsSystem::InitializeModuleInfoL()
    {	
	LBSLOG(ELogP1, "CLbsSystem::InitializeModuleInfoL()\n");

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    TBuf<32> privatePath;
    User::LeaveIfError(fs.PrivatePath(privatePath));

	TParse parse;
	parse.Set(privatePath, NULL, NULL);
	parse.AddDir(KLbsDir);
	parse.AddDir(KModuleIniDir);
	
	TFindFile findFile(fs);
	TInt moduleCount = 0;
	CDesCArray* nameArray = new(ELeave) CDesCArrayFlat(KMaxInfoModules);
	CleanupStack::PushL(nameArray);
	CDir* dir = NULL;
	TInt err = findFile.FindWildByDir(KIniWildCard, parse.Path(), dir);
	while (err == KErrNone)
		{
		CleanupStack::PushL(dir);
		TInt fileCount = dir->Count();

		// Add each module to array
		for (TInt i = 0; i < fileCount; i++)
		    {
			// Extract the data and put it in the moduleInfo....
			// First work out the full path and name for this file
			parse.Set((*dir)[i].iName, &findFile.File(), NULL);
			// Check if module already exists
			TInt nameIndex;
			if (nameArray->Find(parse.Name(), nameIndex) != 0)
				{
		        // Now extract the data into the module info instance 
				GetModuleInfoL(fs, iModuleInfoArray[moduleCount], parse.FullName());
				nameArray->AppendL(parse.Name());
				moduleCount++;
				if (moduleCount > KMaxInfoModules)
					{
					LBSLOG_ERR(ELogP3, "Leave in CLbsSystem::InitializeModuleInfoL() - too many files found\n");
					User::Leave(KErrTooBig);
					}
				}
		    }
		CleanupStack::PopAndDestroy(dir);
		dir = NULL;
		// Find next file
		err = findFile.FindWild(dir);	
		}
	CleanupStack::PopAndDestroy(nameArray);
	CleanupStack::PopAndDestroy(&fs);
    
	const TPositionModuleInfoExtended* moduleInfoArray[] = 
	    {
		&iModuleInfoArray[0],
		&iModuleInfoArray[1],
		&iModuleInfoArray[2],
		&iModuleInfoArray[3],
		&iModuleInfoArray[4],
		&iModuleInfoArray[5]
	    };

    
    LbsModuleInfo::InitializeL(moduleInfoArray, moduleCount);

    }

/* Read in quality profile information from the given .ini file.

The file will likely have multiple quality profiles in it, so
each one must be read in and stored in the given array

@param aFile File to parse for quality profile information
@param aQualityArray Array in which to store the parsed quality profile info.
*/
void CLbsSystem::GetQualityProfileInfoL(RFs& aFs, 
		    							RArray<TQualityProfile>& aQualityArray,
		    							const TDesC& aFileName)
	{
	LBSLOG(ELogP1, "CLbsSystem::GetQualityProfileInfoL()\n");
    
    // Check the file is present.
	TUint att;
	TInt err = aFs.Att(aFileName, att);
	if(err != KErrNone)
		{
		LBSLOG_ERR(ELogP3, "Leave in CLbsSystem::GetQualityProfileInfoL() - file not found\n");
		User::Leave(err);
		}

	// Open the .ini file reader
    CLbsIniFileReader* fileReader = CLbsIniFileReader::NewL(aFs, aFileName);
	CleanupStack::PushL(fileReader);

	_LIT(KKeyProfileID, "ProfileID");
	_LIT(KKeyMaxTime, "MaxTime");
	_LIT(KKeyHorizontalAccuracy, "HorizontalAccuracy");
	_LIT(KKeyVerticalAccuracy, "VerticalAccuracy");
	_LIT(KKeyMeasurementInitialTime, "MeasurementInitialTime");
	_LIT(KKeyMeasurementIntermediateTime, "MeasurementIntermediateTime");
	_LIT(KFinalNetPositionLag, "FinalNetPositionLag");
	
	// Read in all the quality profile info, section by section.
	while (fileReader->FindNextSection())
		{
		TInt result;
		TBool foundVar = fileReader->FindVar(KKeyProfileID, result);
		if (foundVar)
			{
			TQualityProfile qualityProfile;
#ifdef _DEBUG		
			// Validate the Profile ID to ensure not a duplicate
			// (UDEB builds only)
			TInt count = aQualityArray.Count();
			for (TInt i = 0; i<count; i++)
				{
				if (result == aQualityArray[i].QualityProfileId())
					{
					// duplicate ID
					LBSLOG_ERR(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - Duplicate Profile ID\n");
					User::Panic(KLbsRootPanic, ELbsPanicInvalidProfileIniFile);
					}
				}
#endif // _DEBUG
			
			// Set the profile Id
			qualityProfile.SetQualityProfileId(result);
			
			// Set the request quality
			foundVar = fileReader->FindVar(KKeyMaxTime, result);
			if (foundVar)
				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetMaxFixTime(microseconds);
				}

			TReal32 resultReal32;
			foundVar = fileReader->FindVar(KKeyHorizontalAccuracy, resultReal32);
			if (foundVar)
				{
				qualityProfile.SetMinHorizontalAccuracy(resultReal32);
				}

			foundVar = fileReader->FindVar(KKeyVerticalAccuracy, resultReal32);
			if (foundVar)
				{
				qualityProfile.SetMinVerticalAccuracy(resultReal32);
				}

			foundVar = fileReader->FindVar(KKeyMeasurementInitialTime, result);
			if (foundVar)
				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetMeasurementInitialFixTime(microseconds);
				}

			foundVar = fileReader->FindVar(KKeyMeasurementIntermediateTime, result);
			if (foundVar)
				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetMeasurementIntermediateFixTime(microseconds);
				}
			
 			foundVar = fileReader->FindVar(KFinalNetPositionLag, result);
 			if (foundVar)
 				{
				TTimeIntervalMicroSeconds microseconds = static_cast<TInt64>(result) * KSecToMicroSecTransfer;
				qualityProfile.SetFinalNetPositionLag(microseconds);
 				}
			
#ifdef _DEBUG		
			// Log the profile for UDEB builds only
 			LBSLOG_ERR2(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - Profile ID: %d\n", qualityProfile.QualityProfileId());
 			LBSLOG_ERR2(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - MinHorizontalAccuracy: %d\n", qualityProfile.MinHorizontalAccuracy());
 			LBSLOG_ERR2(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - MinVerticalAccuracy: %d\n", qualityProfile.MinVerticalAccuracy());
 			LBSLOG_ERR2(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - MaxFixTime: %d\n", qualityProfile.MaxFixTime());
 			LBSLOG_ERR2(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - MeasurementInitialFixTime: %d\n", qualityProfile.MeasurementInitialFixTime());
 			LBSLOG_ERR2(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - MeasurementIntermediateFixTime: %d\n", qualityProfile.MeasurementIntermediateFixTime());
 			LBSLOG_ERR2(ELogP3, "CLbsSystem::GetQualityProfileInfoL() - FinalNetPositionLag: %d\n", qualityProfile.FinalNetPositionLag());
#endif // _DEBUG


			// Add the quality profile to the list
			User::LeaveIfError(aQualityArray.Append(qualityProfile));
			}
		}

	CleanupStack::PopAndDestroy(fileReader);	
	}

/* Read in the quality profile information.

The quality profile data is stored in a single .ini file (which contains
one or more quality profiles).

The .ini file may reside on the system drive (C: or D:) or in the ROM (Z:).
If it is in both then the file on the system drive will take precedence
over the one in ROM.
*/	
void CLbsSystem::InitializeQualityProfileInfoL()
	{
	LBSLOG(ELogP1, "CLbsSystem::InitializeQualityProfileInfoL()\n");

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TBuf<32> privatePath;
	User::LeaveIfError(fs.PrivatePath(privatePath));

	TParse parse;
	parse.Set(privatePath, NULL, NULL);
	parse.AddDir(KLbsDir);
	
	RArray<TQualityProfile> qualityArray;
	CleanupClosePushL(qualityArray);
	
	qualityArray.ReserveL(5);

	// Only want to use the first file that is found.
	// The way TFindFile::FindByDir works, it will search
	// C: and D: before Z:, which is what we want.
	TFindFile findFile(fs);
	TInt err = findFile.FindByDir(KQualityProfileIniName, parse.Path());
	if (err == KErrNone)
		{
		GetQualityProfileInfoL(fs, qualityArray, findFile.File());
		}
	
	// Publish the quality profile info
	LbsQualityProfile::InitializeL(qualityArray);
	
	CleanupStack::PopAndDestroy(&qualityArray);
	CleanupStack::PopAndDestroy(&fs);
	}

void CLbsSystem::DefineLbsPropertiesL()
    {
	LBSLOG(ELogP1, "CLbsSystem::DefineLbsPropertiesL()\n");
    
    /*
     * Define the properties used by the Root Process API
     */
    RLbsProcessSupervisor::InitializeL();

#if defined __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC
	TBool locationManagementSupported(EFalse);
#else
#ifdef SYMBIAN_FEATURE_MANAGER
	TBool locationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
#else
	TBool locationManagementSupported(ETrue);
#endif //SYMBIAN_FEATURE_MANAGER
#endif // __WINSCW__ && defined SYMBIAN_CELLMO_CENTRIC

	/*
	 * Define the properties used by the Lbs Internal API
	 */
	if(locationManagementSupported)
		{
		InitializeModuleInfoL();
		}
    
    InitializeQualityProfileInfoL();

    RLbsPositionUpdateRequests::InitializeL();
    TPositionExtendedSatelliteInfo posSatInfo;
    RLbsPositionUpdates::InitializeL(posSatInfo);
	if(locationManagementSupported)
		{
		TPositionModuleStatus positionModuleStatus;
		RLbsModuleStatus::InitializeL(positionModuleStatus);
	    RLbsSystemModuleEvent::InitializeL();
		}
    RLbsGpsMeasurementUpdates::InitializeL();

	/*
	 * Define the properties used by the API for the 
	 * transport of assistance data.
	 */
	if(locationManagementSupported)
		{
		RAssistanceDataCache::InitializeL();
		}
    
	/*
	 * Define the properties used by the API for the 
	 * Network Request Handler <-> Network Gateway interface
	 * AGPS manager <-> Network Gateway interface.
	 */
    RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2NRH);
    RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNRH2NG);
	if(locationManagementSupported)
		{
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2AGPS);
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelAGPS2NG);
		RLbsNetworkLocationRequests::InitializeL();
		RLbsNetworkLocationResponses::InitializeL();
		}
	RLbsNetworkPositionUpdates::InitializeL();
	RLbsNetworkRegistrationStatus::InitializeL();
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2SUPLPSY);
	RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelSUPLPSY2NG);
	
	
	LbsSuplPushProps::InitializeL();

	// Initialize the NI-LR Positioning Status Property.
	// Note the MO-LR Positioning Status Property is initialized by the
	// location server
	LbsPositioningStatusProps::InitializeNiPropertyL();

#if defined(_DEBUG)   
	//Initialize the OOM Test property.
	ROomRequest::InitializeL();
#endif     
    }

void CLbsSystem::DeleteLbsProperties()
	{
	TRAP_IGNORE(DeleteLbsPropertiesL());
	}
	
void CLbsSystem::DeleteLbsPropertiesL()
    {
	LBSLOG(ELogP1, "CLbsSystem::DeleteLbsPropertiesL()\n");
    // Delete the properties defined for all the internal APIs

    // TODO: Need to decide what to do on an error - KErrNotFound is fine
    // because the property doesn't need deleting, but it's not clear
    // what other errors could occur.
    
    // Root API
    RLbsProcessSupervisor::ShutDownL();

    // LBS Internal API
    LbsModuleInfo::ShutDownL();
    RLbsPositionUpdateRequests::ShutDownL();
    RLbsPositionUpdates::ShutDownL();
    RLbsModuleStatus::ShutDownL();
    RLbsSystemModuleEvent::ShutDownL();
    LbsQualityProfile::ShutDownL();
    RLbsGpsMeasurementUpdates::ShutDownL();

    // LBS Assistance Data
    RAssistanceDataCache::ShutDownL();

    // LBS Net Internal API
    RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2NRH);
    RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNRH2NG);
    RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2AGPS);
    RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelAGPS2NG);
    RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelNG2SUPLPSY);
    RLbsNetChannel::ShutDownL(RLbsNetChannel::EChannelSUPLPSY2NG);
	RLbsNetworkLocationRequests::ShutDownL();
	RLbsNetworkLocationResponses::ShutDownL();
	RLbsNetworkPositionUpdates::ShutDownL();
	RLbsNetworkRegistrationStatus::ShutDownL();
	
#if defined(_DEBUG)	
	//Delete oom key defined in RProperty object.
	ROomRequest::ShutDownL();
#endif    
    }

TBool CLbsSystem::LbsIsPersistentL()
    {
	LBSLOG(ELogP1, "CLbsSystem::LBSIsPersistentL()\n");
	
    TInt lbsIsTransient;
    CRepository* rep;
    rep = CRepository::NewL(KLbsCenRepUid); 

    __ASSERT_ALWAYS(rep != NULL, User::Panic(KLbsRootPanic, ELbsPanicNoIniFile));
    
    CleanupStack::PushL(rep);
    rep->Get(KLbsSystemTransientKey, lbsIsTransient);
    
    CleanupStack::PopAndDestroy(rep);
    
    if(lbsIsTransient == 1)
        {
		LBSLOG(ELogP2, "CLbsSystem::LBSIsPersistentL() - LBS system is transient\n");
    	return(EFalse); // LBS system is not persistent
        }
	LBSLOG(ELogP2, "CLbsSystem::LBSIsPersistentL() - LBS system is persistent\n");
    return(ETrue);
    }

TBool CLbsSystem::LbsRootIsPersistentL()
    {
	LBSLOG(ELogP1, "CLbsSystem::LBSRootIsPersistentL()\n");
	
    TInt lbsRootIsTransient;
    CRepository* rep;
    rep = CRepository::NewL(KLbsCenRepUid); 

    __ASSERT_ALWAYS(rep != NULL, User::Panic(KLbsRootPanic, ELbsPanicNoIniFile));
    
    CleanupStack::PushL(rep);
    rep->Get(KLbsRootProcessTransientKey, lbsRootIsTransient);
    
    CleanupStack::PopAndDestroy(rep);
    
    if(lbsRootIsTransient == 1)
        {
		LBSLOG(ELogP2, "CLbsSystem::LBSRootIsPersistentL() - LBSRoot process is transient\n");
    	return(EFalse); // LBSRoot is not persistent
        }
	LBSLOG(ELogP2, "CLbsSystem::LBSRootIsPersistentL() - LBSRoot process is persistent\n");
    return(ETrue);
    }

//
//CLbsSystem
//
CLbsSystem* CLbsSystem::NewL()
    {
	LBSLOG(ELogP1, "CLbsSystem::NewL()\n");
	CLbsSystem* self;
	self = new (ELeave) CLbsSystem();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
    }
    
CLbsSystem::CLbsSystem() 
:	CActive(EPriorityStandard),
	iSystemIsUp(EFalse),
	iPendingAction(EActionNone),
	iReportStartupFailure(EFalse),
	iUsers(0),
	iSystemStateListener(NULL)
	{
	CActiveScheduler::Add(this);
	}
    
void CLbsSystem::StartLbsL()
	{
	LBSLOG(ELogP1, "CLbsSystem::StartLbsL()\n");
	
	User::LeaveIfError(iOverseer->StartupL());

	iState = ELbsSystemUp;
    }

/*
 * Closing the system is a bit tricky, as each process has to be closed 
 * asynchronously, in order to guarantee that all have been closed before we 
 * attempt to start them again. So do it in two parts, the first of which 
 * uses the active object's iStatus to detect when it can go onto the second part
 */
void CLbsSystem::StopLbsPart1()
    {
	LBSLOG(ELogP1, "CLbsSystem::StopLbsPart1()\n");
    if(iOverseer)
    	{
		LBSLOG(ELogP2, "iOverseer found. Closing it \n");
    	iState = ELbsSystemClosing;
    	iStatus = KRequestPending;

    	// Report that closedown is in progress if required
        if(iConfirmCloseDownComplete)
            {
        	iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownInProgress);
            }

    	iOverseer->CloseDown(&iStatus);
    	SetActive();
    	}
    else
    	{
		LBSLOG(ELogP2, "iOverseer NOT found. Just deleting properties \n");
    	// Didn't get as far as starting any processes. Just need to delete
    	// the properties defined at startup.
		StopLbsPart2();
    	}
    }

/*
 * For a failure at startup, all that matters is to tell 
 * the proceses to close, as we won't be attempting it again.
 */
void CLbsSystem::StopLbsNow()
    {
	LBSLOG(ELogP1, "CLbsSystem::StopLbsNow()\n");
    if(iOverseer)
    	{
		LBSLOG(ELogP2, "iOverseer found. Stopping processes \n");
    	iOverseer->StopProcesses();
    	}
    else
    	{
		LBSLOG(ELogP2, "iOverseer NOT found. Just deleting properties \n");
    	// Didn't get as far as starting any processes. Just need to delete
    	// the properties defined at startup.
 		DeleteLbsProperties();
    	}
    }
    
void CLbsSystem::StopLbsPart2()
    {
	LBSLOG(ELogP1, "CLbsSystem::StopLbsPart2()\n");
 	
	if(iReportStartupFailure)
		{
		// If the system is being brought down after a failure to bring it
		// up on user request, report the failure now.
		iStartupRequestHandler->ConfirmStartup(ELbsStartupFail);		
		}

	LBSLOG(ELogP1, "CLbsSystem::StopLbsPart2() LBS System closedown is complete\n");
    iState = ELbsSystemDown;

    // Send confirmation if client requested it
	if (iConfirmCloseDownComplete)
		{
		LBSLOG(ELogP1, "CLbsSystem::StopLbsPart2() Sending confirmation of LBS closedown\n");
		iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownComplete);
		}
    
	// Shut this process down if there are no pending startup requests AND either
	// LBSRoot is transient or we are doing a complete closedown
	if(iPendingAction != EActionBringLbsUp && (!iLbsRootAlwaysUp || iCompleteCloseDown))
		{
	    delete iOverseer;
	    iOverseer = 0;
	 	DeleteLbsProperties();
		
		// This delay is artificial; it is to allow any processes listening
		// to the property written to by iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownComplete)
		// to read the property before it is deleted when CLbsSystem::~CLbsSystem() is called.
		User::After(1000);
		LBSLOG(ELogP1, "CLbsSystem::StopLbsPart2() Root process exiting as requested closedown is complete\n");
		CActiveScheduler::Stop();
		}
    }

void CLbsSystem::ConstructL()
    {
    LBSLOG(ELogP1, "CLbsSystem::ConstructL()\n");

    // Sign up for system state notifications
    iSystemStateListener = CLbsSystemStateListener::NewL(*this);
    iSystemStateListener->StartToListenL();
    }

void CLbsSystem::HandleStartupCompletionL()
    {
	LBSLOG(ELogP1, "CLbsSystem::HandleStartupCompletionL()\n");
	/*
     * Find out whether the command line parameter specifies that we
     * should set everything up now, or wait till a component requests
     * LBS system startup
     */
    iLbsAlwaysUp = LbsIsPersistentL();
    iLbsRootAlwaysUp = LbsRootIsPersistentL();

    /* If ProcessIsPersistentL leaves (because there is no cenrep file) then
     * the startup/closedown properties will not be initialized. 
     * This is OK (as there is no system to startup) but it means that an attempt
     * to set the properties will fail, so the Lbs Root API must handle the case
     * when the properties are not defined, before it tries to set them
     */
     
    RLbsSystemController::InitializeL();
    
    iStartupRequestHandler = CLbsSystemStartupRequestHandler::NewL(this, KLbsRootApiUid);
    iCloseDownRequestHandler = CLbsSystemCloseDownRequestHandler::NewL(this, KLbsRootApiUid);

	DefineLbsPropertiesL();
	iOverseer = CProcessOverseer::NewL();
	
	if (iOverseer->IsRunning())
		{
		iState = ELbsSystemUp;
		}
	else
		{
		iState = ELbsSystemDown;
		// If LBS is in permanent mode and its not running, start it now
	    if(iLbsAlwaysUp)
	        {
			LBSLOG(ELogP2, "CLbsSystem::HandleStartupCompletionL() - trying to start LBS system\n");
			TRAPD(err, StartLbsL());
			if(err != KErrNone)
				{
				// Tidy up the LBS system in case any of it was started
				StopLbsNow();	        	        	
				LBSLOG_ERR(ELogP3, "Leave in CLbsSystem::HandleStartupCompletionL()\n");
				User::Leave(err);
				}
	        }
	    else
	        {
			LBSLOG(ELogP2, "CLbsSystem::HandleStartupCompletionL() - NOT trying to start LBS system\n");
	        }        
		}
    }
    
CLbsSystem::~CLbsSystem()    
    {
	delete iOverseer;
    delete iCloseDownRequestHandler;
    delete iStartupRequestHandler;
    delete iSystemStateListener;
    }


void CLbsSystem::OnResetRequest()
	{
	LBSLOG(ELogP1, "CLbsSystem::OnResetRequest()\n");
    iOverseer->ResetProcesses();
    }

/* This is a (test-only) forced close down request.

   This function should only ever be called for test purposes.
   It will force the system to shut down, even if there are
   clients still using LBS.
*/
void CLbsSystem::OnCompleteCloseDownRequest()
	{
	LBSLOG(ELogP1, "CLbsSystem::OnCompleteCloseDownRequest()\n");

	switch(iState)
		{
		case ELbsSystemUp:
			{
			// Set pending action to 'none', so that system isn't re-started
			// after it has been shut down.
			iPendingAction = EActionNone;
			
			// Set a flag that this is a complete shutdown
			iCompleteCloseDown = ETrue;
			
			// Shutdown is asynchronous, so start the process here.
			iConfirmCloseDownComplete = ETrue;
			StopLbsPart1();
			}
		break;
		
		case ELbsSystemClosing:
			{
			LBSLOG(ELogP1, "CLbsSystem LBS already closing down\n");
			
			// Set a flag that this is a complete shutdown
			iCompleteCloseDown = ETrue;
			
			// Confirmation will be sent when closedown has completed
			iConfirmCloseDownComplete = ETrue;
			}
		break;
		
		case ELbsSystemDown:
			{
			LBSLOG(ELogP1, "CLbsSystem LBS already closed down, reconfirming\n");
			iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownComplete);
			
			LBSLOG(ELogP1, "CLbsSystem::OnCompleteCloseDownRequest() Root process exiting as complete closedown requested\n");
			// This delay is artificial; it is to allow any processes listening
			// to the property written to by iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownComplete)
			// to read the property before it is deleted when CLbsSystem::~CLbsSystem() is called.
			User::After(1000);
			CActiveScheduler::Stop();
			}
		break;
		
		}
    }
    
void CLbsSystem::OnCloseDownRequest(TBool aConfirmCloseDownComplete)
    {
	LBSLOG(ELogP1, "CLbsSystem::OnCloseDownRequest()\n");

	LBSLOG2(ELogP2, "CLbsSystem::OnCloseDownRequest(): iUsers = %d\n", iUsers);
    if(iUsers)
        {
        --iUsers;    	
        }
	LBSLOG2(ELogP2, "CLbsSystem::OnCloseDownRequest(): iUsers = %d\n", iUsers);
	
    iConfirmCloseDownComplete = aConfirmCloseDownComplete;

    switch(iState)
        {
        default:
        case ELbsSystemDown:
        	{        	
			LBSLOG(ELogP2, "CLbsSystem::OnCloseDownRequest(): state = ELbsSystemDown\n");
             if(iConfirmCloseDownComplete)
                 {
             	 if(iUsers)
             	     {             	 	
             	     // Shouldn't happen - system down when there are users
	                 iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownRejected);
             	     }
             	 else
                     {
	                 iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownComplete);             	
                     }
                 }
        	break;
        	}
        
        case ELbsSystemClosing:
			LBSLOG(ELogP2, "CLbsSystem::OnCloseDownRequest(): state = ELbsSystemClosing\n");
            // Ignore the request - we're doing it anyway.
            // Confirmation will be sent when closedown is complete
        break;
        
        case ELbsSystemUp:
            {
			LBSLOG(ELogP2, "CLbsSystem::OnCloseDownRequest(): state = ELbsSystemUp\n");
            if(iLbsAlwaysUp)
                {
				LBSLOG(ELogP2, "CLbsSystem::OnCloseDownRequest(): non-transient system; reject request\n");
            	// Can't close it down. Respond with that information.
                if(iConfirmCloseDownComplete)
                    {
                	iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownRejected);
                    }
                }
            else
                {            	
                if(iUsers == 0)
                    {                	
					LBSLOG(ELogP2, "CLbsSystem::OnCloseDownRequest(): no users left; close down\n");
	                StopLbsPart1();
                    }
                else
                    {
					LBSLOG(ELogP2, "CLbsSystem::OnCloseDownRequest(): users left; reject request\n");
            	    // Can't close it down - someone else still needs us. Reject the request.
                    if(iConfirmCloseDownComplete)
                        {
                	    iCloseDownRequestHandler->ConfirmCloseDown(ELbsCloseDownRejected);
                        }                	
                    }
                }
            }
        break;
        }
	
    }
    
void CLbsSystem::OnStartupRequest()
    {
	LBSLOG(ELogP1, "CLbsSystem::OnStartupRequest()\n");

    switch(iState)
        {        
        case ELbsSystemUp:
            {
            if (iLbsRootAlwaysUp) // We can only do user counting if LBSRoot process is always running
            	{
            	++iUsers;
            	LBSLOG2(ELogP2, "CLbsSystem::OnStartupRequest(): iUsers = %d\n", iUsers);
            	}
            // No need to do anything. Just register that there's another user.
	        iStartupRequestHandler->ConfirmStartup(ELbsStartupSuccess);        	
            }
        break;
        
        case ELbsSystemDown:
            {
			LBSLOG(ELogP2, "state = ELbsSystemDown\n");
			TRAPD(err, StartLbsL());
			if(err == KErrNone)
				{	        	
				iStartupRequestHandler->ConfirmStartup(ELbsStartupSuccess);

				if (iLbsRootAlwaysUp) // We can only do user counting if LBSRoot process is always running
	            	{
	            	++iUsers;
	            	LBSLOG2(ELogP2, "CLbsSystem::OnStartupRequest(): iUsers = %d\n", iUsers);
					}
	            else
	            	{
	            	// This process can now be shut down as it is transient
	        		LBSLOG(ELogP1, "CLbsSystem::OnStartupRequest() Root process exiting as requested startup is complete\n");
	        		// This delay is artificial; it is to allow any processes listening
	        		// to the property written to by iStartupRequestHandler->ConfirmStartup(ELbsStartupSuccess)
	        		// to read the property before it is deleted when CLbsSystem::~CLbsSystem() is called.
	        		User::After(1000);
	        		CActiveScheduler::Stop();
	            	}
				}
			else
				{
				// Tidy up the LBS system in case any of it was started
				LBSLOG(ELogP3, "CLbsSystem::OnStartupRequest() : Error reported starting system. Close it down\n");
				iReportStartupFailure = ETrue;
				StopLbsPart1();
				}
            }
        break;

        case ELbsSystemClosing:
            {
			LBSLOG(ELogP2, "CLbsSystem::OnStartupRequest() : state = ELbsSystemClosing\n");
            // Something has requested that the system be brought up while
            // it's being taken down. The easiest thing to do is to latch 
            // the request and wait till the system is stable to action it.
            iPendingAction = EActionBringLbsUp;
            }
        break;
        }
    }
    
void CLbsSystem::RunL()
	{
	LBSLOG(ELogP1, "CLbsSystem::RunL()\n");
	switch(iState)
		{
		case ELbsSystemClosing:
			{
			StopLbsPart2();

			// We might have received a request to start the system while it was
			// being closed down. If so, action it now.
			//
			if(iPendingAction == EActionBringLbsUp)
				{
				LBSLOG(ELogP2, "CLbsSystem::RunL(): discovered pending action to start system\n");
				iPendingAction = EActionNone;
				TRAPD(err, StartLbsL());
				if(err == KErrNone)
					{	        	
					LBSLOG(ELogP2, "CLbsSystem::RunL(): startup successful\n");
		            if (iLbsRootAlwaysUp) // We can only do user counting if LBSRoot process is always running
		            	{
		            	++iUsers;
		            	LBSLOG2(ELogP2, "CLbsSystem::OnStartupRequest(): iUsers = %d\n", iUsers);
		            	}
					iStartupRequestHandler->ConfirmStartup(ELbsStartupSuccess);
					}
				else
					{
					// Tidy up the LBS system in case any of it was started
					LBSLOG(ELogP3, "CLbsSystem::RunL() : Error reported starting system. Close it down\n");
					iReportStartupFailure = ETrue;
					StopLbsPart1();
					}
				}
	         }
	     }
    }

void CLbsSystem::DoCancel()
    {
	
    }
//
//CProcessGuardian
//
/**
 * Factory function for the class.
 * 
 * @param	"RThread& aSubThread"
 *			A thread to monitor for panics
 * @xxxx
 */
CProcessGuardian* CProcessGuardian::NewL(const TDesC& aProcessName, 
										 TUid aUid, 
										 TBool aIsServer,
										 MLbsProcessGuardianObserver& aObserver)
	{
	LBSLOG(ELogP1, "CProcessGuardian::NewL()\n");
	CProcessGuardian* self;
	self = new (ELeave) CProcessGuardian(aUid, aIsServer, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aProcessName);
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Constructor for the class.
 * Add the object to the active scheduler
 * 
 * @param	MLbsProcessPanicObserver* aObserver
 *			A process to monitor for panics
 * @xxxx
 */
CProcessGuardian::CProcessGuardian(TUid aUid,
										TBool aIsServer,
										MLbsProcessGuardianObserver& aObserver)
:	CActive(CActive::EPriorityIdle),
    iIsServer(aIsServer),
    iUid(aUid),
	iObserver(aObserver),
	iState(EProcessStateIdle)
	{
	//Add ourself to the active scheduler
	CActiveScheduler::Add(this);

	}
/**
 * 
 * Determine if the process is already running, if so logon to monitor for panics
 */
void CProcessGuardian::ConstructL(const TDesC& aProcessName)
	{
	LBSLOG(ELogP1, "CProcessGuardian::ConstructL()\n");
	iProcessName = aProcessName.AllocL();
	
	// We should close the default handle
	iProcess.Close();
	__ASSERT_DEBUG(iProcess.Handle() == NULL, User::Invariant());

	// Create the supervisor link to the process
	iLbsSupervisor.OpenL(iUid);
	
	// Find the process and logon to it (if its running)
	FindProcessAndLogon();
	}

/**
 * 
 * Startup the process and logon to monitor for panics
 */
TInt CProcessGuardian::Startup()
	{
	LBSLOG(ELogP1, "CProcessGuardian::StartupL()\n");

	if (iProcess.Handle() != NULL)
		{
		if (iProcess.ExitType() == EExitPending)
			{
			LBSLOG(ELogP2, "CProcessGuardian::StartupL() - Process already running\n");
			iState = EProcessStateRunning;
			return KErrAlreadyExists;
			}
		// Process is no longer running, so close the handle ready to restart
		iProcess.Close();
		}

	// We know the process isn't running at this point
	iState = EProcessStateIdle;
	
	// Start the process
	TInt err = KErrNone;

	if(!iIsServer)
		{
		TProcessStartParams params;
		params.SetProcessFileName(*iProcessName);
		params.SetProcessName(*iProcessName);
		params.SetRendezvousRequired(ETrue);
		LBSLOG(ELogP2, "CProcessGuardian::StartupL() - Launching process\n");
		err = CProcessLaunch::ProcessLaunch(params);
		}
	else
		{
		// we guessed wrong, so try to start as a server
		TServerStartParams params;
		params.SetServerFileName(*iProcessName);
		params.SetServerName(*iProcessName);
		params.SetNumberOfServerSlots(KServerFrameworkDefaultMessageSlots);
		params.SetServerUids(KNullUid, KNullUid, iUid);
		LBSLOG(ELogP2, "CProcessGuardian::StartupL() - Launching server process\n");
		err = CServerLaunch::ServerLaunch(params);
		}
	
	if (err == KErrNone)
		{
		// Process started, find it and logon so we can track when it exits
		err = FindProcessAndLogon();
		if (err == KErrNone)
			{
			// Found the process so set our state to running
			iState = EProcessStateRunning;
			}
		}
	return err;
	}

/**
 * 
 * Find the process and logon if it is running
 */
TInt CProcessGuardian::FindProcessAndLogon()
	{
	// Make sure we don't already have an open handle for this process
	__ASSERT_DEBUG(iProcess.Handle() == NULL, User::Invariant());
	
	RProcess process;
	TFullName fullName;
	_LIT(KStar, "*");
	fullName.Append(*iProcessName);
	fullName.Append(KStar);

	// Find the process by name
	TFindProcess findProcess;
	findProcess.Find(fullName);
	TInt err = KErrNone;
	while (err = findProcess.Next(fullName), err == KErrNone)
		{
		TInt processOpenErr = process.Open(findProcess);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// found a running instance of the process,
				// so store this as the process owned by this
				// CProcessGuardian.
				iProcess = process;
				LBSLOG(ELogP2, "CProcessGuardian::FindProcessAndLogon() - Running process found\n");
				break;
				}
			}
			process.Close();
		}
	
	if (iProcess.Handle() == NULL)
		{
		LBSLOG(ELogP2, "CProcessGuardian::StartupL() - Running process not found\n");
		return KErrNotFound;
		}

	iState = EProcessStateRunning;
	LBSLOG(ELogP2, "CProcessGuardian::StartupL() - logging on to process\n");
	iProcess.Logon(iStatus);
	SetActive();
	
	return KErrNone;
	}


/**
 * Destructor for this class.
 * Make sure that the undertaker was cancelled before deleted. 
 * @xxxx
 */
CProcessGuardian::~CProcessGuardian()
	{
	LBSLOG(ELogP1, "CProcessGuardian DESTRUCTOR\n");
	Cancel();
	delete iProcessName;
	delete iCloseTimer;
	iLbsSupervisor.Close();
	if(iProcess.Handle() != 0)
		{
		iProcess.Close();		
		}
	}

/**
 * This function is triggered when the process being monitored terminates.
 * If the process being monitored panics, report it to the observer.
 *
 * @xxxx
 */
void CProcessGuardian::RunL()
	{
	LBSLOG(ELogP1, "CProcessGuardian::RunL()\n");

	switch(iState)
	    {
	    case EProcessStateRunning:
	    case EProcessStateClosingDown:
	        {
#ifdef ENABLE_LBS_DEV_LOGGER
	        TFileName* processName = new TFileName;
	        if(processName != NULL)
	        	{
	        	iProcess.FullName((*processName));
	        	if (processName->Length() > 0)
	        		{
	        		LBSLOG3(ELogP3, "Process exited: %S, reason %d\n", processName,	iProcess.ExitReason());
	        		}
	        	delete processName;
	        	}
#endif
			// Stop the timer if it is running
	        if (iCloseTimer)
	        	{
	        	iCloseTimer->Stop();
	        	}
	        
			// Notify the observer (the CProcessOverseer) 
			// that this process has exited. 
			// It may be because of a panic, in which case 
			// iProcess.ExitType() will return EExitPanic
    	    iObserver.OnProcessExit(this, 
    	    						iProcess.ExitType(), 
    	    						iProcess.ExitCategory(), 
    	    						iProcess.ExitReason());
	        break;
	        }
	    case EProcessStateIdle:
	        {
	        // Should never get here.
	    	iState = EProcessStateIdle;
	    	break;
	        }
	    }
	}

/**
 * Stop monitoring the process for the process exiting. 
 * This function is executed by calling Cancel() and 
 * has to be done before the object is deleted.
 *
 */
void CProcessGuardian::DoCancel()
	{
	iProcess.LogonCancel(iStatus);
	}

TUid CProcessGuardian::ProcessUid()
    {
	return(iUid);
    }
    
TBool CProcessGuardian::IsRunning()
	{
	return(iState!=EProcessStateIdle);
	}

TInt CProcessGuardian::CloseDown()
    {
	LBSLOG(ELogP1, "CProcessGuardian::CloseDown()\n");

	iState = EProcessStateClosingDown;
	if (iProcess.Handle() == NULL)
		{
		// Process wasn't started, so no need to shut down
		return KErrNotFound;
		}
	else if (iProcess.ExitType() == EExitPending)
        {
        // Process is currently running
#ifdef ENABLE_LBS_DEV_LOGGER
        TFileName* processName = new TFileName;
        if(processName != NULL)
        	{
        	iProcess.FullName((*processName));
        	if (processName->Length() > 0)
        		{
        		LBSLOG2(ELogP2, "Closing running process %S\n", processName);
        		}
        	delete processName;
        	}
#endif

        // Start a timer so we don't wait forever if the process doesn't close down
        delete iCloseTimer;
        iCloseTimer = 0;
        TRAPD(error, iCloseTimer = CProcessCloseTimer::NewL(*this,iObserver));
        if(error != KErrNone)
            {
            return KErrGeneral;
            }
        
        iCloseTimer->Start(KLbsProcessCloseTimeout);
        
        // Make the close down request
    	iLbsSupervisor.CloseDownProcess();	
        }
    else
    	{
    	// Process has already closed down for some reason
#ifdef ENABLE_LBS_DEV_LOGGER
    	TFileName* processName = new TFileName;
    	if(processName != NULL)
    		{
    		iProcess.FullName((*processName));
    		if(processName->Length() > 0)
    			{
            	LBSLOG2(ELogP2, "Closing exited (or panicked) process %S\n", processName);
    			}
    		delete processName;
    		}
#endif

    	iObserver.OnProcessExit(this, iProcess.ExitType(), 
    							iProcess.ExitCategory(), 
    							iProcess.ExitReason());
    	}

	return KErrNone;
    }
 
/*
 **************************************** CProcessOverseer *********************
 */
CProcessOverseer::CProcessOverseer() :
	iState(EOverseerIdle)
    {	
    }

CProcessOverseer* CProcessOverseer::NewL()
    {
	LBSLOG(ELogP1, "CProcessOverseer::NewL()\n");
	CProcessOverseer* self;
	self = new (ELeave) CProcessOverseer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
    }

void CProcessOverseer::ConstructL()
    {
	LBSLOG(ELogP1, "CProcessOverseer::ConstructL()\n");
	AttachLbsProcessesL();
    }

CProcessOverseer::~CProcessOverseer()
    {
    iLbsProcess.ResetAndDestroy();
    }
    
TInt CProcessOverseer::StartupL()
	{
	LBSLOG(ELogP1, "CProcessOverseer::Startup()\n");
	// Reset system restart count
	iSystemRestartCount = 0;
	StartLbsProcessesL();
	return iCreateStatus;
	}

void CProcessOverseer::CloseDown(TRequestStatus* aCloseDownCompletionStatus)
    {
	LBSLOG(ELogP1, "CProcessOverseer::CloseDown()\n");
    iCloseDownCompletionStatus = aCloseDownCompletionStatus;
	StopLbsProcesses();
    }

void CProcessOverseer::AttachLbsProcessesL()
	{
	LBSLOG(ELogP1, "CProcessOverseer::AttachLbsProcessesL()\n");
    /*
     * Read from the cenrep repository the list of processes to attach to.
     * Within the file these are organised as process names indexed 
     * consecutively from KLbsRootProcessNameFirstKey
     */
    CRepository* rep(NULL);
    TRAP_IGNORE(rep = CRepository::NewL(KLbsCenRepUid));
    if(rep == NULL)
        {
		LBSLOG_ERR(ELogP3, "Leave in CProcessOverseer::AttachLbsProcessesL(): cen rep file not found\n");
        User::Leave(KErrNotFound);    	
        }
    CleanupStack::PushL(rep);

    const TInt KMaxProcessNameLength = 100;
	TBuf<KMaxProcessNameLength> processNameBuf;
	
    /* We finish reading and initialising processes when the first 
     * failure happens
     */    
    TInt err = KErrNone;
    TBool finished = EFalse;
    
    // Initalize the count of processes we expect to hear from
    TInt count = 0;
    
    // This is to count of how many processes are already running
    TInt runningCount = 0;
    
    while(!finished)
        {
        TInt processUidVal;
        TBool processIsServer;
        err = rep->Get(KLbsRootProcessNameFirstKey + count, processNameBuf);
        if(err != KErrNone)
            {
			LBSLOG2(ELogP3, "CProcessOverseer::AttachLbsProcessesL(): error %d reported by cenrep read\n", err);
        	finished = ETrue;
            // It's not an error if the process name is not found, as
            // that's how we know we've reached the end
            if(err == KErrNotFound)
                {
				LBSLOG(ELogP3, "CProcessOverseer::AttachLbsProcessesL(): error cancelled; KErrNoen is OK\n");
        	    err = KErrNone;            	
                }
            }
        if(!finished)
            {
            err = rep->Get(KLbsRootProcessUidFirstKey + count, processUidVal);        	
			LBSLOG2(ELogP2, "CProcessOverseer::AttachLbsProcessesL(): read process UID. Error  = %d\n", err);
            finished = err != KErrNone;
            }
        if(!finished)
            {
            err = rep->Get(KLbsRootProcessIsServerFirstKey + count, processIsServer);        	
			LBSLOG2(ELogP2, "CProcessOverseer::AttachLbsProcessesL(): read process Is server. Error  = %d\n", err);
            finished = err != KErrNone;
            }
        if(!finished)        
            { 
            CProcessGuardian* newProcess = 0;
            LBSLOG2(ELogP3, "Creating CProcessGuardian for process %S", &processNameBuf);
	        TRAP(err, newProcess = CProcessGuardian::NewL(processNameBuf,
	                                            		  TUid::Uid(processUidVal),
	                                            		  processIsServer,
	                                            		  *this));
#ifdef ENABLE_LBS_DEV_LOGGER
			if (err != KErrNone)
				{
				LBSLOG2(ELogP2, "Failed to create CProcessGuardian for process. Error  = %d\n", err);
				}
#endif
            finished = err != KErrNone;
	        if(!finished)
	            {	        	
        	    iLbsProcess.AppendL(newProcess);
                ++count;
                if (newProcess->IsRunning())
                	{
                	++runningCount;
                	}
	            }
            }
        }
    CleanupStack::PopAndDestroy(rep);

	// Rather than leaving (which would mean having to undo any
	// processes which we did manage to start), finish off but flag
	// the status. This means that the caller can check whether 
	// there was an error, and if so, close the overseer in the 
	// normal way.
	iCreateStatus = err;

	iState = (runningCount > 0 ? EOverseerRunning : EOverseerIdle);
	}

void CProcessOverseer::StartLbsProcessesL()
    {
	LBSLOG(ELogP1, "CProcessOverseer::StartLbsProcessesL()\n");

	iState = EOverseerStarting;

	TInt err = KErrNone;
	const TInt processCount = iLbsProcess.Count();
	for (TInt i=0; i<processCount; ++i)
		{
		LBSLOG3(ELogP2, "CProcessOverseer::StartLbsProcesses() starting process %d of %d\n", i+1, processCount);
		CProcessGuardian* process = iLbsProcess[i];
		err = process->Startup();
		
		if (err == KErrNone || err == KErrAlreadyExists)
			{
			err = KErrNone;
			}
		else
			{
			LBSLOG2(ELogP2, "Failed to start process. Error  = %d\n", err);
			break;
			}
		}

	// Rather than leaving (which would mean having to undo any
	// processes which we did manage to start), finish off but flag
	// the status. This means that the caller can check whether 
	// there was an error, and if so, close the overseer in the 
	// normal way.
	iCreateStatus = err;
	if (err == KErrNone)
		{
		LBSLOG(ELogP2, "CProcessOverseer::StartLbsProcesses() All processes started succesfully\n");
		iState = EOverseerRunning;
		}
    }

TBool CProcessOverseer::IsRunning()
	{
	return(iState == EOverseerRunning);
	}

void CProcessOverseer::StopProcesses()
	{
	LBSLOG(ELogP1, "CProcessOverseer::StopProcesses()\n");
	const TInt processCount = iLbsProcess.Count();
	LBSLOG2(ELogP2, "CProcessOverseer::StopProcesses() stopping %d processes\n", processCount);
	for (TInt i=processCount-1; i>=0; --i)
		{
		CProcessGuardian* process = iLbsProcess[i];
		process->CloseDown();
		}
	
	}

void CProcessOverseer::ResetProcesses()
    {
	LBSLOG(ELogP1, "CProcessOverseer::ResetProcesses()\n");
	
	// The reset request is broadcast to all processes that
	// are attached to the process supervisor, so we 
	// don't need to call reset on each CProcessGuardian individually.
	RLbsProcessSupervisor processSupervisor;
	CleanupClosePushL(processSupervisor);
	processSupervisor.OpenL(RProcess().SecureId());
	processSupervisor.ResetProcess();
	CleanupStack::PopAndDestroy(&processSupervisor);
    }
    
void CProcessOverseer::StopLbsProcesses()
    {
	LBSLOG(ELogP1, "CProcessOverseer::StopLbsProcesses()\n");
    switch(iState)
        {
        case EOverseerStarting: // one or more processes may have been started 
        						// before an error was reported.
        case EOverseerRunning:
            {            
            iState = EOverseerClosingProcesses;
            iCurrentProcess = 0;
			CloseNextProcess();
	        break;
            }
        case EOverseerIdle:
        case EOverseerClosingProcesses:
            // do nothing
            break;
        }
        
    }

/* Called when a process exits.
   Usually this will be when the process exits after shutting
   down normally (i.e. after is has been told to shut down).
*/
void CProcessOverseer::OnProcessExit(const CProcessGuardian* /*aGuardian*/, 
									 TExitType aExitType,
									 const TDesC& /*aExitCategory*/,
									 TInt /*aExitReason*/)
	{
	LBSLOG(ELogP1, "CProcessOverseer::OnProcessExit()\n");
	
	switch(iState)
	    {
	    case EOverseerRunning:
	    	{
	    	LBSLOG(ELogP2, "CProcessOverseer::OnProcessExit(): state = EOverseerRunning.\n");
	    	
	    	// Usually should only get here if a monitored process 
	    	// has either panicked or been killed
	    	if (aExitType == EExitPanic)
	    		{
	    		// A process has panicked. Simply shutdown
	    		// all of the other processes and then 
	    		// attempt a full restart.
	    		StopLbsProcesses();
	    		}
	    	break;
	    	}
	    case EOverseerClosingProcesses:
	        {
			LBSLOG(ELogP2, "CProcessOverseer::OnProcessExit(): state = EOverseerClosingProcesses.\n");
			
	        // A process has reported that it has closed itself down in response 
	        // to a closedown request.
	        //
            CloseNextProcess();
            break; 
	        }
	    case EOverseerIdle:
	    case EOverseerStarting:
	    default:
	    	{
	    	// do nothing in this case
	    	break;
	    	}
	    }	
	}

void CProcessOverseer::OnProcessCloseTimeout(const CProcessGuardian* /*aGuardian*/)
	{
	LBSLOG(ELogP1, "CProcessOverseer::OnProcessCloseTimeout()\n");
	
	switch(iState)
		{
	    case EOverseerClosingProcesses:
	        {
			LBSLOG(ELogP2, "CProcessOverseer::OnProcessCloseTimeout(): state = EOverseerClosingProcesses.\n");
			
	        // Timeout occurred when waiting for process to close
	        // Proceed to close the next waiting process
	        //
            CloseNextProcess();
            break; 
	        }
	    default:
	    	{
	    	// do nothing in this case
	    	break;
	    	}
		}
	}

void CProcessOverseer::CloseNextProcess()
	{
    TInt processCount = iLbsProcess.Count();

	while (iCurrentProcess < processCount)
		{
		LBSLOG3(ELogP2, "CProcessOverseer::CloseNextProcess() closing processes %d of %d\n", iCurrentProcess+1, processCount);

        CProcessGuardian* process = iLbsProcess[iCurrentProcess];
    	++iCurrentProcess;
        if (process->CloseDown() == KErrNone)
        	{
        	return;
        	}
        // If there was an error closing this process, then go on to the next one.
		}

	if (iCurrentProcess >= processCount)
		{
		LBSLOG(ELogP2, "CProcessOverseer::CloseNextProcess(): Last process closed\n");
        
        // All open processes have been closed, so 
        // restart the system if needed. Or simply
        // report close-down complete if it was requested
        // by a client.
        if(iCloseDownCompletionStatus)
            {
            iState = EOverseerIdle;
            User::RequestComplete(iCloseDownCompletionStatus, KErrNone); 
            iCloseDownCompletionStatus = NULL;
            }
        else
            {
        	if (iSystemRestartCount < KMaxSystemStartRetries)
        		{
        		LBSLOG(ELogP2, "CProcessOverseer::CloseNextProcess(): Restarting processes\n");
				TRAPD(restartErr, StartLbsProcessesL());
				if(restartErr != KErrNone)
					{
					// The only way there can be a failure on a restart attempt is 
					// if the cenrep file isn't found. Difficult to see
					// how this can happen if it didn't fail on the first attempt,
					// but in any case there's no point retrying
        			User::Panic(KLbsRootPanic, KLbsPanicSystemStartFailure);
					}
				++iSystemRestartCount;
        		}
        	else
        		{
        		// That's enough attempts. No point carrying on, so panic.
        		User::Panic(KLbsRootPanic, KLbsPanicSystemStartFailure);
        		}					
            }
        }
	}


CProcessCloseTimer* CProcessCloseTimer::NewL(const CProcessGuardian& aGuardian, 
												 MLbsProcessGuardianObserver& aObserver)
	{
	LBSLOG(ELogP1, "CProcessCloseTimer::NewL()\n");
	CProcessCloseTimer* self;
	self = new (ELeave) CProcessCloseTimer(aGuardian, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CProcessCloseTimer::~CProcessCloseTimer()
	{
	Cancel();
	}
	
void CProcessCloseTimer::Start(const TTimeIntervalMicroSeconds32 aDelay)
	{
	LBSLOG(ELogP1, "CProcessCloseTimer::Start()\n");
	After(aDelay);
	}

void CProcessCloseTimer::Stop()
	{
	LBSLOG(ELogP1, "CProcessCloseTimer::Stop()\n");
	Cancel();
	}
	
void CProcessCloseTimer::RunL()
	{
	LBSLOG(ELogP1, "CProcessCloseTimer::RunL() Begin\n");
	iObserver.OnProcessCloseTimeout(&iGuardian);
	}

CProcessCloseTimer::CProcessCloseTimer(const CProcessGuardian& aGuardian,
		MLbsProcessGuardianObserver& aObserver) :
	CTimer(CActive::EPriorityStandard),
	iGuardian(aGuardian),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CProcessCloseTimer::ConstructL()
	{
	LBSLOG(ELogP1, "CProcessCloseTimer::ConstructL()\n");
	// call the base class version
	CTimer::ConstructL();
	}
