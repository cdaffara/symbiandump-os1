// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmcommandlistresourcereaderimpl.h"

#include <bautils.h>
#include <barsc2.h>
#include <ssm/ssmcommandlist.h>
#include <ssm/ssmconditionalcallback.h>

#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmcommand.h>
#include "ssmcommandbase.h"
#include "ssmcommandparameters.h"
#include "ssmcommandlistinterface.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
#include "ssmcommandlistimpl.h"
#ifdef __WINS__
#include <u32hal.h>

//Literal to be appended with the main folder path to get the extended folder path
_LIT(KExtendedCommandListPath, "_ext\\");

//Used for checking the presence of keyword for using extended folder in epoc.ini
static TBool IsExtendedFolderDisabled()
    {
    TBool isMinimalBootDisable = EFalse;
    
    //If the keyword "MINIMALSTART" is found in epoc.ini then only commands from main folder are 
    //executed else commands from both main and extended folder are executed.
    UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalBoolProperty, (TAny*)"MINIMALSTART", &isMinimalBootDisable);
    return isMinimalBootDisable;
    }
#endif // __WINS__
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE


CSsmCommandListResourceReaderImpl* CSsmCommandListResourceReaderImpl::NewL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback)
	{
	CSsmCommandListResourceReaderImpl* self = new(ELeave) CSsmCommandListResourceReaderImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aCommandListPath, aConditionalCallback);
	CleanupStack::Pop(self);
	return self;
	}

CSsmCommandListResourceReaderImpl::CSsmCommandListResourceReaderImpl()
	{
	}

CSsmCommandListResourceReaderImpl::~CSsmCommandListResourceReaderImpl()
	{
	delete iPreparer;
	delete iInitialiser;
	delete iResourcePool;
	}

void CSsmCommandListResourceReaderImpl::ConstructL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback)
	{
	iResourcePool = new(ELeave) CResourcePool;
	iInitialiser = CInitialiser::NewL(aFs, aCommandListPath, *iResourcePool);
	iPreparer = new(ELeave) CPreparer(*iResourcePool, aConditionalCallback);
	}

void CSsmCommandListResourceReaderImpl::Initialise(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(!Busy(), PanicNow(KPanicCmdResourceReader, EInUse1));
	iInitialiser->Start(aStatus);
	}

void CSsmCommandListResourceReaderImpl::InitialiseCancel()
	{
	iInitialiser->Cancel();
	}

void CSsmCommandListResourceReaderImpl::PrepareCommandList(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmState& aState, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(!Busy(), PanicNow(KPanicCmdResourceReader, EInUse2));
	iPreparer->Start(aCommandList, aCommandListId, aState, aStatus);
	}

void CSsmCommandListResourceReaderImpl::PrepareCommandList(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(!Busy(), PanicNow(KPanicCmdResourceReader, EInUse3));
	iPreparer->Start(aCommandList, aCommandListId, aSwp, aStatus);
	}

void CSsmCommandListResourceReaderImpl::PrepareCommandListCancel()
	{
	iPreparer->Cancel();
	}

TBool CSsmCommandListResourceReaderImpl::IsCommandListReady() const
	{
	// enables caller to find out if the command list they passed
	// to PrepareCommandList is fully prepared or not
	return iPreparer->IsCommandListReady();
	}

TBool CSsmCommandListResourceReaderImpl::Busy() const
	{
	return iInitialiser->IsActive() || iPreparer->IsActive();
	}

void CSsmCommandListResourceReaderImpl::GetCommandListIdsL(RArray<TInt>& aArray) const
	{
	iResourcePool->GetCommandListIdsL(aArray);
	}

// CResourcePool
CSsmCommandListResourceReaderImpl::CResourcePool::CResourcePool()
	{
	}

CSsmCommandListResourceReaderImpl::CResourcePool::~CResourcePool()
	{
	iMappings.Close();
	iResourceFiles.ResetAndDestroy();
	iResourceFileNames.ResetAndDestroy();
	}

TBool CSsmCommandListResourceReaderImpl::CResourcePool::IsEmpty() const
	{
	return !(iMappings.Count() > 0);
	}

