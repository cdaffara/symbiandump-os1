// sudo.cpp
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

#define __INCLUDE_CAPABILITY_NAMES__
#include <fshell/ioutils.h>
#include <f32image.h>
#include <fshell/memoryaccesscmd.h>
#include <e32rom.h>

using namespace IoUtils;

class CCmdSudo : public CMemoryAccessCommandBase, public MCommandExtensionsV2
	{
public:
	static CCommandBase* NewLC();
	~CCmdSudo();
private:
	CCmdSudo();
	static void DeleteModifiedBinary(TAny* aSelf);
	void DeleteModifiedBinary();
	TBool FileExists(const TDesC& aFileName);
	void FixupExeInMemoryL(RProcess& aProcess);
	void CalculateCaps();

	void FindExeL();
	void CopyExeLC();
	void FixupExeL();
	void RunExeL();
	void FixupCoreExeLC();

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void RunL();

private: // From MCommandExtensionsV2
	virtual void CtrlCPressed();

private:
	HBufC* iCmd;
	HBufC* iArgs;
	RPointerArray<HBufC> iAdd;
	RPointerArray<HBufC> iRemove;
	TCapabilitySet iCapsToAdd;
	TCapabilitySet iCapsToRemove;
	TUint iSid;
	TUint iVid;
	TUint iHeapMin;
	TUint iHeapMax;
	TUint iStackSize;
	TInt iProcessPriority;

	TBool iKeep;
	TBool iWait;
	TBool iChangeBinaryOnDisk;
	TBool iFileIsInCore;

	TFileName iPath;
	TFileName iNewPath;
	CArrayPtrFlat<HBufC>* iPathsToCleanup;
	// following 2 are temporaries needed during CopyExeLC
	TFileName iTempSrc;
	TFileName iTempDest;

	RChildProcess iChildProcess;
	};


