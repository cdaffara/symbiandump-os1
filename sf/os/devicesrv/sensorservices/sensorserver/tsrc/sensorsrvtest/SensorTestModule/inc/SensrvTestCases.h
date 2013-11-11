/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



#ifndef SENSRVTESTCASES_H
#define SENSRVTESTCASES_H

//  INCLUDES
#include <e32property.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "sensrvtypes.h"
#include <sensrvdatalistener.h>
#include <sensrvchannelinfo.h>
#include <sensrvpropertylistener.h>
#include <sensrvchannelconditionlistener.h>
#include <sensrvconditionevaluator.h>
#include <sensrvchannellistener.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CSensrvChannelFinder;
class CSensrvChannel;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CSensrvTestCases test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CSensrvTestCases) : public CActive,
                                      public MSensrvDataListener,
                                      public MSensrvPropertyListener,
                                      public MSensrvChannelConditionListener,
                                      public MSensrvChannelListener
    {
    public: // enum

        enum TSensrvTestCase
            {
            ECreateAndDestroyChannelFinder,
            EFindChannels,
            EOpenChannel,
            ECloseChannel,
            EOpenClosePerformance,
            EStartDataListening,
            EStopDataListening,
            EStartListeningWhenChannelIsNotOpen,
            EPanicClient,
            EGetProperty,
            ESetProperty,
            EStartPropertyListening,
            EStopPropertyListening,
            EGetAllProperties,
            EWaitSSYUnloading,
            ECreateNewProcess,
            EKillProcess,
            EAddCondition,
            EAddConditionSet,
            ERemoveConditions,
            EStartConditionListening,
            EStopConditionListening,
            EStartChannelChangeListening,
            EStopChannelChangeListening,
            EGetAllPropertiesWithPropertyIdL,
            EGetAllPropertiesWithArrayIndexL
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSensrvTestCases* NewL();

        /**
        * Destructor.
        */
        virtual ~CSensrvTestCases();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        TInt StartTestCase( TSensrvTestCase aTestCase, CStifItemParser& aItem, TInt aCompletedKey );

        void StopTest();

        inline TBool IsTestCaseCompleted() { return iTestCaseCompleted; }

        inline void SetGetDataDelay( TInt aDelay ) { iGetDataDelay = aDelay; }

        inline RMutex& Mutex() { return iMutex; };

        TInt PropertyChangedCount();
        void ClearPropertyChangedCount();

        TInt ConditionMetCount();

        TInt FoundChannelCount();

        TInt GetSetPropertySuccessIndicatorCount();
        TInt GetSetPropertySuccessIndicator();
        void ClearSetPropertySuccessIndicator();

        TInt AdjustChannels(CStifItemParser& aItem);
        TInt StoreFoundChannelsL(CStifItemParser& aItem);
	TInt SetDefaultValueL(CStifItemParser& aItem);

    public: // Functions from base classes

        void DoCancel();

        void RunL();

        TInt RunError( TInt aError );

    public: //From MSensrvDataListener

        /**
        * Indicates data is available in the receiving buffer. A client can read the
        * data through ReadData()-function in a related channel object. Data is valid until
        * the data received notification will occurs again.
        *
        * @since S60 5.0
        * @param[in] aChannel Reference to the related channel object
        * @param[in] aCount Data object count in receiving buffer.
        * @param[in] aDataLost Data object lost count.
        */
        void DataReceived( CSensrvChannel& aChannel,
                                   TInt aCount,
                                   TInt aDataLost);

        /**
        * Data listening failed.
        * If error was fatal, channel has also been closed and sensor server session terminated.
        * If error was minor, some data has potentially been lost.
        *
        * @param[in] aChannel Reference to the related channel object
        * @param[in] aError Error code.
        * @since S60 5.0
        */
        void DataError( CSensrvChannel& aChannel,
                                TSensrvErrorSeverity aError);

    	/**
    	* Returns a pointer to a specified interface - to allow future extension
    	* of this class without breaking binary compatibility
        *
        * @since S60 5.0
    	* @param aInterfaceUid Identifier of the interface to be retrieved
    	* @param aInterface A reference to a pointer that retrieves the specified interface.
    	*/
    	void GetDataListenerInterfaceL( TUid aInterfaceUid,
    	                                        TAny*& aInterface );

    public: //From MSensrvPropertyListener

        /**
        * Notification about the changed value of a property.
        * If this was used to set the property value, no notification is received.
        *
        * @since S60 5.0
        * @param[in] aChannel Channel object of related channel.
        * @param[in] aChangedProperty Changed property.
        */
        void PropertyChanged( CSensrvChannel& aChannel, const TSensrvProperty& aChangedProperty );

        /**
        * Property listening failed.
        * If error was fatal, channel has also been closed and sensor server session terminated.
        * If error was minor, some property changes have potentially been missed.
        *
        * @since S60 5.0
        * @param[in] aHandler Pointer to the related property handler
        * @param[in] aError Error code.
        */
        void PropertyError( CSensrvChannel& aChannel, TSensrvErrorSeverity aError );

        /**
        * Set a listener for the indication, if the setting of the property
        * succeeded.
        *
        * @since S60 5.0
        * @param[in] aIndication holds the indicator if the property setting was
        *            successful.
        * @param[in] aError Error code.
        */
        void SetPropertySuccessIndicationChanged( TSetPropertySuccessIndicator aIndication );

    	/**
    	* Returns a pointer to a specified interface extension - to allow future extension
    	* of this class without breaking binary compatibility
        *
        * @since S60 5.0
    	* @param aInterfaceUid Identifier of the interface to be retrieved
    	* @param aInterface A reference to a pointer that retrieves the specified interface.
    	*/
    	void GetPropertyListenerInterfaceL( TUid aInterfaceUid,
    	                                            TAny*& aInterface );

    public: //From MSensrvChannelConditionListener

        /**
        * Indicates that a channel condition set is met. If a client wants to use same
        * condition set after notification, the client has to add condition set again
        * to channel object. Otherwise the condition set is not used anymore and the
        * client can destroy it.
        *
        * If the set contains more than one condition, it is not known which of the
        * individual conditions in the set were met.
        *
        * @since S60 5.0
        * @param[in] aChannel Related channel object.
        * @param[in] aChannelConditionSet Channel condition set that is met.
        * @param[in] aValue Channel data value which met the condition
        */
        virtual void ConditionMet( CSensrvChannel& aChannel,
                                   CSensrvChannelConditionSet& aChannelConditionSet,
                                   TDesC8& aValue );

        /**
        * Condition listening failed.
        * If error was fatal, channel has also been closed and sensor server session
        * terminated. If error was minor, some condition matches have potentially been
        * missed or delayed.
        *
        * @since S60 5.0
        * @param[in] aChannel Pointer to the related channel object
        * @param[in] aError Error code.
        */
        virtual void ConditionError ( CSensrvChannel& aChannel,
                                      TSensrvErrorSeverity aError );

    	/**
    	* Returns a pointer to a specified interface extension - to allow future extension
    	* of this class without breaking binary compatibility
        *
        * @since S60 5.0
    	* @param aInterfaceUid Identifier of the interface to be retrieved
    	* @param aInterface A reference to a pointer that retrieves the specified interface.
    	*/
    	void GetChannelConditionListenerInterfaceL( TUid aInterfaceUid,
    	                                                    TAny*& aInterface );

    public: // From MSensrvChannelListener

        /**
        * Indicates channel availability change caused by installation or removal
        * of new software.
        *
        * @since S60 5.0
        * @param[in] aDetectedChannel Channel information of the channel.
        * @param[in] aChangeType If ESensrvChannelAdded, channel is a newly available
        *                        channel.
        *                        If ESensrvChannelRemoved, channel was removed and is
        *                        no longer available.
        */
        virtual void ChannelChangeDetected( const TSensrvChannelInfo& aDetectedChannel,
                                            TSensrvChannelChangeType aChangeType );

        /**
        * Channel listening failed.
        * If error was fatal, sensor server session has also been terminated, and
        * related finder is no longer usable.
        * If error was minor, some notifications have potentially been lost.
        *
        * @since S60 5.0
        * @param[in] aChannelFinder Reference to the related channelfinder object
        * @param[in] aError Error code.
        */
        virtual void ChannelDetectionError( CSensrvChannelFinder& aChannelFinder,
                                            TSensrvErrorSeverity aError );

    	/**
    	* Returns a pointer to a specified interface extension - to allow future extension
    	* of this class without breaking binary compatibility
        *
        * @since S60 5.0
    	* @param aInterfaceUid Identifier of the interface to be retrieved
    	* @param aInterface A reference to a pointer that retrieves the specified interface.
    	*/
    	void GetChannelListenerInterfaceL( TUid aInterfaceUid,
    	                                           TAny*& aInterface );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CSensrvTestCases( /*CTestModuleIf& aTestModuleIf*/ );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Parse parameters.
        */

        TInt ParseParameters( CStifItemParser& aItem, TSensrvTestCase aTestCase );

        /**
        * parse channel info
        */
        TInt ParseChannelInfo( TSensrvChannelInfo& aChannelInfo, CStifItemParser& aItem );

        /**
        * Set test result
        */
        void SetResult( TInt aResult );

        /**
        * Create and destroy channel finder.
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CreateAndDestroyChannelFinderL();

        /**
        * Find channels.
        * Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt),
        *              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt FindChannelsL();

        /**
        * Find channels.
        * Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt),
        *              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt FindChannels( RSensrvChannelInfoList& aChannelInfoList, TSensrvChannelInfo& aSearchConditions );

        /**
        * Find channels.
        * Parameters: ChannelId(TUint32), ContextType(TInt), Quantity(TInt),
        *              ChannelType(TUint32), dataSize(TInt), Location(TDesC8), VendorId(TDesC8)(optional)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt CheckFindResult( TSensrvChannelInfo& aSearchConditions );

        /**
        * Open the channel
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt OpenChannelL();

        /**
        * Close the channel
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CloseChannelL();

        /**
        * Open close performance test.
        * Parameters: Open-Close Count(TInt)
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt OpenClosePerformanceL();

        /**
        * Start channel data listening
        * Parameters: dataCount, timeout, repeatCount
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */
        virtual TInt StartDataListeningL();

        /**
        * Stop channel data listening
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */
        virtual TInt StopDataListeningL();

        /**
        * Start channel data listening when channel is not open
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */
        virtual TInt StartListeningWhenChannelIsNotOpenL();


        /**
        * Panic client
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt PanicClient();

        virtual TInt WaitSSYUnloadingL();
        /**
        * Start/Stop channel property listening
        * Parameters:
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */
        virtual TInt StartPropertyListeningL();
        virtual TInt StopPropertyListeningL();

        virtual TInt GetPropertyL();
        virtual TInt SetPropertyL();
        virtual TInt GetAllPropertiesWithPropertyIdL();
        virtual TInt GetAllPropertiesWithArrayIndexL();
        virtual TInt GetAllPropertiesL();


		virtual TInt CreateNewProcessL();
		virtual TInt KillProcess();

        virtual TInt AddConditionL();
        virtual TInt AddConditionSetL();
        virtual TInt RemoveConditionsL();

        /**
        * Start channel condition listening
        * Parameters: dataCount, timeout, repeatCount
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */
        virtual TInt StartConditionListeningL();

        /**
        * Stop channel condition listening
        *
        * @since S60 5.0
        * @param aItem Script line containing parameters.
        * @return void Description
        */
        virtual TInt StopConditionListeningL();

        virtual TInt StartChannelChangeListeningL();
        virtual TInt StopChannelChangeListeningL();

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

        TSensrvTestCase iTestCase;

        TBool iTestCaseCompleted;

        TBool iIsPanicked;

        // Not owned
        //CStifItemParser* iItemParser;

        // owned
        CSensrvChannelFinder* iChannelFinder;

        // owned
        CSensrvChannel*       iSensorChannel;

        // owned
        RSensrvChannelInfoList     iChannelInfoList;
        RSensrvChannelInfoList     iStoredInfoList;

        TInt iCompletedKey;

        /**
        * Thread handle for owning thread
        */
        RThread iThread;

        RMutex iMutex;

        TInt iGetDataDelay;

        TInt iPreviousAxis_xData;
        TInt iPreviousAxis_yData;
        TInt iPreviousAxis_zData;
        TTime iPreviousTimeStamp;

        /**
        * Parameters
        */

        TBool iChannelOpen;

        TSensrvChannelInfo iChannelInfo;

        TInt iOpenCloseCount;

        TInt iDataListeningCount;
        TInt iDataListeningTimeout;
        TInt iDataListeningRepeatCount;
        TInt iDataListeningRepeatCounter;

        TInt iSetPropertyIdTInt;
        TInt iGetPropertyIdTInt;
        TInt iGetItemIndex;

        TSensrvProperty iPropDataRate;
        TSensrvProperty iPropMeasureRange;
        TSensrvProperty iPropSensorDescription;
		TSensrvProperty iPropAvailability;
        TSensrvProperty iPropChannelScale;

        TInt iPropertyChangedCount;

        TInt iProcessCount;

        // Condition holders
        RSensrvChannelConditionList iConditionList;
        RSensrvChannelConditionSetList iConditionSetList;

        TInt iConditionType;
        TInt iConditionOperator;
        TInt iConditionIndex;
        TInt iConditionValue;
        TInt iConditionChannelType;

        TInt iConditionSetType;
        TInt iConditionSetAddTwice;
        TInt iConditionSetReadd;

        TInt iConditionListeningCount;
        TInt iConditionListeningTimeout;
        TInt iConditionListeningRepeatCount;
        TInt iConditionListeningRepeatCounter;
        RArray<TInt> iConditionListeningExpectedValuesArray;

        TInt iConditionRemoveNonexistent;

        CSensrvConditionEvaluator* iConditionEvaluator;

        TInt iChannelChangeListeningRepeatCount;
        TInt iChannelChangeListeningRepeatCounter;
        TInt iExpectedChannelChangeType;
        TBuf8<KSensrvVendorIdLength> iExpectedVendor;
        TSetPropertySuccessIndicator iExpectedPropertyIndicationType;
        TInt iExpectedUnloads;

        TSetPropertySuccessIndicator iSetPropertySuccessIndicator;
        TInt iSetPropertySuccessIndicatorCount;
    };

#endif      // SENSRVTESTCASES_H

// End of File