void CSsmCommandListResourceReaderImpl::CResourcePool::Reset()
	{
	iMappings.Reset();
	iResourceFiles.ResetAndDestroy();
	iResourceFileNames.ResetAndDestroy();
	}

void CSsmCommandListResourceReaderImpl::CResourcePool::AppendL(const CResourceFile* aResourceFile)
	{
	iResourceFiles.AppendL(aResourceFile);
	}

void CSsmCommandListResourceReaderImpl::CResourcePool::AppendL(const TDesC& aFileName)
	{
	HBufC* name = aFileName.AllocLC();
	iResourceFileNames.AppendL(name);
	CleanupStack::Pop(name);
	}


void CSsmCommandListResourceReaderImpl::CResourcePool::AppendL(const TMapping& aMapping)
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE 
	//check if the mapping already exists
	TInt index = iMappings.Find(aMapping);
	if (index != KErrNotFound)
		{
		DEBUGPRINT2(_L("Duplicate Substate found at index: %d"), index);
		}
	TInt err = iMappings.InsertInSignedKeyOrderAllowRepeats(aMapping);
#else
	TInt err = iMappings.InsertInSignedKeyOrder(aMapping); 
    if(KErrAlreadyExists == err) 
		{ 
		PanicNow(KPanicCmdResourceReader, EDuplicateSubstateInCommandList); 
		} 
#endif
    User::LeaveIfError(err);     
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
// This function will get all the mappings which have the same commandlistId's (substates) into aMappingArray
void CSsmCommandListResourceReaderImpl::CResourcePool::UpdateMappingArrayL(TInt aCommandListId, RArray<TMapping>& aMappingArray) const
	{
	aMappingArray.Reset();
	TMapping mapping(aCommandListId, 0, NULL);
	const TInt mappingCount = iMappings.Count();
	TInt mappingIndex = iMappings.FindL(mapping);

	for(; mappingIndex < mappingCount; ++mappingIndex)
		{
		aMappingArray.AppendL(iMappings[mappingIndex]);
		if ((mappingIndex >= (mappingCount - 1)) || (iMappings[mappingIndex].iCommandListId !=
		iMappings[mappingIndex + 1].iCommandListId))
			{
			//Breaking as it reached end of array or completed appending the mappings for
			//the given commandlist id
			break;
			}
		}
	DEBUGPRINT3(_L("Mapping Array for substate %04x Contains %d entries"), iMappings[mappingIndex].iCommandListId, aMappingArray.Count());
	}

#else
CSsmCommandListResourceReaderImpl::TMapping CSsmCommandListResourceReaderImpl::CResourcePool::MappingL(TInt aCommandListId) const
	{
	TMapping mapping(aCommandListId, 0, NULL);
	const TInt index = iMappings.FindL(mapping);
	return iMappings[index];
	}

#endif
void CSsmCommandListResourceReaderImpl::CResourcePool::GetCommandListIdsL(RArray<TInt>& aArray) const
	{
	TInt count = iMappings.Count();
	__ASSERT_ALWAYS(count > 0, PanicNow(KPanicCmdResourceReader, ENotInitialized3));
	if (aArray.Count() != 0)
		{
		SSMLOGLEAVE(KErrArgument);
		}
	aArray.ReserveL(count);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TInt i = 0;
	for(i = 0; i < iMappings.Count() - 1 ; ++i)
		{
		if(iMappings[i].iCommandListId != iMappings[i+1].iCommandListId)
			{
			aArray.AppendL(iMappings[i].iCommandListId);
			}
		}
	//append the last element anyway as it is already compared
	aArray.AppendL(iMappings[i].iCommandListId);
	aArray.Compress();
		
	DEBUGPRINT2(_L("The number of substates in resource files : %d"),iMappings.Count() );
	DEBUGPRINT2(_L("The number of substates after filtering duplicates : %d"),aArray.Count());
#else
	for(TInt i = 0; i < count; ++i)
		{
		aArray.AppendL(iMappings[i].iCommandListId);
		}
#endif
	}

const TDesC& CSsmCommandListResourceReaderImpl::CResourcePool::FileNameForResourceFileL(const CResourceFile* aResourceFile) const
	{
	TInt count = iResourceFiles.Count();
	for(TInt i = 0; i < count; ++i)
		{
		if(iResourceFiles[i] == aResourceFile)
			{
			return *iResourceFileNames[i];
			}
		}
	SSMLOGLEAVE(KErrNotFound);
	return KNullDesC16();
	}

