/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_TESTPSYBASE_H__
#define __CT_LBS_TESTPSYBASE_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KActiveRequest = 0;
const TInt KBufSize = 40;

// FORWARD DECLARATIONS
class TPositionInfo;
class CPositioner;
class TPositionSatelliteInfo;
class TPositionCourseInfo;
class HPositionGenericInfo;
class MPositionerStatus;

// CLASS DECLARATION

class CT_LbsTestPsyBase : public CActive
    {
    public: // Singleton functions
        
        /**
        * @return a pointer to the singleton object
        */
		IMPORT_C static CT_LbsTestPsyBase* InstanceL(MPositionerStatus* aPositionerStatus);

        /**
        * Releases the instance
        */
		IMPORT_C void Release();
      
    public: // New functions
        
        /**
        * Requests position info asynchronously.
        * @param aStatus a request status to update during the request
        * @param aPosition will point to the requested position when
        *                  the request is done.
        */
        IMPORT_C virtual void NotifyPositionUpdate(
            /* IN/OUT */	TPositionInfo* aPosInfo,
			/* OUT	  */	TRequestStatus* aStatus
            );
  
        /**
        * Cancels a position info request.
        */
        IMPORT_C virtual void CancelNotifyPositionUpdate(
			/* IN/OUT */	TRequestStatus* aStatus);
	   /**
		* Sets the inteval in microseconds between requests
		*/
		IMPORT_C void SetTimerDelay(
			/* IN */		const TTimeIntervalMicroSeconds& aTimerDelay);
	
		enum TRequestsHandling
			{
			EBundling = 100,
			EQueuing
			};

		/**
		* Sets which type of request handling the PSY should use
		*/
		IMPORT_C void SetRequestsHandling(
			/* IN */		const TRequestsHandling& aRequestsHandling);
		
		/**
		* Sets which type classes the PSY support
		*/
		IMPORT_C void SetSupportedClasses(
			/* IN */		const TUint32& aSupportedClasses);

		/**
		* Sets which type capabilities PSY support
		*/
		IMPORT_C void SetCapabilities(
			/* IN */		const TUint32& aCapabilities);
		
        IMPORT_C void SetUpdateType(
            /* IN */        const TUint32& aUpdateType);
		
        /**
		* Sets max age, 0 if not supported
		*/
		IMPORT_C void SetMaxAge(
			/* IN */		const TTime& aMaxAge);
        
        /**
		* Sets module. uid
		*/
		IMPORT_C void SetModuleId(
            /* IN */        const TUid& aModuleId);
					

		/**
		* Returns number of ongoing requests 
		*/
		IMPORT_C TInt RequestsStatusCount() const;

		/**
		* Returns index of aStatus
		*/
		IMPORT_C TInt FindStatus(TRequestStatus* aStatus) const;
		
		/**
		* Reports PSY status
		*/
		IMPORT_C void ReportStatus(
			/* IN */		const TInt aDeviceStatus, 
			/* IN */		const TInt aDataQuality);

        /**
		* Writes msg to c:\\Trace.txt
		*/
        IMPORT_C static void Trace(TBuf8 <KBufSize> msg); 

	protected:								   
		
		IMPORT_C virtual void SetTPositionInfo(
			/* IN/OUT */	TPositionInfo& aInfo, 
            /* IN */        const TBool& aUseCache=EFalse);
		
		IMPORT_C void SetPositionInfoL(TPositionInfo* aPosInfo, const TBool& aUseCache=EFalse);

		/**
		* Sets TPositionCourseInfo unique variables
		*/
		IMPORT_C virtual void SetTPositionCourseInfo(
			/* IN/OUT */		TPositionCourseInfo& aCourseInfo);
           
        /**
		* Clear TPositionSatelliteInfo unique variables
		*/
		IMPORT_C void ClearTPositionSatelliteInfo(
			/* IN/OUT */				TPositionSatelliteInfo& aSatelliteInfo);

		/**
		* Sets TPositionSatelliteInfo unique variables
		*/
		IMPORT_C virtual void SetTPositionSatelliteInfo(
			/* IN/OUT */				TPositionSatelliteInfo& aSatelliteInfo, 
            /* IN */                    const TBool& aCache=EFalse);

		/**
		* Sets address capabilities unique variables
		*/
		IMPORT_C virtual void SetAddressGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo);
	
		/**
		* Sets speed capabilities unique variables
		*/	
		IMPORT_C virtual void SetSpeedGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo);
		
		/**
		* Sets direction capabilities unique variables
		*/
		IMPORT_C virtual void SetDirectionGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo);

		/**
		* Sets compass capabilities unique variables
		*/
		IMPORT_C virtual void SetCompassGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo);
		
		/**
		* Sets building capabilities unique variables
		*/
		IMPORT_C virtual void SetBuildingGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo);
		
		/**
		* Sets NMEA capabilities unique variables
		*/
		IMPORT_C virtual void SetNMEAGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo);
		      
        /**
		* Sets satellite capabilities unique variables
		*/
		IMPORT_C virtual void SetSatelliteGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo, 
            /* IN */                    const TBool& aCache=EFalse);
		
		/**
		* Sets media capabilities unique variables
		*/
		IMPORT_C virtual void SetMediaGenericInfoL(
			/* IN/OUT */				HPositionGenericInfo& aGenericInfo);

		/**
		* Sets aPosInfo with cached position if max age is supported and 
		* cached position is not to old. 	
		*/
		IMPORT_C virtual TBool CheckMaxAge(
			/* IN/OUT */		TPositionInfo* aPosInfo);
        
       
	protected:// Constructors and destructor
        
       /**
       * C++ default constructor.
       */
       IMPORT_C CT_LbsTestPsyBase(MPositionerStatus* aPositionerStatus);

		/**
        * Destructor.
        */
        IMPORT_C virtual ~CT_LbsTestPsyBase();

		/**
        * By default EPOC constructor is private.
        */
        IMPORT_C void ConstructL();

		/**
        * Completes a request at index with the specified error code
		* @param aIndex the index.
        * @param aErrorCode the error code - KErrNone if everything went OK.
        */
		IMPORT_C void CompleteRequest(
				/* IN  */   TInt aIndex,
				/* IN  */   TInt aErrorCode
			);
	private: // New functions

		void StartRequest();
		
		/**
        * Completes all requests with the specified error code
        * @param aErrorCode the error code - KErrNone if everything went OK.
        */
        void CompleteRequests(
            /* IN  */   TInt aErrorCode
            );

	 protected: // Functions from base classes

        /**
        * From CActive. Handles an active object’s request completion
        * event.
        */
        IMPORT_C void RunL();

        /**
        * From CActive. Implements cancellation of an outstanding
        * request
        */
        IMPORT_C void DoCancel();

		/**
        * From CActive. Handles a leave occurring in the request
        * completion event handler RunL()
        * @param aError The leave code
        */
        IMPORT_C TInt RunError(
            /* IN  */   TInt aError
            );

	private:

        // By default, prohibit copy constructor
        CT_LbsTestPsyBase( const CT_LbsTestPsyBase& );
        // Prohibit assigment operator
        CT_LbsTestPsyBase& operator= ( const CT_LbsTestPsyBase& );

	protected:   // Data
		TTime						iMaxAge;
	    TTime                       iPosTime;
        
	private:    // Data
		
		TInt						iReferenceCounter;
        TUint						iRequests;

		RArray<TPositionInfo*>		iRequestsPosInfo;
		RArray<TRequestStatus*>		iRequestsStatus;
    
		RTimer						iTimer;
		
		MPositionerStatus*			iPositionerStatus;

		TTimeIntervalMicroSeconds	iTimerDelay;
		TUint32						iSupportedClasses;
		TUint32						iCapabilities;
		TUint32						iUpdateType;

        TRequestsHandling			iRequestsHandling;

		TBool						iActiveRequestCompleted;
        TUid                        iModuleId;
        TTime                       iSatTime;
        TTime                       iSatGenericTime;
        TTime                       iLastSave;
	};

#endif      // __CT_LBS_TESTPSYBASE_H__
            
// End of File
