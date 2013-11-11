/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Configuration class of Orientation SSY
*
*/


// Includes
#include "OrientationConfiguration.h"

// ----------------------------------------------------------------------------------
// TOrientationConfiguration::TOrientationConfiguration()
// ----------------------------------------------------------------------------------
//  
TOrientationConfiguration::TOrientationConfiguration()
    {
    }

// ----------------------------------------------------------------------------------
// TOrientationConfiguration::GetTimerToOrientation()
// ----------------------------------------------------------------------------------
//  
TInt TOrientationConfiguration::GetTimerToOrientation( 
    TSensrvOrientationData::TSensrvDeviceOrientation aOrientation )
    {
    TInt retVal( 0 );
    if( aOrientation == TSensrvOrientationData::EOrientationDisplayUp )
        {
        retVal = iOrientationDisplayUp.iTimerValueInMilSeconds;
        }
    else if( aOrientation == TSensrvOrientationData::EOrientationDisplayDown )
        {
        retVal = iOrientationDisplayDown.iTimerValueInMilSeconds;
        }
    else if( aOrientation == TSensrvOrientationData::EOrientationDisplayLeftUp )
        {
        retVal = iOrientationDisplayLeftUp.iTimerValueInMilSeconds;
        }
    else if( aOrientation == TSensrvOrientationData::EOrientationDisplayRightUp )
        {
        retVal = iOrientationDisplayRightUp.iTimerValueInMilSeconds;
        }
    else if( aOrientation == TSensrvOrientationData::EOrientationDisplayUpwards )
        {
        retVal = iOrientationDisplayUpwards.iTimerValueInMilSeconds;
        }
    else if( aOrientation == TSensrvOrientationData::EOrientationDisplayDownwards )
        {
        retVal = iOrientationDisplayDownwards.iTimerValueInMilSeconds;
        }
    else
        {
        retVal = 1;
        }
    return retVal;
    }
    
// End of file
