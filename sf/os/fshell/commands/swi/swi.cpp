// swi.cpp
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

#include "swi.h"
#include <fshell/common.mmh>
#include <fshell/ltkutils.h>

typedef TBuf<1> TUserInput;
_LIT(KSwiYes,	"y");
_LIT(KSwiNo,	"n");
_LIT(KSwiJarExtension,	"*.jar");
_LIT(KSwiJadExtension,	"*.jad");
#ifndef SYMBIAN_JAVA_NOT_INCLUDED
_LIT8(KDefaultJadMimeType, "text/vnd.sun.j2me.app-descriptor");
_LIT8(KDefaultJarMimeType, "application/java-archive");
#endif
_LIT(KDefaultMatch, "*");

//
// CCmdSwi
// class to handle the fshell console user interaction
//
CCommandBase* CCmdSwi::NewLC()
	{
	CCmdSwi* self = new(ELeave) CCmdSwi();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSwi::~CCmdSwi()
	{
#ifndef SYMBIAN_JAVA_NOT_INCLUDED
	delete iMidletHandler;
#endif
	delete iSisHandler;
	delete iMatch;
	}

CCmdSwi::CCmdSwi():
CCommandBase(CCommandBase::EManualComplete)
	{
	SetExtension(this);
	}

const TDesC& CCmdSwi::Name() const
	{
	_LIT(KName, "swi");
	return KName;
	}

void CCmdSwi::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCommand, _L("operation"));

	_LIT(KArgFilename, "sis-file");
	aArguments.AppendFileNameL(iSisFile, KArgFilename);
	}
	
void CCmdSwi::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptUid, "uid");
	aOptions.AppendUintL((TUint&)iUid.iUid, KOptUid);

	_LIT(KOptVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KOptVerbose);

	_LIT(KOptQuiet, "quiet");
	aOptions.AppendBoolL(iQuiet, KOptQuiet);

	_LIT(KOptMatch, "match");
	aOptions.AppendStringL(iMatch, KOptMatch);
	}

void CCmdSwi::DoRunL()
	{
	if (iMatch == NULL)
		{
		iMatch = KDefaultMatch().AllocL();
		}

	iSisHandler = CSwiSisInstallerAO::NewL(*this, FsL(), iVerbose, iQuiet);
#ifndef SYMBIAN_JAVA_NOT_INCLUDED
	iMidletHandler = CSwiMidletInstallerAO::NewL(*this, FsL(), iVerbose, iQuiet);
#endif

	switch (iCommand)
		{
	case EList:
		//TODO matching
		TRAPL(iSisHandler->ListInstalledAppsL(iUid, *iMatch), _L("Couldn't list installed SIS files"));
#ifndef SYMBIAN_JAVA_NOT_INCLUDED
		TRAPL(iMidletHandler->ListInstalledAppsL(iUid, *iMatch), _L("Couldn't list installed midlets"));
#endif
		Complete(KErrNone);
		break;
		
	case EUninstall:
		if (!iOptions.IsPresent(&iUid))
			{
			LeaveIfErr(KErrArgument, _L("UID must be specified for \"uninstall\""));
			}
		TRAPD(error, iSisHandler->UninstallL(iUid));
		if (error != KErrNone)
			{
#ifndef SYMBIAN_JAVA_NOT_INCLUDED
			// possibly a midlet
			TRAP(error, iMidletHandler->UninstallL(iUid));
#endif
			}
		LeaveIfErr(error, _L("Failed to uninstall package 0x%08x"), iUid.iUid);
		break;
		
	case EInstall:
		if (!iArguments.IsPresent(1))
			{
			LeaveIfErr(KErrArgument, _L("SIS file must be specified for \"install\""));
			}
		User::LeaveIfError(Stdin().SetReadMode(RIoReadHandle::ELine));
#ifndef SYMBIAN_JAVA_NOT_INCLUDED
		if (IsMidlet())
			{
			TRAPL(iMidletHandler->InstallL(iSisFile), _L("Failed to install midlet '%S'"), &iSisFile);
			}
		else
#endif
			{
			TRAPL(iSisHandler->InstallL(iSisFile), _L("Failed to install sis '%S'"), &iSisFile);
			}
		break;
		}

	}
	
TBool CCmdSwi::IsMidlet()
	{
	if ((iSisFile.MatchF(KSwiJarExtension) != KErrNotFound) || (iSisFile.MatchF(KSwiJadExtension) != KErrNotFound))
		return ETrue;
	return EFalse;
	}

//
// CCmdSwi::GetAnswer
// only for use when not in quiet mode
// Reads input from stdin, determines whether it's a 'yes' or 'no', returning ETrue, EFalse respectively
//
TBool CCmdSwi::GetAnswer()
	{
	ASSERT(!iQuiet);
	TUserInput in;
	for (;;)
		{
		if (Stdin().Read(in) != KErrNone)
			{
			return EFalse;
			}
		Stdout().Write(in);
		if (in.MatchF(KSwiYes) == 0)
			{
			return ETrue;
			}
		if (in.MatchF(KSwiNo) == 0)
			{
			return EFalse;
			}
		// else it's an unrecognised response
		}
	}

//
// MCmdSwiParent hooks
//
RIoConsoleReadHandle& CCmdSwi::Input()
	{
	return Stdin();
	}

RIoConsoleWriteHandle& CCmdSwi::Output(TInt aError)
	{
	if (aError == KErrNone)
		{
		return Stdout();
		}
	return Stderr();
	}

//
// CCmdSwi::Finished
// callback from the underlying AO's RunL
//
void CCmdSwi::Finished(TInt aError)
	{
	Complete(aError);
	}

