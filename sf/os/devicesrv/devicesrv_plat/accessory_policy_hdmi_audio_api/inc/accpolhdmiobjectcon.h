/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Object container for hdmi policy objects.
 *
 */

#ifndef ACCPOLHDMIAUDIOCON_H
#define ACCPOLHDMIAUDIOCON_H

#include <accpolobjectcon.h>
#include <accpolhdmiaudioformat.h>
#include <accpolhdmilatency.h>
#include <accpolhdmisink.h>
#include <accpolhdmispeakerallocation.h>
#include <accpolhdmivideoformat.h>

/**
 *  Stores HDMI related policy objects.
 *
 *  @accpolaudiodevicetopology.lib
 */
NONSHARABLE_CLASS( CAccPolHdmiObjectCon ): public CAccPolObjectCon
    {

public:

    /**
     * Destructor.
     */
    virtual ~CAccPolHdmiObjectCon();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiObjectCon* NewL( CAccPolObjectCon* aObject = NULL );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CAccPolHdmiObjectCon* NewLC( CAccPolObjectCon* aObject = NULL );
    
public: // New methods
    
    /**
     * Get format objects.
     * If objects cannot be found, the array is left empty.
     * 
     * Note: Ownership of the objects are not transfered!
     * 
     * @param aArray Format object array.
     * @return None. 
     */
    IMPORT_C void GetAudioFormatObjectsL(
        RAccPolHdmiAudioFormatArray& aArray ) const;
    
    /**
     * Get latency objects.
     * If objects cannot be found, the array is left empty.
     * 
     * Note: Ownership of the objects are not transfered!
     * 
     * @param aArray Latency object array. 
     * @return None. 
     */
    IMPORT_C void GetLatencyObjectsL(
        RAccPolHdmiLatencyArray& aArray ) const;

    /**
     * Get HDMI sink objects.
     * In normal conditions the array will contain only one object.
     * If objects cannot be found, the array is left empty.
     * 
     * Note: Ownership of the objects are not transfered!
     * 
     * @param aArray Sink object array.
     * @return None. 
     */
    IMPORT_C void GetHdmiSinkObjectsL(
        RAccPolHdmiSinkArray& aArray ) const;

    /**
     * Get speaker allocation objects.
     * In normal conditions the array will contain only one object.
     * If objects cannot be found, the array is left empty.
     * 
     * Note: Ownership of the objects are not transfered!
     * 
     * @param aArray Speaker allocation object array.
     * @return None. 
     */
    IMPORT_C void GetHdmiSpeakerAllocationObjectsL(
        RAccPolHdmiSpeakerAllocationArray& aArray ) const;
    
    /**
     * Get video objects.
     * In normal conditions the array will contain only one object.
     * If objects cannot be found, the array is left empty.
     *
     * Note: Ownership of the objects are not transfered!
     * 
     * @param aArray Video format object array.
     * @return None.
     */
    IMPORT_C void GetHdmiVideoFormatObjectsL(
        RAccPolHdmiVideoFormatArray& aArray ) const;

public: // From base classes

    // @see CAccPolObjectBase
    virtual CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL );

    // @see CAccPolObjectBase
    virtual void InternalizeL( RReadStream& aStream );

    // @see CAccPolObjectBase
    virtual void ExternalizeL( RWriteStream& aStream ) const;

private:

    /**
     * C++ default constructor.
     */
    CAccPolHdmiObjectCon( CAccPolObjectCon* aObject = NULL );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    };

#endif // ACCPOLHDMIAUDIOCON_H