CCommandBase* CCmdSudo::NewLC()
	{
	CCmdSudo* self = new(ELeave) CCmdSudo();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSudo::~CCmdSudo()
	{
	delete iCmd;
	delete iArgs;
	iAdd.ResetAndDestroy();
	iRemove.ResetAndDestroy();
	
	if (iPathsToCleanup)
		{
		for (TInt i = 0; i < iPathsToCleanup->Count(); i++)
			{
			HBufC* file = (*iPathsToCleanup)[i];
			//Printf(_L("Deleting file %S (not really)\n"), file);
			Fs().Delete(*file);
			delete file;
			}
		}
	delete iPathsToCleanup;
	iChildProcess.Close();
	}

CCmdSudo::CCmdSudo()
	: CMemoryAccessCommandBase(EManualComplete | ECaptureCtrlC)
	{
	SetExtension(this);
	}

TCapability CapabilityFromString(const TDesC& aName)
	{
	TBuf<32> cap;
	for (TInt i = 0; i < ECapability_Limit; i++)
		{
		cap.Copy(TPtrC8((TUint8*)CapabilityNames[i]));
		if (aName.CompareF(cap) == 0)
			{
			return (TCapability)i;
			}
		}
	return ECapability_None;
	}

void CCmdSudo::DoRunL()
	{
	iPathsToCleanup = new(ELeave) CArrayPtrFlat<HBufC>(8);
	CalculateCaps();
	FindExeL();
	if (iChangeBinaryOnDisk)
		{
#ifndef __WINS__
		// Don't try and use IsFileInRom on WINSCW, the emulator makes a mess of it and anyway, shadowing isn't supported so it being in the core is irrelevant
		iFileIsInCore = FsL().IsFileInRom(iPath) != NULL;
#endif
		if (iFileIsInCore)
			{
			// Things in core have to be handled differently
			iNewPath = iPath; // We don't call CopyExeLC so something has to set this
			FixupCoreExeLC();
			}
		else
			{
			CopyExeLC();
			FixupExeL();
			}
		}
	else
		{
		if (iHeapMin || iHeapMax  || iStackSize) LeaveIfErr(KErrArgument, _L("Heap or stack sizes cannot be modified unless you specify --disk"));
		if (iKeep) LeaveIfErr(KErrArgument, _L("--keep option makes no sense if --disk isn't specified."));

		CleanupStack::PushL((CBase*)NULL); // RunExeL expects a cleanup item for DeleteModifiedBinary, which isn't necessary when --disk isn't specified
		// Everything else is taken care of from RunExeL
		}
	RunExeL();
	}

void CCmdSudo::FindExeL()
	{
	if (!iChangeBinaryOnDisk)
		{
		// If we're not changing on disk, it's fine to just use the path as-is and let RProcess::Create sort it out
		iPath = *iCmd;
		iNewPath = iPath;
		return;
		}

#ifdef __WINS__
	PrintWarning(_L("On WINS, exe-name must be a complete path to a E32 exe"));
	iPath = *iCmd;
#else

	// We can't reliably use RProcess::Create then FileName because the reason we're calling sudo may be because
	// the capabilities don't allow it to load.
	_LIT(KExe, ".exe");
	_LIT(KSysBin, "\\sys\\bin\\");
	iPath = *iCmd;
	if (iPath.Right(KExe().Length()).CompareF(KExe) != 0)
		{
		iPath.Append(KExe);
		}
	TParsePtrC parse(iPath);
	if (!parse.PathPresent())
		{
		TFindFile find(FsL());
		TInt found = find.FindByDir(iPath, KSysBin);
		LeaveIfErr(found, _L("Couldn't locate file %S"), &iPath);
		iPath = find.File();
		}
#endif
	}

void CCmdSudo::CopyExeLC()
	{
	iNewPath = iPath;
	iNewPath[0] = 'c'; // Has to be on C otherwise the loader performs hash checks
	iNewPath.Append(_L(".sudoed.exe"));

	CleanupStack::PushL(TCleanupItem(&CCmdSudo::DeleteModifiedBinary, this));

	TInt err = FsL().MkDirAll(iNewPath); // In case C:\sys\bin doesn't exist yet
	if (err && err != KErrAlreadyExists)
		{
		LeaveIfErr(err, _L("Couldn't create C:\\sys\\bin"));
		}

	CFileMan* fm = CFileMan::NewL(Fs());
	CleanupStack::PushL(fm);

	LeaveIfErr(fm->Copy(iPath, iNewPath), _L("Couldn't copy file from %S to %S"), &iPath, &iNewPath);
	// Clear the read-only bit in the case where we've copied from Z drive
	LeaveIfErr(Fs().SetAtt(iNewPath, 0, KEntryAttReadOnly), _L("Couldn't unset read-only flag"));

	/* TODO this code looks like it should work but doesn't. Something in cone or similar is not behaving the way it looks like it should.

	// In case it's an app which will rely on having its main rsc and its mbm file on the same drive as it, copy them over too
	// (Damn cone for not searching drives or working relative to the reg rsc...)
	TParsePtrC parse(iPath);
	TPtrC exename = parse.Name();

	_LIT(KResFmt, "%c:\\Resource\\Apps\\%S.rsc");
	_LIT(KResDestFmt, "C:\\Resource\\Apps\\%S.sudoed.rsc");
	iTempSrc.Format(KResFmt, iPath[0], &exename);
	iTempDest.Format(KResDestFmt, &exename);

	iPathsToCleanup->SetReserveL(iPathsToCleanup->Count() + 2);
	iPathsToCleanup->AppendL(iTempDest.AllocL());
	LeaveIfErr(fm->Copy(iTempSrc, iTempDest), _L("Couldn't copy ancillary file %S to %S"), &iTempSrc, &iTempDest);
	LeaveIfErr(Fs().SetAtt(iTempDest, 0, KEntryAttReadOnly), _L("Couldn't unset read-only flag of %S"), &iTempDest);

	_LIT(KMbmFmt, "%c:\\Resource\\Apps\\%S.mbm");
	iTempSrc.Format(KMbmFmt, iPath[0], &exename);
	iTempDest.Format(KMbmFmt, 'c', &exename);
	if (!FileExists(iTempDest))
		{
		iPathsToCleanup->AppendL(iTempDest.AllocL());
		LeaveIfErr(fm->Copy(iTempSrc, iTempDest), _L("Couldn't copy ancillary file %S to %S"), &iTempSrc, &iTempDest);
		LeaveIfErr(Fs().SetAtt(iTempDest, 0, KEntryAttReadOnly), _L("Couldn't unset read-only flag of %S"), &iTempDest);
		}
	*/
	CleanupStack::PopAndDestroy(fm);
	}

void CCmdSudo::FixupExeL()
	{
	// Now fix up the capabilities or other stuff
	RFile file;
	CleanupClosePushL(file);
	LeaveIfErr(file.Open(Fs(), iNewPath, EFileWrite|EFileStream|EFileShareAny), _L("Couldn't open file"));

	E32ImageHeaderV* imageHeader=new(ELeave)E32ImageHeaderV;
	CleanupStack::PushL(imageHeader);
	TPckg<E32ImageHeaderV> ptr(*imageHeader);
	LeaveIfErr(file.Read(ptr, sizeof(E32ImageHeaderV)), _L("Couldn't read E32ImageHeader"));

	SSecurityInfo& secinfo = imageHeader->iS;
	for (TInt i = 0; i < ECapability_Limit; i++)
		{
		TCapability cap = (TCapability)i;
		if (iCapsToAdd.HasCapability(cap)) secinfo.iCaps.AddCapability(cap);
		if (iCapsToRemove.HasCapability(cap)) reinterpret_cast<TCapabilitySet*>(&secinfo.iCaps)->RemoveCapability(cap);
		}

	if (iOptions.IsPresent(&iSid))
		{
		secinfo.iSecureId = iSid;
		}
	if (iOptions.IsPresent(&iVid))
		{
		secinfo.iVendorId = iVid;
		}
	if (iHeapMin)
		{
		imageHeader->iHeapSizeMin = iHeapMin;
		}
	if (iHeapMax)
		{
		imageHeader->iHeapSizeMax = iHeapMax;
		}
	if (iStackSize)
		{
		imageHeader->iStackSize = iStackSize;
		}
	if (iProcessPriority)
		{
		imageHeader->iProcessPriority = iProcessPriority;
		}

	// Update e32 checksum
	imageHeader->iHeaderCrc = KImageCrcInitialiser;
	TUint32 crc = 0;
	Mem::Crc32(crc, imageHeader, imageHeader->TotalSize());
	imageHeader->iHeaderCrc = crc;

	LeaveIfErr(file.Write(0, ptr), _L("Couldn't write updated header back to file"));
	CleanupStack::PopAndDestroy(2, &file); // imageHeader, file
	}

void CCmdSudo::RunExeL()
	{
	// Now actually execute it
#ifdef __WINS__
	if (iChangeBinaryOnDisk)
		{
		PrintWarning(_L("Updated file written to %S. Not actually executing it."), &iNewPath);
		CleanupStack::Pop(); // DeleteModifiedBinary
		return;
		}
#endif

	TRAPL(iChildProcess.CreateL(iNewPath, iArgs ? *iArgs : KNullDesC(), IoSession(), Stdin(), Stdout(), Stderr(), Env()), _L("Failed to execute %S"), &iNewPath);
	if (iKeep)
		{
		Printf(_L("Executing %S...\r\n"), &iNewPath);
		}
	CleanupStack::Pop(); // DeleteModifiedBinary


	if (!iChangeBinaryOnDisk)
		{
		// Time to get memaccess involved
		TRAPD(err, FixupExeInMemoryL(iChildProcess.Process()));
		if (err)
			{
			iChildProcess.Process().Kill(err);
			iChildProcess.Close();
			User::Leave(err);
			}
		}

	if (iWait)
		{
		Printf(_L("Process is created but not yet resumed. Press a key to continue...\r\n"));
		ReadKey();
		Printf(_L("Resuming process...\r\n"));
		}

	iChildProcess.Run(iStatus);
	SetActive();
	}

const TDesC& CCmdSudo::Name() const
	{
	_LIT(KName, "sudo");
	return KName;
	}

void CCmdSudo::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iCmd, _L("exe-name"));
	aArguments.AppendStringL(iArgs, _L("arguments"));
	}

