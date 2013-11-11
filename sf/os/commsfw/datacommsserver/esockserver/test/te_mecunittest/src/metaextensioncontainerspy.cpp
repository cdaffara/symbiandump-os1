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
// metaextensioncontainerspy.cpp
//

/**
@file
*/

#include "metaextensioncontainerspy.h"

TUint32 TMetaExtensionContainerSpy::ChainLength(const ESock::RMetaExtensionContainerC& aMec)
    {
    ASSERT(aMec.Container());
    ASSERT(aMec.Container()->Type() == ESock::CMetaExtensionContainerImplBase::EMetaExtensionContainer);
    const ESock::CMetaExtensionContainer* mecImpl = static_cast<const ESock::CMetaExtensionContainer*>(aMec.Container());
    TUint32 length = 0;
    while (mecImpl)
        {
        ++length;
        if (mecImpl->iBaseContainer
        		&& mecImpl->iBaseContainer->Type() == ESock::CMetaExtensionContainerImplBase::EMetaExtensionContainer)
        	{
            mecImpl = static_cast<const ESock::CMetaExtensionContainer*>(mecImpl->iBaseContainer);        	
        	}
        else
        	{
        	mecImpl = NULL;
        	}
        }
    
    return length;
    }

