// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// testextensions.cpp
//

/**
@file
*/

#include "testextensions.h"
#include <elements/metadata.h>


START_ATTRIBUTE_TABLE(TTestExtension1, TTestExtension1::EUid, TTestExtension1::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()


START_ATTRIBUTE_TABLE(TTestExtension2, TTestExtension2::EUid, TTestExtension2::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()


START_ATTRIBUTE_TABLE(TTestExtension3, TTestExtension3::EUid, TTestExtension3::ETypeId)
// No attributes even though there are data members. The structure is not serialised
END_ATTRIBUTE_TABLE()

