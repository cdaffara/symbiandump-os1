//audiocodec.cpp

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
//




// INCLUDES
#include "resourcedata.h"
#include <barsc.h>
#include <a3f/a3f_trace_utils.h>

_LIT(KDC_RESOURCE_FILES_DIR, "\\resource\\a3f\\");
const TInt KAssumedResourceId =1;

const TSampleRateTableEntry KRateTableLookup[] = {
							{ 8000, EMMFSampleRate8000Hz },
							{ 11025, EMMFSampleRate11025Hz },
							{ 12000, EMMFSampleRate12000Hz },
							{ 16000, EMMFSampleRate16000Hz },
							{ 22050, EMMFSampleRate22050Hz },
							{ 24000, EMMFSampleRate24000Hz },
							{ 32000, EMMFSampleRate32000Hz },
							{ 44100, EMMFSampleRate44100Hz },
							{ 48000, EMMFSampleRate48000Hz },
							{ 64000, EMMFSampleRate64000Hz },
							{ 88200, EMMFSampleRate88200Hz },
							{ 96000, EMMFSampleRate96000Hz },
						};

const TAudioModeTableEntry KModeTableLookup[] = {
							{ EMMFMono, {KA3FModeMonoValue} },
							{ EMMFStereo, {KA3FModeStereoNonInterleavedValue} },
							};

const TInt KMaxSampleRateIndex = 11;

// ============================ MEMBER FUNCTIONS ===============================

CResourceData::CResourceData()
	{
	TRACE_CREATE();
	}

// -----------------------------------------------------------------------------
// CResourceData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CResourceData* CResourceData::NewL(TUid aResourceUid)
	{
	DP_STATIC_CONTEXT( CResourceData::NewL *CD0*, CtxDevSound, DPAPI);
	DP1_IN("aResourceUid = 0x%x", aResourceUid);

	CResourceData* obj = NULL;
	obj = new ( ELeave ) CResourceData;
	CleanupStack::PushL( obj );
	obj->ConstructL( aResourceUid );
	CleanupStack::Pop(obj);

	DP0_RET(obj, "obj = 0x%x" );
	}