// CActiveBase
CSsmCommandListResourceReaderImpl::CActiveBase::CActiveBase(TInt aPriority)
	:CActive(aPriority), iAction(EIdle)
	{
	}

CSsmCommandListResourceReaderImpl::CActiveBase::~CActiveBase()
	{
	}

void CSsmCommandListResourceReaderImpl::CActiveBase::Start()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CSsmCommandListResourceReaderImpl::CActiveBase::CompleteClientRequest(TInt aReason)
	{
	if (iRequestStatus)
		{
		User::RequestComplete(iRequestStatus, aReason);
		iRequestStatus = NULL;
		}
	}

// CInitialiser
CSsmCommandListResourceReaderImpl::CInitialiser* CSsmCommandListResourceReaderImpl::CInitialiser::NewL(RFs& aFs, const TDesC& aCommandListPath, CResourcePool& aResourcePool)
	{
	CInitialiser* self = new(ELeave) CInitialiser(aFs, aResourcePool);
	CleanupStack::PushL(self);
	self->ConstructL(aCommandListPath);
	CleanupStack::Pop(self);
	return self;
	}

CSsmCommandListResourceReaderImpl::CInitialiser::CInitialiser(RFs& aFs, CResourcePool& aResourcePool)
	:CActiveBase(EPriorityStandard),  iFs(aFs), iResourcePool(aResourcePool)
#if defined(SYMBIAN_SSM_FLEXIBLE_MERGE) && defined(__WINS__)
	, iExtendedEntryIndex(0)
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE && __WINS__
	{
	CActiveScheduler::Add(this);
	}

CSsmCommandListResourceReaderImpl::CInitialiser::~CInitialiser()
	{
	Cancel();
	ResetToUninitialisedState();
	iPath.Close();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iSystemDrivePath.Close();
#ifdef __WINS__ 
	iExtendedPath.Close();
#endif //__WINS__

#endif
	}

void CSsmCommandListResourceReaderImpl::CInitialiser::ResetToUninitialisedState()
	{
	delete iResourceFileEntries;
	iResourceFileEntries = NULL;
	iEntryIndex = 0;
	iResourcePool.Reset();
	iAction = EIdle;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	delete iRssFileEntriesInSysDrive;
	iRssFileEntriesInSysDrive = NULL;
#ifdef __WINS__ 
	delete iExtendedResourceFileEntries;
	iExtendedResourceFileEntries = NULL;
#endif //__WINS__
#endif
	}

void CSsmCommandListResourceReaderImpl::CInitialiser::ConstructL(const TDesC& aCommandListPath)
	{
	if (aCommandListPath.Length() == 0)
		{
		SSMLOGLEAVE(KErrArgument);
		}
	iPath.CreateL(aCommandListPath);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	// get the system drive char and create system drive path 
	TChar systemDriveChar = iFs.GetSystemDriveChar();
	//append the system drive
	iSystemDrivePath.CreateL(iPath.MaxLength());
	iSystemDrivePath.Append(systemDriveChar);
	//append the system drive path which is same as ROM drive path minus ROM drive char 
	iSystemDrivePath.Append(iPath.Right(iPath.Length()-1));
#endif
	}

void CSsmCommandListResourceReaderImpl::CInitialiser::Start(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iRequestStatus == NULL, PanicNow(KPanicCmdResourceReader, EInUse1));
	aStatus = KRequestPending;
	iRequestStatus = &aStatus;

	iAction = EInitialiseFirstStep;
	CActiveBase::Start();
	}

