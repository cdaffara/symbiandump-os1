/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the Orientation effect observer class.
*
*
*/


#ifndef MORIENTATIONOBSERVER_H
#define MORIENTATIONOBSERVER_H

// INCLUDES

#include <e32base.h>
#include <OrientationBase.h>
#include <MAudioEffectObserver.h>


// CLASS DECLARATION

/**
*  This is the location effect observer class. Object interested in receiving notification
*  when the COrientation object changes state should derive from this class and implement
*  its interface.
*
*  @since 3.0
*/

class MOrientationObserver : public MAudioEffectObserver
	{
	public:
		static const TUint KOrientationChanged = KSpecificEffectBase + 1;
		static const TUint KOrientationVectorsChanged = KSpecificEffectBase + 2;
	};

#endif	// of MORIENTATIONOBSERVER_H

// End of File