void CCmdSudo::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendStringL(iAdd, _L("add-cap"));
	aOptions.AppendStringL(iRemove, _L("remove-cap"));
	aOptions.AppendUintL(iSid, _L("sid"));
	aOptions.AppendUintL(iVid, _L("vid"));
	aOptions.AppendUintL(iHeapMin, _L("heap-min"));
	aOptions.AppendUintL(iHeapMax, _L("heap-max"));
	aOptions.AppendUintL(iStackSize, _L("stack-size"));
	aOptions.AppendIntL(iProcessPriority, _L("process-priority"));
	aOptions.AppendBoolL(iKeep, _L("keep"));
	aOptions.AppendBoolL(iChangeBinaryOnDisk, _L("disk"));
	aOptions.AppendBoolL(iWait, _L("wait"));
	}

void CCmdSudo::DeleteModifiedBinary(TAny* aSelf)
	{
	CCmdSudo* self = static_cast<CCmdSudo*>(aSelf);
	self->DeleteModifiedBinary();
	}

void CCmdSudo::DeleteModifiedBinary()
	{
	if (iFileIsInCore)
		{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
		iMemAccess.FreeShadowMemory((TLinAddr)Fs().IsFileInRom(iPath), sizeof(TRomImageHeader));
#endif
		}
	else
		{
		TInt err = Fs().Delete(iNewPath);
		if (err && err != KErrNotFound && err != KErrPathNotFound) PrintError(err, _L("Couldn't delete file %S"), &iNewPath);
		}
	}

