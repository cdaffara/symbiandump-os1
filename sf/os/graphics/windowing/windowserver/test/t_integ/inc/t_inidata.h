// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#ifndef __T_INI_DATA_H__
#define __T_INI_DATA_H__

#include <e32base.h>

/**
 * Macro to read a configuration value from an ini file.
 * @see CTIniData::ReadIniL
 */

#define READ_STR(A, B, C)  {\
                           C = CTIniData::ReadStrL( TPtrC((const TText *)L ## #A) ,B);\
                           }

#define READ_INT(A, B, C)  {\
                           C = CTIniData::ReadIntL( TPtrC((const TText *)L ## #A) ,B);\
                           }

class CTIniData
	{
public:
	/**
	* Read ini value
	*
	* @param	aIniParam The name of the parameter to be read in
	* @param	aConfigFile The name of the configuration file (including path)
	* @return	HBufC* The value returned
	* @leave	System wide error code.
	*/
	IMPORT_C static HBufC* ReadStrL(const TPtrC& aIniParam, const TDesC& aConfigFile);

	/**
	* Read ini value
	*
	* @param	aIniParam The name of the parameter to be read in
	* @param	aConfigFile The name of the configuration file (including path)
	* @return	TInt The value returned
	* @leave	System wide error code.
	*/
	IMPORT_C static TInt   ReadIntL(const TPtrC& aIniParam, const TDesC& aConfigFile);
	};

#endif // __T_INI_DATA_H__
