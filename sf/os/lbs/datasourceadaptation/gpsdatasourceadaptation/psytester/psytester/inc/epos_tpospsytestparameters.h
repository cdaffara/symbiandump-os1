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



#ifndef TPOSPSYTESTPARAMETERS_H
#define TPOSPSYTESTPARAMETERS_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
/**
*  Class containing parameters for PSY Tester
*/
class TPosPSYTestParameters
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        TPosPSYTestParameters();

    public: // New functions
	
		/**
        * Setter for number of memory requests
        * @param aNrOfMemoryRequests value specifying 
        *        the value read from resource file
        */
		void SetNumberOfMemoryRequests(
		/* IN */ TInt aNrOfMemoryRequests
		);

		/**
        * Setter for number of location requests
        * @param aNrOfLocationRequests value specifying 
        *        the value read from resource file
        */
		void SetNumberOfLocationRequests(
		/* IN */ TInt aNrOfLocationRequests
		);

		/**
        * Setter for number of partial update requests
        * @param aNrOfPartialUpdateRequests value specifying 
        *        the value read from resource file
        */
		void SetNumberOfPartialUpdateRequests(
		/* IN */ TInt aNrOfPartialUpdateRequests
		);

		/**
        * Setter for number of stress test instances
        * @param aNrOfStressTestInstances value specifying 
        *        the value read from resource file
        */
		void SetNumberOfStressTestInstances(
		/* IN */ TInt aNrOfStressTestInstances
		);

		/**
        * Setter for number of multiple session test instances
        * @param aNrOfMultipleSessionTestInstances value specifying 
        *        the value read from resource file
        */
		void SetNumberOfMultipleSessionTestInstances(
		/* IN */ TInt aNrOfMultipleSessionTestInstances
		);

		/**
        * Setter for number of cancel multiple session test instances
        * @param aNrOfMultipleSessionCancelTestInstances value specifying 
        *        the value read from resource file
        */
		void SetNumberOfMultipleSessionCancelTestInstances(
		/* IN */ TInt aNrOfMultipleSessionCancelTestInstances
		);

		/**
        * Setter for heap size percentage level, i.e. the percentage value
		* that the heap size is allowed to increase with
        * @param aHeapSizePercentageLevel value specifying 
        *        the value read from resource file
        */
		void SetHeapSizePercentageLevel(
		/* IN */ TInt aHeapSizePercentageLevel
		);

		/**
        * Setter for maximum time to fix value
        * @param aTimeToFixThreshold value specifying 
        *        the value read from resource file
        */
		void SetTimeToFixThreshold(
		/* IN */ TInt aTimeToFixThreshold
		);

		/**
        * Setter for maximum part of location requests that are allowed
		* to complete with a position that is outside the horizontal 
		* accuracy interval. Value 3 means that 1/3	of the requests are
		* allowed to be outside the specified accuracy interval.
        * @param aHorizontalAccuracyThreshold value specifying 
        *        the value read from resource file
        */
		void SetHorizontalAccuracyThreshold(
		/* IN */ TInt aHorizontalAccuracyThreshold
		);

		/**
        * Setter for maximum part of location requests that are allowed
		* to complete with a position that is outside the vertical 
		* accuracy interval. Value 3 means that 1/3	of the requests are
		* allowed to be outside the specified accuracy interval.
        * @param aVerticalAccuracyThreshold value specifying 
        *        the value read from resource file
        */
		void SetVerticalAccuracyThreshold(
		/* IN */ TInt aVerticalAccuracyThreshold
		);

		/**
        * Setter for maximum max age test time
        * @param aMaxAgeTestTime value specifying 
        *        the value read from resource file
        */
		void SetMaxAgeMaxTestTime(
		/* IN */ TInt aMaxAgeMaxTestTime
		);

		/**
        * Setter for max horizontal accuracy value
        * @param aMaxHorizontalAccuracy value specifying 
        *        the value read from resource file
        */
		void SetMaxHorizontalAccuracy(
		/* IN */ TInt aMaxHorizontalAccuracy
		);

		/**
        * Setter for max vertical accuracy value
        * @param aMaxVerticalAccuracy value specifying 
        *        the value read from resource file
        */
		void SetMaxVerticalAccuracy(
		/* IN */ TInt aMaxVerticalAccuracy
		);

		/**
        * Setter for max resonable horizontal accuracy value
        * @param aResonableHorizontalAccuracy value specifying 
        *        the value read from resource file
        */
		void SetResonableHorizontalAccuracy(
		/* IN */ TInt aResonableHorizontalAccuracy
		);

		/**
        * Setter for max resonable vertical accuracy value
        * @param aResonableVertictalAccuracy value specifying 
        *        the value read from resource file
        */
		void SetResonableVertictalAccuracy(
		/* IN */ TInt aResonableVertictalAccuracy
		);

		/**
        * Setter for max time to first fix value
        * @param aMaxTimeToFirstFix value specifying 
        *        the value read from resource file
        */
		void SetMaxTimeToFirstFix(
		/* IN */ TInt aMaxTimeToFirstFix
		);

		/**
        * Getter for number of memory requests
        * @return number of memory requests
        */
		TInt NumberOfMemoryRequests() const;

		/**
        * Getter for number of location requests
        * @return number of location requests
        */
		TInt NumberOfLocationRequests() const;

		/**
        * Getter for number of partial update requests
        * @return number of partial update requests
        */
		TInt NumberOfPartialUpdateRequests() const;

		/**
        * Getter for number of stress test instances
        * @return number of stress test instances
        */
		TInt NumberOfStressTestInstances() const;

		/**
        * Getter for number of multiple session test instances
        * @return number of multiple session test instances
        */
		TInt NumberOfMultipleSessionTestInstances() const;

		/**
        * Getter for number of cancel multiple session test instances
        * @return number of cancel multiple session test instances
        */
		TInt NumberOfMultipleSessionCancelTestInstances() const;

		/**
        * Getter for max percentual increase of heap size allowed
        * @return maximal percentual increase of heap size allowed
        */
		TInt HeapSizePercentageLevel() const;

		/**
        * Getter for time to fix threshold
        * @return time to fix threshold
        */
		TInt TimeToFixThreshold() const;

		/**
        * Getter for horizontal accuracy threshold
		* Value 3 means that 1/3 of the requests are
		* allowed to be outside the specified accuracy interval.
        * @return horizontal accuracy threshold
        */
		TInt HorizontalAccuracyThreshold() const;

		/**
        * Getter for vertical accuracy threshold
		* Value 3 means that 1/3	of the requests are
		* allowed to be outside the specified accuracy interval.
        * @return horizontal accuracy threshold
        */
		TInt VerticalAccuracyThreshold() const;

		/**
        * Getter for max max age test time
        * @return max max age test time
        */
		TInt MaxAgeMaxTestTime() const;

		/**
        * Getter for max horizontal accuracy
        * @return max horizontal accuracy
        */
		TInt MaxHorizontalAccuracy() const;

		/**
        * Getter for max vertical accuracy
        * @return max vertical accuracy
        */
		TInt MaxVerticalAccuracy() const;

		/**
        * Getter for max resonable horizontal accuracy
        * @return max resonable horizontal accuracy
        */
		TInt ResonableHorizontalAccuracy() const;

		/**
        * Getter for max resonable vertical accuracy
        * @return max resonable vertical accuracy
        */
		TInt ResonableVertictalAccuracy() const;

		/**
        * Getter for max time to first fix
        * @return max time to first fix
        */
		TInt MaxTimeToFirstFix() const;
       
    private:    // Data
        
        TInt iNrOfMemoryRequests;
		TInt iNrOfLocationRequests;
		TInt iNrOfPartialUpdateRequests;
		TInt iNrOfStressTestInstances;
		TInt iNrOfMultipleSessionTestInstances;
		TInt iNrOfMultipleSessionCancelTestInstances;
		TInt iHeapSizePercentageLevel;
		TInt iTimeToFixThreshold;
		TInt iHorizontalAccuracyThreshold;
		TInt iVerticalAccuracyThreshold;
		TInt iMaxAgeMaxTestTime;
		TInt iMaxHorizontalAccuracy;
		TInt iMaxVerticalAccuracy;
		TInt iResonableHorizontalAccuracy;
		TInt iResonableVertictalAccuracy;
		TInt iMaxTimeToFirstFix;

};

#endif      // TPOSPSYTESTPARAMETERS_H

// End of File
