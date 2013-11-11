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

#include <centralrepository.h>

#include "te_lbspsyutils.h"
#include "lbssystemcontroller.h"
#include "lbsrootcenrepdefs.h"
#include "te_lbsinifilereader.h"
#include "lbsinternalinterface.h"

#include "lbsnetinternalapi.h"
#include "lbsassistancedatacacheapi.h"
#include "lbsngnlmint.h"
#include "lbsreffnpint.h"
#include "lbsnetregstatusint.h"

#include <bautils.h>
#include <lbs/lbsextendedsatellite.h>

#ifdef SYMBIAN_FEATURE_MANAGER
    #include <featdiscovery.h>
    #include <featureuids.h>
#endif

// Locations and paths used when parsing .ini files
_LIT(KLbsDir, "lbs");
_LIT(KModuleIniDir, "locmods");
_LIT(KIniWildCard, "*.ini");
_LIT(KQualityProfileIniName, "lbsprofile.ini");

const TInt KSecToMicroSecTransfer = 1000*1000;

EXPORT_C CTe_SystemStarter::CTe_SystemStarter(TUid aServerId)
/** Constructor.
*/
	{
	iServerId = aServerId;
	}

EXPORT_C CTe_SystemStarter::~CTe_SystemStarter()
/** Destructor.
*/
	{
	}

EXPORT_C void CTe_SystemStarter::RestartLbs_RootOnlyL(TBuf8<KMaxFileName>& aFirstExe)
	{
	//Shutdown the whole LBS and restart with only the root process.
	RLbsSystemController lbsSysController;
	lbsSysController.OpenL(iServerId);
	CleanupClosePushL(lbsSysController);	
	User::LeaveIfError(lbsSysController.RequestCompleteCloseDown());
	
	CRepository* rep = CRepository::NewL(TUid::Uid(KLbsCenRepUidValue));
	CleanupStack::PushL(rep);
	
	TInt err = rep->Get(KLbsRootProcessNameFirstKey, aFirstExe);
	if(err==KErrNone)
		{
		User::LeaveIfError(rep->Delete(KLbsRootProcessNameFirstKey));
		}
	else if(err==KErrNotFound)
		{
		aFirstExe = KNullDesC8;
		}
	else
		{
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(rep);
	
	User::LeaveIfError(lbsSysController.RequestSystemStartup());
	
	CleanupStack::PopAndDestroy(&lbsSysController);  
	}


void CTe_SystemStarter::GetQualityProfileInfoL(RFs& aFs, 
                                        RArray<TQualityProfile>& aQualityArray,
                                        const TDesC& aFileName)
    {
    // Check the file is present.
    TUint att;
    TInt err = aFs.Att(aFileName, att);
    if(err != KErrNone)
        {
        User::Leave(err);
        }

    // Open the .ini file reader
    CTe_LbsIniFileReader* fileReader = CTe_LbsIniFileReader::NewL(aFs, aFileName);
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
                    User::Panic(KLbsNetPsyTestPanic, fileReader->ELbsPanicInvalidProfileIniFile);
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
            
            // Add the quality profile to the list
            User::LeaveIfError(aQualityArray.Append(qualityProfile));
            }
        }

    CleanupStack::PopAndDestroy(fileReader);    
    }

void CTe_SystemStarter::InitializeQualityProfileInfoL()
    {
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
    
    qualityArray.Reserve(5);

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


void CTe_SystemStarter::GetModuleInfoL(RFs& aFs, TPositionModuleInfoExtended& aExtendedModuleInfo, 
                                          const TDesC& aFileName)
    {
    TPositionQuality posQuality;
    
    // Check the file is present.
    TUint att;
    TInt err = aFs.Att(aFileName, att);
    if(err != KErrNone)
        {
        User::Leave(err);
        }

    CTe_LbsIniFileReader* fileReader = CTe_LbsIniFileReader::NewL(aFs, aFileName);
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
    
void CTe_SystemStarter::InitializeModuleInfoL()
    {   
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
    
    TPositionModuleInfoExtended moduleInfoArray[KMaxInfoModules];
    
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
                GetModuleInfoL(fs, moduleInfoArray[moduleCount], parse.FullName());
                nameArray->AppendL(parse.Name());
                moduleCount++;
                if (moduleCount > KMaxInfoModules)
                    {
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
    
    const TPositionModuleInfoExtended* duplicateModuleInfoArray[] = 
        {
        &moduleInfoArray[0],
        &moduleInfoArray[1],
        &moduleInfoArray[2],
        &moduleInfoArray[3],
        &moduleInfoArray[4],
        &moduleInfoArray[5]
        };
    
    LbsModuleInfo::InitializeL(duplicateModuleInfoArray, moduleCount);
    }


EXPORT_C void CTe_SystemStarter::DefineLbsPropertiesL()
    {
    
#ifdef SYMBIAN_FEATURE_MANAGER
    TBool locationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
#else
    TBool locationManagementSupported(ETrue);
#endif
    
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
    
    }

EXPORT_C void CTe_SystemStarter::DeleteLbsPropertiesL()
    {
    // LBS Internal API
    LbsModuleInfo::ShutDownL();
    RLbsPositionUpdateRequests::ShutDownL();
    RLbsPositionUpdates::ShutDownL();
    RLbsModuleStatus::ShutDownL();
    RLbsSystemModuleEvent::ShutDownL();
    LbsQualityProfile::ShutDownL();
    
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
    }
EXPORT_C void CTe_SystemStarter::RestartLbs_NormalL(TBuf8<KMaxFileName>aFirstExe)
	{
	//We need to shutdown the whole LBS (including the root process) so any subsequent tests are able to start 
	//the root process with new settings if necessary.
	//This is done for the device build.
	RLbsSystemController lbsSysController;
	lbsSysController.OpenL(iServerId);
	CleanupClosePushL(lbsSysController);	
	User::LeaveIfError(lbsSysController.RequestCompleteCloseDown());
	CleanupStack::PopAndDestroy(&lbsSysController);
			
	if(aFirstExe.Length())
		{
		CRepository* rep = CRepository::NewL(TUid::Uid(KLbsCenRepUidValue));
		rep->Set(KLbsRootProcessNameFirstKey, aFirstExe);
		delete rep;
		}
	}
