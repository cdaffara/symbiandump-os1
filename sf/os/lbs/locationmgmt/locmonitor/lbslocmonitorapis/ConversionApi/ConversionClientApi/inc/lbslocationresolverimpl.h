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
* Description: Class that implements location information converter 
* information
*
*/


#ifndef LBSLOCATIONINFOCONVERTERIMPL_H_
#define LBSLOCATIONINFOCONVERTERIMPL_H_


#include <e32base.h>
#include <lbs/lbslocationinfo.h>
#include "rlbslocmonitorsession.h"
#include "rlbslocinfoconverter.h"
#include <lbs/lbslocationinfoconvertercommon.h>


// forward declaration
class MLbsLocationResolverObserver;


/**
 * Class implements the services exposed by CLbsLocationInfoConverter
 */
class CLbsLocationResolverImpl : public CActive
    {
public:
    /**
     * Allocates and construct a new CLbsLocationResolverImpl
     *
     * @param[in] aObserver Handle to the observer implementation
     * @param[in] aConversionModuleId ID of the conversion plugin to be used.
     * @return  Pointer to the implementation class of Location Info converter.
     */
    static CLbsLocationResolverImpl* NewL( MLbsLocationResolverObserver& aObserver,
                      		                    const TUid aConversionModuleId  );
    
    /**
     * Destructor
     */
    ~CLbsLocationResolverImpl();
    
    /**
     * Converts one area info to position info
     * @param[in] aLocationInfo location information corresponding to 
     * a geographical location is passed and on successful completion of this 
     * method,position information corresponding to the location information 
     * is returned in the observer callback.
     * @param[in] aConversionPrefs Conversion preferences suggested by client
     * @param[in] aRequestedInfo Location information that is requested. 
     *
     * @leave KErrArgument If the location info array is empty or one of system
     * wide error codes.     
     */
    void ResolveLocationL(const CLbsLocationInfoBase& aLocationInfo);

    /**
     * Cancels get location info operation.
     */                                                                                    
    void CancelResolveLocation();
    
    /**
     * Sets the option for the conversion module's connection preferences [ eg. "online without prompt"]
     * @param[in] aConversionPrefs - Enum specifying the connection preference
     * @return KErrArgument if the value is invalid, KErrNone otherwise
     */
    TInt SetConversionPreferences(TLbsConversionPrefs aConversionPrefs);

    /**
     * Returns the option currently set for the conversion module's 
     * connection preference [ eg. "local cache"]
     * @return The enum corresponding to the currently set connection preferences (ELbsConversionNotDefined will be	
     * returned if the conversion preferences have not been set yet)
     */
    TLbsConversionPrefs ConversionPreferences();
    
protected: // From CActive
    void RunL();
    
    void DoCancel();
    
private:
    /**
     * Default constructor
     */
    CLbsLocationResolverImpl( MLbsLocationResolverObserver& aObserver );
    
    /**
     * Copy constructor. Restricted by default.
     */
    CLbsLocationResolverImpl( CLbsLocationResolverImpl& );
    
    /**
     * Overloaded assignment operator. Restricted by default.
     */
    CLbsLocationResolverImpl& operator=( CLbsLocationResolverImpl& );
    
    /**
     * Symbian 2 phase constructor.
     */
    void ConstructL( const TUid aConversionModuleId );
  
private: // data
    
    /**
     * Copy of locationinfo passed in by client
     */
    CLbsLocationInfo* iLocationInfo;
    
    /**
     * Reference to the observer.
     */
    MLbsLocationResolverObserver& iObserver;

    /**
     * Handle to session
     */
    RLbsLocMonitorSession iLocMoniterSession;
    
    /** 
     * Handle to subsession
     */
    RLbsLocInfoConverter iLocInfoConverter;   
    
    /**
     * Client specified conversion module id.
     */
    TUid iConversionModuleId;
    
    /**
     * Conversion module's specific connection preferences
     */
    TLbsConversionPrefs iConversionPrefs;
    };

#endif // LBSLOCATIONINFOCONVERTERIMPL_H_
