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
* Description:  This is the definition of the Distance Attenuation effect
*                observer class.
*
*
*/


#ifndef DISTANCEATTENUATIONOBSERVER_H
#define DISTANCEATTENUATIONOBSERVER_H

// INCLUDES

#include <e32base.h>
#include <DistanceAttenuationBase.h>
#include <MAudioEffectObserver.h>


// CLASS DECLARATION

/**
*  This is the distance attenuation effect observer class. Object interested in receiving notification
*  when the CDistanceAttenuation object changes state should derive from this class and implement its
*  interface.
*
*  @since 3.0
*/

class MDistanceAttenuationObserver : public MAudioEffectObserver
	{
	public:
		static const TUint KDistanceAttenuationChanged = KSpecificEffectBase + 1;
	};

#endif	// of DISTANCEATTENUATIONOBSERVER_H

// End of File