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

#define __INCLUDE_CAPABILITY_NAMES__

#include <e32test.h>
#include <f32file.h>
#include <test/testexecutelog.h>
#include "tdbtsisui.h"
#include "languagetable.h"

#include <swi/launcher.h>
#include <e32capability.h>

using namespace Swi;

/* Functions Literals */
_LIT(KDisplayInstall,               "DisplayInstall");
_LIT(KDisplayGrantCapabilities,     "DisplayGrantCapabilities");
_LIT(KDisplayLanguage,              "DisplayLanguage");
_LIT(KDisplayDrive,                 "DisplayDrive");
_LIT(KDisplayUpgrade,               "DisplayUpgrade");
_LIT(KDisplayOptions,               "DisplayOptions");
_LIT(KHandleInstallEvent,           "HandleInstallEvent");
_LIT(KHandleCancellableInstallEvent,"HandleCancellableInstallEvent");
_LIT(KDisplaySecurityWarning,       "DisplaySecurityWarning");
_LIT(KDisplayOcspResult,            "DisplayOcspResult");
_LIT(KDisplayCannotOverwriteFile,   "DisplayCannotOverwriteFile");
_LIT(KDisplayUninstall,             "DisplayUninstall");
_LIT(KDisplayText,                  "DisplayText");
_LIT(KDisplayError,                 "DisplayError");
_LIT(KDisplayDependencyBreak,       "DisplayDependencyBreak");
_LIT(KDisplayApplicationsInUse,     "DisplayApplicationsInUse");
_LIT(KDisplayQuestion,              "DisplayQuestion");
_LIT(KDisplayMissingDependency,     "DisplayMissingDependency");

CUISisAdaptor::CUISisAdaptor()
	{
	}

CUISisAdaptor* CUISisAdaptor::NewLC(CTestExecuteLogger& aLogger)
	{
	CUISisAdaptor* self = new(ELeave) CUISisAdaptor();
	CleanupStack::PushL(self);
	self->ConstructL(aLogger);
	return self;
	}

CUISisAdaptor* CUISisAdaptor::NewL(CTestExecuteLogger& aLogger)
	{
	CUISisAdaptor* self = NewLC(aLogger);
	CleanupStack::Pop(self);
	return self;
	}

void CUISisAdaptor::ConstructL(CTestExecuteLogger& aLogger)
	{
	iLogger = aLogger;
	}
	
CUISisAdaptor::~CUISisAdaptor()
	{
	}

void CUISisAdaptor::DisplayAppInfo(const TDesC& aPreText, const CAppInfo& aAppInfo)
	{
	INFO_PRINTF7(_L("\t%S {\"%S\", \"%S\", V%u.%u.%u}"),
		&aPreText,
		&aAppInfo.AppName(), &aAppInfo.AppVendor(),
		aAppInfo.AppVersion().iMajor,
		aAppInfo.AppVersion().iMinor,
		aAppInfo.AppVersion().iBuild);
	}

void CUISisAdaptor::DisplayBoolReturn(TBool aReturn)
	{
	_LIT(KTrue, "True");
	_LIT(KFalse, "False");
	INFO_PRINTF2(_L("\tReturning %S"), aReturn ? &KTrue : &KFalse);
	}

void CUISisAdaptor::DisplayIntReturn(TInt aReturn)
	{
	INFO_PRINTF2(_L("\tReturning %d"), aReturn);
	}
	

//
// Implementation for the virtual functions for the MUiHandler class. 
//

/**
 * Displays Install dialog and logs all incoming information.
 *
 * @param aAppInfo      Application information.
 * @param aLogo         Optional bitmap logo.
 * @param aCertificates Array of end-entity certificates (can be empty).
 *
 * @return A value from script: \b ETrue to continue, \b EFalse to abort 
 *         installation.
 */
