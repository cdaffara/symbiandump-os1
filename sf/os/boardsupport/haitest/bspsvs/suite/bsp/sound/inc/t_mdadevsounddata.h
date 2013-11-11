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

/**
@test
@internalComponent

This contains CT_MdaDevSoundData
*/

#if (!defined __T_MDADEVSOUND_DATA_H__)
#define __T_MDADEVSOUND_DATA_H__

//	User Includes
#include "DataWrapperBase.h"
#include "sound_util.h"
#include "t_soundactivecallback.h"

//	EPOC includes
#include <mdasound.h>
#include <e32twin.h>

/**
 * Class to wrap RMdaDevSound APIs.
 *
 */
class CT_MdaDevSoundData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static CT_MdaDevSoundData*	NewL(RMdaDevSound* aServerStore);
	/**
	* Public destructor
	*/
	~CT_MdaDevSoundData();

	/**
	* Process a command read from the ini file
	*
	* @param	aCommand requiring command to be processed
	* @param	aSection the section in the ini file requiring the command to be processed
	* @param	aAsyncErrorIndex the index of asynchronous command error code belongs to.
	* 
	* @leave	system wide error
	*
	* @return	ETrue if the command is processed
	*/
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	* Return a pointer to the object that the data wraps
	*
	* @return	pointer to the object that the data wraps
	*/
	virtual TAny*	GetObject();
	virtual void	SetObjectL(TAny* aAny);
	virtual void	DisownObjectL();

	inline virtual 	TCleanupOperation CleanupOperation();

	/**
	* Query to see if there are any outstanding requests
	*
	* @return ETrue if there are any outstanding requests
	*/
	
	void RunL(CActive* aActive, TInt aIndex);
	void DoCancel(CActive* aActive, TInt aIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_MdaDevSoundData(RMdaDevSound* aServerStore);
	/**
	* Second phase construction
	*/
	void ConstructL();

private:
	static void CleanupOperation( TAny* aAny );

	/**
	* General commands
	*/
	void	DoCmdNewL();
	void	DoCmdOpen(const TDesC& aSection);
	void	DoCmdClose();
	void	DoCmdDestructor();
	void	DoCmdIsMdaSound();
	void	DoCmdVersionRequired(const TDesC& aSection);

	/**
	* Playback commands
	*/
	void	DoCmdPlayVolume(const TDesC& aSection);
	void	DoCmdSetPlayVolume(const TDesC& aSection);
	void	DoCmdPlayDataL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdCancelPlayData();
	void	DoCmdFlushPlayBuffer();
	void	DoCmdNotifyPlayErrorL(const TInt aAsyncErrorIndex);
	void	DoCmdCancelNotifyPlayError();
	void	DoCmdBytesPlayed(const TDesC& aSection);
	void	DoCmdResetBytesPlayed();
	void	DoCmdPausePlayBuffer();
	void	DoCmdResumePlaying();

	/**
	* Recording commands
	*/
	void	DoCmdRecordDataL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void	DoCmdCancelRecordData();
	void	DoCmdFlushRecordBuffer();
	void	DoCmdRecordLevel(const TDesC& aSection);	
	void	DoCmdSetRecordLevel(const TDesC& aSection);
	void	DoCmdNotifyRecordErrorL(const TInt aAsyncErrorIndex);
	void	DoCmdCancelNotifyRecordError();
	
	/**
	* Format commands
	*/
	void	DoCmdPlayFormatsSupportedL(const TDesC& aSection);
	void	DoCmdGetPlayFormatL(const TDesC& aSection);
	void	DoCmdSetPlayFormatL(const TDesC& aSection);
	void	DoCmdRecordFormatsSupportedL(const TDesC& aSection);
	void	DoCmdGetRecordFormatL(const TDesC& aSection);
	void	DoCmdSetRecordFormatL(const TDesC& aSection);
	
	/**
	* Utility commands
	*/
	void DoCmdUtilityLoadDrivers(const TDesC& aSection);
	void DoCmdUtilityFreeDrivers(const TDesC& aSection);
	void DoCmdUtilityDeleteFileL(const TDesC& aSection);

	/**
	* Helper commands
	*/
	void	DoCleanup();
	

private:
	/** RMdaDevSound class instance that is tested */
	RMdaDevSound*										iMdaDevSound;
	
	/** Reference to RMdaDevSound object stored on server side */
	RMdaDevSound* 										iServerStore;

	/** The request status for playback errors */
	RPointerArray<CActiveCallback>						iNotifyPlayError;

	/** The request status for recording errors */
	RPointerArray<CActiveCallback>						iNotifyRecordError;

	/** The request status for sound playback */
	RPointerArray<CT_SoundActiveCallback>				iPlay;

	/** The request status for sound recording */
	RPointerArray<CT_SoundActiveCallback>				iRecord;
	};

#define GET_MANDATORY_STRING_PARAMETER(aParamName, aSection, aResult)		GetCommandStringParameter(aParamName, aSection, aResult, (TText8*)__FILE__, __LINE__, ETrue)
#define GET_MANDATORY_INT_PARAMETER(aParamName, aSection, aResult)		GetCommandIntParameter(aParamName, aSection, aResult,(TText8*) __FILE__, __LINE__, ETrue)
#define GET_MANDATORY_INT64_PARAMETER(aParamName, aSection, aResult)		GetCommandInt64Parameter(aParamName, aSection, aResult,(TText8*) __FILE__, __LINE__, ETrue)
#define GET_MANDATORY_BOOL_PARAMETER(aParamName, aSection, aResult)		GetCommandBoolParameter(aParamName, aSection, aResult, (TText8*)__FILE__, __LINE__, ETrue)

#define GET_OPTIONAL_STRING_PARAMETER(aParamName, aSection, aResult)		GetCommandStringParameter(aParamName, aSection, aResult, (TText8*) __FILE__, __LINE__, EFalse)
#define GET_OPTIONAL_INT_PARAMETER(aParamName, aSection, aResult)		GetCommandIntParameter(aParamName, aSection, aResult,(TText8*) __FILE__, __LINE__, EFalse)
#define GET_OPTIONAL_INT64_PARAMETER(aParamName, aSection, aResult)		GetCommandInt64Parameter(aParamName, aSection, aResult,(TText8*) __FILE__, __LINE__, EFalse)
#define GET_OPTIONAL_BOOL_PARAMETER(aParamName, aSection, aResult)		GetCommandBoolParameter(aParamName, aSection, aResult,(TText8*) __FILE__, __LINE__, EFalse)

#endif /* __T_MDADEVSOUND_DATA_H__ */
