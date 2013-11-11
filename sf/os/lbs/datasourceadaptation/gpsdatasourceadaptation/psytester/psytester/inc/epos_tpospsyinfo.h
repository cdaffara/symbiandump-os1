/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef TPOSPSYINFO_H
#define TPOSPSYINFO_H

//  INCLUDES
#include <e32base.h>
#include <lbscommon.h>
#include "epos_tpospsybaseinfo.h"

// CLASS DECLARATION
/**
*  Class containing information about a PSY
*/
class TPosPSYInfo
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TPosPSYInfo();

    public: // New functions

        /**
        * Setter for Interface version value
        * @param aIntfVersion value specifying 
        *        the PSY's Interface version value
        */
        void SetInterfaceVersion(
        /* IN */ const TInt aIntfVersion
        );

		/**
        * Setter for priority value
        * @param aPriority value specifying 
        *        the PSY's module priority value
        */
        void SetPriority(
        /* IN */ const TInt aPriority
        );
        
        /**
        * Setter for availability value
        * @param aStatus value specifying 
        *        the PSY's availability status value
        */
        void SetStatus(
        /* IN */ const TInt aStatus
        );
        
        /**
        * Setter for Time To First Fix value
        * @param aTime value specifying 
        *        the PSY's TimeToFirstFix value
        */
        void SetTimeToFirstFix(
        /* IN */ const TTimeIntervalMicroSeconds aTime
        );
    
        /**
        * Setter for Time To Fix value
        * @param aTime value specifying 
        *        the PSY's TimeToFix value
        */
        void SetTimeToFix(
        /* IN */ const TTimeIntervalMicroSeconds aTime
        );
        
        /**
        * Setter for Horizontal Accuracy value
        * @param aAccuracy value specifying 
        *        the PSY's Horizontal accuracy value
        */
        void SetHorizontalAccuracy(
        /* IN */ const TReal32 aAccuracy
        );
    
        /**
        * Setter for Vertical Accuracy value
        * @param aAccuracy value specifying 
        *        the PSY's Vertical accuracy value
        */
        void SetVerticalAccuracy(
        /* IN */ const TReal32 aAccuracy
        );
        
        /**
        * Setter for Cost Indicator value
        * @param aCostIndicator value specifying 
        *        the PSY's Cost properties
        */
        void SetCostIndicator(
        /* IN */ const TPositionQuality::TCostIndicator aCostIndicator
        );
        
        /**
        * Setter for Power Consumption value
        * @param aPowerConsumption value specifying 
        *        the PSY's Power consumption properties
        */
        void SetPowerConsumption(
        /* IN */ const TPositionQuality::TPowerConsumption aPowerConsumption
        );
    
        /**
        * Setter for Technology Type value
        * @param aTechnologyType value specifying 
        *        the PSY's Technology properties
        */
        void SetTechnologyType(
        /* IN */ const TPositionModuleInfo::TTechnologyType aTechnologyType
        );

        /**
        * Setter for Device Location value
        * @param aDeviceLocation, value specifying 
        *        the PSY's Device location properties
        */
        void SetDeviceLocation(
        /* IN */ const TPositionModuleInfo::TDeviceLocation aDeviceLocation
        );
        
        /**
        * Setter for Capability value
        * @param aCapability, value specifying 
        *        the PSY's capability properties
        */
        void SetCapabilities(
        /* IN */ const TPositionModuleInfo::TCapabilities aCapability
        );

        /**
        * Setter for Classes Supported value
        * @param aClassesSupported value specifying 
        *        the PSY's supported classes
        */
        void SetClassesSupported(
        /* IN */ const TUint32 aClassesSupported
        );
    
        /**
        * Setter for Quality Timeout value
        * @param aQualityTimeout value specifying 
        *        the PSY's quality timeout value
        */
        void SetQualityTimeout(
        /* IN */ const TUint32 aQualityTimeout
        );
    
        /**
        * Setter for Visibility value
        * @param aVisibility value specifying 
        *        the PSY's visibility value
        */
        void SetVisibility(
        /* IN */ const TInt aVisibility
        );

        /**
        * Setter for PSY Base Info value
        * @param aPSYBaseInfo value specifying 
        *        the PSY's basic properties as version name and uid
        */
        void SetPSYBaseInfo(
        /* IN */ const TPosPSYBaseInfo aPSYBaseInfo
        );
        
		/**
        * Setter for PSY Values Set Boolean value
        * @param Indicates if the Boolean should be set or
        * reset
        */
		void SetValuesSet(
		/* IN */ const TBool aSet);
		
        /**
        * Getter for PSY Base Info value
        * @return value specifying 
        *         the PSY's basic properties as version name and uid
        */
        TPosPSYBaseInfo PSYBaseInfo() const;
        
		/**
        * Getter for Interface version value
        * @return value specifying 
        *         the PSY's interface version value
        */
        TInt InterfaceVersion() const;
    
    	/**
        * Getter for priority value
        * @return value specifying 
        *         the PSY's priority value
        */
        TInt Priority() const;
    

		/**
        * Getter for availability status value
        * @return value specifying 
        *         the PSY's status value
        */
        TInt Status() const;
        
        /**
        * Getter for Time To First Fix value
        * @return value specifying 
        *         the PSY's TimeToFirstFix value
        */
        TTimeIntervalMicroSeconds TimeToFirstFix() const;
    
        /**
        * Getter for Time To Fix value
        * @return value specifying 
        *         the PSY's TimeToFix value
        */
        TTimeIntervalMicroSeconds TimeToFix() const;
    
        /**
        * Getter for Horizontal Accuracy value
        * @return value specifying 
        *         the PSY's Horizontal accuracy value
        */
        TReal32 HorizontalAccuracy() const;
    
        /**
        * Getter for Vertical Accuracy value
        * @return value specifying 
        *         the PSY's Vertical accuracy value
        */
        TReal32 VerticalAccuracy() const;
    
        /**
        * Getter for Cost Indicator value
        * @return value specifying 
        *         the PSY's Cost properties
        */
        TPositionQuality::TCostIndicator CostIndicator() const;
    
        /**
        * Getter for Power Consumption value
        * @return value specifying 
        *         the PSY's Power consumption properties
        */
        TPositionQuality::TPowerConsumption PowerConsumption() const;
    
        /**
        *Getter for Technology Type value
        * @return value specifying 
        *         the PSY's Technology properties
        */
        TPositionModuleInfo::TTechnologyType TechnologyType() const;
    
        /**
        * Getter for Device Location value
        * @return value specifying 
        *         the PSY's Device location properties
        */
        TPositionModuleInfo::TDeviceLocation DeviceLocation() const;
    
        /**
        * Getter for Capabilities value
        * @return value specifying 
        *         the PSY's capability properties
        */
        TPositionModuleInfo::TCapabilities Capabilities() const;

        /**
        * Getter for Classes supported value
        * @return value specifying 
        *         the PSY's supported classes
        */
        TUint32 ClassesSupported() const;
    
        /**
        * Getter for QualityTimeout value
        * @return value specifying 
        *         the PSY's quality timeout value
        */
        TUint32 QualityTimeout() const;
    
        /**
        * Getter for Visibility value
        * @return value specifying 
        *         the PSY's visibility value
        */
        TInt Visibility() const;

         // By default, prohibit copy constructor
        TPosPSYInfo( const TPosPSYInfo& );
        
        /**
        * Getter for iValuesSet
        * @return value specifying 
        *         if the values were initialized after
        * 		  reading the central repository
        */
        TBool ValuesSet() const;
       
    private:    // Data
        
        // This Boolean provides information on whether the
        // values are set or not
        
        TBool iValuesSet;
        
        TInt iInterfaceVersion;
        TInt iPriority;
        TInt iStatus;
        TTimeIntervalMicroSeconds iFirstTimeToFix;
        TTimeIntervalMicroSeconds iTimeToFix;
        TReal32 iHorizontalAccuracy;
        TReal32 iVerticalAccuracy;
        TPositionQuality::TCostIndicator iCostIndicator;
        TPositionQuality::TPowerConsumption iPowerConsumption;
        TPositionModuleInfo::TTechnologyType iTechnologyType;
        TPositionModuleInfo::TDeviceLocation iDeviceLocation;
        TPositionModuleInfo::TCapabilities iCapability;
        TUint32 iClassesSupported;
        TInt iVisibility;
        TPosPSYBaseInfo iPSYBaseInfo;
};

#endif      // TPOSPSYINFO_H

// End of File
