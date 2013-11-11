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
class MLbsLocationInfoConverterObserver;


/**
 * Class implements the services exposed by CLbsLocationInfoConverter
 */
class CLbsLocationInfoConverterImpl : public CActive
    {
public:
    /**
     * Allocates and construct a new CLbsLocationInfoConverterImpl
     *
     * @param[in] aObserver Handle to the observer implementation
     * @param[in] aConversionModuleId ID of the conversion plugin to be used.
     * @return  Pointer to the implementation class of Location Info converter.
     */
    static CLbsLocationInfoConverterImpl* NewL( MLbsLocationInfoConverterObserver& aObserver,
                      		                    const TUid aConversionModuleId  );
    
    /**
     * Destructor
     */
    ~CLbsLocationInfoConverterImpl();
    
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
    void ConvertLocationInfoL( CLbsLocationInfoBase& aLocationInfo,
                               TLbsConversionPrefs aConversionPrefs, 
                               const TLbsConversionOutputInfoMask aRequestedInfo );

    /**
     * Converts the area informations of a group of geographical locations
     * to its corresponding position information. 
     * @param[in][out] aLocationInfoArray Array of location informations  
     * corresponding to geographical locations are passed and on successful  
     * completion of this method,position information corresponding to the 
     * location informations are returned in the observer callback.
     * @param[in] aConversionPrefs Conversion preferences suggested by client
     *
     * @leave KErrArgument If the location info array is empty or one of 
     * system wide error codes.
     */                                                       
    void ConvertLocationInfoL( RLbsLocationInfoBaseArray& aLocationInfoArray,
            			       TLbsConversionPrefs aConversionPrefs,
            			       const TLbsConversionOutputInfoMask aRequestedInfo );
    
    /**
     * Cancels get location info operation.
     */                                                                                    
    void CancelConvertLocationInfo();
    
protected: // From CActive
    void RunL();
    
    void DoCancel();
    
private:
    /**
     * Default constructor
     */
    CLbsLocationInfoConverterImpl( MLbsLocationInfoConverterObserver& aObserver );
    
    /**
     * Copy constructor. Restricted by default.
     */
    CLbsLocationInfoConverterImpl( CLbsLocationInfoConverterImpl& );
    
    /**
     * Overloaded assignment operator. Restricted by default.
     */
    CLbsLocationInfoConverterImpl& operator=( CLbsLocationInfoConverterImpl& );
    
    /**
     * Symbian 2 phase constructor.
     */
    void ConstructL( const TUid aConversionModuleId );
  
private: // data
    
    /**
     * Reference to the observer.
     */
    MLbsLocationInfoConverterObserver& iObserver;

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
    };

#endif // LBSLOCATIONINFOCONVERTERIMPL_H_
