/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef SENSRVTESTTYPES_H
#define SENSRVTESTTYPES_H

#include<sensrvproperty.h>


typedef TUint TSensrvTestProcessID;
typedef TUint TSensrvTestProcessCommand;

const TSensrvTestProcessID KProcessIDFirst  = 1; //0b000001;
const TSensrvTestProcessID KProcessIDSecond = 2; //0b000010;
const TSensrvTestProcessID KProcessIDThrid  = 3; //0b000011;
const TSensrvTestProcessID KProcessIDFourth = 4; //0b000100;
    
const TSensrvTestProcessCommand KProcessCommandSetProperty  = 1;
const TSensrvTestProcessCommand KProcessCommandOpenChannel  = 2;
const TSensrvTestProcessCommand KProcessCommandCloseChannel = 3;
const TSensrvTestProcessCommand KProcessCommandFindChannel  = 4;

/**
* Data to be transferred to test process
*/
class TSensrvProcessData 
    {
public:
    TSensrvTestProcessID      iProcessID;//bitmask; to define destination process
    TSensrvTestProcessCommand iCommand;
    TInt                      iExpectedResult;
    TSensrvProperty           iProperty;
    };



#endif // SENSRVTESTTYPES_H
