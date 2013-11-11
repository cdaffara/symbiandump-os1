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



#ifndef TONEHWDEVICE_INL
#define TONEHWDEVICE_INL

#include "tonehwdevice.h"

inline TUint TToneCustomInterface::Volume() 
	{
	return iVolume;
	};
		
inline	void TToneCustomInterface::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration) 
	{
	iRampDuration = aRampDuration;
	};
	
inline TTimeIntervalMicroSeconds& TToneCustomInterface::VolumeRamp() 
	{
	return iRampDuration;
	};
	
#endif	

// End of File
