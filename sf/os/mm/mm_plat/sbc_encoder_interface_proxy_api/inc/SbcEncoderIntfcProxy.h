/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface proxy for BT SBC Encoder configuration CI.
*
*/


#ifndef CSBCENCODERINTFCPROXY_H
#define CSBCENCODERINTFCPROXY_H

// INCLUDES
#include <SbcEncoderIntfc.h>
#include <mmfcontrollerframework.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCustomInterfaceUtility;
class MCustomCommand;

// CLASS DECLARATION

/**
*  Proxy for SBC Encoder configuration interface.
*  This proxy translates the interface API calls
*  to messages and sends them to the message handler.
*
*  @lib SbcEncoderIntfcProxy.lib
*  @since S60 3.0
*/
class CSbcEncoderIntfcProxy : public CSbcEncoderIntfc
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
		IMPORT_C static CSbcEncoderIntfcProxy* NewL(
                        TMMFMessageDestinationPckg aMessageHandler,
                        MCustomCommand& aCustomCommand,
                        CCustomInterfaceUtility* aCustomInterfaceUtility);

        /**
        * Destructor.
        */
		IMPORT_C virtual ~CSbcEncoderIntfcProxy();

    public: // New functions

    public: // Functions from CSbcEncoderIntfc

        /**
        * Retrieves the sampling frequencies supported by the encoder.
        * @since S60 3.0
        * @param aSupportedSamplingFrequencies array returning the supported
        *        frequencies.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedSamplingFrequencies(
		                      RArray<TUint>& aSupportedSamplingFrequencies);

        /**
        * Retrieves the channel configurations supported by the encoder.
        * @since S60 3.0
        * @param aSupportedChannelModes array returning the supported
        *        frequencies.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedChannelModes(
		                      RArray<TSbcChannelMode>& aSupportedChannelModes);

        /**
        * Retrieves array of supported block numbers by the encoder.
        * @since S60 3.0
        * @param aNumBlocks array returning block numbers.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedNumOfBlocks(
		                      RArray<TUint>& aSupportedNumOfBlocks);

        /**
        * Retrieves the number of subbands supported by the encoder.
        * @since S60 3.0
        * @param aSupportedNumOfSubbands array returning the supported number
        *        of subbands.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedNumOfSubbands(
		                      RArray<TUint>& aSupportedNumOfSubbands);

        /**
        * Retrieves the allocation methods supported by the encoder.
        * @since S60 3.0
        * @param aSupportedAllocationMethods array returning the supported
        *        allocation methods.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedAllocationMethods(
		                      RArray<TSbcAllocationMethod>&
		                      aSupportedAllocationMethods);

        /**
        * Retrieves the supported bitpool range by the encoder.
        * @since S60 3.0
        * @param aMinSupportedBitpoolSize returning the min bitpool size.
        * @param aMaxSupportedBitpoolSize returning the max bitpool size.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSupportedBitpoolRange(
		                      TUint& aMinSupportedBitpoolSize,
		                      TUint& aMaxSupportedBitpoolSize);

        /**
        * Sets the sampling frequency for encoding.
        * @since S60 3.0
        * @param aSamplingFrequency requested encoding sampling frequency.
        * @return void
        */
		IMPORT_C virtual void SetSamplingFrequency(TUint aSamplingFrequency);

        /**
        * Retrieves the current encode sampling frequency.
        * @since S60 3.0
        * @param aSamplingFrequency returns current encode sampling frequency.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetSamplingFrequency(TUint& aSamplingFrequency);

        /**
        * Sets the channel mode for encoding.
        * @since S60 3.0
        * @param aChannelMode requested encoding channel mode.
        * @return void
        */
		IMPORT_C virtual void SetChannelMode(TSbcChannelMode aChannelMode);

        /**
        * Retrieves the current encode channel mode.
        * @since S60 3.0
        * @param aChannelMode returns current encode channel mode.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetChannelMode(TSbcChannelMode& aChannelMode);

        /**
        * Sets the number of subbands for encoding.
        * @since S60 3.0
        * @param aNumofSubbands requested encoding number of subbands.
        * @return void
        */
		IMPORT_C virtual void SetNumOfSubbands(TUint aNumOfSubbands);

        /**
        * Retrieves the current encode number of subbands.
        * @since S60 3.0
        * @param aNumOfSubbands returns current encode number of subbands.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetNumOfSubbands(TUint& aNumOfSubbands);

        /**
        * Sets the number of blocks for encoding.
        * @since S60 3.0
        * @param aNumOfBlocks requested encoding number of blocks.
        * @return void
        */
		IMPORT_C virtual void SetNumOfBlocks(TUint aNumOfBlocks);

        /**
        * Retrieves the current encode number of blocks.
        * @since S60 3.0
        * @param aNumOfBlocks returns current encode number of blocks.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetNumOfBlocks(TUint& aNumOfBlocks);

        /**
        * Sets the allocation method for encoding.
        * @since S60 3.0
        * @param aAllocationMethod requested encoding allocation method.
        * @return void
        */
		IMPORT_C virtual void SetAllocationMethod(
		                      TSbcAllocationMethod aAllocationMethod);

        /**
        * Retrieves the current encode allocation method.
        * @since S60 3.0
        * @param aAllocationMethod returns current encode allocation method.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetAllocationMethod(
		                      TSbcAllocationMethod& aAllocationMethod);

        /**
        * Sets the bitpool size for encoding.
        * @since S60 3.0
        * @param aBitpoolSize requested encoding number of blocks.
        * @return void
        */
		IMPORT_C virtual void SetBitpoolSize(TUint aBitpoolSize);

        /**
        * Retrieves the current encode bitpool size.
        * @since S60 3.0
        * @param aBitpoolSize returns current encode bitpool size.
        * @return TInt status
        */
		IMPORT_C virtual TInt GetBitpoolSize(TUint& aBitpoolSize);

        /**
        * Applies configuration to the encoder.
        * @since S60 3.0
        * @return TInt - status
        */
		IMPORT_C virtual TInt ApplyConfig();


    protected:  // New functions
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
		CSbcEncoderIntfcProxy(TMMFMessageDestinationPckg aMessageHandler,
                              MCustomCommand& aCustomCommand,
                              CCustomInterfaceUtility* aCustomInterfaceUtility);

        /**
        * Symbian 2nd phase constructor
        */
		void ConstructL();

        /**
        * Called by GetSupportedSamplingFrequencies under trap harness.
        * Retrieves the sampling frequencies supported by the encoder.
        * @since S60 3.0
        * @param aSupportedSamplingFrequencies array returning the supported
        *        frequencies.
        * @return TInt status
        */
		void GetSupportedSamplingFrequenciesL(
		     RArray<TUint>& aSupportedSamplingFrequencies);

        /**
        * Called by GetSupportedChannelModes under trap harness.
        * Retrieves the channel configurations supported by the encoder.
        * @since S60 3.0
        * @param aSupportedChannelModes array returning the supported
        *        frequencies.
        * @return TInt status
        */
		void GetSupportedChannelModesL(
		     RArray<TSbcChannelMode>& aSupportedChannelModes);

        /**
        * Called by GetSupportedNumOfBlocks under trap harness.
        * Retrieves array of supported block numbers by the encoder.
        * @since S60 3.0
        * @param aNumBlocks array returning block numbers.
        * @return TInt status
        */
        void GetSupportedNumOfBlocksL(
             RArray<TUint>& aSupportedNumOfBlocks);

        /**
        * Called by GetSupportedNumOfSubbands under trap harness.
        * Retrieves the allocation methods supported by the encoder.
        * @since S60 3.0
        * @param aSupportedAllocationMethods array returning the supported
        *        allocation methods.
        * @return TInt status
        */
		void GetSupportedNumOfSubbandsL(
		     RArray<TUint>& aSupportedNumOfSubbands);

        /**
        * Called by GetSupportedAllocationMethods under trap harness.
        * Retrieves the allocation methods supported by the encoder.
        * @since S60 3.0
        * @param aSupportedAllocationMethods array returning the supported
        *        allocation methods.
        * @return TInt status
        */
		void GetSupportedAllocationMethodsL(
		     RArray<TSbcAllocationMethod>& aSupportedAllocationMethods);

        /**
        * Called by GetSupportedBitpoolRange under trap harness.
        * Retrieves the supported bitpool range by the encoder.
        * @since S60 3.0
        * @param aMinSupportedBitpoolSize returning the min bitpool size.
        * @param aMaxSupportedBitpoolSize returning the max bitpool size.
        * @return TInt status
        */
    	void GetSupportedBitpoolRangeL(TUint& aMinSupportedBitpoolSize,
		                               TUint& aMaxSupportedBitpoolSize);
		/*
        * Utility method that reads stream from 8-bit descriptor, converts it
        * to TUint data items and then copies them to the aArray.
        * @since S60 3.0
        * @param RArray<TUint>& - an array of TUint data elements
        * @param TPtr8 - pointer to 8-bit descriptor
        * @param TUint - number of the array elements
        * @return void
		*/
        void PopulateArrayL(RArray<TUint>& aArray, TPtr8 aPtr, TUint aCount);


    public:     // Data
    protected:  // Data
    private:    // Data

        /**
         * Structure containing SBC encoder configuration data
         */
    	class TSbcEncConf
    	    {
    	    public:

    	    // These contain config data
        	TUint                iSamplingFrequency;
        	TSbcChannelMode      iChannelMode;
        	TUint                iNumOfSubbands;
        	TUint                iNumOfBlocks;
        	TSbcAllocationMethod iAllocationMethod;
        	TUint                iBitpoolSize;

        	// These indicate whether corresponding data member has been set
        	TBool iSamplingFrequencySet;
        	TBool iChannelModeSet;
        	TBool iNumOfSubbandsSet;
        	TBool iNumOfBlocksSet;
        	TBool iAllocationMethodSet;
        	TBool iBitpoolSizeSet;
    	    };

        // Handle to object used for sending IPC commands
		MCustomCommand& iCustomCommand;

		// Message object destined for the message handler
		TMMFMessageDestinationPckg iMessageHandler;

		// Handle to the utility object used in the interface framework
		CCustomInterfaceUtility* iCustomInterfaceUtility;

    	// Working config - before ApplyConfig()
    	TSbcEncConf iSbcEncConf;

    	// Current config - after ApplyConfig()
    	TSbcEncConf iSbcEncConfCurrent;

    	// Flag indicating readiness of the data to be applied
    	TBool iHasBeenApplied;

	};

#endif // CSBCENCODERINTFCPROXY_H

// End of File
