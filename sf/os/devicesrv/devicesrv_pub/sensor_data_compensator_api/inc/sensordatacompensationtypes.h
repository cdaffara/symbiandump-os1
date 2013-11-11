/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor data compensation types definitions.
*
*/


#ifndef SENSORDATACOMPENSATIONTYPES_H
#define SENSORDATACOMPENSATIONTYPES_H

/**
* Sensor data compensation types
*/  
enum TSensorCompensationType
    {
    ESensorCompensateDeviceOrientation = 0, // Use device orientation in compensation
    ESensorCompensateUIOrientation,         // Use UI orientation in compensation
    ESensorCompensateDeviceAndUIOrientation // Use both device and UI orientation in compensation
    };

#endif // SENSORDATACOMPENSATIONTYPES
