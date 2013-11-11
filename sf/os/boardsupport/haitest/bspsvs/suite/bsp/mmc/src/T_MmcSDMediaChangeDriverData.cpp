/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "T_MmcSDMediaChangeDriverData.h"

//MMCSD Commands index
/*@{*/

_LIT(KEnumNull,							" ");

_LIT(KCmdMediaState,					"MediaState");
_LIT(KCmdForceMediaChange,				"ForceMediaChange");
_LIT(KCmdReplayCount,					"ReplayCount");

_LIT(KExpectedMediaState,				"expectedmediastate");
_LIT(KExpectedeventcount,				"expectedeventcount");

_LIT(KOpened,							"EOpened");
_LIT(KClosed,							"EClosed");

const CDataWrapperBase::TEnumEntryTable	CT_MMCSDMediaChangeDriverData::iEnumMediaState[] =
	{
//	Enum as a descriptor					Enum
	KOpened,								EOpened,
	KClosed,								EClosed,
	KEnumNull,								-1
	};

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_MMCSDMediaChangeDriverData* CT_MMCSDMediaChangeDriverData::NewL()
	{
	CT_MMCSDMediaChangeDriverData*	ret=new (ELeave) CT_MMCSDMediaChangeDriverData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_MMCSDMediaChangeDriverData::CT_MMCSDMediaChangeDriverData()
:	CT_MmcSDDriverData()
,	iAsyncErrorIndex(0)
,	iCons(NULL)
	{
	}

void CT_MMCSDMediaChangeDriverData::ConstructL()
/**
 * Second phase construction
 *
 * @internalComponent
 *
 * @return	N/A
 *
 * @pre		None
 * @post	None
 *
 * @leave	system wide error
 */
	{
	CT_MmcSDDriverData::ConstructL();
	if(!iCons)
		{
		iCons = GetConsoleL();
		}

	}

CT_MMCSDMediaChangeDriverData::~CT_MMCSDMediaChangeDriverData()
/**
 * Public destructor
 */
	{
	delete iCons;
	}

TBool CT_MMCSDMediaChangeDriverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdMediaState)
		{
		DoCmdMediaState(aSection);
		}
 	else if ( aCommand==KCmdForceMediaChange)
		{
		DoCmdForceMediaChange();
		}
	else if ( aCommand==KCmdReplayCount)
		{
		DoCmdReplayCount(aSection);
		}
	else
		{
		ret=CT_MmcSDDriverData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}

/**
 * Get the Media State Information
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDMediaChangeDriverData::DoCmdMediaState(const TDesC& aSection)
	{
	TInt err;
	TInt mediaState;
	err= iMmcSDController->MediaStateInfo(mediaState);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("MMC/SD : Error in getting MediaState information, error code = %d \n"),err);
			SetBlockResult(EFail);
			}
		else
			{
			switch(mediaState)
				{
			case EOpened:
					{
					INFO_PRINTF1(_L("MMC/SD Current MediaChange MediaState - Opened"));
					break;
					}
			case EClosed:
					{
					INFO_PRINTF1(_L("MMC/SD Current MediaChange MediaState - Closed"));
					break;
					}
			default:
					{
					ERR_PRINTF2(_L("MMC/SD : Current Media State is not supported by the platform-MediaState= %d \n"),mediaState);
					SetBlockResult(EFail);
					break;
					}
				}
				TInt	expectedmediaState;
				if ( GetEnumFromConfig(aSection, KExpectedMediaState(), iEnumMediaState, expectedmediaState) )
					{
					if(mediaState != expectedmediaState)
						{
						ERR_PRINTF3(_L("mediaState(%d) != expectedmediaState(%d)"), mediaState, expectedmediaState);
						SetBlockResult(EFail);
						}
					}
				else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("expectedmediaState Value Missing From INI File"));
				}
			}
	}
/**
 * Force Media to Change the State
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDMediaChangeDriverData::DoCmdForceMediaChange()
	{
	TInt err;
	err= iMmcSDController->ForceMediaChange();
	if(err != KErrNone)
			{
			ERR_PRINTF2(_L("MMC/SD : Error in forcing media state, error code = %d \n"),err);
			SetError(err);
			}
	}

/**
 * Count of media events yet to be acknowledged.
 *
 * @param aSection			The section in the ini containing data for the command
 *
 * @return					void
 *
 * @leave					System wide error
 */
void CT_MMCSDMediaChangeDriverData::DoCmdReplayCount(const TDesC& aSection)
	{
	TInt err;
	TInt mediaEventCount;
	err=iMmcSDController->MediaChangeCounter(mediaEventCount);
	if(err != KErrNone)
			{
			ERR_PRINTF2(_L("MMC/SD : Error in reading media event replay count, error code = %d \n"),err);
			SetBlockResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("MMC/SD: Media Event Count    = %d \n"),mediaEventCount);
			TInt	expectedmediaEventCount;
			if(GetBoolFromConfig(aSection, KExpectedeventcount(), expectedmediaEventCount))
				{
				if(mediaEventCount != expectedmediaEventCount)
					{
					ERR_PRINTF3(_L("mediaEventCount(%d) != expectedchangedState(%d)"), mediaEventCount, expectedmediaEventCount);
					SetBlockResult(EFail);
					}
				}
			else
				{
				SetBlockResult(EFail);
				WARN_PRINTF1(_L("expectedchangedState Value Missing From INI File"));
				}
			}
	}