/**
@panic ENonNullResourceFileEntries if the Resource File Entries is Not Null
@panic ENullResourceFileEntries if the Resource File Entries is Null
@panic EInvalidRunLAction if the RunL Action is invalid
*/
void CSsmCommandListResourceReaderImpl::CInitialiser::RunL()
	{
	SSMLOGLEAVEIFERROR(iStatus.Int());

	switch (iAction)
		{
	case EInitialiseFirstStep:
		DoInitialiseFirstStepL();
		break;
	case EInitialiseNextStep:
		DoInitialiseNextStepL();
		break;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	case EInitialiseSysDriveStep:
		DoInitialiseSysDriveStepL();
		break;
#ifdef __WINS__ 
	case EInitialiseExtFolderStep:
	    DoInitialiseExtFolderStepL();
	    break;
#endif //__WINS__
#endif
	default:
		PanicNow(KPanicCmdResourceReader, EInvalidRunLAction);
		break;
		}

	if (iAction == EIdle)
		{
		CompleteClientRequest(KErrNone); // must not leave between here and end of RunL
		return;
		}

	CActiveBase::Start(); // run again
	}

void CSsmCommandListResourceReaderImpl::CInitialiser::DoCancel()
	{
	// iStatus is always completed before this object is set active, so no need to complete iStatus with KErrCancel
	CompleteClientRequest(KErrCancel);
	ResetToUninitialisedState();
	}

TInt CSsmCommandListResourceReaderImpl::CInitialiser::RunError(TInt aError)
	{
	DEBUGPRINT2(_L("Resource reader initialiser reason (%d)"), aError);
	CompleteClientRequest(aError);
	ResetToUninitialisedState();
	return KErrNone;
	}

void CSsmCommandListResourceReaderImpl::CInitialiser::DoInitialiseFirstStepL()
	{
	if (!iResourcePool.IsEmpty())
		{
		// already initialised, don't count this as an error
		iAction = EIdle;
		return;
		}

	// get list of command list resource filenames
	TFileName path(iPath);
	_LIT(KStar, "*");
	path.Append(KStar);
	const TUid KUidResourceFile = {0x101f4a6b};
	__ASSERT_ALWAYS(iResourceFileEntries == NULL, PanicNow(KPanicCmdResourceReader, ENonNullResourceFileEntries));
	// The error cannot be KErrPathNotFound as fallback will be used if there is not startup path
	// Even if there are no resource files, this succeeds
	// User::LeaveIfError handles other error conditions
	User::LeaveIfError(iFs.GetDir(path, TUidType(KUidResourceFile, TUid::Uid(KUidSsmCommandListResourceFile)), ESortNone, iResourceFileEntries));
	iEntryIndex = iResourceFileEntries->Count();

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
#ifdef __WINS__
	if(!IsExtendedFolderDisabled())
	    {
	    TFileName extPath(iPath);
	    extPath.Replace(iPath.Length() - 1, 1, KExtendedCommandListPath);
	    iExtendedPath.CreateL(extPath);
	    DEBUGPRINT2(_L("Extended list path is %S"), &extPath);

	    extPath.Append(KStar);
	    const TInt extErr = (iFs.GetDir(extPath, TUidType(KUidResourceFile, TUid::Uid(KUidSsmCommandListResourceFile)), ESortNone, iExtendedResourceFileEntries));
	    if (KErrNone == extErr)
	        {
	        iExtendedEntryIndex = iExtendedResourceFileEntries->Count();
	        DEBUGPRINT2(_L("Number of resource files in extended folder : %d"), iExtendedEntryIndex);
	        }	    
	    }
#endif //__WINS__

	DEBUGPRINT2(_L("Number of resource files in ROM Drive : %d"),iEntryIndex );
	// Now, get list of command list resource filenames from system drive too
	TFileName sysPath(iSystemDrivePath);
	sysPath.Append(KStar);
	TInt err = (iFs.GetDir(sysPath, TUidType(KUidResourceFile, TUid::Uid(KUidSsmCommandListResourceFile)), ESortNone, iRssFileEntriesInSysDrive));
	if (KErrNone == err)
		{
		iSysDriveEntryIndex = iRssFileEntriesInSysDrive->Count();
		DEBUGPRINT2(_L("Number of resource files in System Drive : %d"),iSysDriveEntryIndex );
		}
	// all other error conditions are ignored as there is no compulsion for resource files to be present on system drive
	else if(KErrPathNotFound == err)
		{
		//the path for SCLs on system drive does not exist
		DEBUGPRINT1(_L("System Drive does not contain command lists"));
		}
	if(iEntryIndex == 0 && iSysDriveEntryIndex == 0)
		{
		DEBUGPRINT3(_L("Command list resource file directories (%S) and (%S) do not contain any command list resource files"), &path, &sysPath);
		SSMLOGLEAVE(KErrNotFound);
		}
#else
	if (iEntryIndex == 0)
		{
		DEBUGPRINT2(_L("Command list resource file directory (%S) does not contain any command list resource files"), &path);
		SSMLOGLEAVE(KErrNotFound);
		}
#endif
	iAction = EInitialiseNextStep;
	}

