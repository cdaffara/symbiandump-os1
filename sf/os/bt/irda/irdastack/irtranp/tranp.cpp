// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "debug.h"
#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>
#include <f32file.h>
#include "tranp.h"
#include "tranpprot.h"



//
// -- IrTranP Configuration definition --
//

EXPORT_C TTranpConfig::TTranpConfig()
/** Constructor */
	{
	iPMID = KDefaultPMID;// Set Primary Machine ID
	iSMID = KNullPMID;// Set Secondary NULL
	}

EXPORT_C TTranpConfig::~TTranpConfig()
/** Destructor */
	{
	}

EXPORT_C void TTranpConfig::SetPrimaryMachine(const TDesC8 &aPMID)
/** Sets the primary machine Id.

@param aPMID A byte sequence defining the primary machine Id */
	{
	iPMID = aPMID;
	}

EXPORT_C const TDesC8& TTranpConfig::PrimaryMachine()
/** Returns the primary machine Id

@return A byte sequence defining the primary machine Id */
	{
	return iPMID;
	}

EXPORT_C void TTranpConfig::SetSecondaryMachine(const TDesC8 &aSMID)
/** Sets the secondary machine Id.

@param aSMID A byte sequence defining the secondary machine Id */
	{
	iSMID = aSMID;
	}

EXPORT_C const TDesC8& TTranpConfig::SecondaryMachine()
/** Gets the secondary machine Id

@return A byte sequence defining the secondary machine Id */
	{
	return iSMID;
	}

//
// -- IrTranP UPF Picture defnition --
//

EXPORT_C TTranpPicture::TTranpPicture() : iPicture(KNullDesC8),iThumbNail(KNullDesC8) 
/** Constructor */
	{
	iUPFPicture = NULL;
	iX = 1; // Default Aspect Ratio
	iY = 1;
	iLatticeSize = EFREE;
	iHeapData = NULL;
	}

EXPORT_C TTranpPicture::~TTranpPicture()
/** Destructor */
	{
	delete iHeapData;
	}

EXPORT_C void TTranpPicture::SetFilename(const TDesC8& aName)
/** Sets the file name of the picture in DOS 8.3 format.

@param aName The filename in 8.3 format. */
	{
	iFilename = aName;
	}

EXPORT_C const TDesC8& TTranpPicture::Filename()
/** Gets the DOS 8.3 format file name of the picture.

@return The filename in 8.3 format. */
	{
	return iFilename;
	}

EXPORT_C const TDesC8* TTranpPicture::Picture()
/** Gets a pointer to a non-modifiable descriptor containing the image data. 
The image is in JPEG format.

@return A pointer to the image data. */
	{
	return iUPFPicture;
	}

EXPORT_C const TDesC8* TTranpPicture::GetThumbNail()
/** Returns a pointer to a descriptor containing a thumbnail representation of 
the image. The thumbnail is in JPEG format.

@return A pointer to the thumbnail data. */
	{
	return NULL;
	}

EXPORT_C void TTranpPicture::SetDesiredLatticeSize(TLatticeSize aSize, TInt /*aWidth*/, TInt /*aHeight*/)
	{
	//This will set the desired lattice size.
	iLatticeSize = aSize;
	}

EXPORT_C void TTranpPicture::GetLatticeSize(TLatticeSize& aSize, TInt& aWidth, TInt& aHeight)
	{
	aSize = iLatticeSize;
	aWidth = iWidth;
	aHeight = iHeight;
	}

EXPORT_C void TTranpPicture::SetLongFilename(const TDesC8& aName)
/** Sets the file name of the picture in SJIS or as an ISO8859-1 character string.

@param aName The filename in long format. */
	{
	iLongFilename = aName;
	}

EXPORT_C const TDesC8& TTranpPicture::LongFilename()
/** Returns the file name of the picture in SJIS or as an ISO8859-1 character string.

@return The filename in long format. */
	{
	return iLongFilename;
	}

EXPORT_C void TTranpPicture::SetTime(const TDesC8& aTime)
/** Sets the time stamp attribute iTime to aTime for the picture.

@param aTime The time stamp in YYYYMMDDHHMMSS character format. */
	{
	iTime = aTime;
	}

EXPORT_C const TDesC8& TTranpPicture::Time()
/** Returns the time stamp attribute for the picture

@return iTime The time stamp in YYYYMMDDHHMMSS character format. */
	{
	return iTime;
	}

EXPORT_C TInt TTranpPicture::SaveAsJpeg(const TFileName& aFileName)
/** Saves the image data in JPEG format to the specified file on the host device.

@param aFileName The name of the file to contain the JPEG format image data. 
@return KErrNone if successful, otherwise one of the other system wide error 
codes. */
	{
	TInt err;
	RFs* fileServer = NULL;
	RFile* dbgFile = NULL;

	// Need to stop this from leaving and return error
	TRAP(err, fileServer = new (ELeave) RFs;
	dbgFile = new (ELeave) RFile);
	
	if (err == KErrNone)
		{ 
		// Connect to Files server
		err = fileServer->Connect();
		if(err == KErrNone)
			{
			// Create the file
			err = dbgFile->Create(*fileServer, aFileName, EFileShareExclusive);
			if(err == KErrNone)
				{
				// Save the file
				err = dbgFile->Write(iPicture);
				if(err == KErrNone)
						{
						err = dbgFile->Flush();
						}
				}
			dbgFile->Close();
			}
		fileServer->Close();
		}
	delete dbgFile;
	delete fileServer;
	return err;
	}

