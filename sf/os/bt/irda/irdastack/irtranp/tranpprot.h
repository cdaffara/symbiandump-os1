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
//

#ifndef _TRANPPROT_H
#define _TRANPPROT_H

class BFTP;

NONSHARABLE_CLASS(CTranpProtocol) : public CBase
	{
public:
	CTranpProtocol(MTranpNotification& aNotifier);
	~CTranpProtocol();
	
    // Constructs a TranP Session
	static CTranpProtocol* NewL(MTranpNotification& aNotifier);
	void ConstructL(const MTranpNotification& aNotifier);

    // Methods for establishing a connection
	void Connect();
	void Disconnect();
	void Abort();

    // Query capabilities of peer
	void Query(CTranpSession::TTranP aWhat);

    // Receive picture from peer
	void Get(TTranpPicture& aPicture);

    // Send picture to peer
	void Put(const TTranpPicture& aPicture);

	// Change configuration parameters
    void Config(const TTranpConfig& aConfig);
	TTranpConfig Config(const TTranpConfig& aConfig) const;

public:
	// Sets ownership of allocated memory to TTranpPicture object passed in Get() method
	void SetHeapData(HBufC8* aHeapData) {iTranpPicture->iHeapData=aHeapData;}
	// Removes allocated memory from the TTranpPicture object passed in Get() method
	void DeleteHeapData() {if((iTranpPicture->iHeapData)!=NULL)
								{
								delete iTranpPicture->iHeapData;
								iTranpPicture->iHeapData=NULL;
								}
							}
	
	void SetPicture(TDesC8& aPicture, TDesC8& aFilename, TDesC8& aUPFPicture, TDesC8& aLongFilename);

private:
	BFTP* iBFTP;
	TTranpPicture* iTranpPicture;
	MTranpNotification& iNotifier;
	};

#endif //_TRANPPROT_H