void CSsmCommandListResourceReaderImpl::CInitialiser::DoInitialiseNextStepL()
	{
	__ASSERT_ALWAYS(iResourceFileEntries != NULL, PanicNow(KPanicCmdResourceReader, ENullResourceFileEntries));
	if (iEntryIndex--)
		{
		TFileName filename(iPath);
		filename.Append((*iResourceFileEntries)[iEntryIndex].iName);
		CResourceFile* const resourceFile = OpenResourceFileL(filename);
		ParseFileL(resourceFile);
		}

	else
		{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		iAction = EInitialiseSysDriveStep;
#else
		// initialisation complete
		iAction = EIdle;
#endif
		delete iResourceFileEntries;
		iResourceFileEntries = NULL;
		}
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
void CSsmCommandListResourceReaderImpl::CInitialiser::DoInitialiseSysDriveStepL()
	{
	if (iSysDriveEntryIndex--)
		{
		TFileName filename(iSystemDrivePath);
		filename.Append((*iRssFileEntriesInSysDrive)[iSysDriveEntryIndex].iName);
		CResourceFile* const resourceFile = OpenResourceFileL(filename);
		ParseFileL(resourceFile);
		}
	else
		{
        iAction = EIdle;
        delete iRssFileEntriesInSysDrive;
        iRssFileEntriesInSysDrive = NULL;

#ifdef __WINS__
        if(!IsExtendedFolderDisabled())
            {
            //Initialise the command list from extended list only if it is enabled
            iAction = EInitialiseExtFolderStep;    
            }
#endif //__WINS__
		}
	}

#ifdef __WINS__
void CSsmCommandListResourceReaderImpl::CInitialiser::DoInitialiseExtFolderStepL()
    {
    //Append the file names from the extended folder only if it is enabled 
    if (iExtendedEntryIndex--)
        {
        TFileName filename(iExtendedPath);
        filename.Append((*iExtendedResourceFileEntries)[iExtendedEntryIndex].iName);
        CResourceFile* const resourceFile = OpenResourceFileL(filename);
        ParseFileL(resourceFile);
        }
    else
        {
        // initialisation complete
        iAction = EIdle;
        delete iExtendedResourceFileEntries;
        iExtendedResourceFileEntries = NULL;
        }
    }
#endif //__WINS__
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE

CResourceFile* CSsmCommandListResourceReaderImpl::CInitialiser::OpenResourceFileL(const TDesC& aFileName)
	{
	// open the resource file
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(iFs, aFileName, EFileRead | EFileShareReadersOnly));

	// read entire resource file into a buffer
	TInt fileSize(0);
	User::LeaveIfError(file.Size(fileSize));
	RBuf8 buf;
	buf.CreateL(fileSize);
	CleanupClosePushL(buf);
	User::LeaveIfError(file.Read(buf));

	// create a CResourceFile from the buffer and add it to array (the CResourceFile takes its own copy of the buffer)
	CResourceFile* const resourceFile = CResourceFile::NewL(buf);
	CleanupStack::PushL(resourceFile);
	iResourcePool.AppendL(resourceFile);
	CleanupStack::Pop(resourceFile);
	iResourcePool.AppendL(aFileName);
	CleanupStack::PopAndDestroy(&buf);
	CleanupStack::PopAndDestroy(&file);
	return resourceFile;
	}

