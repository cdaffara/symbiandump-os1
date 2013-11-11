/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#define __INCLUDE_CAPABILITY_NAMES__

#include <e32test.h>
#include <f32file.h>
#include <test/testexecutelog.h>
#include <swi/launcher.h>
#include <e32capability.h>
#include "ctlbsinstallui.h"

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
	INFO_PRINTF1(_L("DisplayIntReturn():"));
	_LIT(KTrue, "True");
	_LIT(KFalse, "False");
	INFO_PRINTF2(_L("\tReturning %S"), aReturn ? &KTrue : &KFalse);
	}

void CUISisAdaptor::DisplayIntReturn(TInt aReturn)
	{
	INFO_PRINTF1(_L("DisplayIntReturn():"));
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
TBool CUISisAdaptor::DisplayInstallL(const CAppInfo& /*aAppInfo*/,
										const CApaMaskedBitmap* /*aLogo*/,
										const RPointerArray<CCertificateInfo>& /*aCertificates*/)
	{
	INFO_PRINTF1(_L("DisplayInstallL():"));
	return ETrue;
	}

TBool CUISisAdaptor::DisplayGrantCapabilitiesL(const CAppInfo& /*aAppInfo*/, 
	const TCapabilitySet& /*aCapabilitySet*/)
	{
	
	INFO_PRINTF1(_L("DisplayGrantCapabilitiesL():"));
	return ETrue;
	}


TInt CUISisAdaptor::DisplayLanguageL(const CAppInfo& /*aAppInfo*/, 
						 const RArray<TLanguage>& /*aLanguages*/)
	{
	INFO_PRINTF1(_L("DisplayLanguageL():"));
	return 0;
	}


TInt CUISisAdaptor::DisplayDriveL(const CAppInfo& /*aAppInfo*/,
						 TInt64 /*aSize*/,const RArray<TChar>& /*aDriveLetters*/,
						 const RArray<TInt64>& /*aDriveSpaces*/)
	{
	INFO_PRINTF1(_L("DisplayDriveL():"));
	return 0; // Use first drive
	}


TBool CUISisAdaptor::DisplayUpgradeL(const CAppInfo& /*aAppInfo*/,
	const CAppInfo& /*aExistingAppInfo*/)
	{
	INFO_PRINTF1(_L("DisplayUpgradeL():"));
	return ETrue;
	}

TBool CUISisAdaptor::DisplayOptionsL(const CAppInfo& /*aAppInfo*/,
										const RPointerArray<TDesC>& /*aOptions*/,
										RArray<TBool>& /*aSelections*/)
	{
	INFO_PRINTF1(_L("DisplayOptionsL():"));
	return 	ETrue;
	}

TBool CUISisAdaptor::HandleInstallEventL(
						const CAppInfo& /*aAppInfo*/,
						TInstallEvent   /*aEvent*/, 
						TInt            /*aValue*/, 
						const TDesC&    /*aDes*/)
	{
	INFO_PRINTF1(_L("HandleInstallEventL"));
	return ETrue;
	}

void CUISisAdaptor::HandleCancellableInstallEventL(
						const CAppInfo&          /*aAppInfo*/,
						TInstallCancellableEvent /*aEvent*/, 
						MCancelHandler&          /*aCancelHandler*/,
						TInt                     /*aValue*/, 
						const TDesC&             /*aDes*/)
	{
	INFO_PRINTF1(_L("HandleCancellableInstallEventL"));
	}

TBool CUISisAdaptor::DisplaySecurityWarningL(
		const CAppInfo& /*aAppInfo*/,
		TSignatureValidationResult /*aSigValidationResult*/,
		RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
		RPointerArray<CCertificateInfo>& /*aCertificates*/,
		TBool /*aInstallAnyway*/)
	{
	INFO_PRINTF1(_L("DisplaySecurityWarningL():"));
	return ETrue;
	}

TBool CUISisAdaptor::DisplayOcspResultL(const CAppInfo& /*aAppInfo*/,
										   TRevocationDialogMessage /*aMessage*/, 
										   RPointerArray<TOCSPOutcome>& /*aOutcomes*/, 
										   RPointerArray<CCertificateInfo>& /*aCertificates*/,
										   TBool /*aWarningOnly*/)
	{
	INFO_PRINTF1(_L("DisplayOcspResultL():"));
	return ETrue;
	}

void CUISisAdaptor::DisplayCannotOverwriteFileL(const CAppInfo& /*aAppInfo*/,
						 const CAppInfo& /*aInstalledAppInfo*/,
						 const TDesC& /*aFileName*/)
	{
	INFO_PRINTF1(_L("DisplayCannotOverwriteFileL():"));
	}

TBool CUISisAdaptor::DisplayUninstallL(const CAppInfo& /*aAppInfo*/)
	{
	INFO_PRINTF1(_L("DisplayUninstallL():"));
	return ETrue;
	}

TBool CUISisAdaptor::DisplayTextL(const CAppInfo& /*aAppInfo*/, 
									 TFileTextOption /*aOption*/, const TDesC& /*aText*/)
	{
	INFO_PRINTF1(_L("DisplayTextL():"));
	return ETrue;
	}

void CUISisAdaptor::DisplayErrorL(const CAppInfo& /*aAppInfo*/,
						 TErrorDialog /*aType*/, const TDesC& /*aDes*/)
	{
	INFO_PRINTF1(_L("DisplayErrorL():"));
	}

TBool CUISisAdaptor::DisplayDependencyBreakL(const CAppInfo& /*aAppInfo*/,
					      const RPointerArray<TDesC>& /*aComponents*/)
	{
	INFO_PRINTF1(_L("DisplayDependencyBreakL():"));
	return ETrue;
	}

TBool CUISisAdaptor::DisplayApplicationsInUseL(const CAppInfo& /*aAppInfo*/, 
							const RPointerArray<TDesC>& /*aAppNames*/)
	{
	INFO_PRINTF1(_L("DisplayApplicationsInUseL():"));
	return ETrue;
	}

TBool CUISisAdaptor::DisplayQuestionL(const CAppInfo& /*aAppInfo*/, 
							TQuestionDialog /*aQuestion*/, const TDesC& /*aDes*/)
	{
	INFO_PRINTF1(_L("DisplayQuestionL():"));
	return ETrue;
	}


TBool CUISisAdaptor::DisplayMissingDependencyL(const CAppInfo& /*aAppInfo*/,
	const TDesC& /*aDependencyName*/,TVersion /*aWantedVersionFrom*/,
	TVersion /*aWantedVersionTo*/,TVersion /*aInstalledVersion*/)
	{
	INFO_PRINTF1(_L("DisplayMissingDependencyL():"));
	return ETrue;
	}
// End of file
