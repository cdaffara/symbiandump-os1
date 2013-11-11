/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MGAINHELPER_H
#define MGAINHELPER_H

#include <a3f/a3fbase.h>

/**
 */
class MGainHelper
	{
public:
	
	// Allows retrieve the gain applied to low level adaptation
	virtual TInt GetGain(TInt& aGain) const = 0;

	// Set the gain to the low level adaptation
	virtual TInt SetGain(RArray<TAudioChannelGain>& aChannels) = 0;

	//Select the ramp operation and the volump ramp duration. 
	virtual TInt ConfigureRamp(TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration) = 0;
	};

#endif // MGAINHELPER_H