EXE_BOILER_PLATE(CCmdSudo)

TBool CCmdSudo::FileExists(const TDesC& aFileName)
	{
	TEntry entry;
	return Fs().Entry(aFileName, entry) == KErrNone;
	}

void CCmdSudo::FixupExeInMemoryL(RProcess& aProcess)
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	LoadMemoryAccessL();
	TProcessProperties prop;
	prop.iCapsToAdd = iCapsToAdd;
	prop.iCapsToRemove = iCapsToRemove;
	prop.iProcessPriority = iProcessPriority;
	if (iOptions.IsPresent(&iSid))
		{
		prop.iSid = iSid;
		}
	if (iOptions.IsPresent(&iVid))
		{
		prop.iVid = iVid;
		}
	LeaveIfErr(iMemAccess.SetProcessProperties(aProcess, prop), _L("Couldn't set process properties using memoryaccess"));
#else
	(void)aProcess;
	LeaveIfErr(KErrNotSupported, _L("Can't fixup process in memory without MemoryAccess, try the --disk option instead"));
#endif
	}

void CCmdSudo::CalculateCaps()
	{
	_LIT(KAll, "All");
	// Add caps
	iCapsToAdd.SetEmpty();
	for (TInt i = 0; i < iAdd.Count(); i++)
		{
		const TDesC& capName = *iAdd[i];
		TCapability cap = CapabilityFromString(capName);
		if (cap == ECapability_None)
			{
			if (capName.CompareF(KAll) == 0)
				{
				// The pseudo-cap 'All'
				iCapsToAdd.SetAllSupported();
				}
			else
				{
				PrintWarning(_L("Couldn't understand capability name %S"), &capName);
				}
			}
		else
			{
			iCapsToAdd.AddCapability(cap);
			}
		}
	// Remove caps
	iCapsToRemove.SetEmpty();
	for (TInt i = 0; i < iRemove.Count(); i++)
		{
		const TDesC& capName = *iRemove[i];
		TCapability cap = CapabilityFromString(capName);
		if (cap == ECapability_None)
			{
			if (capName.CompareF(KAll) == 0)
				{
				// The pseudo-cap 'All'
				iCapsToRemove.SetAllSupported();
				}
			else
				{
				PrintWarning(_L("Couldn't understand capability name %S"), &capName);
				}
			}
		else
			{
			iCapsToRemove.AddCapability(cap);
			}
		}

	TBool noOptions = (iAdd.Count() == 0) && (iRemove.Count() == 0) && !iOptions.IsPresent(&iSid) && !iOptions.IsPresent(&iVid) && (iHeapMin == 0) && (iHeapMax == 0) && (iStackSize == 0) && (iProcessPriority == 0);

	if (noOptions)
		{
		// Default to All -TCB
		iCapsToAdd.SetAllSupported();
		iCapsToRemove.AddCapability(ECapabilityTCB);
		}
	}

