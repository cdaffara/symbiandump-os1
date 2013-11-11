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

#include <iostream>

#include "messagedefparser\constidentifier.h"

namespace Parser
{

void CConstIdentifier::Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aOutput) const
    {
    (void)aData;
    (void)aLength;
    (void)aOptions;
    aOutput << Name();
    }

} // namespace Parser