#define CASE_RETURN_LIT(XXX, YYY) case XXX: { _LIT(_KLit, #YYY); return &_KLit; }
const TDesC* CCmdSwi::StringifyError(TInt aError) const
	{
	switch (aError)
		{
		CASE_RETURN_LIT(-10100, KErrSISFieldIdMissing);
		CASE_RETURN_LIT(-10101, KErrSISFieldLengthMissing);
		CASE_RETURN_LIT(-10102, KErrSISFieldLengthInvalid);
		CASE_RETURN_LIT(-10103, KErrSISStringInvalidLength);
		CASE_RETURN_LIT(-10104, KErrSISSignedControllerSISControllerMissing);
		CASE_RETURN_LIT(-10105, KErrSISControllerSISInfoMissing);
		CASE_RETURN_LIT(-10106, KErrSISInfoSISUidMissing);
		CASE_RETURN_LIT(-10107, KErrSISInfoSISNamesMissing);
		CASE_RETURN_LIT(-10108, KErrSISFieldBufferTooShort);
		CASE_RETURN_LIT(-10109, KErrSISStringArrayInvalidElement);
		CASE_RETURN_LIT(-10110, KErrSISInfoSISVendorNamesMissing);
		CASE_RETURN_LIT(-10111, KErrSISInfoSISVersionMissing);
		CASE_RETURN_LIT(-10112, KErrSISControllerSISSupportedLanguagesMissing);
		CASE_RETURN_LIT(-10113, KErrSISSupportedLanguagesInvalidElement);
		CASE_RETURN_LIT(-10114, KErrSISLanguageInvalidLength);
		CASE_RETURN_LIT(-10115, KErrSISContentsSISSignedControllerMissing);
		CASE_RETURN_LIT(-10116, KErrSISContentsSISDataMissing);
		CASE_RETURN_LIT(-10117, KErrSISDataSISFileDataUnitMissing);
		CASE_RETURN_LIT(-10118, KErrSISFileDataUnitTargetMissing);
		CASE_RETURN_LIT(-10119, KErrSISFileOptionsMissing);
		CASE_RETURN_LIT(-10120, KErrSISFileDataUnitDescriptorMissing);
		CASE_RETURN_LIT(-10121, KErrSISFileDataDescriptionMissing);
		CASE_RETURN_LIT(-10122, KErrSISContentsMissing);
		CASE_RETURN_LIT(-10123, KErrSISEmbeddedControllersMissing);
		CASE_RETURN_LIT(-10124, KErrSISEmbeddedDataUnitsMissing);
		CASE_RETURN_LIT(-10125, KErrSISControllerOptionsMissing);
		CASE_RETURN_LIT(-10126, KErrSISExpressionMissing);
		CASE_RETURN_LIT(-10127, KErrSISExpressionStringValueMissing);
		CASE_RETURN_LIT(-10128, KErrSISOptionsStringMissing);
		CASE_RETURN_LIT(-10129, KErrSISFileOptionsExpressionMissing);
		CASE_RETURN_LIT(-10130, KErrSISExpressionHeadValueMissing);
		CASE_RETURN_LIT(-10131, KErrSISEmbeddedSISOptionsMissing);
		CASE_RETURN_LIT(-10132, KErrSISInfoSISUpgradeRangeMissing);
		CASE_RETURN_LIT(-10133, KErrSISDependencyMissingUid);
		CASE_RETURN_LIT(-10134, KErrSISDependencyMissingVersion);
		CASE_RETURN_LIT(-10135, KErrSISDependencyMissingNames);
		CASE_RETURN_LIT(-10136, KErrSISPrerequisitesMissingDependency);
		CASE_RETURN_LIT(-10137, KErrSISControllerMissingPrerequisites);
		CASE_RETURN_LIT(-10138, KErrSISUpgradeRangeMissingVersion);
		CASE_RETURN_LIT(-10139, KErrSISUnexpectedFieldType);
		CASE_RETURN_LIT(-10140, KErrSISExpressionUnknownOperator);
		CASE_RETURN_LIT(-10141, KErrSISArrayReadError);
		CASE_RETURN_LIT(-10142, KErrSISArrayTypeMismatch);
		CASE_RETURN_LIT(-10143, KErrSISInvalidStringLength);
		CASE_RETURN_LIT(-10144, KErrSISCompressionNotSupported);
		CASE_RETURN_LIT(-10145, KErrSISTooDeeplyEmbedded);
		CASE_RETURN_LIT(-10146, KErrSISInvalidTargetFile);
		CASE_RETURN_LIT(-10147, KErrSISWouldOverWrite);
		CASE_RETURN_LIT(-10148, KErrSISInfoMissingRemoveDirectories);
		CASE_RETURN_LIT(-10149, KErrSISNotEnoughSpaceToInstall);
		CASE_RETURN_LIT(-10200, KErrInstallerLeave);
		CASE_RETURN_LIT(-10250, KErrPolicyFileCorrupt);
		CASE_RETURN_LIT(-10251, KErrSignatureSchemeNotSupported);
		CASE_RETURN_LIT(-10252, KErrDigestNotSupported);
		CASE_RETURN_LIT(-10253, KErrBadHash);
		CASE_RETURN_LIT(-10254, KErrSecurityError);
		CASE_RETURN_LIT(-10263, KErrBadUsage);
		CASE_RETURN_LIT(-10260, KErrInvalidType); 
		CASE_RETURN_LIT(-10261, KErrInvalidExpression); 
		CASE_RETURN_LIT(-10262, KErrExpressionToComplex); 
		CASE_RETURN_LIT(-10255, KErrMissingBasePackage); 
		CASE_RETURN_LIT(-10256, KErrInvalidUpgrade); 
		CASE_RETURN_LIT(-10257, KErrInvalidEclipsing);
		CASE_RETURN_LIT(-10258, KErrWrongHeaderFormat);
		CASE_RETURN_LIT(-10259, KErrCapabilitiesMismatch); 
		CASE_RETURN_LIT(-10270, KErrLegacySisFile);
		CASE_RETURN_LIT(-10271, KErrInvalidSoftwareTypeRegistrationFile); 
		default:
			return NULL;
		}
	}

