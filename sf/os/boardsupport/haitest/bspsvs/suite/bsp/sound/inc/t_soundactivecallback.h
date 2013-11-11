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

This contains CT_SoundActiveCallback
*/

#if (!defined __T_SOUNDACTIVECALLBACK_H__)
#define __T_SOUNDACTIVECALLBACK_H__

// User includes
#include "sound_util.h"
#include "ActiveCallbackBase.h"

/**
* Callback class for playing and recording processes
*/
class CT_SoundActiveCallback : public CActiveCallbackBase
	{
public:
	
	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static CT_SoundActiveCallback*	NewL(CDataWrapperBase& aCallback, TInt aPriority=EPriorityStandard);
	
	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static CT_SoundActiveCallback*	NewLC(CDataWrapperBase& aCallback, TInt aPriority=EPriorityStandard);

	/**
	* Public destructor
	*/
	~CT_SoundActiveCallback();
	
	/**
	* Setter method for section name
	*/
	void	SetSection(const TDesC& aSection);
	
	/**
	* Method for creating sound data buffer.
	*/
	void	CreateSoundDataBufferL(TInt aLength);
	
protected:
	
	/**
	* Protected constructor. First phase construction
	*/
	CT_SoundActiveCallback(CDataWrapperBase& aCallback, TInt aPriority);

public:
	/** Position in buffer being played or recorded to */ 
	TInt						iDataPosition;
	/** Buffer being played or recorded to */
	TSoundUtil::RAudioBuffer* 	iSoundData;
	/** Section name associated with command */
	TDesC*						iSection;
	/** Block in buffer */
	TPtr8 						iBufferBlock;
	};

#endif /* __T_SOUNDACTIVECALLBACK_H__ */
