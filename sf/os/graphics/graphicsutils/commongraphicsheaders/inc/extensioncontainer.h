// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef EXTENSIONCONTAINER_H_
#define EXTENSIONCONTAINER_H_

class CExtensionContainer:public CBase
    {
public:
    /** Get Extension Interface.
     *  Implemented using the CBase::Extension_() mechanism 
     *  Note that the pointer returned is only good for the lifetime of the called CBase derived object.
     *  @param  aExtensionId    The GUID/ Well-known ID of the interface
     *  @return pointer to the interface or NULL if not available
     **/
        inline TAny* GetInterface(TUint aExtensionId);
    /** Get Extension Interface - templated helper.
     *  Resolves the ID and returned pointer based on the class name.
     *  Note that the pointer returned is only good for the lifetime of the called CBase derived object.
     *  Class name should support ETypeId intergral value, else use non-template version. 
     *  @param  MClass  The class of the interface with embedded GUID / Well known ID 
     *  @return pointer to the interface or NULL if not available
     **/
        template <class MClass> MClass* GetInterface()
        {
            return static_cast<MClass*>(GetInterface(MClass::ETypeId));
        }
    };

inline TAny* CExtensionContainer::GetInterface(TUint aExtensionId)
    {
    TAny*   retVal=NULL;
    //Note that extension is intentionally not overloaded in CExtensionContainer
    if (this->Extension_(aExtensionId,retVal,NULL)<KErrNone)
        {
        return NULL;
        }
    else
        {
        return retVal;
        }
    }

#endif /* EXTENSIONCONTAINER_H_ */
