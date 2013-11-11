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



#ifndef MSTREAMPOSITIONCONTROL_H
#define MSTREAMPOSITIONCONTROL_H

#include <a3f/a3fbase.h>

/**
 This class allow interaction with the physical layer in order to get
 */
class MStreamPositionControl
	{
public:
	/**
	Get the position of the control based on samples played or recorded
	@return the position of the control
	*/
	virtual TInt GetControlPosition(TTimeIntervalMicroSeconds& aPosition) = 0;

	virtual void ResetControlPosition() = 0;
	};

#endif // MSTREAMPOSITIONCONTROL_H
