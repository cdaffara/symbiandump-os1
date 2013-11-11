//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file contains the entry point for the CSY module.
//

// DllMain.cpp

/** @file DllMain.cpp
 *
 */

#include <cs_port.h>
#include "Portfactory.h"
#include "CsyDebugLogger.h"



EXPORT_C CSerial* LibEntry()
/**
 * The single exported function.
 * @return Pointer to and ownership of a new CSerial-derived object
 */
	{
	LOGHEADER();
	return (CPortFactory::NewL());
	}
