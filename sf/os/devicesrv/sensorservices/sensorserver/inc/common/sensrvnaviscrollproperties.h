/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Naviscroll property definitions.
*
*/



#ifndef SENSRVNAVISCROLLPROPERTIES_H
#define SENSRVNAVISCROLLPROPERTIES_H

//  INCLUDES
#include <sensrvtypes.h>
#include <e32base.h>

// CONSTANTS


/**
* - Name:         Naviscroll resolution
* - Type:         TInt 
* - Scope:        
* - Groups:       KSensrvNaviscrollGroup
* - Mandatory:    No
* - Capability:
* - Description:  Resolution of the naviscroll.
*/
const TSensrvPropertyId KSensrvNaviscrollResolution = 0x00020001;

/**
* The TSensrvPowerMode represents current power mode of the naviscroll
*
* Possible values:
* - ESensrvPowerModeNormal, 
* - ESensrvPowerModeLow, 
* - ESensrvPowerModeOff, 
*
*/
enum TSensrvPowerMode
{
  ESensrvPowerModeNormal = 0,
  ESensrvPowerModeLow,
  ESensrvPowerModeOff 
};

/**
* - Name:         Naviscroll Power mode.
* - Type:         TInt 
* - Scope:        Sensor specific property
* - Groups:       KSensrvNaviscrollGroup
* - Mandatory:    No
* - Capability:
* - Description:  Powermode for naviscroll control. Use TSensrvPowerMode-values.
*/
const TSensrvPropertyId KSensrvPropIdPowerMode = 0x00020002;

/**
* - Name:         Inactivity timer in milliseconds
* - Type:         TInt 
* - Scope:        Sensor specific property
* - Groups:       KSensrvNaviscrollGroup
* - Mandatory:    No
* - Capability:
* - Description:  Inactivity timer for naviscroll control. 
*/
const TSensrvPropertyId KSensrvPropIdInactivityTimer = 0x00020003;


#endif //SENSRVNAVISCROLLPROPERTIES_H

// End of File


