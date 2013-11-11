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
// tuiscriptadaptors.h
// 
//

#ifndef __MSP_TUI_H__
#define __MSP_TUI_H__


#include <Swi/msisuihandlers.h>
#include <test/testexecutelogger.h>

using namespace Swi;

const TInt KBufSize = 5;

enum TFunctions{
	EScript = 0,
	EDisplayInstall,
    EDisplayGrantCapabilities,
	EDisplayLanguage,
	EDisplayDrive,
	EDisplayUpgrade,
	EDisplayOptions,
	EHandleInstallEvent,
	EHandleCancellableInstallEvent,
	EDisplaySecurityWarning,
	EDisplayOcspResult,
	EDisplayCannotOverwriteFile,
	EDisplayUninstall,
	EDisplayText,
	EDisplayError,
	EDisplayDependencyBreak,
	EDisplayApplicationsInUse,
	EDisplayQuestion,
	EDisplayMissingDependency,
};

enum TLeveltags{
	EItemIndex =1,
	EReturn
};


class CUISisAdaptor : public CBase, public MUiHandler
	{
public:	
	~CUISisAdaptor();
	static CUISisAdaptor* NewL(CTestExecuteLogger& aLogger);
	static CUISisAdaptor* NewLC(CTestExecuteLogger& aLogger);
	
	// Displays install dialog, logging all incoming information.
	TBool DisplayInstallL(const CAppInfo& aAppInfo,
						  const CApaMaskedBitmap* aLogo,
						  const RPointerArray<CCertificateInfo>& aCertificates);

	TBool DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo,
									const TCapabilitySet& aCapabilitySet);

	TInt DisplayLanguageL(const CAppInfo& aAppInfo, 
						  const RArray<TLanguage>& aLanguages);
	
	TInt DisplayDriveL(const CAppInfo& aAppInfo,TInt64 aSize,
					   const RArray<TChar>& aDriveLetters,
					   const RArray<TInt64>& aDriveSpaces);
	
	TBool DisplayUpgradeL(const CAppInfo& aAppInfo,
						  const CAppInfo& aExistingAppInfo);
	
	TBool DisplayOptionsL(const CAppInfo& aAppInfo, 
						  const RPointerArray<TDesC>& aOptions,
						  RArray<TBool>& aSelections); 
	
	TBool HandleInstallEventL(const CAppInfo& aAppInfo,
							  TInstallEvent aEvent, TInt aValue=0, 
							  const TDesC& aDes=KNullDesC);
	
	void HandleCancellableInstallEventL(const CAppInfo& aAppInfo,
										TInstallCancellableEvent aEvent, 
										MCancelHandler& aCancelHandler,TInt aValue=0, 
										const TDesC& aDes=KNullDesC);
	
	TBool DisplaySecurityWarningL(const CAppInfo& aAppInfo,
								  TSignatureValidationResult aSigValidationResult,
								  RPointerArray<CPKIXValidationResultBase>& aPkixResults,
								  RPointerArray<CCertificateInfo>& aCertificates,
								  TBool aInstallAnyway);

	TBool DisplayOcspResultL(const CAppInfo& aAppInfo,
							 TRevocationDialogMessage aMessage, 
							 RPointerArray<TOCSPOutcome>& aOutcomes,
							 RPointerArray<CCertificateInfo>& aCertificates,
							 TBool aWarningOnly);

	void DisplayCannotOverwriteFileL(const CAppInfo& aAppInfo,
									 const CAppInfo& aInstalledAppInfo,
									 const TDesC& aFileName);

	TBool DisplayUninstallL(const CAppInfo& aAppInfo);
	
	TBool DisplayTextL(const CAppInfo& aAppInfo, 
					   TFileTextOption aOption,
					   const TDesC& aText);

	void DisplayErrorL(const CAppInfo& aAppInfo,
					   TErrorDialog aType, 
					   const TDesC& aParam);
	
	TBool DisplayDependencyBreakL(const CAppInfo& aAppInfo,
								  const RPointerArray<TDesC>& aComponents);
	
	TBool DisplayApplicationsInUseL(const CAppInfo& aAppInfo, 
									const RPointerArray<TDesC>& aAppNames);
	
	TBool DisplayQuestionL(const CAppInfo& aAppInfo, 
						   TQuestionDialog aQuestion, const TDesC& aDes=KNullDesC);
	
	TBool DisplayMissingDependencyL(const CAppInfo& aAppInfo,
									const TDesC& aDependencyName,TVersion aWantedVersionFrom,
									TVersion aWantedVersionTo,TVersion aInstalledVersion);
	
	// functions to handle displaying info to file
	void DisplayAppInfo(const TDesC& aPreText, const CAppInfo& aAppInfo);
	void DisplayBoolReturn(TBool aReturn);
	void DisplayIntReturn(TInt aReturn);
	
	// function to check progress bar
	void CheckFinalProgressBarL();

	// Function that aborts a test case when the expected
	// installation size does not match the calculated size.
	void CheckExpectedAndCalculatedInstallSizesL(const TDesC& aAppInfoName, TInt64 aSize);

	// function to check if certain dialogs have been called
	TBool GrantUserCapsDialogCalled() const;
		
	// Use to enable and disable cancel testing inside dialogs
	void SetCancelDialogTesting(TBool aCancelTesting = ETrue);
	
	// Set the dialog number to cancel on
	void SetCancelDialogNumber(TInt aDialogNumber);

	// Use to enable and disable dialog delays (for async cancel testing)
	void SetDialogDelay(TTimeIntervalMicroSeconds32 aUsec);

	// Reset the current dialog number, and set iInstallationCancelled to False
	void ResetCurrentDialogNumber(TInt aDialogNumber = 0);
	TBool InstallationCancelled() const;
	
	// Use the same log macro than testexecute
	inline CTestExecuteLogger& Logger() {return iLogger;};

private:
	CUISisAdaptor();
	void ConstructL(CTestExecuteLogger& aLogger);				 
	void ScriptParseL();
	TInt iFinalProgress;
	TInt iCumulativeProgress;
	CTestExecuteLogger iLogger;
	};

#endif // __MSP_TUI_H__
