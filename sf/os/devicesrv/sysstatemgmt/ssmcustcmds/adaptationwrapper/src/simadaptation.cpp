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
* Description:
* Implementation of SSM Compatibility Layer SIM Adaptation plug-in factory.
*
*/

#include <ssm/startupadaptationadapter.h>

// ---------------------------------------------------------------------------
// CreateSimAdaptationL
// ---------------------------------------------------------------------------
//
EXPORT_C MSimAdaptation* CreateSimAdaptationL()
    {
	return CStartupAdaptationAdapter::NewSimAdaptationL();
    }
