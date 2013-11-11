// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for qos conversion class
// 
//

/**
 @file
 @internalComponent
*/

#ifndef R99QOSCONVERSION_H
#define R99QOSCONVERSION_H

class CR99QoSConversion : public CBase
	{
	
	public:
		enum TConvRecordType
			{
			EValidRecord,
			EDeprecatedRecord,
			EConvertionError
			};

	public:
		static CR99QoSConversion* NewL();
		~CR99QoSConversion();
		TConvRecordType CheckAndConvert(TPtrC &aColumn, TPtrC &aSetting);
		void PrintValues(CFileDump* aFileDumper);
		
		void SetNext(CR99QoSConversion *aNext);
		void SetId(TInt aId);

		CR99QoSConversion* GetNext();
		TInt GetId();
		
		TBool GetModificationFlag();
		
	protected:
		void ConstructL();			

	private:
		void PrintLine(CFileDump* aFileDumper, TPtrC aCol, TInt aSetting);			
		void PrintLine(CFileDump* aFileDumper, TPtrC aCol, TPtrC aSetting);	

	private:
		CR99QoSConversion *iNext;					//next object within the uni-direction list
		TInt iId;									//object identification number
		TBool iModificationFlag;						//indicates the object was updated with some conversion data
		
		//R99 parameters
		TUint32 iGPRSReqTrafficClass;				//< Requested traffic class    
		TUint32 iGPRSMinTrafficClass;				//< Minimum acceptable traffic class
		TUint32 iGPRSReqDeliveryOrder;				//< Requested value for sequential SDU delivery
		TUint32 iGPRSMinDeliveryOrder;				//< Minimum acceptable value for sequential SDU delivery
		TUint32 iGPRSReqDeliverErroneousSDU;		//< Requested value for erroneous SDU delivery
		TUint32 iGPRSMinDeliverErroneousSDU;		//< Minimum acceptable value for erroneous SDU delivery
		TInt32  iGPRSReqMaxSDUSize;	                //< Request maximum SDU size
		TInt32  iGPRSMinAcceptableMaxSDUSize;		//< Minimum acceptable SDU size
		TInt32  iGPRSReqMaxUplinkRate;				//< Requested maximum bit rates on uplink 
		TInt32  iGPRSReqMinUplinkRate;				//< Requested minimum bit rates on uplink 
		TInt32  iGPRSReqMaxDownlinkRate;			//< Requested maximum bit rates on downlink 
		TInt32  iGPRSReqMinDownlinkRate;			//< Requested minimum bit rates on downlink 
		TUint32 iGPRSReqBER;						//< Requested target BER
		TUint32 iGPRSMaxBER;						//< Maximum acceptable target BER
		TUint32 iGPRSReqSDUErrorRatio;				//< Requested target SDU error ratio
		TUint32 iGPRSMaxSDUErrorRatio;				//< Maximum acceptable target SDU error ratio
		TUint32 iGPRSReqTrafficHandlingPriority;    //< Requested traffic handling priority
		TUint32 iGPRSMinTrafficHandlingPriority;    //< Minimum acceptable traffic handling priority
		TInt32  iGPRSReqTransferDelay;				//< Requested transfer delay (in milliseconds)
		TInt32  iGPRSMaxTransferDelay;				//< Maximum acceptable  transfer delay (in milliseconds)
		TInt32  iGPRSReqGuaranteedUplinkRate;		//< Requested guaranteed bit rates on uplink 
		TInt32  iGPRSMinGuaranteedUplinkRate;		//< Minimum acceptable guaranteed bit rates on uplink
		TInt32  iGPRSReqGuaranteedDownlinkRate;		//< Requested guaranteed bit rates on downlink
		TInt32  iGPRSMinGuaranteedDownlinkRate;		//< Minimum acceptable guaranteed bit rates on downlink
		TBool   iGPRSSignallingIndication;			//< Signalling indication
      TBool   iGPRS_ImCnSignallingIndication;     //< Im Cn Signalling indication
      TUint32 iGPRSSourceStatisticsDescriptor;	//< Static source descriptor
   };

#endif
// CR99QoSConversion_H
