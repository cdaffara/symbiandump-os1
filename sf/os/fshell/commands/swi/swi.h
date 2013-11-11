// swi.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef _SWI_CMD_
#define _SWI_CMD_

#include <fshell/ioutils.h>
#include <e32cmn.h>
#include <swi/asynclauncher.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>

#ifndef SYMBIAN_JAVA_NOT_INCLUDED
	#include <midpinstallationlaunchinfo.h>
	#include <midletsuiteattributes.h>
	#include <javafilehandle.h>
	#include <javainstaller.h>
	#include <javaremover.h>
	#include <mjavainstallerui.h>
	#include <mjardownloaderui.h>
	#include <mjavaregistry.h>
#endif // SYMBIAN_JAVA_NOT_INCLUDED

using namespace IoUtils;
using namespace Swi;

//
// MCmdSwiParent
//
class MCmdSwiParent
	{
public:
	virtual RIoConsoleReadHandle& Input() = 0;
	virtual RIoConsoleWriteHandle& Output(TInt aError = KErrNone) = 0;
	virtual void Finished(const TInt aError) = 0;
	virtual TBool GetAnswer() = 0;
	};

#ifndef SYMBIAN_JAVA_NOT_INCLUDED
	//
	// CSwiMidletInstallerAO
	// active object managing the actual installation/uninstallation of jar/jad files
	//
	class CSwiMidletInstallerAO : public CActive, public MJavaInstallerUI, public MJarDownloaderUI, public MJavaRemoverUI
		{
	public:
		static CSwiMidletInstallerAO* NewL(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet);
		~CSwiMidletInstallerAO();
		
		void InstallL(TFileName& aInstallFile);
		void UninstallL(const TUid& aPackageUid);
		void ListInstalledAppsL(TUid aAppUid, const TDesC& aMatchString);
		TBool IsJad(TDes& aMidlet);
		TBool IsJar(TDes& aMidlet);
	
		// java UI handlers
		virtual TBool StartInstallL(const CMIDletSuiteAttributes& aMIDlet);
		virtual TBool SelectDriveL(const CMIDletSuiteAttributes& aMIDlet, TInt aSpaceRequired, TChar& aDrive, TBool& aSufficientSpace);
		virtual TBool ReplaceExistingMIDletL(const CMIDletSuiteAttributes& aMIDlet, const TAppVersion& aOldVersion);
		virtual TBool UpgradeRMSL(const CMIDletSuiteAttributes& aMIDlet, const TAppVersion& aOldVersion);
		virtual TBool MIDletUntrustedL(const CMIDletSuiteAttributes& aMIDlet);
		virtual void CertificateHasNoRootL(const CMIDletSuiteAttributes& aMIDlet, const CPKIXCertChain& aCertChain, const CPKIXValidationResult& aValidationResult);
		virtual void BadSignatureL(const CMIDletSuiteAttributes& aMIDlet, const CPKIXCertChain& aCertChain, const CPKIXValidationResult& aValidationResult);
		virtual TBool PerformRevocationCheckL(TBool& aDoCheck);
		virtual void StartRevocationCheckL();
		virtual void FinishedRevocationCheckL();
		virtual void StartIconConversionL();
		virtual void FinishedIconConversionL();
		virtual TBool OCSPWarningL(const CMIDletSuiteAttributes& aMIDlet, TRevocationMsg aRevocationMsg, const TOCSPOutcome* aOCSPOutcome);
		virtual void OCSPErrorL(const CMIDletSuiteAttributes& aMIDlet, TRevocationMsg aRevocationMsg, const TOCSPOutcome* aOCSPOutcome);
		virtual TBool MIDletInUseL(const CMIDletSuiteAttributes& aMIDlet);
		virtual void OTAReportResponseL(TInt aOTAResponseCode, const TDesC& aReportBody);
		virtual TInt CopyStarted(TInt aSize);
		virtual TInt DownloadStarted(TInt aSize);
		virtual TInt UpdateProgress(TInt aSize);
		virtual TBool GetUsernamePasswordL(HBufC8*& aUsername, HBufC8*& aPassword);	
		virtual TBool ConfirmRemovalL(const TDesC& aRemovalMessage);
		virtual void InitialiseProgressBarL(TInt aMaximumLength);
		virtual void UpdateProgressBarL(TInt aUnits);
		virtual TBool MIDletInUseL();
		virtual TBool FileInUseL(const TDesC& aFileName);
	private:
		CSwiMidletInstallerAO(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet);
		void ConstructL();
		void DisplayPackageL(MJavaRegistryMIDletEntry& aPackage);
		
		inline RIoConsoleReadHandle& Stdin(){return iParent.Input();}
		inline RIoConsoleWriteHandle& Stdout(){return iParent.Output();}
		inline RIoConsoleWriteHandle& Stderr(){return iParent.Output(KErrGeneral);}
	
		// from CActive
		void DoCancel();
		void RunL();
	private:
		MCmdSwiParent& iParent;
		RFs& iFs;
		TBool iVerbose;
		TBool iQuiet;
		CJavaInstaller* iInstaller;
		CJavaFileHandle* iJadHandle;
		CJavaFileHandle* iJarHandle;
		CMIDPInstallationLaunchInfo* iInfo;
		CJavaRemover* iRemover;
		};
#endif // SYMBIAN_JAVA_NOT_INCLUDED