EXPORT_C TInt TTranpPicture::LoadFromJpeg(const TFileName& /*aFileName*/)
	/** Loads JPEG format image data from the specified file on the host device.
	
	@param aFileName The name of the file containing the JPEG format image data. 
	@return KErrNone if successful, otherwise one of the other system wide error 
	codes. */
	{
	return KErrNotSupported;
	}

EXPORT_C TInt TTranpPicture::SaveAsUPF(const TFileName& aFileName)
/** Saves the image data in native UPF format to the specified file on the host 
device.

@param aFileName The name of the file to contain the native UPF format image 
data.
@return KErrNone if successful, otherwise one of the other system wide error 
codes. */
	{
	TInt err;
	RFs* fileServer = NULL;
	RFile* dbgFile = NULL;

	// Need to stop this from leaving and return error
	TRAP(err, fileServer = new (ELeave) RFs;
	dbgFile = new (ELeave) RFile);
	
	if (err == KErrNone)
		{ 
		// Connect to Files server
		err = fileServer->Connect();
		if(err == KErrNone)
			{
			// Create the file
			err = dbgFile->Create(*fileServer, aFileName, EFileShareExclusive);
			if(err == KErrNone)
				{
				// Save the file
				err = dbgFile->Write(*iUPFPicture);
				if(err == KErrNone)
						{
						err = dbgFile->Flush();
						}
				}
			dbgFile->Close();
			}
		fileServer->Close();
		}
	delete dbgFile;
	delete fileServer;
	return err;
	}

EXPORT_C TInt TTranpPicture::LoadFromUPF(const TFileName& /*aFileName*/)
/** Loads native UPF format image data from the specified file on the host device.

@param aFileName The name of the file containing the native UPF format image 
data.
@return KErrNone if successful, otherwise one of the other system wide error 
codes. */
	{
	return KErrNotSupported;
	}

CTranpSession::CTranpSession()
/** Constructor */
	{
	}

EXPORT_C CTranpSession::~CTranpSession()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iTranpProtocol;
#ifdef _DEBUGTRANP_
	RDebug::Close();
#endif
	}
	
EXPORT_C CTranpSession* CTranpSession::NewL(MTranpNotification& aNotifier)
/** Constructs and returns a pointer to a new picture transmission session.

@param aNotifier An implementation of the picture transmission callback interface. 

@return A pointer to the new picture transmission session. */
	{
	DEBUG_OPEN();
	SESSIONPRINT(_L("CTranpSession::NewL()\n"));
	CTranpSession* self = new(ELeave) CTranpSession();
	CleanupStack::PushL(self);
	self->ConstructL(aNotifier);
	CleanupStack::Pop();
	return self;
	}

void CTranpSession::ConstructL(MTranpNotification& aNotifier)
	{
	SESSIONPRINT(_L("CTranpSession::ConstructL()\n"));
	iTranpProtocol = CTranpProtocol::NewL(aNotifier);
	}
    
EXPORT_C void CTranpSession::Connect()
/** No Longer used.

irda client libraries left in to avoid breaks */

	{
	}

EXPORT_C  void CTranpSession::Disconnect()
/** No Longer used.

irda client libraries left in to avoid breaks */

	{
	}
    
EXPORT_C  void CTranpSession::Query(TTranP /*aWhat*/)
/** No Longer used.

irda client libraries left in to avoid breaks */

	{
	}

EXPORT_C  void CTranpSession::Abort()
/** Aborts any transmission in progress. */
	{
	iTranpProtocol->Abort();
	}
    
EXPORT_C  void CTranpSession::Get(TTranpPicture& aPicture)
/** Receives a picture from a peer device.

The following sequence of events is expected:

A peer device connects successfully, resulting in a call to the Connect() 
callback function.

The peer device sends picture data, resulting in successive calls to the ProgressIndication() 
callback function indicating how much of the picture data has been received.

Reception of picture data is complete, resulting in a call to the GetComplete() 
call back function.

Disconnection from the peer device, resulting in a call the Disconnected() 
callback function.

If the connection with the peer device is dropped at any stage in the transmission, 
then this results in a call to the Error() callback function.

@param aPicture The picture object 
@see MTranpNotification::Connected()
@see MTranpNotification::ProgressIndication()
@see MTranpNotification::GetComplete()
@see MTranpNotification::Disconnected()
@see MTranpNotification::Error() */
	{
	iTranpProtocol->Get(aPicture);
	}
    
EXPORT_C  void CTranpSession::Put(const TTranpPicture& /*aPicture*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}
	
EXPORT_C  void CTranpSession::Config(const TTranpConfig& /*aConfig*/)
/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	}

EXPORT_C  TTranpConfig CTranpSession::Config() const

/** No Longer used.

irda client libraries left in to avoid breaks */
	{
	return iConfig;
	}


