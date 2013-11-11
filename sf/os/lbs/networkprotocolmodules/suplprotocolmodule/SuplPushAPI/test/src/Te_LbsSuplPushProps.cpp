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
// A unit test wrapper over the LbsSuplPushProps
// 
//

#include <e32property.h>

#include "Te_LbsSuplErrorPattern.h"
#include "Te_LbsSuplPushRProperty.h"
	
/** Replaces the RProperty class with its test wrapper - RTe_LbsSuplPushRProperty. */
#define RProperty RTe_LbsSuplPushRProperty

#include "lbssuplpushprops.cpp"
