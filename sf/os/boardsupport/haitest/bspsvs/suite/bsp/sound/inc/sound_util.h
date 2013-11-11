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


#if (!defined __TSOUNDUTIL_H__)
#define __TSOUNDUTIL_H__

//	EPOC Includes
#include <mdasound.h>

//	User Includes
#include "DataWrapperBase.h"


/**
 * Class that contains methods for loading and releasing sound drivers and 
 * default constants
 *
 */
class TSoundUtil
	{
public:
	//constant definitions
	const static TInt KAudioBufferMaxSize	=	2048;
	const static TInt KDefaultBlockSize		=	8192;

	/**
	 * Class that contains methods for saving and loading audio files
	 *
	 */
	class RAudioBuffer : public RBuf8
		{
	public:
		TInt SaveAudioDataL(const TDesC& aFile);
		TInt LoadAudioDataL(const TDesC& aFile);
		};
		
public:
	static TInt		LoadSoundDrivers(const TDesC& aLdd, const TDesC& aPdd);
	static TInt		FreeSoundDrivers(const TDesC& aLdd, const TDesC& aPdd);
	
	static TInt		DeleteFileL(const TDesC& aFile);
	
public:
	
	static const CDataWrapperBase::TEnumEntryTable iEnumTableSoundEncoding[];
	};

#endif /* __TSOUNDUTIL_H__ */
