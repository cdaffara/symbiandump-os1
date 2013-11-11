// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Surface Manager API
//

#ifndef __SURFACE_HINTS_H__
#define __SURFACE_HINTS_H__

//- Include Files  ----------------------------------------------------------

#include <e32cmn.h>


//- Namespace ---------------------------------------------------------------

namespace surfaceHints
{

//- Constants ---------------------------------------------------------------

/** Hint of the surface content.
    @see TSurfaceContent for possible values
*/
const TInt KSurfaceContent = 0x1;

/** Hint of the expected update rate of the surface content.
    Value for a surface containing e.g. 25 fps video the value should be 25.
    For a static UI element the value should be 0.
    @see TSurfaceUpdate
*/
const TInt KSurfaceUpdate = 0x2;

/** Hint whether the surface content is copy protected and can it be
    shown on external displays.
    @see TSurfaceProtection for possible values.
*/
const TInt KSurfaceProtection = 0x3;

/** Hint about the surface’s characteristics or properties,
   For example if a surface can be persisted by the effects engine.
   @see TSurfaceCharacteristics for possible values.
*/
const TInt KSurfaceCharacteristics = 0x4;


/** Values used for the KSurfaceContent key */
enum TSurfaceContent
    {
    /** No specific use-case */
    EGeneric,
    /** Camera viewfinder frames */
    EViewFinder,
    /** Images captured by camera */
    EStillImage,
    /** Decoded video frames */
    EVideoPlayback,
    /** Video frames from video telephony */
    EVideoTelephony,
    /** EGL surface */
    EGfx,
    /** Main UI surface */
    EUi,
    /** Composition target surface */
    ECompositionTarget,
    /** Indicates that the surface has to accessible by ARM.
        This can be orr'ed with other TSurfaceContent enumerations. */
    EArmAccess = 0x80000000
    };


/** Values used for the KSurfaceProtection key. The values are bitmasks and can be combined
* e.g. EAllowAnalogProtectionRequired | EAllowDigitalProtectionRequired.
*/
enum TSurfaceProtection
    {
    /**
    * Not allowed on external outputs
    */
    EAllowInternalOnly                  = 0x00000000,

    /**
    * Allowed on all external outputs
    */
    EAllowAllExternals                  = 0xFFFFFFFF,

    /**
    * Allow passing content over analog outputs,
    * e.g. composite and S-video
    */
    EAllowAnalog                        = 0x00000010,

    /**
    * Allow output over an analog output channel which has a protection
    * mechanism
    */
    EAllowAnalogProtectionRequired      = 0x00000020,

    /**
    * Allow passing content over digital outputs,
    * e.g. DVI and HDMI
    */
    EAllowDigital                       = 0x00000200,

    /**
    * Licensed product must attempt to engage HDCP to protect the content.
    * However it should be passed through to HDMI even if HDCP is not engaged or fails to engage.
    */
    EAllowDigitalProtectionRequested    = 0x00000400,

    /**
    * Licensed product is required to engage HDCP to protect the content.
    * If HDCP is not engaged or can not be engaged the content must not be passed through to HDMI.
    */
    EAllowDigitalProtectionRequired     = 0x00000800,
    };


/** Values used for the KSurfaceCharacteristics key. The values are bitmasks and can be combined.
*/
enum TSurfaceCharacteristics
    {
    /**
    * Surface cannot be persisted once it has been closed by the creator
    */
    ENotPersistable = 1,
    };


class TSurfaceUpdate
    {
    /** Constructor.
        @param aUpdateRate   How often the surface content is redrawn per second.
        @param aTearingFree  When ETrue surface updates should be synchronized
                             with display refresh rate, otherwise surface can
                             be updated as fast as possible.
    */
    inline TSurfaceUpdate(TUint aUpdateRate, TBool aTearingFree);

    /** Converts a value to TSurfaceUpdate */
    inline TSurfaceUpdate(TInt aValue);

    /** Converts TSurfaceUpdate to a signed integer, so it can be used as
        a value for KSurfaceUpdate key. */
    inline operator TInt() const;

    /** Getter for surface update rate.
        @return updates per second
    */
    inline TUint UpdateRate() const;

    /** Getter for surface update synchronization.
        @return ETrue - updates should be synchronized with display refresh rate,
                EFalse - surface can be updated as fast as possible.
    */
    inline TBool TearingFree() const;

    private:
        TUint iValue;
    };


//- Forward Declarations ----------------------------------------------------


//- Class Definitions -------------------------------------------------------


//- Inline Functions --------------------------------------------------------

TSurfaceUpdate::TSurfaceUpdate(TUint aUpdateRate, TBool aTearingFree)
    : iValue( ( aUpdateRate & 0xFFFF ) | ( aTearingFree ? 0x80000000 : 0x0 ) )
    {
    }
TSurfaceUpdate::TSurfaceUpdate(TInt aValue)
    : iValue( static_cast<TUint>( aValue ) )
    {
    }

TSurfaceUpdate::operator TInt() const
    {
    return static_cast<TInt>( iValue );
    }

TUint TSurfaceUpdate::UpdateRate() const
    {
    return ( iValue & 0xFFFF );
    }

TBool TSurfaceUpdate::TearingFree() const
    {
    return ( iValue & 0x80000000 ) ? ETrue : EFalse;
    }

} //namespace surfaceHints

#endif //__SURFACE_HINTS_H__

// End of File