void CCmdSudo::FixupCoreExeLC()
	{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	LoadMemoryAccessL();

	const TRomImageHeader* imageHeader = (const TRomImageHeader*)FsL().IsFileInRom(iPath);
	if (!imageHeader) LeaveIfErr(KErrNotFound, _L("In FixupCoreExeLC but IsFileInRom returned null??"));

	SCapabilitySet caps = imageHeader->iS.iCaps;
	for (TInt i = 0; i < ECapability_Limit; i++)
		{
		TCapability cap = (TCapability)i;
		if (iCapsToAdd.HasCapability(cap)) caps.AddCapability(cap);
		if (iCapsToRemove.HasCapability(cap)) reinterpret_cast<TCapabilitySet*>(&caps)->RemoveCapability(cap);
		}
	TPckg<SCapabilitySet> pkg(caps);
	LeaveIfErr(iMemAccess.WriteShadowMemory((TLinAddr)&imageHeader->iS.iCaps, pkg), _L("Couldn't write shadow memory for caps"));

	if (iOptions.IsPresent(&iSid))
		{
		TPckg<TUint> pkg(iSid);
		LeaveIfErr(iMemAccess.WriteShadowMemory((TLinAddr)&imageHeader->iS.iSecureId, pkg), _L("Couldn't write shadow memory for sid"));
		}
	if (iOptions.IsPresent(&iVid))
		{
		TPckg<TUint> pkg(iVid);
		LeaveIfErr(iMemAccess.WriteShadowMemory((TLinAddr)&imageHeader->iS.iVendorId, pkg), _L("Couldn't write shadow memory for sid"));
		}
	if (iHeapMin)
		{
		TPckg<TUint> pkg(iHeapMin);
		LeaveIfErr(iMemAccess.WriteShadowMemory((TLinAddr)&imageHeader->iHeapSizeMin, pkg), _L("Couldn't write shadow memory for iHeapSizeMin"));
		}
	if (iHeapMax)
		{
		TPckg<TUint> pkg(iHeapMax);
		LeaveIfErr(iMemAccess.WriteShadowMemory((TLinAddr)&imageHeader->iHeapSizeMax, pkg), _L("Couldn't write shadow memory for iHeapSizeMax"));
		}
	if (iStackSize)
		{
		TPckg<TUint> pkg(iStackSize);
		LeaveIfErr(iMemAccess.WriteShadowMemory((TLinAddr)&imageHeader->iStackSize, pkg), _L("Couldn't write shadow memory for iStackSize"));
		}
	if (iProcessPriority)
		{
		TPckg<TUint> pkg(iProcessPriority);
		LeaveIfErr(iMemAccess.WriteShadowMemory((TLinAddr)&imageHeader->iPriority, pkg), _L("Couldn't write shadow memory for iProcessPriority"));
		}

	// DeleteModifiedBinary does the right thing in the case of shadowing
	CleanupStack::PushL(TCleanupItem(&CCmdSudo::DeleteModifiedBinary, this));
#else
	LeaveIfErr(KErrNotSupported, _L("Can't fixup an exe in Core image without memoryaccess"));
#endif
	}

void CCmdSudo::CtrlCPressed()
	{
	Printf(_L("Ctrl-C pressed, killing %S and cleaning up.\r\n"), &iNewPath);
	SetErrorReported(ETrue);
	iChildProcess.Process().Kill(KErrAbort);
	}

void CCmdSudo::RunL()
	{
	if (!iKeep)
		{
		DeleteModifiedBinary();
		}
	Complete(iStatus.Int());
	}
