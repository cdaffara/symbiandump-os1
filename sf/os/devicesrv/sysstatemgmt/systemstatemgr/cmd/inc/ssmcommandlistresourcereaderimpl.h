// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMCOMMANDLISTRESOURCEREADERIMPL_H__
#define __SSMCOMMANDLISTRESOURCEREADERIMPL_H__

#include <e32base.h>
#include <f32file.h>

#include <ssm/ssmstate.h>
#include <ssm/ssmswp.h>
#include <ssm/ssmcmd.hrh>
#include <barsread2.h>

class CResourceFile;
class MSsmConditionalCallback;
class MSsmCommandList;

/** Private implementation of CSsmCommandListResourceReader.

@internalComponent
*/
NONSHARABLE_CLASS(CSsmCommandListResourceReaderImpl) : public CBase
	{
public:
	static CSsmCommandListResourceReaderImpl* NewL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback);
	~CSsmCommandListResourceReaderImpl();

	void Initialise(TRequestStatus& aStatus);
	void InitialiseCancel();
	void PrepareCommandList(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmState& aState, TRequestStatus& aStatus);
	void PrepareCommandList(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmSwp& aSwp, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	TBool IsCommandListReady() const;
	TBool Busy() const;
	void GetCommandListIdsL(RArray<TInt>& aArray) const;
	
private:
	enum TInternalAction
		{
		EIdle,
		EInitialiseFirstStep,
		EInitialiseNextStep,
		EInitialiseSysDriveStep,
		EPrepareFirstStep,
		EPrepareNextStep,
		EInitialiseExtFolderStep
		};

	NONSHARABLE_CLASS(TMapping)
		{
	public:
		TMapping(TInt aCommandListId, TInt aResourceId, CResourceFile* aResourceFile) : iCommandListId(aCommandListId), iResourceId(aResourceId), iResourceFile(aResourceFile) {}
		TInt iCommandListId;	//this should always be the first member data
		TInt iResourceId;
		CResourceFile* iResourceFile;	// CSsmCommandListResourceReaderImpl takes the ownership
		};

	NONSHARABLE_CLASS(CResourcePool) : public CBase
		{
	public:
		CResourcePool();
		~CResourcePool();
		TBool IsEmpty() const;
		void Reset();
		void AppendL(const CResourceFile* aResourceFile);
		void AppendL(const TDesC& aFileName);
		void AppendL(const TMapping& aMapping);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		void UpdateMappingArrayL(TInt aCommandListId, RArray<TMapping>& aMappingArray) const;
#else
		TMapping MappingL(TInt aCommandListId) const;
#endif	//SYMBIAN_SSM_FLEXIBLE_MERGE
		void GetCommandListIdsL(RArray<TInt>& aArray) const;
		const TDesC& FileNameForResourceFileL(const CResourceFile* aResourceFile) const;

	private:
		RPointerArray<CResourceFile> iResourceFiles;
		RArray<TMapping> iMappings;
		RPointerArray<HBufC> iResourceFileNames;
		};

	NONSHARABLE_CLASS(CActiveBase) : public CActive
		{
	public:
		~CActiveBase();
	protected:
		CActiveBase(TInt aPriority);
		void Start();
		void CompleteClientRequest(TInt aReason);
	protected:
		TRequestStatus* iRequestStatus;
		TInternalAction iAction;
		};

	NONSHARABLE_CLASS(CInitialiser) : public CActiveBase
		{
	public:
		static CInitialiser* NewL(RFs& aFs, const TDesC& aCommandListPath, CResourcePool& aResourcePool);
		void Start(TRequestStatus& aStatus);
		~CInitialiser();
	private:
		CInitialiser(RFs& aFs, CResourcePool& aResourcePool);
		void ConstructL(const TDesC& aCommandListPath);
		void ResetToUninitialisedState();
		void DoInitialiseFirstStepL();
		void DoInitialiseNextStepL();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		void DoInitialiseSysDriveStepL();
#ifdef __WINS__ 
		void DoInitialiseExtFolderStepL();
#endif
#endif
		CResourceFile* OpenResourceFileL(const TDesC& aFileName);
		void ParseFileL(CResourceFile* aResourceFile);
		// from CActive
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	private:
		RFs& iFs;
		RBuf iPath;
		CResourcePool& iResourcePool;
		CDir* iResourceFileEntries;
		TInt iEntryIndex;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		RBuf iSystemDrivePath;
		CDir* iRssFileEntriesInSysDrive;
		TInt iSysDriveEntryIndex;
#ifdef __WINS__ 
	    CDir* iExtendedResourceFileEntries;
	    RBuf iExtendedPath;
	    TInt iExtendedEntryIndex;
#endif //__WINS__
	      
#endif	//SYMBIAN_SSM_FLEXIBLE_MERGE
		};

	NONSHARABLE_CLASS(CPreparer) : public CActiveBase
		{
	public:
		CPreparer(const CResourcePool& aResourcePool, MSsmConditionalCallback& aConditionalCallback);
		~CPreparer();
		void Start(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmState& aState, TRequestStatus& aStatus);
		void Start(MSsmCommandList& aCommandList, TInt aCommandListId, const TSsmSwp& aSwp, TRequestStatus& aStatus);
		TBool IsCommandListReady();
	private:
		void ResetReadyForNextPrepare();
		void DoPrepareFirstStepL();
		void DoPrepareNextStepL();
		void AddCommandToListL(TSsmCommandType aType, RResourceReader& aCommandReader, TInt aConditionalInfoResourceId);
		// from CActive
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	private:
		const CResourcePool& iResourcePool;
		MSsmConditionalCallback& iConditionalCallback;

		// input parameters for current prepare operation
		MSsmCommandList* iCommandList;	// Not owning
		TInt iCommandListId;
		TSsmState iState;
		TSsmSwp iSwpChange;

		// working variables for current prepare operation
		TMapping iMapping; // mapping used for current prepare
		RResourceReader iListReader;
		TInt iCommandIndex;
		TInt iBatchIndex; // number of commands left to read in current RunL
		TBool iCommandListReady;
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
		RArray<TMapping> iMappingArray;
		RArray<TInt> iCommandIndexInRssFile;
		TInt iCurrentCommandListInMapping;
#endif	//SYMBIAN_SSM_FLEXIBLE_MERGE
		};

private:
	CSsmCommandListResourceReaderImpl();
	void ConstructL(RFs& aFs, const TDesC& aCommandListPath, MSsmConditionalCallback& aConditionalCallback);

private:
	CResourcePool* iResourcePool;
	CInitialiser* iInitialiser;
	CPreparer* iPreparer;
	};

#endif // __SSMCOMMANDLISTRESOURCEREADERIMPL_H__
