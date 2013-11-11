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

This contains CT_CurrentSoundFormatData
*/

#ifndef T_CURRENTSOUNDFORMATDATA_H_
#define T_CURRENTSOUNDFORMATDATA_H_

//	User Includes
#include "t_mdadevsounddata.h"

/**
 * Class to wrap RMdaDevSound::TCurrentSoundFormat
 *
 */
class CT_CurrentSoundFormatData : public CDataWrapperBase
	{
private:
	/** constant for max descriptor length   */
	const static 	TInt KDesMaxLength = 128;

public:

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static CT_CurrentSoundFormatData*	NewL();

	/**
	* Public destructor
	*/
	~CT_CurrentSoundFormatData();

	virtual TAny*	GetObject();
	virtual void	SetObjectL(TAny* aAny);
	virtual void	DisownObjectL();
	inline virtual 	TCleanupOperation CleanupOperation();
	
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	
private:
	/**
	* Private constructor. First phase construction
	*/
	CT_CurrentSoundFormatData();
	/**
	* Second phase construction
	*/
	void	ConstructL();
	
	static void CleanupOperation( TAny* aAny );
	
	/**
	* Helpers
	*/
	void	DoCleanup();
	void	DoInitiateSupportedFormatsL(const TDesC& aSection);
	
	/**
	* Commands
	*/
	void	DoCmdNewL(const TDesC& aSection);
	void	DoCmdDestructor();
	void	DoCmd_iRate_get(const TDesC& aSection);
	void	DoCmd_iEncoding_getL(const TDesC& aSection);
	void	DoCmd_iChannels_get(const TDesC& aSection);
	void	DoCmd_iBufferSize_get(const TDesC& aSection);
	void	DoCmd_iRate_set(const TDesC& aSection);
	void	DoCmd_iEncoding_set(const TDesC& aSection);
	void	DoCmd_iChannels_set(const TDesC& aSection);
	void	DoCmd_iBufferSize_set(const TDesC& aSection);

private:
	/** Instance for manipulating current sound format   */
	RMdaDevSound::TCurrentSoundFormat*	iCurrentFormat;	
	};

#endif /*T_CURRENTSOUNDFORMATDATA_H_*/
