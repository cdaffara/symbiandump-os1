/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef T_MMFDEVVIDEORECORDDATA_H_
#define T_MMFDEVVIDEORECORDDATA_H_

//Epoc includes
#include <devvideorecord.h>
#include <devvideobase.h>

//User includes
#include "datawrapperbase.h"
#include "t_devvideoconstants.h"

class CT_MMFDevVideoRecordData : public CDataWrapperBase, public MMMFDevVideoRecordObserver 
{
public://construction
	static CT_MMFDevVideoRecordData* NewL();
	virtual ~CT_MMFDevVideoRecordData();

public:
	virtual TAny* GetObject();

protected://construction
	CT_MMFDevVideoRecordData();
	void ConstructL();

public://commands
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);	
private://commands
	void DoCmdNewL(const TTEFSectionName& aSection);
	void DoCmdDestructor();
	void DoCmdSelectEncoderL(const TTEFSectionName& aSection);
	void DoCmdSetInputFormatL();
	void DoCmdSetOutputFormatL(const TTEFSectionName& aSection);
	void DoCmdSetBufferOptionsL(const TTEFSectionName& aSection);
	void DoCmdSetSourceMemoryL();
	void DoCmdSetMinRandomAccessRate(const TTEFSectionName& aSection);
	void DoCmdInitialize(const TInt aAsyncErrorIndex);
	void DoCmdSetComplexityLevel(const TTEFSectionName& aSection);
	void DoCmdStart(const TInt aAsyncErrorIndex);
	void DoCmdPause();
	void DoCmdResume();
	void DoCmdStop();
  void DoCmdSetRateControlOptions(const TTEFSectionName& aSection);

	
private://callbacks
	void MdvroReturnPicture(TVideoPicture *aPicture);
	void MdvroSupplementalInfoSent();
	void MdvroNewBuffers();
	void MdvroFatalError(TInt aError);
	void MdvroInitializeComplete(TInt aError);
	void MdvroStreamEnd();
	
private://helpers
	void SetPictureTypeL(const TTEFSectionName& aSection);
	void CreateBuffers();
	int CreateFiles(const TTEFSectionName& aSection);
	int OpenFile(TFileName& aOutFileName, TFileName& aInFileName);
	void HandleNewBuffer();
	void HandleReturnedPicture();
	TInt HandleOutputRequest(/*TInt aNumberOfPictures*/);
	void SaveAndReturnBuffer(TVideoOutputBuffer* tmp);
	TInt FillAndSendBuffer();
	void DoCmdUtilityGetEnvironmentL();
	void DestroyData();
private:
	/**
	 * Async Error
	 */
	TInt iAsyncErrorIndex;
	/**
	 * Wrapped object
	 */
	CMMFDevVideoRecord* iDevvr;
	/**
	 * Buffer control
	 */
	TInt iBufferCount;	
	/**
	 * Time control
	 */
	TUint32 iTimeStamp;
	/**
	 * Number of written pictures
	 */
	TInt iWrittenPictures;
	/**
	 * Number of returned pictures
	 */
	TInt iReturnedPictures;
	/**
	 * Compressed buffers
	 */
	TInt iCompressedBuffers;
	/** 
	 * Number of  buffered pictures
	 */
	TInt iBufferedPictures;
	/**
	 * Control the recording ending
	 */
	TBool iStopAfterAllFramesReturned;
	/**
	 * Control the number of inputs
	 */
	TBool iInputEnd;
	/**
	 * Control Frame rate
	 */
	TInt iFrameRate;
	/**
	 * Control bit rate
	 */
	TInt iBitRate;
	/**
	 * Picture size
	 */
	TSize iPictSize;
	/**
	 * Buffer list
	 */
	RPointerArray<TUint8>*iDatabuf;
	/**
	 * Id for hardware encoder
	 */
	THwDeviceId iEncHWDevId;
	/**
	 * One uncompressed video picture.
	 * Used for both decoded picture output as well as uncompressed picture input
	 */
	TVideoPicture iPicture[KBufferInputPictures];
	/**
	 * Number of requests
	 */
	TInt iRequests;
	/**
	 * Type of requests
	 */
	TInt iRequestTypes;
	/** Number of output requests
	 */
	TInt iOutputRequests;
	/**
	 * File server
	 */
	RFs   iFs;
	/**
	 * Input file
	 */
	RFile iInFile;
	/**
	 * Output file
	 */
	RFile iOutFile;
	/**
	 * Specification enum
	 */
	static const TEnumEntryTable iHrdVbvSpecification[];
};
#endif /*T_MMFDEVVIDEORECORDDATA_H_*/
