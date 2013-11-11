// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
*/

#ifndef MMFMEDIACLIENTEXTDISPLAYINTERFACE_H_
#define MMFMEDIACLIENTEXTDISPLAYINTERFACE_H_

#include <e32base.h>

#define MMF_MEDIA_CLIENT_EXT_DISPLAY_INTERFACE_V3

/**

Enumeration used to indicate external display current connection status

**/
enum TExtDisplayConnectionProviderConnType
    {
    EExtDisplayConnectionProviderConnTypeDisconnected,
    EExtDisplayConnectionProviderConnTypeHdmi,
    EExtDisplayConnectionProviderConnTypeAnalog
    };

/**

Mixin class capable of handling events from provider

*/
class MExtDisplayConnectionProviderCallback
    {
public:
    virtual void MedcpcExtDisplayNotifyConnected(TExtDisplayConnectionProviderConnType aExtDisplayConnType) = 0;
    };

/**

Mixin class for external display connection provider plugins.

*/
class CExtDisplayConnectionProviderInterface : public CBase
	{
public:

    /**
    Allocates and constructs a new provider plugin.

    Uses ECOM to instantiate a new provider plugin. Can leave with any of the system-wide error codes.

    @leave  KErrNotSupported There is no support for external displays.
    @return A pointer to the new provider plugin.

    */
    static CExtDisplayConnectionProviderInterface* NewL();

    /**
    Function to be implemented by connection provider that allows the callback to be set.
    
    @param  aCallback
        A reference to an object that can be used by the provider to send events back to the client.
    */
    virtual void SetExtDisplayConnectionProviderCallback(MExtDisplayConnectionProviderCallback& aCallback) = 0;

    /**
    Function to be implemented by connection provider that indicates the external display connection type 
    
    @return One of values defined in TExtDisplayConnectionProviderConnType
    */
    virtual TExtDisplayConnectionProviderConnType ExtDisplayConnType() = 0;
	
    /**
    Function to be implemented by connection provider that indicates the display id that should
    be used for the external display port.
    
    @return TInt the display id for the external display port
    */
    virtual TInt ExtDisplayId() = 0;

    /**
    Default destructor.
    */
    IMPORT_C virtual ~CExtDisplayConnectionProviderInterface();

protected:
    /**
    Constructor.
    */
    IMPORT_C CExtDisplayConnectionProviderInterface();
    
private:
    /**
    Required by ECOM.
    */
    TUid iInstanceKey;
	};

#endif /* MMFMEDIACLIENTEXTDISPLAYINTERFACE_H_ */
