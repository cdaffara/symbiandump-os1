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
* Description: Class used by client applications to convert data 
* such as such as WlanInfo or CellIds into TLocality position information
*
*/

/**
@file
@publishedPartner
@prototype
*/

#ifndef LBS_LOCATIONRESOLVER_H
#define LBS_LOCATIONRESOLVER_H


#include <e32base.h>
#include <lbs/lbslocationcommon.h>
#include <lbs/lbslocationinfo.h>
#include <lbs/lbslocationinfoconvertercommon.h>

class CLbsLocationResolverImpl;


class MLbsLocationResolverObserver
    {
        
public:

    /** 
     * Gets the current version of the observer interface.
     * Derived classes should not change the implementation.
     *
     * @return Version number (currently 1.0.0) 
     */
    IMPORT_C virtual TVersion Version()  const;
    
    /**
     * The callback is used by the LBS subsystem to inform a client application 
     * that a simple CLbsLocationResolver conversion process initiated through the 
     * CLbsLocationResolver::ResolveLocationL method has been finished.
     * 
     * @param aStatus status of conversion, KErrNone for success, standard system error code
     *  in other cases. Note that in the event of a cancellation this method will not execute.
     * @param aBasicLocationInfo the location conversion that was requested. 
     * If the conversion has failed the values will be uninitialised 
     */
    virtual void OnResolveLocationComplete(TInt aStatus, const TLocality& aLocality) = 0;
    
    };


NONSHARABLE_CLASS(CLbsLocationResolver) : public CBase
    {

public:

    /**
     * Allocates and construct a new CLbsLocationResolver
     * @param[in] aObserver An observer for location conversion event.
     * @param[in] aConverterModuleId UID of the conversion plugin to be used.
     */
    IMPORT_C static CLbsLocationResolver* NewL(MLbsLocationResolverObserver& aObserver,
                                                const TUid aConverterModuleId);
    
    /** 
     * Allocates and construct a new CLbsLocationResolver
     * @param[in] aObserver An observer for location conversion event.
     */
    IMPORT_C static CLbsLocationResolver* NewL(MLbsLocationResolverObserver& aObserver);

    /** 
     * Destructor. 
     */
    IMPORT_C ~CLbsLocationResolver();
    
    /**
     * A simple function that converts a single Location Info into a TLocality
     * No preferences or masks may be set. 
     * The result is returned in the MLbsLocationResolverObserver 
     * callback via OnResolveLocationComplete
     * 
     * Only one outstanding conversion can be open at a time for each client.
     * Attempting to perform more than one will result in a leave with KErrInUse
     * 
     * @param[in] aLocationInfo the information to be converted into a location.
     */
    IMPORT_C void ResolveLocationL(const CLbsLocationInfo& aLocationInfo); 
   
    /**
     * Sets the option for the conversion module's connection preferences [ eg. "online without prompt"]
     * @param[in] aConversionPref - Enum specifying the connection preferences 
     * @return KErrArgument if the value is invalid, KErrNone otherwise
     */
    IMPORT_C TInt SetConversionPreferences(TLbsConversionPrefs aConversionPrefs);

    /**
     * Returns the option currently set for the conversion module's 
     * connection preference [ eg. "local cache"]
     * @return The enum corresponding to the currently set connection preferences (ELbsConversionNotDefined will be	
     * returned if the conversion preferences have not been set yet)
     */
    IMPORT_C TLbsConversionPrefs ConversionPreferences();
    
    /** 
     * Cancels an ongoing conversion operation.
     * If there is no ongoing conversion the cancellation request is ignored.
     *
     * The MLbsLocationResolverObserver callback will not be invoked 
     * if cancellation is successful.
     */
    IMPORT_C void CancelResolveLocation();

protected:
    /** 
     * Default constructor 
     */
    CLbsLocationResolver();
    
    /** 
     * Symbian 2nd phase constructor. 
     */
    void ConstructL(MLbsLocationResolverObserver& aObserver, const TUid aConverterModuleId);


private:
    /** 
     * Copy constructor. Restricted by default. 
     */
    CLbsLocationResolver(CLbsLocationResolver&);
    
    /** 
     * Overloaded assignment operator. Restricted by default. 
     */
    CLbsLocationResolver& operator=(CLbsLocationResolver&);
    
private:

    /** 
     * Pointer to implementation class 
     */
    CLbsLocationResolverImpl* iLocationResolverImpl;
    
    };


#endif // LBS_LOCATIONRESOLVER_H
