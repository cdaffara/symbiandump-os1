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

This contains CT_SoundFormatsSupportedData 
*/

#ifndef T_SOUNDFORMATSSUPPORTEDDATA_H_
#define T_SOUNDFORMATSSUPPORTEDDATA_H_

//	User Includes
#include "t_mdadevsounddata.h"

/**
 * Class to wrap RMdaDevSound::TSoundFormatsSupported APIs.
 *
 */
class CT_SoundFormatsSupportedData : public CDataWrapperBase
	{
public:

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static CT_SoundFormatsSupportedData *	NewL();

	/**
	* Public destructor
	*/
	~CT_SoundFormatsSupportedData ();

	virtual TAny*	GetObject();
	virtual void	SetObjectL(TAny* aAny);
	virtual void	DisownObjectL();

	inline virtual 	TCleanupOperation CleanupOperation();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	
	/** constant for max descriptor length   */
	const static 	TInt KDesMaxLength = 128;
	
private:
	/**
	* Private constructor. First phase construction
	*/
	CT_SoundFormatsSupportedData ();
	
	/**
	* Second phase construction
	*/
	void	ConstructL();
	
	static void CleanupOperation( TAny* aAny );

	/**
	* Helpers
	*/
	void	DoCleanup();
	
	/**
	* Commands
	*/
	void	DoCmdNew();
	void	DoCmdDestructor();
	void	DoCmd_iMinRate_get(const TDesC& aSection);
	void	DoCmd_iMaxRate_get(const TDesC& aSection);
	void	DoCmd_iMinBufferSize_get(const TDesC& aSection);
	void	DoCmd_iMaxBufferSize_get(const TDesC& aSection);
	void	DoCmd_iMinVolume_get(const TDesC& aSection);
	void	DoCmd_iMaxVolume_get(const TDesC& aSection);
	void	DoCmd_iChannels_get(const TDesC& aSection);
	void	DoCmd_iEncodings_getL(const TDesC& aSection);

private:
	/** Instance for manipulating sound formats supported   */
	RMdaDevSound::TSoundFormatsSupported*	iFormatsSupported;
	};

#endif /*T_SOUNDFORMATSSUPPORTEDDATA_H_*/
