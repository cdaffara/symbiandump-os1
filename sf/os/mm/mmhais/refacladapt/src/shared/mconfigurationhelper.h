/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Save as expressly licensed to you by Symbian Software Ltd, all rights reserved.
* 
*
*/



#ifndef MCONFIGURATIONHELPER_H
#define MCONFIGURATIONHELPER_H

#include <a3f/a3fbase.h>

/**
 This class allow interaction with the physical layer in order to get
 */
class MConfigurationHelper
	{
public:

	/**
	This returns a list of supported sample rates. The list need just include those of the “standard list” 
	that are supported. Includes rates supported by sample rate conversion. 
	GetSupportedConfigurationComplete() callback shows result.
	@param aSupportedRates
	@return an error code 
	*/
	virtual TInt GetSupportedModes(RArray<TUid>& aSupportedModes) = 0;
	virtual TInt GetSupportedSampleRates(RArray<TInt>& aSupportedRates) = 0;
	};

#endif // MCONFIGURATIONHELPER_H
