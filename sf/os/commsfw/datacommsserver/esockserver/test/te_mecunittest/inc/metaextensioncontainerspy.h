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
// metaextensioncontainerspy.h
//

/**
@file
*/


#ifndef METAEXTENSIONCONTAINERSPY_H
#define METAEXTENSIONCONTAINERSPY_H

#include <comms-infras/ss_rmetaextensioncontainer.h>
#include "../../../inc/ss_cmetaextensioncontainer.h"

class TMetaExtensionContainerSpy
	{
public:
    static TUint32 ChainLength(const ESock::RMetaExtensionContainerC& aMec);
    
    static inline const ESock::CMetaExtensionContainerImplBase* Container(const ESock::RMetaExtensionContainerC& aMec)
        {
        return aMec.iContainerImpl;
        }
    
    static inline ESock::CMetaExtensionContainer* AppendableExtensionContainer(const ESock::RMetaExtensionContainer& aMec)
        {
        return aMec.AppendableExtensionContainer();
        }
    
    static inline ESock::CMetaExtensionContainerArray* AppendableContainerArray(const ESock::RMetaExtensionContainer& aMec)
        {
        return aMec.AppendableContainerArray();
        }

    static inline TInt ExtensionCount(const ESock::CMetaExtensionContainer& aMecImpl)
        {
        return aMecImpl.iExtensions.Count();
        }

    static inline TInt ContainerCount(const ESock::CMetaExtensionContainerArray& aMecImpl)
        {
        return aMecImpl.iContainers.Count();
        }
    };
  
#endif
// METAEXTENSIONCONTAINERSPY_H

