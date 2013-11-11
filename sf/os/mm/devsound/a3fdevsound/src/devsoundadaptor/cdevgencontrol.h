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



#ifndef CDEVGENCONTROL_H
#define CDEVGENCONTROL_H

#include <e32base.h>

#include "cdevaudiocontrol.h"
#include <a3f/a3f_trace_utils.h>

/**
 * CDevGenControl.
 *
 * ?description
 *
 * @lib mmfdevsoundadaptation.lib
 * @since
 */

NONSHARABLE_CLASS(CDevGenControl) : public CDevAudioControl
	{
public:

	/**
	* Destructor.
	* Releases all resources and deletes all objects owned by this instance.
	*/
	~CDevGenControl();

	static CDevGenControl* NewL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

	void ConstructL(CDevAudio* aDevAudio, MDevSoundAdaptationObserver& aDevSoundObserver);

private:
	CDevGenControl();
	};

#endif // CDEVGENCONTROL_H