void CSsmCommandListResourceReaderImpl::CInitialiser::ParseFileL(CResourceFile* aResourceFile)
	{
	// read root resource
	RResourceReader rootReader;
	const TInt KRootResourceId = 1;
	rootReader.OpenLC(aResourceFile, KRootResourceId);
	const TSsmResourceVersion version = static_cast<TSsmResourceVersion>(rootReader.ReadInt16L());
	if (version != ESsmInitialVersion)
		{
		SSMLOGLEAVE(KErrNotSupported);
		}
	const TInt reserved1 = rootReader.ReadInt16L(); // skip SSM_COMMAND_LIST_ROOT.reserved1
	const TInt commandListMappingResourceId = rootReader.ReadInt32L();
	if (commandListMappingResourceId <= 0)
		{
		DEBUGPRINT1(_L("Command list resource file contains no mappings"));
		SSMLOGLEAVE(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(&rootReader);

	// read mapping resource
	RResourceReader mappingReader;
	mappingReader.OpenLC(aResourceFile, commandListMappingResourceId);
	const TInt mappingCount = mappingReader.ReadInt16L();
	if (!mappingCount)
		{
		DEBUGPRINT1(_L("Command list resource file contains no mappings"));
		SSMLOGLEAVE(KErrNotFound);
		}
	for (TInt i = 0; i < mappingCount; i++)
		{
		// add each mapping to the pool
		TUint commandListId = mappingReader.ReadUint32L();
		TInt resourceId = mappingReader.ReadInt32L();
		TMapping mapping(commandListId, resourceId, aResourceFile);
		iResourcePool.AppendL(mapping);
		}
	CleanupStack::PopAndDestroy(&mappingReader);
	}

// CPreparer
CSsmCommandListResourceReaderImpl::CPreparer::CPreparer(const CResourcePool& aResourcePool, MSsmConditionalCallback& aConditionalCallback)
	:CActiveBase(EPriorityStandard), iResourcePool(aResourcePool), iConditionalCallback(aConditionalCallback), iSwpChange(0, 0), iMapping(0, 0, NULL)
	{
	CActiveScheduler::Add(this);
	}

CSsmCommandListResourceReaderImpl::CPreparer::~CPreparer()
	{
	Cancel();
	ResetReadyForNextPrepare();
	}

void CSsmCommandListResourceReaderImpl::CPreparer::Start(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmState& aState, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iRequestStatus == NULL, PanicNow(KPanicCmdResourceReader, EInUse2));
	__ASSERT_ALWAYS(!iResourcePool.IsEmpty(), PanicNow(KPanicCmdResourceReader, ENotInitialized));
	aStatus = KRequestPending;
	iRequestStatus = &aStatus;

	iCommandList = &aCommandList;
	iCommandListId = aCommandListId;
	iState = aState;
	iCommandListReady = EFalse;

	iAction = EPrepareFirstStep;
	CActiveBase::Start();
	}

void CSsmCommandListResourceReaderImpl::CPreparer::Start(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iRequestStatus == NULL, PanicNow(KPanicCmdResourceReader, EInUse3));
	__ASSERT_ALWAYS(!iResourcePool.IsEmpty(), PanicNow(KPanicCmdResourceReader, ENotInitialized2));
	aStatus = KRequestPending;
	iRequestStatus = &aStatus;

	iCommandList = &aCommandList;
	iCommandListId = aCommandListId;
	iSwpChange = aSwp;
	iCommandListReady = EFalse;

	iAction = EPrepareFirstStep;
	CActiveBase::Start();
	}

/**
@panic EInvalidRunLAction if the RunL Action is invalid
*/
void CSsmCommandListResourceReaderImpl::CPreparer::RunL()
	{
	SSMLOGLEAVEIFERROR(iStatus.Int());

	switch (iAction)
		{
	case EPrepareFirstStep:
		DoPrepareFirstStepL();
		break;
	case EPrepareNextStep:
		DoPrepareNextStepL();
		break;
	default:
		PanicNow(KPanicCmdResourceReader, EInvalidRunLAction);
		break;
		}

	if (iAction == EIdle)
		{
		CompleteClientRequest(KErrNone); // must not leave between here and end of RunL
		return;
		}

	CActiveBase::Start(); // run again
	}

void CSsmCommandListResourceReaderImpl::CPreparer::DoCancel()
	{
	// iStatus is always completed before this object is set active, so no need to complete iStatus with KErrCancel
	CompleteClientRequest(KErrCancel);
	ResetReadyForNextPrepare();
	}

TInt CSsmCommandListResourceReaderImpl::CPreparer::RunError(TInt aError)
	{
	DEBUGPRINT2(_L("Resource reader preparer reason (%d)"), aError);
	CompleteClientRequest(aError);
	ResetReadyForNextPrepare();
	return KErrNone;
	}

