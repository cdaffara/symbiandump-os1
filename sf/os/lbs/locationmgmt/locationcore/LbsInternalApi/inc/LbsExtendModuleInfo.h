// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology 
*/

#ifndef __LBSEXTENDMODULEINFO_H__
#define __LBSEXTENDMODULEINFO_H__

#include <e32std.h>
#include <e32math.h>
#include <lbsrequestor.h>
#include <lbsclasstypes.h>
#include <lbserrors.h>
#include <lbscommon.h>


/** 
@internalTechnology
@released

Maximum characters in manager's executable name
*/
const TInt KPositionMaxExecutableName = 32;


class TPositionModuleInfoExtended
/**
TPositionModuleInfoExtended has a TPositionModuleInfo and
also has some internal information (used for implementation)
@internalTechnology
@released
 */
	{

public:
	/**
	
	 */

/** Defined type for TDeviceGpsModeCapabilities */                                                     
typedef TUint32 TDeviceGpsModeCapabilities;

/**
The enumeration specifies positioning capabilities of an AGPS Integration Module
and is intended to be used as a bitmask.

Currently only the following combinations are supported:
  - EDeviceGpsModeTerminalBased
  - EDeviceGpsModeTerminalAssisted
  - EDeviceGpsModeTerminalBased | EDeviceGpsModeTerminalAssisted
  - EDeviceGpsModeSimultaneousTATB

Specifying a bitmask with more than one mode enabled means that all the modes are supported, but not simultaneously. 
For example “EDeviceGpsModeTerminalBased | EDeviceGpsModeTerminalAssisted” means that a module can support both, 
Terminal Assisted and Terminal Based positioning, but only one at a time.

@internalTechnology
Note: Although the enumeration is classified as Internal Technology it has
corresponding values in a module configuration file.
It means any changes should be applied in a binary compatible way and documentation
of the configuration file must be updated accordingly.

Also, the enumeration may be extended in the future by adding more values.
To maintain compatibility any unrecognized bit-fields must be ignored.
*/
enum _TDeviceGpsModeCapabilities                                                                       
{                                                                                                  
/** No capabilities. Also used to initialise variables. */                                                                             
EDeviceGpsModeNone = 0x0,         
                                                                   
/** Device supports Terminal Based positioning and is also able to
calculate position autonomously if assistance data is unavailable,
or does not arrive on time (e.g. a fix is obtained before assistance data
delivery). */                                                        
EDeviceGpsModeTerminalBased = 0x0001,                                                                  

/** Device supports Terminal Assisted positioning.
The module may, but does not have to be able to calculate position autonomously
if Terminal Assisted positioning does not succeed (e.g. network is unavailable) */                                                     
EDeviceGpsModeTerminalAssisted = 0x0002,                                          
                   
/** Device can perform Terminal Assisted and Terminal Based positioning at the same time. 
It is also able to calculate position autonomously if assistance data is unavailable (e.g. network error), 
or does not arrive on time (e.g. a fix is obtained before assistance data delivery). 
NOTE: If you set this bit then it is not necessary to set either of the previous 2 bits as they are implied */                                                          
EDeviceGpsModeSimultaneousTATB = 0x0004,                                                   
                    
/** Device supports autonomous mode.
This value is currently not supported and has no effect on the behaviour of the LBS subsystem. */                                                             
EDeviceGpsModeAutonomous = 0x0008,                                                                    
};


public:
	IMPORT_C TPositionModuleInfoExtended();
	
	TUint8 iPositionModuleInfoBytes[sizeof(TPositionModuleInfo)];

	/** The moduleID */
	TUid	iModuleId;

	/** The plugin's ID */
	TUid	iDataSourcePluginId;

	/** The manager's executable name */
	TBuf<KPositionMaxExecutableName> iExecutableName;
	
	//** The device's (plugin's) mode capabilities */
	TDeviceGpsModeCapabilities iDeviceGpsModeCapabilities;
	
private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[16];
	};

#endif //__LBSEXTENDMODULEINFO_H__
