// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// IrTranp API 
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef _TRANP_H_
#define _TRANP_H_

#include <e32base.h>

const TInt KErrIrtranpPeerDoesNotHaveIrtranp = -5530;

_LIT8(KDefaultPMID, "\x08\x00\x74\x00\x00\x00\x00\x01"); //Casio's number
_LIT8(KNullPMID, "\x00\x00\x00\x00\x00\x00\x00\x00");

enum TLatticeSize 
/** Encodes the size of a picture.

@see TTranpPicture */
	{
	/** Quarter VGA, 320x240 pixels. */
	EQVGA,
	/** Full VGA, 640x480 pixels. */
	EVGA,
	/** Super VGA, 800x600 pixels. */
	ESVGA,
    /** XGA, 1024x768 pixels. */
	EXGA,
	/** Super XGA, 1280x960 pixels. */
	ESXGA,
	/** Arbitrary size. There is no implied width and height. These values must be 
	explicitly supplied. */
	EFREE
};

NONSHARABLE_CLASS(TTranpConfig)

/** Encapsulates configuration parameters for a session.
@publishedAll
@released
@see CTranpSession */
	{
public:
	IMPORT_C TTranpConfig();
	IMPORT_C ~TTranpConfig();

	IMPORT_C void SetPrimaryMachine(const TDesC8& aPMID);
	IMPORT_C void SetSecondaryMachine(const TDesC8& aSMID);

	IMPORT_C const TDesC8& PrimaryMachine();
	IMPORT_C const TDesC8& SecondaryMachine();

private:
	TBufC8<8> iPMID;
	TBufC8<8> iSMID;

	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

//
// Encapsulation the UPF file format.
//
NONSHARABLE_CLASS(TTranpPicture)

/** Represents a picture in Unified Picture Format (UPF), a standard for image 
data transmitted via infrared communication.

An object of this type is used when transmitting pictures using the IrTranP 
protocol.

A received picture can be saved as a JPEG type for viewing or it can be saved 
in native UPF format. 
@publishedAll
@released */
	{
friend class CTranpProtocol;
public:
	IMPORT_C void SetFilename(const TDesC8& aName);
	IMPORT_C void SetLongFilename(const TDesC8& aName);    
	IMPORT_C void SetTime(const TDesC8& aTime);

	IMPORT_C const TDesC8& Filename();
	IMPORT_C const TDesC8& LongFilename();
	IMPORT_C const TDesC8& Time();
	IMPORT_C const TDesC8* Picture();
	
	IMPORT_C void SetDesiredLatticeSize(TLatticeSize aSize, TInt iWidth = 0, TInt iHeight = 0); // Default is EVGA
	IMPORT_C void GetLatticeSize(TLatticeSize& aSize, TInt& iWidth, TInt& iHeight);

	IMPORT_C const TDesC8* GetThumbNail();

	IMPORT_C TInt SaveAsJpeg(const TFileName& aFileName);
	IMPORT_C TInt LoadFromJpeg(const TFileName& aFileName);
	IMPORT_C TInt SaveAsUPF(const TFileName& aFileName);
	IMPORT_C TInt LoadFromUPF(const TFileName& aFileName);

public:
	IMPORT_C TTranpPicture();
	IMPORT_C ~TTranpPicture();

private:
	TBuf8<31> iFilename;		// fomatted in DOS 8.3 format
	TBuf8<256> iLongFilename;	// in SJIS or ISO8859-1 [Latin-1]
	TBuf8<14> iTime;			// Time string in format YYYYMMDDHHMMSS
	TPtrC8 iPicture;			// Pointer to a buffer conatining the image data [JPEG format]
	TDesC8* iUPFPicture;		// Pointer to a buffer conatining the image data [UPF format]
	TPtrC8 iThumbNail;			// Pointer to a buffer conatining the thumbnail image data [JPEG format] Not implemented.
	TLatticeSize iLatticeSize;	// Size of picture 
	TInt iWidth;	// QVGA:=320x240, VGA:=640x480, SVGA:=800x600, XGA:=1024x768, SXGA:=1280x960, FREE:=m x n
	TInt iHeight;
	TInt iX;		// Aspect Ratio iX:iY
	TInt iY;
	HBufC8* iHeapData;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

// 
// Callback interface for Asynchrnous requests
//
class MTranpNotification
/** A callback interface providing notification of events that occur during a picture 
transmission session.

Users of a picture transmission session must derive from this class an provide 
implementation for all of the pure virtual functions.

@publishedAll
@released
@see CTranpSession */
	{
public:
	
	/** This function is called when a peer device connects successfully to the host 
	device following an attempt to receive a picture. */
	virtual void Connected() = 0;
	
	/** This function is called when a peer device disconnects or is disconnected from 
	the host device. */
	virtual void Disconnected() =0;
	
	/** This function is called when the reception of picture data is complete. */
	virtual void GetComplete() = 0;
	
	/** This function is called during the sending or receiving of picture data and 
	indicates the percentage of the data that has been transmitted.
	
	@param aPercent The percentage of picture data transmitted. */
	virtual void ProgressIndication(TInt aPercent) = 0;
	
	/** This function is called when the sending of picture data is complete. */
	virtual void PutComplete() = 0;
	
	/** This function is called when an operation querying the processing ability of 
	a peer device completes. */
	virtual void QueryComplete(/*return result from query [what]*/)= 0;
	
	/** This function is called when the connection with the peer device drops during 
	picture data transmission.
	
	@param aError An error code associated with the connection drop. Normally, 
	one of the system wide error codes. */
	virtual void Error(TInt aError) =0;

    /**
 	 Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	 @param aInterface UID of the interface to return
	 @param aObject the container for another interface as specified by aInterface
	 */
	IMPORT_C virtual void MTN_ExtensionInterfaceL(TUid aInterface, void*& aObject);	
	};

class CTranpProtocol;

NONSHARABLE_CLASS(CTranpSession) : public CBase

/** Encapsulates the behaviour for sending a picture to a peer device and for receiving 
a picture from a peer device. 

@publishedAll
@released */
	{
public: 
    enum TTranP
    /** The type of information required from a peer device. */
    {
     /** Request still-information from the peer, e.g. resolution and size. */
     EReqImageInfo = 0, 
     /** Request information about the peer's battery status and free memory. */
     EReqStatus, 
     /** Request how many commands can be executed in one SCEP session. */
     EReqCommands
	};

public:
	// Constructs a TranP Session
	IMPORT_C static CTranpSession* NewL(MTranpNotification& aNotifier);

private:
	void ConstructL(MTranpNotification& aNotifier);

public:
	// Methods for establishing a connection
	IMPORT_C void Connect();
	IMPORT_C void Disconnect();
	IMPORT_C void Abort();
	// Query capabilities of peer
	IMPORT_C void Query(TTranP aWhat);
	// Receive picture from peer
	IMPORT_C void Get(TTranpPicture& aPicture);
	// Send picture to peer
	IMPORT_C void Put(const TTranpPicture& aPicture);
	// Change configuration parameters
	IMPORT_C void Config(const TTranpConfig& aConfig);
	IMPORT_C TTranpConfig Config() const;
	IMPORT_C ~CTranpSession();

private:
	CTranpSession();	

private:
	CTranpProtocol* iTranpProtocol;
	TTranpConfig iConfig;
	};

#endif //_TRANP_H_