TBool CSsmCommandListResourceReaderImpl::CPreparer::IsCommandListReady()
	{
	// enables caller to find out if the command list they passed
	// to PrepareCommandList is fully prepared or not
	if (iCommandListReady)
		{
		iCommandListReady = EFalse;
		return ETrue;
		}
	return EFalse;
	}

void CSsmCommandListResourceReaderImpl::CPreparer::ResetReadyForNextPrepare()
	{
	iListReader.Close();
	iCommandIndex = 0;
	iCommandListReady = EFalse;
	iAction = EIdle;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iCommandIndexInRssFile.Reset();
	iMappingArray.Reset();
#endif
	}


void CSsmCommandListResourceReaderImpl::CPreparer::DoPrepareFirstStepL()
	{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iCurrentCommandListInMapping = 0;
	iResourcePool.UpdateMappingArrayL(iCommandListId,iMappingArray);
	TInt substateCount = iMappingArray.Count();
	iCommandIndexInRssFile.ReserveL(substateCount);
	
	// get number of commands from all the lists
	for(TInt substateIndex = 0; substateIndex < substateCount; ++substateIndex )
		{
		iListReader.OpenL(iMappingArray[substateIndex].iResourceFile, iMappingArray[substateIndex].iResourceId);
		//advancing the pointer to point to number of commands in list
		iListReader.AdvanceL(sizeof(TInt32));
		iCommandIndexInRssFile.AppendL(iListReader.ReadUint16L());
		iCommandIndex +=  iCommandIndexInRssFile[substateIndex]; 
		iListReader.Close();
		}
	//open the first resource id and read the commands.
	iListReader.OpenL(iMappingArray[iCurrentCommandListInMapping].iResourceFile, iMappingArray[iCurrentCommandListInMapping].iResourceId);
	// the delay between the commands is read from only the frist command list. In case of other lists, it is ignored.
	iCommandList->SetDelayBetweenCommands(iListReader.ReadInt32L());
	iCommandList->SetResourceFileNameL(iResourcePool.FileNameForResourceFileL(iMappingArray[iCurrentCommandListInMapping].iResourceFile));
	//skip reading command count again and jump to command resourceId
	iListReader.AdvanceL(sizeof(TUint16));
#else
	iMapping = iResourcePool.MappingL(iCommandListId);
	iListReader.OpenL(iMapping.iResourceFile, iMapping.iResourceId);
	iCommandList->SetDelayBetweenCommands(iListReader.ReadInt32L());
	iCommandIndex = iListReader.ReadUint16L(); // command count

	// Set up the resource file name for this list
	iCommandList->SetResourceFileNameL(iResourcePool.FileNameForResourceFileL(iMapping.iResourceFile));
#endif
	
	// Set up the conditional callback
	iCommandList->SetConditionalCallback(iConditionalCallback);
	
	iAction = EPrepareNextStep;
	}