//
// Sis installer AO
//
CSwiSisInstallerAO* CSwiSisInstallerAO::NewL(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet)
	{
	CSwiSisInstallerAO* self = new (ELeave) CSwiSisInstallerAO(aParent, aFs, aVerbose, aQuiet);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiSisInstallerAO::CSwiSisInstallerAO(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iVerbose(aVerbose), iQuiet(aQuiet), iCurrentDrive('c')
	{
	TFileName sessionPath;
	TInt err = aFs.SessionPath(sessionPath);
	if (err == KErrNone)
		{
		iCurrentDrive = sessionPath[0];
		iCurrentDrive.LowerCase();
		}
	CActiveScheduler::Add(this);
	}

CSwiSisInstallerAO::~CSwiSisInstallerAO()
	{
	Cancel();
	delete iLauncher;
	delete iRegPackage;
	iRegistrySession.Close();
	delete iPrefs;
	}

void CSwiSisInstallerAO::ConstructL()
	{
	iLauncher = CAsyncLauncher::NewL();
	User::LeaveIfError(iRegistrySession.Connect());
	iPrefs = Swi::CInstallPrefs::NewL();
	iPrefs->SetPerformRevocationCheck(EFalse); // TODO - not currently supported in CCmdSwi
	}

void CSwiSisInstallerAO::RunL()
	{
	iParent.Finished(iStatus.Int());
	}

void CSwiSisInstallerAO::DoCancel()
	{
	iLauncher->CancelOperation();
	iParent.Finished(KErrCancel);
	}

void CSwiSisInstallerAO::PrintDetails(Swi::CSisRegistryPackage& aPackage)
	{
	if (iVerbose)
		{
		TName myBuf;
		myBuf.Format(_L("\r\nName:\t%S\r\n"), &aPackage.Name());
		Stdout().Write(myBuf);
		myBuf.Format(_L("Vendor:\t%S\r\n"), &aPackage.Vendor());
		Stdout().Write(myBuf);
		myBuf.Format(_L("Uid:\t0x%x\r\n"), aPackage.Uid());
		Stdout().Write(myBuf);
		}
	else
		{
		TBuf<256> buf; buf.Format(_L("0x%08x: "), aPackage.Uid());
		Stdout().Write(buf);
		Stdout().Write(aPackage.Name());
		Stdout().Write(_L("\r\n"));
		}
	}

void CSwiSisInstallerAO::InstallL(TFileName& aInstallFile)
	{
	// install regular sis file
	ASSERT(iPrefs);
	ASSERT(iLauncher);
	iLauncher->InstallL(*this, aInstallFile, *iPrefs, iStatus);
	SetActive();
	}

void CSwiSisInstallerAO::UninstallL(const TUid& aPackageUid)
	{
	// remove regular sis file
	ASSERT(iLauncher);
	ASSERT(!iRegPackage);
	Swi::CSisRegistryPackage* iRegPackage = GetSisRegistryPackageL(aPackageUid);
	iLauncher->UninstallL(*this, *iRegPackage, iStatus);
	SetActive();
	}

//
// CSwiSisInstallerAO::ListInstalledAppsL
// lists apps currently installed on the device
//
void CSwiSisInstallerAO::ListInstalledAppsL(const TUid& aAppUid, const TDesC& aMatchString)
	{
	if (aAppUid.iUid != 0)
		{
		DisplayPackageL(aAppUid);
		}
	else
		{
		ASSERT(iRegistrySession.Handle() > 0);
		RPointerArray<CSisRegistryPackage> packages;
		LtkUtils::CleanupResetAndDestroyPushL(packages);
		iRegistrySession.InstalledPackagesL(packages);
		if (packages.Count() <= 0)
			{
			User::Leave(KErrNotFound);
			}
		TInt count = 0;
		const TInt endCount = packages.Count();
		do
			{
			CSisRegistryPackage* current = packages[count++];
			if ((current->Name().MatchF(aMatchString) >= 0) || (current->Vendor().MatchF(aMatchString) >= 0))
				{
				PrintDetails(*current);
				}
			} while (count < endCount);
		CleanupStack::PopAndDestroy(&packages);
		}
	}

//
// CSwiSisInstallerAO::DisplayPackageL
// find information on the specified package uid and display it on the console
//
void CSwiSisInstallerAO::DisplayPackageL(const TUid& aPackageUid)
	{
	ASSERT(iRegistrySession.Handle() > 0);
	Swi::RSisRegistryEntry entry;
	User::LeaveIfError(entry.Open(iRegistrySession, aPackageUid));
	CleanupClosePushL(entry);
	Swi::CSisRegistryPackage* registryPackage = entry.PackageL();
	CleanupStack::PushL(registryPackage);
	PrintDetails(*registryPackage);
	CleanupStack::PopAndDestroy(2, &entry);	
	}

//
// CSwiSisInstallerAO::GetSisRegistryPackageL
// locate the sis registry package corresponding to the specified package ui
//
Swi::CSisRegistryPackage* CSwiSisInstallerAO::GetSisRegistryPackageL(const TUid& aPackageUid)
	{
	ASSERT(iRegistrySession.Handle() > 0);
	if (!iRegistrySession.IsInstalledL(aPackageUid))
		{
		User::Leave(KErrNotFound); // invalid uid
		}
	// else iterate through the installed packages to find ours
	RPointerArray<Swi::CSisRegistryPackage> packages;
	iRegistrySession.InstalledPackagesL(packages);
	if (packages.Count() <= 0)
		{
		User::Leave(KErrNotFound);
		}
	TInt count = 0;
	const TInt endCount = packages.Count();
	Swi::CSisRegistryPackage* found = NULL;
	do
		{
		Swi::CSisRegistryPackage* current = packages[count];
		 if (current->Uid() == aPackageUid)
			{
			found = current;
			break;
			}
		count++;
		} while (count < endCount);
	ASSERT(found); // IsInstalledL stated it's in there
	return found; 
	}

//
// SWI UI Hooks
//
TBool CSwiSisInstallerAO::DisplayTextL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TFileTextOption /*aOption*/, const TDesC& aText)
	{
	TBool response = ETrue; // default, the user will continue with the installation/uninstallation
	if (!iQuiet)
		{
		Stdout().Write(aText);
		Stdout().Write(_L("Continue [y/n]?\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}

void CSwiSisInstallerAO::DisplayErrorL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TErrorDialog aType, const TDesC& /*aParam*/)
	{
	if (iVerbose)
		{
		switch (aType)
			{
			case Swi::EUiAlreadyInRom:
				Stderr().Write(_L("\r\nCannot overwrite a ROM component.\r\n"));
			break;
			
			case Swi::EUiMissingDependency:
				Stderr().Write(_L("\r\nMissing a dependency.\r\n"));
			break;
			
			case Swi::EUiRequireVer:
				Stderr().Write(_L("\r\nA dependency exists on a specific version of an application which is different from the one installed.\r\n"));
			break;

			case Swi::EUiFileCorrupt:
				Stderr().Write(_L("\r\nSISX file corrupt. Checksums stored in file do not match actual.\r\n"));
			break;

			case Swi::EUiDiskNotPresent:
				Stderr().Write(_L("\r\nAll your bases are belong to us. <disk not present>\r\n"));
			break;

			case Swi::EUiCannotRead:
				Stderr().Write(_L("\r\nCannot read a file which is needed in order to proceed.\r\n"));
			break;

			case Swi::EUiCannotDelete:
				Stderr().Write(_L("\r\nCannot delete a file.\r\n"));
			break;

			case Swi::EUiInvalidFileName:
				Stderr().Write(_L("\r\nA filename specified in the SISX package is not valid.\r\n"));
			break;

			case Swi::EUiInsufficientSpaceOnDrive:
				Stderr().Write(_L("\r\nInsufficient space on the drive.\r\n"));
			break;

			case Swi::EUiCapabilitiesCannotBeGranted:
				Stderr().Write(_L("\r\nApplication requires system capability it is not signed for.\r\n"));
			break;

			case Swi::EUiUnknownFile:
				Stderr().Write(_L("\r\nNot a recognised SISX file.\r\n"));
			break;

			case Swi::EUiMissingBasePackage:
				Stderr().Write(_L("\r\nA base package required for installation is not present.\r\n"));
			break;

			case Swi::EUiConstraintsExceeded:
				Stderr().Write(_L("\r\nConstraints imposed by a developer mode certificate have been exceeded.\r\n"));
			break;

			case Swi::EUiSIDViolation:
				Stderr().Write(_L("\r\nThe package contains a file with a protected SID which is not signed for.\r\n"));
			break;

			case Swi::EUiVIDViolation:
				Stderr().Write(_L("\r\nThe package contains a file with a protected VID which is not signed for.\r\n"));
			break;

			case Swi::EUiSIDMismatch:
				Stderr().Write(_L("\r\nMismatched SID.\r\n"));
			break;

			case Swi::EUiBlockingEclipsingFile:
				Stderr().Write(_L("\r\nThe package contains an invalid eclipsing file which is already in the device other than ROM.\r\n"));
			break;

			default:
				{
				TBuf<64> buf;
				buf.Format(_L("\r\nAn error %d has occured. Aborting.\r\n"), aType);
				Stderr().Write(buf);
				}
			break;
			};
		}
	}
	
TBool CSwiSisInstallerAO::DisplayDependencyBreakL(const Swi::CAppInfo& /*aAppInfo*/, const RPointerArray<TDesC>& /*aComponents*/)
	{
	TBool response = ETrue; // default response is to continue with uninstallation
	if (iVerbose)
		{
		Stdout().Write(_L("\r\nComponent being uninstalled has dependencies which may no longer work. Continue uninstalling [y/n]?\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}

TBool CSwiSisInstallerAO::DisplayApplicationsInUseL(const Swi::CAppInfo& /*aAppInfo*/, const RPointerArray<TDesC>& /*aAppNames*/)
	{
	TBool response = EFalse; // EFalse indicates we don't continue with uninstallation
	if (!iQuiet)
		{
		Stdout().Write(_L("\r\nApplication is currently open. Continue uninstalling? [y/n]\r\n"));
		response = iParent.GetAnswer();
		}
	if (!response)
		{
		Stderr().Write(_L("Uninstall aborted. Application will not be closed.\r\n"));
		}
	return response;
	}

TBool CSwiSisInstallerAO::DisplayQuestionL(const Swi::CAppInfo& /* aAppInfo */, Swi::TQuestionDialog aQuestion, const TDesC& aDes)
	{
	TBool response = ETrue; // default behaviour assumes the user presses 'Yes' to any question
	if (!iQuiet)
		{
		if (aQuestion == Swi::EQuestionIncompatible)
			{
			Stdout().Write(_L("\r\nApplication is not compatible with this device. Install anyway [y/n]?"));
			}
		else if (aQuestion == Swi::EQuestionOverwriteFile)
			{
			Stdout().Write(_L("\r\nSome system files will be overwritten by this installation. Install anyway [y/n]?"));
			}
		else
			{
			TBuf<128> buf; buf.Format(_L("Unrecognised question from engine %d\r\n"), aQuestion);
			Stderr().Write(buf);
			User::Leave(KErrNotSupported);
			}

		if (aDes.Length() > 0)
			{
			Stdout().Write(aDes);
			}
		Stdout().Write(_L("\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}
	
TBool CSwiSisInstallerAO::DisplayInstallL(const Swi::CAppInfo& aAppInfo, const CApaMaskedBitmap* /*aLogo*/, const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/)
	{
	if (iVerbose)
		{
		TBuf<256> myBuf;
		myBuf.Format(_L("NAME:\t\t%S\r\n"), &aAppInfo.AppName());
		Stdout().Write(myBuf);
		myBuf.Format(_L("VENDOR:\t\t%S\r\n"), &aAppInfo.AppVendor());
		Stdout().Write(myBuf);
		myBuf.Format(_L("VERSION:\t%d.%d.%d\r\n"), aAppInfo.AppVersion().iMajor, aAppInfo.AppVersion().iMinor, aAppInfo.AppVersion().iBuild);
		Stdout().Write(myBuf);
		}
	TBool response = ETrue; // default behaviour is to continue the install
	return response;
	}
	
TBool CSwiSisInstallerAO::DisplayGrantCapabilitiesL(const Swi::CAppInfo& /*aAppInfo*/, const TCapabilitySet& /*aCapabilitySet*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (iVerbose)
		{
		// todo verbose mode
		Stdout().Write(_L("\r\nTODO - CCmdSwi::DisplayGrantCapabilitiesL\r\n"));
		}
	return response;
	}
	
TInt CSwiSisInstallerAO::DisplayLanguageL(const Swi::CAppInfo& /*aAppInfo*/, const RArray<TLanguage>& /*aLanguages*/)
	{
	if (iVerbose)
		{
		// todo verbose mode
		Stdout().Write(_L("\r\nTODO - CCmdSwi::DisplayLanguageL\r\n"));
		}
	return 0; // the first language
	}
	
TInt CSwiSisInstallerAO::DisplayDriveL(const Swi::CAppInfo& /*aAppInfo*/, TInt64 aSize, const RArray<TChar>& aDriveLetters, const RArray<TInt64>& aDriveSpaces)
	{
	TInt response = 0; // default to the first known drive
	for (TInt i = 0; i < aDriveLetters.Count(); i++)
		{
		TChar letter = aDriveLetters[i];
		letter.LowerCase();
		if (letter == iCurrentDrive)
			{
			response = i; // Default to using the CWD drive, if it is in the list of available drives
			break;
			}
		}

	if (!iQuiet)
		{
		TBuf<128> info;
		info.Format(_L("Application requires %d bytes free space. Please select installation drive:\r\n"), aSize);
		Stdout().Write(info);
		for (TInt ii = 0 ; ii < aDriveLetters.Count() ; ii++)
			{
			info.Format(_L("%d. \'"), ii);		// pseudo-drive number
			info.Append(aDriveLetters[ii]);		// drive letter
			info.Append(_L("\' "));
			info.AppendNum(aDriveSpaces[ii]); // free space
			info.Append(_L(" bytes free\r\n"));
			Stdout().Write(info);
			}
		TUserInput in;
		User::LeaveIfError(Stdin().Read(in));
		TLex lex(in);
		User::LeaveIfError(lex.Val(response));
		}
	return response;
	}
	
TBool CSwiSisInstallerAO::DisplayUpgradeL(const Swi::CAppInfo& /*aAppInfo*/, const Swi::CAppInfo& /*aExistingAppInfo*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (!iQuiet)
		{
		Stdout().Write(_L("Do you wish to replace the existing installed application [y/n]?\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}
	
TBool CSwiSisInstallerAO::DisplayOptionsL(const Swi::CAppInfo& /*aAppInfo*/, const RPointerArray<TDesC>& /*aOptions*/, RArray<TBool>& /*aSelections*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (iVerbose)
		{
		// todo verbose mode
		Stdout().Write(_L("TODO - CCmdSwi::DisplayOptionsL\r\n"));
		}
	return response;
	}
	
TBool CSwiSisInstallerAO::HandleInstallEventL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TInstallEvent aEvent, TInt /*aValue*/, const TDesC& /*aDes*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (iVerbose)
		{
		switch (aEvent)
			{
			case Swi::EEventUpdateProgressBar:
				Stdout().Write(_L("."));
			break;
			
			case Swi::EEventCompletedInstall:
			case Swi::EEventCompletedUnInstall:
				{
				Stdout().Write(_L("\r\nComplete\r\n"));
				}
			break;

			default:
				// do nothing to inform the user of other cases
			break;
			};
		}
	return response;
	}
	
void CSwiSisInstallerAO::HandleCancellableInstallEventL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TInstallCancellableEvent aEvent, Swi::MCancelHandler& /*aCancelHandler*/, TInt /*aValue*/, const TDesC& /*aDes*/)
	{
	if (iVerbose)
		{
		switch (aEvent)
			{
			case Swi::EEventRemovingFiles:
				{
				Stdout().Write(_L("\r\nRemoving files\r\n"));
				}
			break;
			
			case Swi::EEventShuttingDownApps:
				{
				Stdout().Write(_L("Closing App\r\n"));
				}
			break;

			case Swi::EEventCopyingFiles:
				{
				Stdout().Write(_L("Copying files\r\n"));
				}
			break;

			default:
			break;
			};
		}
	}

TBool CSwiSisInstallerAO::DisplaySecurityWarningL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TSignatureValidationResult /*aSigValidationResult*/, RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/, RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/, TBool /*aInstallAnyway*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (!iQuiet)
		{
		Stdout().Write(_L("\r\nApplication signature cannot be validated. Continue installing [y/n]?\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}
	
TBool CSwiSisInstallerAO::DisplayOcspResultL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TRevocationDialogMessage /*aMessage*/, RPointerArray<TOCSPOutcome>& /*aOutcomes*/, RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/, TBool /*aWarningOnly*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (iVerbose)
		{
		// todo verbose mode
		Stdout().Write(_L("TODO - CCmdSwi::DisplayOcspResultL\r\n"));
		}
	return response;	
	}
	
void CSwiSisInstallerAO::DisplayCannotOverwriteFileL(const Swi::CAppInfo& /*aAppInfo*/, const Swi::CAppInfo& /*aInstalledAppInfo*/, const TDesC& aFileName)
	{
	if (iVerbose)
		{
		Stdout().Write(_L("\r\nCannot overwrite a file required for installation. Aborting -"));
		Stdout().Write(aFileName);
		Stdout().Write(_L(" - \r\n"));
		}
	}
	
TBool CSwiSisInstallerAO::DisplayMissingDependencyL(const Swi::CAppInfo& /*aAppInfo*/, const TDesC& aDependencyName, TVersion /*aWantedVersionFrom*/, TVersion /*aWantedVersionTo*/, TVersion /*aInstalledVersion*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (!iQuiet)
		{
		Stdout().Write(_L("Warning: Depedency is missing or has incorrect version - "));
		Stdout().Write(aDependencyName);
		Stdout().Write(_L(" - \r\n"));
		}
	return response;	
	}
	
TBool CSwiSisInstallerAO::DisplayUninstallL(const Swi::CAppInfo& aAppInfo)
	{
	if (iVerbose)
		{
		TName myBuf;
		myBuf.Format(_L("NAME:\t\t%S\r\n"), &aAppInfo.AppName());
		Stdout().Write(myBuf);
		myBuf.Format(_L("VENDOR:\t\t%S\r\n"), &aAppInfo.AppVendor());
		Stdout().Write(myBuf);
		myBuf.Format(_L("VERSION:\t%d.%d.%d\r\n"), aAppInfo.AppVersion().iMajor, aAppInfo.AppVersion().iMinor, aAppInfo.AppVersion().iBuild);
		Stdout().Write(myBuf);
		}
	TBool response = ETrue; // default behaviour is to continue the uninstall
	return response;	
	}

#ifndef SYMBIAN_JAVA_NOT_INCLUDED
//
// java ui installer
//
CSwiMidletInstallerAO* CSwiMidletInstallerAO::NewL(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet)
	{
	CSwiMidletInstallerAO* self = new (ELeave) CSwiMidletInstallerAO(aParent, aFs, aVerbose, aQuiet);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSwiMidletInstallerAO::CSwiMidletInstallerAO(MCmdSwiParent& aParent, RFs& aFs, TBool aVerbose, TBool aQuiet)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iFs(aFs), iVerbose(aVerbose), iQuiet(aQuiet)
	{
	CActiveScheduler::Add(this);
	}

CSwiMidletInstallerAO::~CSwiMidletInstallerAO()
	{
	Cancel();
	if (iInstaller)
		delete iInstaller;
	if (iJadHandle)
		iJadHandle->CloseFileHandle();
	if (iJarHandle)
		iJarHandle->CloseFileHandle();
	if (iInfo)
		delete iInfo;
	if (iRemover)
		delete iRemover;
	}

void CSwiMidletInstallerAO::ConstructL()
	{
	iInstaller = CJavaInstaller::NewL(*this, *this, KNullDesC8, EFalse, 0);
	}

TBool CSwiMidletInstallerAO::IsJad(TDes& aMidlet)
	{
	if (aMidlet.MatchF(KSwiJadExtension) > 0)
		return ETrue;
	return EFalse;
	}

TBool CSwiMidletInstallerAO::IsJar(TDes& aMidlet)
	{
	if (aMidlet.MatchF(KSwiJarExtension) > 0)
		return ETrue;
	return EFalse;
	}

void CSwiMidletInstallerAO::ListInstalledAppsL(TUid aAppUid, const TDesC& aMatchString)
	{
	MJavaRegistry* reg = MJavaRegistry::CreateL();
	RArray<TUid> list;
	TInt count = 0;
	CleanupStack::PushL(reg);
	if ((aAppUid.iUid != 0) && (reg->SuiteEntryExistsL(aAppUid)))
		{
		MJavaRegistrySuiteEntry* suite = reg->SuiteEntryL(aAppUid);
		CleanupStack::PushL(suite);
		suite->MIDletUidsL(list);
		while (count < list.Count())
			{
			MJavaRegistryMIDletEntry* entry = reg->MIDletEntryL(list[count++]);
			CleanupStack::PushL(entry);
			DisplayPackageL(*entry);
			CleanupStack::Pop(entry);
			entry->Release();
			}
		CleanupStack::Pop(suite);
		suite->Release();
		}
	else
		{
		reg->InstalledMIDletUidsL(list);
		while (count < list.Count())
			{
			MJavaRegistryMIDletEntry* entry = reg->MIDletEntryL(list[count++]);
			CleanupStack::PushL(entry);
			if ((entry->MIDletNameL().MatchF(aMatchString)>=0) || (entry->SuiteEntryL().VendorL().MatchF(aMatchString)>=0))
				{
				DisplayPackageL(*entry);
				}
			CleanupStack::Pop(entry);
			entry->Release();
			}
		}
	CleanupStack::Pop(reg);
	}

//
// CSwiMidleInstallerAO::DisplayPackageL
// find information on the specified package uid and display it on the console
//
void CSwiMidletInstallerAO::DisplayPackageL(MJavaRegistryMIDletEntry& aPackage)
	{
	TName myBuf;
	myBuf.Format(_L("\r\nNAME:\t%S\r\n"), &aPackage.MIDletNameL());
	Stdout().Write(myBuf);
	myBuf.Format(_L("VENDOR:\t%S\r\n"), &aPackage.SuiteEntryL().VendorL());
	Stdout().Write(myBuf);
	myBuf.Format(_L("UID:\t0x%x\r\n"), aPackage.SuiteEntryL().UidL());
	Stdout().Write(myBuf);
	}

//
// CSwiMidletInstallerAO::InstallL
// installs a jar/jad midp 1.0/2.0-compatible midlet
//
void CSwiMidletInstallerAO::InstallL(TFileName& aInstallFile)
	{
	ASSERT(iInstaller);
	ASSERT(!iInfo);
	ASSERT(!iJarHandle);
	ASSERT(!iJadHandle);
	if (aInstallFile.Length() <= 0)
		{
		User::Leave(KErrNotFound);
		}
	if (iVerbose)
		{
		Stdout().Write(_L("Installing Midlet...\r\n"));
		}
	const TInt length = aInstallFile.Length() - 3; // the java midlet filename minus the jad/jar extension name
	TFileName sisterFile(aInstallFile.Left(length));
	if (IsJad(aInstallFile))
		{
		iJadHandle = CJavaFileHandle::NewL(iFs, aInstallFile, KNullDesC, KDefaultJadMimeType());
		// name the associated jar file
		sisterFile.Append(_L("jar"));
		iJarHandle = CJavaFileHandle::NewL(iFs, sisterFile, KNullDesC, KDefaultJarMimeType());
		}
	else
		{
		ASSERT(IsJar(aInstallFile));
		iJarHandle = CJavaFileHandle::NewL(iFs, aInstallFile, KNullDesC, KDefaultJarMimeType());
		// name the associated jad file
		sisterFile.Append(_L("jad"));
		iJadHandle = CJavaFileHandle::NewL(iFs, sisterFile, KNullDesC, KDefaultJadMimeType());
		}
	iInfo = CMIDPInstallationLaunchInfo::NewL(iFs, *iJadHandle, *iJarHandle);
	iInstaller->Install(*iInfo, iStatus);
	SetActive();
	}

//
// CSwiMidletInstallerAO::Uninstall
// uninstalls a jar/jad midp 1.0/2.0-compatible midlet
//
void CSwiMidletInstallerAO::UninstallL(const TUid& aPackageUid)
	{
	ASSERT(!iRemover);
	if (iVerbose)
		{
		Stdout().Write(_L("Removing Midlet...\r\n"));
		}
	iRemover = CJavaRemover::NewL(aPackageUid, *this);
	iRemover->Remove(iStatus);
	SetActive();
	}

void CSwiMidletInstallerAO::RunL()
	{
	iParent.Finished(iStatus.Int());
	}

void CSwiMidletInstallerAO::DoCancel()
	{
	ASSERT(iInstaller);
	ASSERT(iRemover);
	iInstaller->Cancel();
	iRemover->Cancel();
	iParent.Finished(KErrCancel);
	}

//
// java installer UI hooks
//
TBool CSwiMidletInstallerAO::StartInstallL(const CMIDletSuiteAttributes& aMIDlet)
	{
	if (iVerbose)
		{
		TName myBuf;
		myBuf.Format(_L("\r\nNAME:\t\t%S\r\n"), &aMIDlet.MIDletName());
		Stdout().Write(myBuf);
		myBuf.Format(_L("VENDOR:\t\t%S\r\n"), &aMIDlet.MIDletVendor());
		Stdout().Write(myBuf);
		myBuf.Format(_L("VERSION:\t%d.%d.%d\r\n"), aMIDlet.MIDletVersion().iMajor, aMIDlet.MIDletVersion().iMinor, aMIDlet.MIDletVersion().iBuild);
		Stdout().Write(myBuf);
		}
	TBool response = ETrue; // default behaviour is to continue the install
	return response;
	}

TBool CSwiMidletInstallerAO::SelectDriveL(const CMIDletSuiteAttributes& /*aMIDlet*/, TInt aSpaceRequired, TChar& aDrive, TBool& aSufficientSpace)
	{
	aSufficientSpace = ETrue;
	aDrive = 'c';
	if (iQuiet)
		{
		// Default to CWD drive if writeable
		TFileName sessionPath;
		TInt err = iFs.SessionPath(sessionPath);
		if (err == KErrNone)
			{
			TChar drive = sessionPath[0];
			drive.LowerCase();
			TVolumeInfo volInfo;
			if (iFs.Volume(volInfo, ((TUint)drive)-'a') == KErrNone && volInfo.iFree >= aSpaceRequired && !(volInfo.iDrive.iDriveAtt & KDriveAttRom) && !(volInfo.iDrive.iMediaAtt & KMediaAttWriteProtected))
				{
				aDrive = drive;
				}
			}
		}
	else
		{
		// ask the question
		TBuf<128> info;
		info.Format(_L("Midlet requires %d bytes free space. Please select installation drive:\r\n"), aSpaceRequired);
		Stdout().Write(info);

		// cycle through drive list checking for existing drive
		TDriveList drives;
		TVolumeInfo volume;
		TChar driveChar;
		TBool displayed = EFalse;
		User::LeaveIfError(iFs.DriveList(drives));
		for (TInt ii = 0 ; ii < KMaxDrives ; ii++)
			{
			if (drives[ii])
				{
				// drive exists
				if (KErrNone == iFs.Volume(volume, ii))
					{
					// drive is present
					if (volume.iFree >= aSpaceRequired)
						{
						// drive has sufficient space
						aSufficientSpace = ETrue;
						if (!(volume.iDrive.iDriveAtt & KDriveAttRom) && 
							!(volume.iDrive.iMediaAtt & KMediaAttWriteProtected))
							{
							// drive is not a rom drive and is not write protected
							User::LeaveIfError(iFs.DriveToChar(ii, driveChar));
							info.Format(_L("%d. \'"), ii);		// drive number
							info.Append(driveChar);		// drive letter
							info.Append(_L("\' "));
							info.AppendNum(volume.iFree); // free space
							info.Append(_L(" bytes free\r\n"));
							Stdout().Write(info);
							displayed = ETrue;
							}
						}
					}
				}
			}
		// get the answer
		if (displayed)
			{
			TUserInput in;
			User::LeaveIfError(Stdin().Read(in));
			TLex lex(in);
			TInt response;
			User::LeaveIfError(lex.Val(response));
			User::LeaveIfError(iFs.DriveToChar(response, aDrive));
			}
		else
			{
			Stdout().Write(_L("Error. No drives capable of supporting midlet install. Aborting\r\n"));
			return EFalse;
			}
		}
	return ETrue;
	}

TBool CSwiMidletInstallerAO::ReplaceExistingMIDletL(const CMIDletSuiteAttributes& /*aMIDlet*/, const TAppVersion& /*aOldVersion*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (!iQuiet)
		{
		Stdout().Write(_L("Do you wish to replace the existing midlet [y/n]?\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}
	
TBool CSwiMidletInstallerAO::UpgradeRMSL(const CMIDletSuiteAttributes& /*aMIDlet*/, const TAppVersion& /*aOldVersion*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (!iQuiet)
		{
		Stdout().Write(_L("Do you wish to upgrade the existing midlet [y/n]?\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}

TBool CSwiMidletInstallerAO::MIDletUntrustedL(const CMIDletSuiteAttributes& /*aMIDlet*/)
	{
	TBool response = ETrue; // default behaviour is to continue the install
	if (!iQuiet)
		{
		Stdout().Write(_L("\r\nMidlet is untrusted. Continue installing [y/n]?\r\n"));
		response = iParent.GetAnswer();
		}
	return response;
	}

void CSwiMidletInstallerAO::CertificateHasNoRootL(const CMIDletSuiteAttributes& /*aMIDlet*/, const CPKIXCertChain& /*aCertChain*/, const CPKIXValidationResult& /*aValidationResult*/)
	{
	Stderr().Write(_L("\r\nCertificate attached to midlet has no identifiable root. Aborting installation.\r\n"));
	}

void CSwiMidletInstallerAO::BadSignatureL(const CMIDletSuiteAttributes& /*aMIDlet*/, const CPKIXCertChain& /*aCertChain*/, const CPKIXValidationResult& /*aValidationResult*/)
	{
	if (iVerbose)
		{
		Stdout().Write(_L("\r\nWarning: Midlet certificate chain is incomplete. Continuing installation\r\n"));
		}
	}

TBool CSwiMidletInstallerAO::PerformRevocationCheckL(TBool& aDoCheck)
	{
	aDoCheck = EFalse;
	return ETrue;
	}

void CSwiMidletInstallerAO::StartRevocationCheckL()
	{
	if (iVerbose)
		{
		Stdout().Write(_L("\r\nStarted: Revocation check\r\n"));
		}
	}

void CSwiMidletInstallerAO::FinishedRevocationCheckL()
	{
	if (iVerbose)
		{
		Stdout().Write(_L("\r\nFinished: Revocation check\r\n"));
		}
	}

void CSwiMidletInstallerAO::StartIconConversionL()
	{
	}

void CSwiMidletInstallerAO::FinishedIconConversionL()
	{
	}

TBool CSwiMidletInstallerAO::OCSPWarningL(const CMIDletSuiteAttributes& /*aMIDlet*/, TRevocationMsg /*aRevocationMsg*/, const TOCSPOutcome* /*aOCSPOutcome*/)
	{
	if (iVerbose)
		{
		Stdout().Write(_L("\r\nWarning: OCSP failure. Continuing installation.\r\n"));
		}
	return ETrue;
	}

void CSwiMidletInstallerAO::OCSPErrorL(const CMIDletSuiteAttributes& /*aMIDlet*/, TRevocationMsg /*aRevocationMsg*/, const TOCSPOutcome* /*aOCSPOutcome*/)
	{
	Stderr().Write(_L("\r\nOCSP Error. Aborting installation.\r\n"));
	}

TBool CSwiMidletInstallerAO::MIDletInUseL(const CMIDletSuiteAttributes& /*aMIDlet*/)
	{
	TBool response = EFalse; // EFalse indicates we don't continue with uninstallation
	if (!iQuiet)
		{
		// print the question
		Stdout().Write(_L("\r\nMidlet is currently open. Continue installing? [y/n]\r\n"));
		
		// get the answer
		TUserInput in;
		const TInt error = Stdin().Read(in);
		if ((error == KErrNone) && (in.MatchF(KSwiYes) == 0))
			{
			response = ETrue;
			}
		}
	if (!response)
		{
		Stderr().Write(_L("Uninstall aborted. Midlet has not been closed.\r\n"));
		}
	return response;
	}

void CSwiMidletInstallerAO::OTAReportResponseL(TInt aOTAResponseCode, const TDesC& /*aReportBody*/)
	{
	if (iVerbose)
		{
		switch (aOTAResponseCode)
			{
			case CJavaMIDletInstallationResult::EOkay:
				Stdout().Write(_L("\r\nComplete\r\n"));
			break;
			
			default:
				Stdout().Write(_L("Abort\r\n"));
			break;
			};
		}
	}

TInt CSwiMidletInstallerAO::CopyStarted(TInt /*aSize*/)
	{
	return KErrNone;
	}

TInt CSwiMidletInstallerAO::DownloadStarted(TInt /*aSize*/)
	{
	if (iVerbose)
		{
		Stdout().Write(_L("Downloading midlet\r\n"));
		}
	return KErrNone;
	}

TInt CSwiMidletInstallerAO::UpdateProgress(TInt /*aSize*/)
	{
	if (iVerbose)
		{
		Stdout().Write(_L("."));
		}
	return KErrNone;
	}

TBool CSwiMidletInstallerAO::GetUsernamePasswordL(HBufC8*& /*aUsername*/, HBufC8*& /*aPassword*/)
	{
	if (iVerbose)
		{
		Stdout().Write(_L("\r\nTODO: CCmdSwi::GetUsernamePasswordL\r\n"));
		}
	return EFalse;
	}

TBool CSwiMidletInstallerAO::ConfirmRemovalL(const TDesC& /*aRemovalMessage*/)
	{
	TBool response = ETrue; // default behaviour is to continue the uninstall
	return response;	
	}

void CSwiMidletInstallerAO::InitialiseProgressBarL(TInt /*aMaximumLength*/)
	{
	}

void CSwiMidletInstallerAO::UpdateProgressBarL(TInt /*aUnits*/)
	{
	if (iVerbose)
		{
		Stdout().Write(_L("."));
		}
	}

TBool CSwiMidletInstallerAO::MIDletInUseL()
	{
	TBool response = EFalse; // EFalse indicates we don't continue with uninstallation
	if (!iQuiet)
		{
		// print the question
		Stdout().Write(_L("\r\nApplication is currently open. Continue uninstalling? [y/n]\r\n"));
		
		// get the answer
		TUserInput in;
		const TInt error = Stdin().Read(in);
		if ((error == KErrNone) && (in.MatchF(KSwiYes) == 0))
			{
			response = ETrue;
			}
		}
	return response;
	}

TBool CSwiMidletInstallerAO::FileInUseL(const TDesC& /*aFileName*/)
	{
	TBool response = EFalse; // EFalse indicates we don't continue with uninstallation
	if (!iQuiet)
		{
		// print the question
		Stdout().Write(_L("\r\nApplication has a file handle open. Continue uninstalling? [y/n]\r\n"));
		
		// get the answer
		TUserInput in;
		const TInt error = Stdin().Read(in);
		if ((error == KErrNone) && (in.MatchF(KSwiYes) == 0))
			{
			response = ETrue;
			}
		}
	return response;
	}

#endif //  SYMBIAN_JAVA_NOT_INCLUDED

#ifdef EXE_BUILD
EXE_BOILER_PLATE(CCmdSwi)
#endif