//
// CSwiSisInstallerAO
// active object managing the actual installation/uninstallation of sis files
//
class CSwiSisInstallerAO : public CActive, public Swi::MUiHandler
	{
public:
	static CSwiSisInstallerAO* NewL(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet);
	~CSwiSisInstallerAO();
	
	// user commands
	void InstallL(TFileName& aInstallFile);
	void UninstallL(const TUid& aPackageUid);
	void ListInstalledAppsL(const TUid& aAppUid, const TDesC& aMatchString);
	
	// Swi UI handlers
	virtual TBool DisplayTextL(const Swi::CAppInfo& aAppInfo, Swi::TFileTextOption aOption, const TDesC& aText);
	virtual void DisplayErrorL(const Swi::CAppInfo& aAppInfo, Swi::TErrorDialog aType, const TDesC& aParam);
	virtual TBool DisplayDependencyBreakL(const Swi::CAppInfo& aAppInfo, const RPointerArray<TDesC>& aComponents);
	virtual TBool DisplayApplicationsInUseL(const Swi::CAppInfo& aAppInfo, const RPointerArray<TDesC>& aAppNames);
	virtual TBool DisplayQuestionL(const Swi::CAppInfo& aAppInfo, Swi::TQuestionDialog aQuestion, const TDesC& aDes=KNullDesC);
	virtual TBool DisplayInstallL(const Swi::CAppInfo& aAppInfo, const CApaMaskedBitmap* aLogo, const RPointerArray<Swi::CCertificateInfo>& aCertificates);
	virtual TBool DisplayGrantCapabilitiesL(const Swi::CAppInfo& aAppInfo, const TCapabilitySet& aCapabilitySet);
	virtual TInt DisplayLanguageL(const Swi::CAppInfo& aAppInfo, const RArray<TLanguage>& aLanguages);
	virtual TInt DisplayDriveL(const Swi::CAppInfo& aAppInfo, TInt64 aSize, const RArray<TChar>& aDriveLetters, const RArray<TInt64>& aDriveSpaces);
	virtual TBool DisplayUpgradeL(const Swi::CAppInfo& aAppInfo, const Swi::CAppInfo& aExistingAppInfo);
	virtual TBool DisplayOptionsL(const Swi::CAppInfo& aAppInfo, const RPointerArray<TDesC>& aOptions, RArray<TBool>& aSelections);
	virtual TBool HandleInstallEventL(const Swi::CAppInfo& aAppInfo, Swi::TInstallEvent aEvent, TInt aValue=0, const TDesC& aDes=KNullDesC);
	virtual void HandleCancellableInstallEventL(const Swi::CAppInfo& aAppInfo, Swi::TInstallCancellableEvent aEvent, Swi::MCancelHandler& aCancelHandler, TInt aValue=0, const TDesC& aDes=KNullDesC);
	virtual TBool DisplaySecurityWarningL(const Swi::CAppInfo& aAppInfo, Swi::TSignatureValidationResult aSigValidationResult, RPointerArray<CPKIXValidationResultBase>& aPkixResults, RPointerArray<Swi::CCertificateInfo>& aCertificates, TBool aInstallAnyway);
	virtual TBool DisplayOcspResultL(const Swi::CAppInfo& aAppInfo, Swi::TRevocationDialogMessage aMessage, RPointerArray<TOCSPOutcome>& aOutcomes, RPointerArray<Swi::CCertificateInfo>& aCertificates, TBool aWarningOnly);
	virtual void DisplayCannotOverwriteFileL(const Swi::CAppInfo& aAppInfo, const Swi::CAppInfo& aInstalledAppInfo, const TDesC& aFileName);
	virtual TBool DisplayMissingDependencyL(const Swi::CAppInfo& aAppInfo, const TDesC& aDependencyName, TVersion aWantedVersionFrom, TVersion aWantedVersionTo, TVersion aInstalledVersion);
	virtual TBool DisplayUninstallL(const Swi::CAppInfo& aAppInfo);
private:
	CSwiSisInstallerAO(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet);
	void ConstructL();
	
	void PrintDetails(Swi::CSisRegistryPackage& aPackage);
	void DisplayPackageL(const TUid& aPackageUid);
	Swi::CSisRegistryPackage* GetSisRegistryPackageL(const TUid& aPackageUid);

	inline RIoConsoleReadHandle& Stdin(){return iParent.Input();}
	inline RIoConsoleWriteHandle& Stdout(){return iParent.Output();}
	inline RIoConsoleWriteHandle& Stderr(){return iParent.Output(KErrGeneral);}
		
	// from CActive
	void DoCancel();
	void RunL();
private:
	MCmdSwiParent& iParent;
	TBool iVerbose;
	TBool iQuiet;
	CAsyncLauncher* iLauncher;
	Swi::RSisRegistrySession iRegistrySession;
	Swi::CInstallPrefs* iPrefs;
	Swi::CSisRegistryPackage* iRegPackage;
	TChar iCurrentDrive;
	};

//
// CCmdSwi
// Fshell-console based class handling swi related functionality
//
class CCmdSwi : public CCommandBase, public MCmdSwiParent, public MCommandExtensionsV1
	{
public:
	static CCommandBase* NewLC();
	~CCmdSwi();
private:
	CCmdSwi();

	// utilities
	TBool IsMidlet();
	
	// fshell hooks
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	const TDesC* StringifyError(TInt aError) const;
		
	// MCmdSwiParent hooks 	
	virtual RIoConsoleReadHandle& Input();
	virtual RIoConsoleWriteHandle& Output(TInt aError);
	virtual void Finished(TInt aError);
	virtual TBool GetAnswer();
private:
#ifndef SYMBIAN_JAVA_NOT_INCLUDED
	CSwiMidletInstallerAO* iMidletHandler;
#endif
	CSwiSisInstallerAO* iSisHandler;
	TFileName2 iSisFile;
	enum 
		{
		EList,
		EInstall,
		EUninstall,
		} iCommand;
	TUid iUid;
	HBufC* iMatch;
	TBool iVerbose;
	TBool iQuiet;
	};

#endif // _SWI_CMD_