void CSsmCommandListResourceReaderImpl::CPreparer::DoPrepareNextStepL()
	{
	const TInt KBatchSize = 5; // Max number of commands prepared per RunL, this may need to be adjusted if responsiveness is a problem
	iBatchIndex = KBatchSize;
			
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	TBool inProgress = ETrue;
	while (inProgress && iBatchIndex --)
		{
		if (iCommandIndexInRssFile[iCurrentCommandListInMapping] == 0 )
			{
			//the current command list is parsed and all commands added
			// go to the next commnad list
			++iCurrentCommandListInMapping;
			
			if ( (iCurrentCommandListInMapping+1) > iCommandIndexInRssFile.Count() )
				{
				// all command lists are processed. Set the inProgress flag to "EFalse" 
				inProgress = EFalse;
				break;
				}
			else if((iCommandIndexInRssFile[iCurrentCommandListInMapping] == 0))
					{
					//The current commandlist is empty 
					//go to the next commandlist in the mapping.
					continue;
					}
			else
				{
				//open the next resource id and read the commands
				iListReader.Close();
				iListReader.OpenL(iMappingArray[iCurrentCommandListInMapping].iResourceFile, iMappingArray[iCurrentCommandListInMapping].iResourceId);
				// skipping delay between commands as it is already set from first rss file
				iListReader.AdvanceL(sizeof(TInt32));
				iCommandList->SetResourceFileNameL(iResourcePool.FileNameForResourceFileL(iMappingArray[iCurrentCommandListInMapping].iResourceFile));
				//skip reading command count again and jump to command resourceId
				iListReader.AdvanceL(sizeof(TUint16));
				}
			}
#else
	while(iCommandIndex && iBatchIndex--)
		{
		--iCommandIndex; // Note: can't decrement in while expression above, incase iBatchIndex is zero
#endif
		// open command resource
		const TInt commandResourceId = iListReader.ReadInt32L();
		RResourceReader commandReader;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		commandReader.OpenLC(iMappingArray[iCurrentCommandListInMapping].iResourceFile, commandResourceId);
#else
		commandReader.OpenLC(iMapping.iResourceFile, commandResourceId);
#endif
		// read conditional imformation
		const TInt conditionalInfoResourceId = commandReader.ReadInt32L();

		// read command type
		const TSsmCommandType type = static_cast<TSsmCommandType>(commandReader.ReadInt16L());
		commandReader.RewindL(sizeof(TInt16)); // commands expect reader to be pointing to the command type
		
		// construct command and add to command list
		TRAPD(err, AddCommandToListL(type, commandReader, conditionalInfoResourceId));
		if(KErrNone != err)
			{
			DEBUGPRINT2A("Failed to add command of type: %d", type);
			}
		SSMLOGLEAVEIFERROR(err);
		CleanupStack::PopAndDestroy(&commandReader);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE	
		--(iCommandIndexInRssFile[iCurrentCommandListInMapping]);
		--iCommandIndex;
#endif
		}
	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	if (inProgress == EFalse || iCommandIndex == 0) // list preparation complete
#else
	if (iCommandIndex == 0) // list preparation complete
#endif
		{
		ResetReadyForNextPrepare();
		iCommandListReady = ETrue;
		}
	}

void CSsmCommandListResourceReaderImpl::CPreparer::AddCommandToListL(TSsmCommandType aType, RResourceReader& aCommandReader, TInt aConditionalInfoResourceId)
	{
	// construct command from resource and add to list
	CSsmCommandBase* cmd = NULL;

	switch (aType)
		{
	case ESsmCmdPublishSwp:
			{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			TSsmCommandParameters params(aCommandReader, iMappingArray[iCurrentCommandListInMapping].iResourceFile, aConditionalInfoResourceId, iSwpChange);
#else
			TSsmCommandParameters params(aCommandReader, iMapping.iResourceFile, aConditionalInfoResourceId, iSwpChange);
#endif
			cmd = SsmCommandFactory::ConstructCommandFromResourceLC(aType, params);
			break;
			}
	case ESsmCmdPublishSystemState:
			{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			TSsmCommandParameters params(aCommandReader, iMappingArray[iCurrentCommandListInMapping].iResourceFile, aConditionalInfoResourceId, iState);
#else
			TSsmCommandParameters params(aCommandReader, iMapping.iResourceFile, aConditionalInfoResourceId, iState);
#endif
			cmd = SsmCommandFactory::ConstructCommandFromResourceLC(aType, params);
			break;
			}
	default:
			{
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
			TSsmCommandParameters params(aCommandReader, iMappingArray[iCurrentCommandListInMapping].iResourceFile, aConditionalInfoResourceId);
#else
			TSsmCommandParameters params(aCommandReader, iMapping.iResourceFile, aConditionalInfoResourceId);
#endif
			cmd = SsmCommandFactory::ConstructCommandFromResourceLC(aType, params);
			break;
			}
		}
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	// Load the resource file if there is conditional information
	if( cmd && cmd->ConditionalInformation() != 0 )
		{
		//Only if there is conditional information, we will set the command's resource filename
		cmd->SetCommandResourceFileNameL(iResourcePool.FileNameForResourceFileL(iMappingArray[iCurrentCommandListInMapping].iResourceFile));
		}
#endif
	//In the case of flexible merge the commands are appended in the commandlist based on priority. 
	iCommandList->AppendL(cmd);
	CleanupStack::Pop(cmd);
	}


