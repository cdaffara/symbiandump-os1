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

#ifndef RMOCKHLIGHT_H
#define RMOCKHLIGHT_H

#include "rmockhwrmplugin.h"

/**
Specific MockSY API for Light service plugin
*/
class RMockLight : public RMockHwrmPlugin
	{
protected:
	IMPORT_C virtual const TDesC& ServerName();
	};

#endif