// -----------------------------------------------------------------------------
// CResourceData::~CResourceData
// Destructor.
// -----------------------------------------------------------------------------
//
CResourceData::~CResourceData()
	{
	DP_CONTEXT(CResourceData::~CResourceData *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	delete iResourceData;
	DP_OUT();
	}


// -----------------------------------------------------------------------------
// CResourceData::GetSModes
// Reads the capabilities data.
// -----------------------------------------------------------------------------
//
TInt CResourceData::GetSModes(TMode aMode, RArray<TUid>& aModeConfig)
	{
	DP_CONTEXT( CResourceData::GetSampleModes *CD1*, CtxDevSound, DPLOCAL );
	DP1_IN( "TMode aMode = 0x%x", aMode );
	TInt ret = SeekToMode(aMode);
	if (ret != KErrNone)
		{
		DP0_RET(ret, "%d");
		}

	// capability data size
	TInt capDataSize = iResourceReader.ReadInt16();

	// sanity check for cap data size
	if (iResourceReader.Ptr() > iResourceDataEnd-capDataSize)
		{
		DP0( DLERR, "Pointer mismatch with cap data" );
		DP0_RET(KErrCorrupt, "KErrCorrupt" );
		}

	//Getting new Capabilities so clean the array.
	aModeConfig.Reset();

	TInt tempSampleRate = iResourceReader.ReadInt32();
	TInt tempEnconding = iResourceReader.ReadInt32();
	TInt tempMode = iResourceReader.ReadInt32();

	TInt err(KErrNone);

	for (TUint i=0; i<=KMaxModeIndex; i++)
		{
		if((KModeTableLookup[i].iAudioModeValue) & tempMode)
			{
			err = aModeConfig.Append(KModeTableLookup[i].iAudioMode);
			if (err != KErrNone)
				{
				break;
				}
			}
		}

	DP0_RET(err, "");
	}


// -----------------------------------------------------------------------------
// CResourceData::GetSSampleRates
// Reads the capabilities data.
// -----------------------------------------------------------------------------
//
TInt CResourceData::GetSSampleRates(TMode aMode, RArray<TInt>& aSampleRateConfig)
	{
	DP_CONTEXT( CResourceData::GetSSampleRates *CD1*, CtxDevSound, DPLOCAL );
	DP1_IN( "TMode aMode = 0x%x", aMode );
	//TODO to be change every hwdevice is either decode or encode
	//By the moment The same resource file has both modes
	TInt ret = SeekToMode(aMode);
	if (ret != KErrNone)
		{
		DP0_RET(ret, "%d");
		}

	// capability data size
	TInt capDataSize = iResourceReader.ReadInt16();

	// sanity check for cap data size
	if (iResourceReader.Ptr() > iResourceDataEnd-capDataSize)
		{
		DP0( DLERR, "Pointer mismatch with cap data" );
		DP0_RET(KErrCorrupt, "KErrCorrupt" );
		}

	//Getting new Capabilities so clean the array.
	aSampleRateConfig.Reset();

	TInt tempSampleRate = iResourceReader.ReadInt32();
	TInt err(KErrNone);

	for (TUint i=0; i<=KMaxSampleRateIndex; i++)
		{
		if(KRateTableLookup[i].iSampleRate & tempSampleRate)
			{
			err = aSampleRateConfig.Append(KRateTableLookup[i].iSampleRateValue);
			if (err != KErrNone)
				{
				break;
				}
			}
		}

	DP0_RET(err, "");
	}

// -----------------------------------------------------------------------------
// CResourceData::ConstructL
// Symbian 2nd phase constructor can leave.
// Reads the resource file for the correct UID.
// -----------------------------------------------------------------------------
//
void CResourceData::ConstructL(TUid aResourceUid)
	{
	DP_CONTEXT( CResourceData::ConstructL *CD1*, CtxDevSound, DPLOCAL );
	DP1_IN( "aResourceUid = 0x%x", aResourceUid );
	// Open a file server session.
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );

	TBuf16<KResFileNameLength> fileName;
	TBuf16<KResFileNameAndPathLength> pathAndFileName (KDrive); // Z:
	fileName.Format(KFileNameFormat,aResourceUid.iUid); // HwDev0x<UID>
	pathAndFileName.Append(KDC_RESOURCE_FILES_DIR); /*  \\resource\\ */
	pathAndFileName.Append(fileName); // HwDevxxx.rsc
	pathAndFileName.ZeroTerminate();
	HBufC16* fileNamePtr = pathAndFileName.AllocL();
	CleanupStack::PushL( fileNamePtr );
	RResourceFile rscFile;
	rscFile.OpenL(fs, fileNamePtr->Des());
	CleanupClosePushL(rscFile);
	// read the resource data
	iResourceData = rscFile.AllocReadL(KAssumedResourceId);
	// initialize the reader
	iResourceReader.SetBuffer( iResourceData );
	iResourceUid =  aResourceUid;
	CleanupStack::PopAndDestroy( 3, &fs ); // fs, fileNamePtr,rscFile
	DP_OUT();
	}

// -----------------------------------------------------------------------------
// CResourceData::SeekToMode
// Utility method for seeking resource to the correct mode. Leaves with
// KErrNotFound if mode is not found in the opened resource.
// -----------------------------------------------------------------------------
//
TInt CResourceData::SeekToMode(TMode aMode)
	{
	DP_CONTEXT( CResourceData::SeekToMode *CD1*, CtxDevSound, DPLOCAL );
	DP1_IN( "aMode = 0x%x", aMode );
	// set buffer position to beginning
	iResourceReader.SetBuffer(iResourceData);
	// read the resource uid
	TUid resourceUid = {iResourceReader.ReadInt32()};
	if (resourceUid != iResourceUid)
		{
		DP2( DLERR, "Mismatching resource uids resourceUid = %x iResourceUid = %x",resourceUid.iUid,iResourceUid.iUid );
		DP0_RET(KErrCorrupt, "KErrCorrupt" );
		}
	// read the number of mode-entries in the resource
	TInt modeCount = iResourceReader.ReadInt16();
	for (TInt modeIndex = 0; modeIndex < modeCount; modeIndex++)
		{
		// read the mode number
		TMode mode = (TMode) iResourceReader.ReadInt32();
		if (mode == aMode)
			{
			DP0_RET(KErrNone, "" );
			}
		// capability data size
		TInt capDataSize = iResourceReader.ReadInt16();

		// skip capability data
		if (iResourceReader.Ptr() > iResourceDataEnd-capDataSize)
			{
			DP0( DLERR, "Pointer mismatch with cap data" );
			DP0_RET(KErrCorrupt, "KErrCorrupt" );
			}
		iResourceReader.Advance( capDataSize );
		}
	// not found
	DP1( DLERR, "Mode 0x%x not found?", aMode );
	DP0_RET(KErrNotFound, "KErrNotFound" );
	}


