
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TSU_MMF_AFMT_NUMCHANNELS_H__
#define __TSU_MMF_AFMT_NUMCHANNELS_H__

#include <testframework.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfdatasink.h>



class CTestStep_MMF_AFMT_NumChannels : public CTestStep_MMF_AFMT
	{
public: // Constructor/Destructor
	CTestStep_MMF_AFMT_NumChannels(){}
	virtual ~CTestStep_MMF_AFMT_NumChannels(){}

protected: // from CTestStep
	/**
	 * Standard postable (just cleanup)
	 */
	void Close()
		{ Cleanup(); } // Postamble

protected: // New functions
	/**
	 * This function tests the specified files have the correct number of channels.
	 * @param aMono - This file should have only one channel.
	 * @param aStereo - This file should have two channels.
	 * @return TVerdict - If the files have the right number of channels, this
	 * function will return EPass, else EFail.
	 */
	TVerdict TestNumChannelsL(const TDesC& aMono,
			 				  const TDesC& aStereo);


	/** 
	 * This function calls NumChannels() on whichever Format object is instanciated.
	 * @return TInt The number of channels.
	 */
	TInt CallNumChannelsL();


	/** 
	 * This function calls SetNumChannels() on whichever Format object is 
	 * instanciated.
	 * @param aChannels - The value to pass to the SetNumChannels() call
	 */
	void CallSetNumChannelsL(const TInt& aChannels);


	/**
	 * This function tests whether a call to SetChannels will work by setting the 
	 * channels, then getting them again to see if the change has taken place.
	 * @param aMono - This file should not exist, else test may fail.
	 * @param aStereo - This file should not exist, else test may fail.
	 * @return TVerdict - If the files have the right number of channels, this
	 * function will return EPass, else EFail.  
	 */
	TVerdict TestSetNumChannelsL(const TDesC& aMono,
								 const TDesC& aStereo);


	/**
	 * If we are testing the RAW Format object, this function sets the fields
	 * required in the Format object for testing.  If we are not testing the
	 * RAW format, this function does nothing.
	 * @param aChannels - The number of channels to set the raw format as having.
	 */
	void SetupForRaw(const TInt& aChannels);


	/** 
	 * Performs some setup specific to this type of test.
	 */
	void SetupL(const TInt& aUid)
		{ iUid = aUid; }	
	
protected:
	TInt iUid;	// The UID of the Format object we are testing.
	};



#endif