TBool CUISisAdaptor::DisplayInstallL(const CAppInfo& aAppInfo,
										const CApaMaskedBitmap* aLogo,
										const RPointerArray<CCertificateInfo>& aCertificates)
	{
	INFO_PRINTF1(_L("DisplayInstallL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
		
	if (aLogo)
		{
		INFO_PRINTF3(_L("\tLogo: %u x %u px"),aLogo->SizeInPixels().iWidth,
			aLogo->SizeInPixels().iHeight);
		}
		
	if (aCertificates.Count())
		{
		INFO_PRINTF2(_L("\tCertificate(s): %u"),aCertificates.Count());
		for (TInt i=0; i<aCertificates.Count(); i++)
			{
			INFO_PRINTF4(_L("\t[%02u] Subj: \"%S\", Issuer: \"%S\""),i,
				&aCertificates[i]->SubjectName(),
				&aCertificates[i]->IssuerName());
			}
		}
		
	DisplayBoolReturn(ETrue);
	return ETrue;
	}

TBool CUISisAdaptor::DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo, 
	const TCapabilitySet& aCapabilitySet)
   {
   
   INFO_PRINTF1(_L("DisplayGrantCapabilitiesL():"));
  	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	// Build requested capabilities string.
	const TInt KMaxBufSize=512; // should be enough for all capability names
	const TInt KMaxCapName=64; // should be enough for one capability name
	_LIT(KSpace," ");
	HBufC* buf=HBufC::NewLC(KMaxBufSize);
	TPtr p=buf->Des();
	TInt capsAppended=0;
	TBuf8<KMaxCapName> capName8;
	TBuf<KMaxCapName> capName;

	for (TInt i=0; i<ECapability_HardLimit; i++)
		{
		if (aCapabilitySet.HasCapability(static_cast<TCapability>(i)))
			{
			if (capsAppended)
				p.Append(KSpace);
			capName8=reinterpret_cast<const TUint8*>(CapabilityNames[i]);
			capName.Copy(capName8);
			p.Append(capName);
			capsAppended++;
			}
		}

	INFO_PRINTF2(_L("\tRequesting capabilities: %S"),&p);
	CleanupStack::PopAndDestroy(buf);

	DisplayBoolReturn(ETrue);
	return ETrue;
	}


TInt CUISisAdaptor::DisplayLanguageL(const CAppInfo& aAppInfo, 
						 const RArray<TLanguage>& aLanguages)
	{
	INFO_PRINTF1(_L("DisplayLanguageL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* languages=HBufC::NewLC(1024);
	TPtr pLanguages(languages->Des());

	for (TInt i=0; i < aLanguages.Count();)
		{
		pLanguages.AppendFormat(_L("%S"), &KLanguageTable[aLanguages[i]]);
		if (++i  >= aLanguages.Count())
			break;
		pLanguages.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tLanguages: %S"), &pLanguages);
	CleanupStack::PopAndDestroy(languages);

	DisplayIntReturn(0);
	return 0;
	}


TInt CUISisAdaptor::DisplayDriveL(const CAppInfo& aAppInfo,
						 TInt64 /*aSize*/,const RArray<TChar>& aDriveLetters,
						 const RArray<TInt64>& aDriveSpaces)
	{
	INFO_PRINTF1(_L("DisplayDriveL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* drives=HBufC::NewLC(1024);
	TPtr pDrives(drives->Des());

	for (TInt i=0; i < aDriveLetters.Count();)
		{
		pDrives.AppendFormat(_L("%c"), aDriveLetters[i]);
		pDrives.AppendFormat(_L(" (%D bytes)"), I64LOW(aDriveSpaces[i]));
		if (++i  >= aDriveLetters.Count())
			break;
		pDrives.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tDrives: %S"), &pDrives);
	CleanupStack::PopAndDestroy(drives);
	
	DisplayIntReturn(0);
	return 0; // Use first drive
	}


TBool CUISisAdaptor::DisplayUpgradeL(const CAppInfo& aAppInfo,
	const CAppInfo& aExistingAppInfo)
	{
	INFO_PRINTF1(_L("DisplayUpgradeL():"));
	DisplayAppInfo(_L("From App:"), aExistingAppInfo);
	DisplayAppInfo(_L("To App:"), aAppInfo);

	DisplayBoolReturn(ETrue);
	return ETrue;
	}

TBool CUISisAdaptor::DisplayOptionsL(const CAppInfo& aAppInfo,
										const RPointerArray<TDesC>& aOptions,
										RArray<TBool>& aSelections)
	{
	INFO_PRINTF1(_L("DisplayOptionsL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* options=HBufC::NewLC(4096);
	TPtr pOptions(options->Des());

	for (TInt i=0; i < aOptions.Count();)
		{
		pOptions.AppendFormat(_L("%S"), aOptions[i]);
		aSelections[i]= ETrue;
		
		if (++i  >= aOptions.Count())
			break;
		pOptions.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tOptions: %S"), &pOptions);
	CleanupStack::PopAndDestroy(options);

	DisplayBoolReturn(ETrue);
	return 	ETrue;
	}

TBool CUISisAdaptor::HandleInstallEventL(
						const CAppInfo& aAppInfo,
						TInstallEvent   aEvent, 
						TInt            aValue, 
						const TDesC&    aDes)
	{
	INFO_PRINTF1(_L("HandleInstallEventL"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	// update progress
	// We shouldn't cancel on the last dialog, since installation/uninstallation has 
	// finished, and the tests will fail due to cancellation being flagged, but the
	// installation/uninstallation completing with no error, as expected.
	switch (aEvent)
		{
	case EEventSetProgressBarFinalValue:
		INFO_PRINTF2(_L("\tEvent:\tEEventSetProgressBarFinalValue %d"), aValue);
		iCumulativeProgress = 0;
		iFinalProgress = aValue;
		break;

	case EEventUpdateProgressBar:
		INFO_PRINTF4(_L("\tEvent:\tEEventUpdateProgressBar %d - %d/%d"), 
					 aValue, iCumulativeProgress, iFinalProgress);
		iCumulativeProgress += aValue;
		break;
      
	case EEventAbortedInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventAbortedInstall"));
		break;

	case EEventCompletedInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventCompletedInstall"));
		break;

	case EEventOcspCheckEnd:
		INFO_PRINTF1(_L("\tEvent:\tEEventOcspCheckEnd"));
		break;

	case EEventCompletedNoMessage:
		INFO_PRINTF1(_L("\tEvent:\tEEventCompletedNoMessage"));
		break;

	case EEventAbortedUnInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventAbortedUnInstall"));
		break;

	case EEventCompletedUnInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventCompletedUnInstall"));
		break;

	case EEventLeave:
		INFO_PRINTF2(_L("\tEvent:\tEEventLeave %d"), aValue);
		break;

	default:
		INFO_PRINTF4(_L("\tResult:\tUnknown (%d) Value: %d Des %S"), aEvent, aValue, &aDes);
		ASSERT(false);
		}

	return ETrue;
	}

void CUISisAdaptor::HandleCancellableInstallEventL(
						const CAppInfo&          aAppInfo,
						TInstallCancellableEvent aEvent, 
						MCancelHandler&          /*aCancelHandler*/,
						TInt                     aValue, 
						const TDesC&             aDes)
	{
	INFO_PRINTF1(_L("HandleCancellableInstallEventL"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aEvent)
		{
	case EEventRemovingFiles:
		INFO_PRINTF1(_L("\tEvent:\tEEventRemovingFiles"));
      break;
	case EEventShuttingDownApps:
		INFO_PRINTF1(_L("\tEvent:\tEEventShuttingDownApps"));
      break;
	case EEventCopyingFiles:
		INFO_PRINTF1(_L("\tEvent:\tEEventCopyingFiles"));
      break;
	case EEventOcspCheckStart:
		INFO_PRINTF1(_L("\tEvent:\tEEventOcspCheckStart"));
      break;
	
	default:
		INFO_PRINTF4(_L("\tEvent:\tUnknown (%d) Value: %d Des %S"), aEvent, aValue, &aDes);
		}
	}

TBool CUISisAdaptor::DisplaySecurityWarningL(
		const CAppInfo& aAppInfo,
		TSignatureValidationResult aSigValidationResult,
		RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
		RPointerArray<CCertificateInfo>& /*aCertificates*/,
		TBool /*aInstallAnyway*/)
	{
	_LIT(KTrue, "True");
	_LIT(KFalse, "False");
	
	INFO_PRINTF1(_L("DisplaySecurityWarningL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aSigValidationResult)
	{
	case EValidationSucceeded:
		INFO_PRINTF1(_L("\tResult:\tEValidationSucceeded"));
      break;
    case ESignatureSelfSigned:
    	INFO_PRINTF1(_L("\tResult:\tESignatureSelfSigned"));
    	break;
 	case ENoCertificate:
		INFO_PRINTF1(_L("\tResult:\tENoCertificate"));
        break;
	case ECertificateValidationError:
		INFO_PRINTF1(_L("\tResult:\tECertificateValidationError"));
      break;
	case ESignatureNotPresent:
		INFO_PRINTF1(_L("\tResult:\tESignatureNotPresent"));
      break;
	case ESignatureCouldNotBeValidated:
		INFO_PRINTF1(_L("\tResult:\tESignatureCouldNotBeValidated"));
      break;
	case ENoCodeSigningExtension:
		INFO_PRINTF1(_L("\tResult:\tENoCodeSigningExtension"));
      break;
 	case ENoSupportedPolicyExtension:
		INFO_PRINTF1(_L("\tResult:\tENoSupportedPolicyExtension"));
      break;
	case EMandatorySignatureMissing:
		INFO_PRINTF1(_L("\tResult:\tEMandatorySignatureMissing"));
      break;
 	default:
 		INFO_PRINTF2(_L("\tResult:\tUnknown (%d)"), aSigValidationResult);
      break;
 	};

	return ETrue;
	}

TBool CUISisAdaptor::DisplayOcspResultL(const CAppInfo& aAppInfo,
										   TRevocationDialogMessage aMessage, 
										   RPointerArray<TOCSPOutcome>& aOutcomes, 
										   RPointerArray<CCertificateInfo>& aCertificates,
										   TBool aWarningOnly)
	{
	INFO_PRINTF1(_L("DisplayOcspResultL():"));
	INFO_PRINTF6(_L("\t  App: { \"%S\" \"%S\" V%u.%u.%u }"),&aAppInfo.AppName(),
		&aAppInfo.AppVendor(),aAppInfo.AppVersion().iMajor,
		aAppInfo.AppVersion().iMinor,aAppInfo.AppVersion().iBuild);
	INFO_PRINTF2(_L("\t  Msg: %u"),aMessage);
	TInt numCerts=aCertificates.Count();
	TInt numOutcomes=aOutcomes.Count();
	INFO_PRINTF3(_L("\tCerts: %u, outcomes: %u"),numCerts,numOutcomes);
	if (numCerts==numOutcomes)
		{
		for (TInt i=0; i<numCerts; i++)
			{
			INFO_PRINTF5(_L("\t Cert: [%u] DN = \"%S\", outcome = {%d, %d}"),
				i,&aCertificates[i]->SubjectName(),
				aOutcomes[i]->iStatus,aOutcomes[i]->iResult);
			}
		}
	_LIT(KYes,"Yes");
	_LIT(KNo,"No");
	INFO_PRINTF2(_L("\tFatal: %S"), aWarningOnly ? &KNo : &KYes);

	DisplayBoolReturn(ETrue);
	return ETrue;
	}

void CUISisAdaptor::DisplayCannotOverwriteFileL(const CAppInfo& aAppInfo,
						 const CAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName)
	{
	INFO_PRINTF1(_L("DisplayCannotOverwriteFileL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	INFO_PRINTF2(_L("\tFile : %S"), &aFileName);
	DisplayAppInfo(_L("From App:"), aInstalledAppInfo);
	}

TBool CUISisAdaptor::DisplayUninstallL(const CAppInfo& aAppInfo)
	{
	INFO_PRINTF1(_L("DisplayUninstallL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	DisplayBoolReturn(ETrue);
	return ETrue;
	}

TBool CUISisAdaptor::DisplayTextL(const CAppInfo& aAppInfo, 
									 TFileTextOption aOption, const TDesC& aText)
	{
	INFO_PRINTF1(_L("DisplayTextL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aOption)
		{
	case EInstFileTextOptionContinue:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionContinue"));
      break;
	case EInstFileTextOptionSkipOneIfNo:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionSkipOneIfNo"));
      break;
	case EInstFileTextOptionAbortIfNo:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionAbortIfNo"));
      break;
	case EInstFileTextOptionExitIfNo:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionExitIfNo"));
      break;

	default:
		INFO_PRINTF2(_L("Option: Unknown (%d)"), aOption);
		}
		
	INFO_PRINTF2(_L("Text: \"%S\""), &aText);

	DisplayBoolReturn(ETrue);
	return ETrue;
	}

void CUISisAdaptor::DisplayErrorL(const CAppInfo& aAppInfo,
						 TErrorDialog aType, const TDesC& aDes)
	{
	INFO_PRINTF1(_L("DisplayErrorL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	switch (aType)
		{
	case EUiAlreadyInRom:
		ERR_PRINTF1(_L("\tError:\tEUiAlreadyInRom"));
      break;
	case EUiMissingDependency:
		ERR_PRINTF1(_L("\tError:\tEUiMissingDependency"));
      break;
	case EUiRequireVer:
		ERR_PRINTF1(_L("\tError:\tEUiRequireVer"));
      break;
	case EUiRequireVerOrGreater:
		ERR_PRINTF1(_L("\tError:\tEUiRequireVerOrGreater"));
      break;
	case EUiFileCorrupt:
		ERR_PRINTF1(_L("\tError:\tEUiFileCorrupt"));
      break;
	case EUiDiskNotPresent:
		ERR_PRINTF1(_L("\tError:\tEUiDiskNotPresent"));
      break;
	case EUiCannotRead:
		ERR_PRINTF1(_L("\tError:\tEUiCannotRead"));
      break;
	case EUiCannotDelete:
		ERR_PRINTF1(_L("\tError:\tEUiCannotDelete"));
      break;
	case EUiInvalidFileName:
		ERR_PRINTF1(_L("\tError:\tEUiInvalidFileName"));
      break;
	case EUiFileNotFound:
		ERR_PRINTF1(_L("\tError:\tEUiFileNotFound"));
      break;
	case EUiInsufficientSpaceOnDrive:
		ERR_PRINTF1(_L("\tError:\tEUiInsufficientSpaceOnDrive"));
      break;
	case EUiCapabilitiesCannotBeGranted:
		ERR_PRINTF1(_L("\tError:\tEUiCapabilitiesCannotBeGranted"));
      break;
	case EUiUnknownFile:
		ERR_PRINTF1(_L("\tError:\tEUiUnknownFile"));
      break;
	case EUiMissingBasePackage:
		ERR_PRINTF1(_L("\tError:\tEUiMissingBasePackage"));
      break;
	case EUiConstraintsExceeded:
		ERR_PRINTF1(_L("Constraints imposed by a developer mode certificate have been exceeded."));
      break;
	case EUiSIDViolation:
		ERR_PRINTF1(_L("The package contains a file with a protected SID which is not signed for."));
      break;
	case EUiVIDViolation:
		ERR_PRINTF1(_L("The package contains a file with a protected VID which is not signed for.	"));
      break;
	case EUiNoMemoryInDrive:
		ERR_PRINTF1(_L(" No enough space in the selected drive to install the package."));
      break;
	case EUiUIDPackageViolation:
		ERR_PRINTF1(_L("The package uid is protected but not signed for."));
      break;
	case EUiOSExeViolation:
		ERR_PRINTF1(_L("The package contains an invalid eclipsing EXE that would eclipse an exe in ROM.	"));
      break;

	default:
		ERR_PRINTF3(_L("\tError:\tUnknown (%d) Des %S"), aType, &aDes);
	 	};

	}

TBool CUISisAdaptor::DisplayDependencyBreakL(const CAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents)
	{
	INFO_PRINTF1(_L("DisplayDependencyBreakL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* components=HBufC::NewLC(1024);
	TPtr pComponents(components->Des());

	for (TInt i=0; i < aComponents.Count();)
		{
		pComponents.AppendFormat(_L("%S"), aComponents[i]);
		if (++i  >= aComponents.Count())
			break;
		pComponents.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tComponents: %S"), &pComponents);
	CleanupStack::PopAndDestroy(components);

	DisplayBoolReturn(ETrue);
	return ETrue;
	}

TBool CUISisAdaptor::DisplayApplicationsInUseL(const CAppInfo& aAppInfo, 
							const RPointerArray<TDesC>& aAppNames)
	{
	INFO_PRINTF1(_L("DisplayApplicationsInUseL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* applications=HBufC::NewLC(1024);
	TPtr pApplications(applications->Des());

	for (TInt i=0; i < aAppNames.Count();)
		{
		pApplications.AppendFormat(_L("%S"), aAppNames[i]);
		if (++i  >= aAppNames.Count())
			break;
		pApplications.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tComponents: %S"), &pApplications);
	CleanupStack::PopAndDestroy(applications);

	DisplayBoolReturn(ETrue);
	return ETrue;
	}

TBool CUISisAdaptor::DisplayQuestionL(const CAppInfo& aAppInfo, 
							TQuestionDialog aQuestion, const TDesC& aDes)
	{
	INFO_PRINTF1(_L("DisplayQuestionL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aQuestion)
		{
	case EQuestionIncompatible:
		INFO_PRINTF1(_L("\tQuestion:\tEQuestionIncompatible"));
      break;
	
	case EQuestionOverwriteFile:
		INFO_PRINTF1(_L("\tQuestion:\tEQuestionOverwriteFile"));
		INFO_PRINTF2(_L("\tFile Name:\t%S"), &aDes);
      break;

	default:
		INFO_PRINTF3(_L("\tQuestion:\tUnknown (%d) Des %S"), aQuestion, &aDes);
		}
	
	DisplayBoolReturn(ETrue);
	return ETrue;
	}


TBool CUISisAdaptor::DisplayMissingDependencyL(const CAppInfo& aAppInfo,
	const TDesC& aDependencyName,TVersion aWantedVersionFrom,
	TVersion aWantedVersionTo,TVersion aInstalledVersion)
	{
	INFO_PRINTF1(_L("DisplayMissingDependencyL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	INFO_PRINTF2(_L("\tMissing dependency name: %S"),&aDependencyName);
	INFO_PRINTF7(_L("\t         Wanted version: %u.%u.%u - %u.%u.%u"),
		aWantedVersionFrom.iMajor,aWantedVersionFrom.iMinor,
		aWantedVersionFrom.iBuild,
		aWantedVersionTo.iMajor,aWantedVersionTo.iMinor,
		aWantedVersionTo.iBuild);
	INFO_PRINTF4(_L("\t       Existing version: %u.%u.%u"),
		aInstalledVersion.iMajor,aInstalledVersion.iMinor,
		aInstalledVersion.iBuild);

	DisplayBoolReturn(ETrue);
	return ETrue;
	}
// End of file
