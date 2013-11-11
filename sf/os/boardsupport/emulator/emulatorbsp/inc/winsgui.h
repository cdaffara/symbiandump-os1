// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\inc\winsgui.h
// 
// WARNING: This file contains some APIs which are internal and are subject
//          to change without notice. Such APIs should therefore not be used
//          outside the Kernel and Hardware Services package.
//

#ifndef __WINSGUI_H__
#define __WINSGUI_H__

#include <e32def.h>

class WinsGui
	{
public:
	/**
	Returns the current emulator GUI configuration
	@publishedPartner
	@deprecated Emulator Deprecated
	*/
	IMPORT_C static TInt CurrentConfiguration();
	};


#endif //__WINSGUI_H__
