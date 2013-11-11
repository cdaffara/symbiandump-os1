// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedAll
 @released
*/
#ifndef BTSDP_H
#define BTSDP_H

#include <e32base.h>
#include <bttypes.h>


/** The UID of the SDP Server. 

@see TBTServiceSecurity */
static const TUid KUidServiceSDP			= {0x100095FA};

/** Attribute ID. */
typedef TUint16 TSdpAttributeID;

// Universal Attributes
/** Service Record Handle. */
static const TSdpAttributeID KSdpAttrIdServiceRecordHandle = 0x0000;
/** Service ClassID List. */
static const TSdpAttributeID KSdpAttrIdServiceClassIDList = 0x0001;
/** Service Record State. */
static const TSdpAttributeID KSdpAttrIdServiceRecordState = 0x0002;
/** ServiceID. */
static const TSdpAttributeID KSdpAttrIdServiceID = 0x0003;
/** Protocol Descriptor List. */
static const TSdpAttributeID KSdpAttrIdProtocolDescriptorList = 0x0004;
/** Browse Group List. */
static const TSdpAttributeID KSdpAttrIdBrowseGroupList = 0x0005;
/** Language Base Attribute ID List. */
static const TSdpAttributeID KSdpAttrIdLanguageBaseAttributeIDList = 0x0006;
/** Service Info Time To Live. */
static const TSdpAttributeID KSdpAttrIdServiceInfoTimeToLive = 0x0007;
/** Service Availability. */
static const TSdpAttributeID KSdpAttrIdServiceAvailability = 0x0008;
/** Bluetooth Profile Descriptor List. */
static const TSdpAttributeID KSdpAttrIdBluetoothProfileDescriptorList = 0x0009;
/** Documentation URL. */
static const TSdpAttributeID KSdpAttrIdDocumentationURL = 0x000A;
/** Client Executable URL. */
static const TSdpAttributeID KSdpAttrIdClientExecutableURL = 0x000B;
/** Icon URL. */
static const TSdpAttributeID KSdpAttrIdIconURL = 0x000C;
/** Additional protocol descriptor list. */
static const TSdpAttributeID KSdpAttrIdAdditionalProtocolDescriptorList = 0x000D;


/** PAN Profile Security enforcement */
static const TSdpAttributeID KSdpAttrIdSecurityDescription = 0x030a;

/** PAN Profile, NAP access type */
static const TSdpAttributeID KSdpAttrIdNetAccessType = 0x030b;

/** PAN Profile, NAP maximum throughput of uplink network (octets/second) */
static const TSdpAttributeID KSdpAttrIdMaxNetAccessRate = 0x030c;

/** PAN Profile, NAP IPv4 Subnet */
static const TSdpAttributeID KSdpAttrIdIPv4Subnet = 0x030d;

/** PAN Profile, NAP IPv6 Subnet */
static const TSdpAttributeID KSdpAttrIdIPv6Subnet = 0x030e;

/** SupportedCapabilities */
static const TSdpAttributeID KSdpAttrIdSupportedCapabilities = 0x0310;
/** SupportedFeatures */
static const TSdpAttributeID KSdpAttrIdSupportedFeatures = 0x0311;
/** SupportedFunctions */
static const TSdpAttributeID KSdpAttrIdSupportedFunctions = 0x0312;


// Language specific attributes

/** Primary Language. */
static const TSdpAttributeID KSdpAttrIdBasePrimaryLanguage = 0x0100;
static const TSdpAttributeID KSdpAttrIdCharacterEncodingUTF8 = 0x006a;

/** Service Name.

This is an offset that should be added to the attribute ID base (contained 
in the LanguageBaseAttributeIDList attribute). */
static const TInt KSdpAttrIdOffsetServiceName = 0x0000; // + Language offset


/** Service Description.

This is an offset that should be added to the attribute ID base (contained 
in the LanguageBaseAttributeIDList attribute). */
static const TInt KSdpAttrIdOffsetServiceDescription = 0x0001; // + Language offset


/** Provider Name.

This is an offset that should be added to the attribute ID base (contained 
in the LanguageBaseAttributeIDList attribute). */
static const TInt KSdpAttrIdOffsetProviderName = 0x0002; // + Language offset


// SDP server specific attributes
/** Version Number List. */
static const TSdpAttributeID KSdpAttrIdSdpServerVersionNumberList = 0x0200;
/** Service Database State. */
static const TSdpAttributeID KSdpAttrIdSdpServerServiceDatabaseState = 0x0201;
/** GroupID. */
static const TSdpAttributeID KSdpAttrIdSdpServerGroupID = 0x0200;


// Protocol UUIDs (add to these as necessary)
static const TInt KL2CAPUUID = 0x0100;
static const TInt KBnepUUID = 0x000f;
static const TInt KAvctpUUID = 0x0017;
static const TInt KAvdtpUUID = 0x0019;
static const TInt KObexProtocolUUID = 0x0008;
static const TInt KRFCommUUID = 0x0003;

// Service class UUIDs (add to these as necessary)
/** L2CAP UUID used for continuation tests*/
static const TUint32	KSdpContTestUUID = 0x100;
/** Serial Port UUID */
static const TInt KSerialPortUUID = 0x1101;
/** Dial Networking UUID */
static const TInt KDialUpNetworkingUUID = 0x1103;
/** General Networking UUID */
static const TInt KGenericNetworkingUUID = 0x1201;
/** Fax UUID */
static const TInt KFaxUUID = 0x1111;
/** Generic Telaphony UUID */
static const TInt KGenericTelephonyUUID = 0x1204;
/** Obex UUID */
static const TInt KObexUUID = 0x1105;
/** Attribute number for the Protocol Descriptor List */
static const TInt KProtocolDescriptorListUUID = 0x0004;
/** Public Browse Group UUID */
static const TInt KPublicBrowseGroupUUID = 0x1002;
/** Browse Group Descriptor Service Class UUID */
static const TInt KBrowseGroupDescriptorServiceClassUUID = 0x1001;
/** Service Discovery Server Service Class UUID */
static const TInt KServiceDiscoveryServerServiceClassUUID = 0x1000;
/** PAN Service Class UUID */
static const TInt KPanUUUID = 0x1115;
/** PAN NAP Service Class UUID */
static const TInt KPanNapUUID = 0x1116;
/** PAN GN Service Class UUID */
static const TInt KPanGnUUID = 0x1117;
/** Audio Source (service class) UUID */
static const TInt KAudioSourceUUID = 0x110a;
/** Audio Sink (service class) UUID */
static const TInt KAudioSinkUUID = 0x110b;
/** Remote control target (service class) UUID */
static const TInt KAVRemoteControlTargetUUID = 0x110c;
/** Advanced audio distribution (A2DP) (profile descriptor) UUID */
static const TInt KAdvancedAudioDistributionUUID = 0x110d;
/** Remote control (service class) UUID */
static const TInt KAVRemoteControlUUID = 0x110E;
/** Remote control controller (service class) UUID */
static const TInt KAVRemoteControlControllerUUID = 0x110F;
/** PBAP PSE Service Class UUID */
static const TInt KPbapPseUUID = 0x112F;
  
 
// Language codes, as per "ISO 639:1988 (E/F)"
/** Afar */
static const TInt16 KLanguageAfar = 0x6161; // aa
/** Abkhazian */
static const TInt16 KLanguageAbkhazian = 0x6162; // ab
/** Afrikaans */
static const TInt16 KLanguageAfrikaans = 0x6166; // af
/** Amharic */
static const TInt16 KLanguageAmharic = 0x616D; // am
/** Arabic */
static const TInt16 KLanguageArabic = 0x6172; // ar
/** Assamese */
static const TInt16 KLanguageAssamese = 0x6173; // as
/** Aymara */
static const TInt16 KLanguageAymara = 0x6179; // ay
/** Azerbaijani */
static const TInt16 KLanguageAzerbaijani = 0x617A; // az
/** Bashkir */
static const TInt16 KLanguageBashkir = 0x6261; // ba
/** Byelorussian */
static const TInt16 KLanguageByelorussian = 0x6265; // be
/** Bulgarian */
static const TInt16 KLanguageBulgarian = 0x6267; // bg
/** Bihari */
static const TInt16 KLanguageBihari = 0x6268; // bh
/** Bislama */
static const TInt16 KLanguageBislama = 0x6269; // bi
/** Bengali */
static const TInt16 KLanguageBengali = 0x626E; // bn - also Bangala
/** Tibetan */
static const TInt16 KLanguageTibetan = 0x626F; // bo
/** Breton */
static const TInt16 KLanguageBreton = 0x6272; // br
/** Catalan */
static const TInt16 KLanguageCatalan = 0x6361; // ca
/** Corsican */
static const TInt16 KLanguageCorsican = 0x636F; // co
/** Czech */
static const TInt16 KLanguageCzech = 0x6373; // cs
/** Welsh */
static const TInt16 KLanguageWelsh = 0x6379; // cy
/** Danish */
static const TInt16 KLanguageDanish = 0x6461; // da
/** German */
static const TInt16 KLanguageGerman = 0x6465; // de
/** Bhutani */
static const TInt16 KLanguageBhutani = 0x647A; // dz
/** Greek */
static const TInt16 KLanguageGreek = 0x656C; // el
/** English */
static const TInt16 KLanguageEnglish = 0x656E; // en
/** Esperanto */
static const TInt16 KLanguageEsperanto = 0x656F; // eo
/** Spanish */
static const TInt16 KLanguageSpanish = 0x6573; // es
/** Estonian */
static const TInt16 KLanguageEstonian = 0x6574; // et
/** Basque */
static const TInt16 KLanguageBasque = 0x6575; // eu
/** Persian */
static const TInt16 KLanguagePersian = 0x6661; // fa
/** Finnish */
static const TInt16 KLanguageFinnish = 0x6669; // fi
/** Fiji */
static const TInt16 KLanguageFiji = 0x666A; // fj
/** Faroese */
static const TInt16 KLanguageFaroese = 0x666F; // fo
/** French */
static const TInt16 KLanguageFrench = 0x6672; // fr
/** Frisian */
static const TInt16 KLanguageFrisian = 0x6679; // fy
/** Irish */
static const TInt16 KLanguageIrish = 0x6761; // ga
/** Scots Gaelic */
static const TInt16 KLanguageScotsGaelic = 0x6764; // gd
/** Galician */
static const TInt16 KLanguageGalician = 0x676C; // gl
/** Guarani */
static const TInt16 KLanguageGuarani = 0x676E; // gn
/** Gujarati */
static const TInt16 KLanguageGujarati = 0x6775; // gu
/** Hausa */
static const TInt16 KLanguageHausa = 0x6861; // ha
/** Hebrew */
static const TInt16 KLanguageHebrew = 0x6865; // he - formerly iw
/** Hindi */
static const TInt16 KLanguageHindi = 0x6869; // hi
/** Croatian */
static const TInt16 KLanguageCroatian = 0x6872; // hr
/** Hungarian */
static const TInt16 KLanguageHungarian = 0x6875; // hu
/** Armenian */
static const TInt16 KLanguageArmenian = 0x6879; // hy
/** Interlingua */
static const TInt16 KLanguageInterlingua = 0x6961; // ia
/** Indonesian */
static const TInt16 KLanguageIndonesian = 0x6964; // id - formerly in
/** Interlingue */
static const TInt16 KLanguageInterlingue = 0x6965; // ie
/** Inupiak */
static const TInt16 KLanguageInupiak = 0x696B; // ik
/** Icelandic */
static const TInt16 KLanguageIcelandic = 0x6973; // is
/** Italian */
static const TInt16 KLanguageItalian = 0x6974; // it
/** Inuktitut */
static const TInt16 KLanguageInuktitut = 0x6975; // iu
/** Japanese */
static const TInt16 KLanguageJapanese = 0x6A61; // ja
/** Javanese */
static const TInt16 KLanguageJavanese = 0x6A77; // jw
/** Georgian */
static const TInt16 KLanguageGeorgian = 0x6B61; // ka
/** Kazakh */
static const TInt16 KLanguageKazakh = 0x6B6B; // kk
/** Greenlandic */
static const TInt16 KLanguageGreenlandic = 0x6B6C; // kl
/** Cambodian */
static const TInt16 KLanguageCambodian = 0x6B6D; // km
/** Kannada */
static const TInt16 KLanguageKannada = 0x6B6E; // kn
/** Korean */
static const TInt16 KLanguageKorean = 0x6B6F; // ko
/** Kashmiri */
static const TInt16 KLanguageKashmiri = 0x6B73; // ks
/** Kurdish */
static const TInt16 KLanguageKurdish = 0x6B75; // ku
/** Kirghiz */
static const TInt16 KLanguageKirghiz = 0x6B79; // ky
/** Latin */
static const TInt16 KLanguageLatin = 0x6C61; // la
/** Lingala */
static const TInt16 KLanguageLingala = 0x6C6E; // ln
/** Laothian */
static const TInt16 KLanguageLaothian = 0x6C6F; // lo
/** Lithuanian */
static const TInt16 KLanguageLithuanian = 0x6C74; // lt
/** Latvian */
static const TInt16 KLanguageLatvian = 0x6C76; // lv - also Lettish
/** Malagasy */
static const TInt16 KLanguageMalagasy = 0x6D67; // mg
/** Maori */
static const TInt16 KLanguageMaori = 0x6D69; // mi
/** Macedonian */
static const TInt16 KLanguageMacedonian = 0x6D6B; // mk
/** Malayalam */
static const TInt16 KLanguageMalayalam = 0x6D6C; // ml
/** Mongolian */
static const TInt16 KLanguageMongolian = 0x6D6E; // mn
/** Moldavian */
static const TInt16 KLanguageMoldavian = 0x6D6F; // mo
/** Marathi */
static const TInt16 KLanguageMarathi = 0x6D72; // mr
/** Malay */
static const TInt16 KLanguageMalay = 0x6D73; // ms
/** Maltese */
static const TInt16 KLanguageMaltese = 0x6D74; // mt
/** Burmese */
static const TInt16 KLanguageBurmese = 0x6D79; // my
/** Nauru */
static const TInt16 KLanguageNauru = 0x6E61; // na
/** Nepali */
static const TInt16 KLanguageNepali = 0x6E65; // ne
/** Dutch */
static const TInt16 KLanguageDutch = 0x6E6C; // nl
/** Norwegian */
static const TInt16 KLanguageNorwegian = 0x6E6F; // no
/** Occitan */
static const TInt16 KLanguageOccitan = 0x6F63; // oc
/** Oromo */
static const TInt16 KLanguageOromo = 0x6F6D; // om - (Afan)
/** Oriya */
static const TInt16 KLanguageOriya = 0x6F72; // or
/** Punjabi */
static const TInt16 KLanguagePunjabi = 0x7061; // pa
/** Polish */
static const TInt16 KLanguagePolish = 0x706C; // pl
/** Pashto */
static const TInt16 KLanguagePashto = 0x7073; // ps
/** Portuguese */
static const TInt16 KLanguagePortuguese = 0x7074; // pt
/** Quechua */
static const TInt16 KLanguageQuechua = 0x7175; // qu
/** Rhaeto Romance */
static const TInt16 KLanguageRhaetoRomance = 0x726D; // rm
/** Kirundi */
static const TInt16 KLanguageKirundi = 0x726E; // rn
/** Romanian */
static const TInt16 KLanguageRomanian = 0x726F; // ro
/** Russian */
static const TInt16 KLanguageRussian = 0x7275; // ru
/** Kinyarwanda */
static const TInt16 KLanguageKinyarwanda = 0x7277; // rw
/** Sanskrit */
static const TInt16 KLanguageSanskrit = 0x7361; // sa
/** Sindhi */
static const TInt16 KLanguageSindhi = 0x7364; // sd
/** Sangho */
static const TInt16 KLanguageSangho = 0x7367; // sg
/** SerboCroatian */
static const TInt16 KLanguageSerboCroatian = 0x7368; // sh
/** Sinhalese */
static const TInt16 KLanguageSinhalese = 0x7369; // si
/** Slovak */
static const TInt16 KLanguageSlovak = 0x736B; // sk
/** Slovenian */
static const TInt16 KLanguageSlovenian = 0x736C; // sl
/** Samoan */
static const TInt16 KLanguageSamoan = 0x736D; // sm
/** Shona */
static const TInt16 KLanguageShona = 0x736E; // sn
/** Somali */
static const TInt16 KLanguageSomali = 0x736F; // so
/** Albanian */
static const TInt16 KLanguageAlbanian = 0x7371; // sq
/** Serbian */
static const TInt16 KLanguageSerbian = 0x7372; // sr
/** Siswati */
static const TInt16 KLanguageSiswati = 0x7373; // ss
/** Sesotho */
static const TInt16 KLanguageSesotho = 0x7374; // st
/** Sundanese */
static const TInt16 KLanguageSundanese = 0x7375; // su
/** Swedish */
static const TInt16 KLanguageSwedish = 0x7376; // sv
/** Swahili */
static const TInt16 KLanguageSwahili = 0x7377; // sw
/** Tamil */
static const TInt16 KLanguageTamil = 0x7461; // ta
/** Telugu */
static const TInt16 KLanguageTelugu = 0x7465; // te
/** Tajik */
static const TInt16 KLanguageTajik = 0x7467; // tg
/** Thai */
static const TInt16 KLanguageThai = 0x7468; // th
/** Tigrinya */
static const TInt16 KLanguageTigrinya = 0x7469; // ti
/** Turkmen */
static const TInt16 KLanguageTurkmen = 0x746B; // tk
/** Tagalog */
static const TInt16 KLanguageTagalog = 0x746C; // tl
/** Setswana */
static const TInt16 KLanguageSetswana = 0x746E; // tn
/** Tonga */
static const TInt16 KLanguageTonga = 0x746F; // to
/** Turkish */
static const TInt16 KLanguageTurkish = 0x7472; // tr
/** Tsonga */
static const TInt16 KLanguageTsonga = 0x7473; // ts
/** Tatar */
static const TInt16 KLanguageTatar = 0x7474; // tt
/** Twi */
static const TInt16 KLanguageTwi = 0x7477; // tw
/** Uighur */
static const TInt16 KLanguageUighur = 0x7567; // ug
/** Ukrainian */
static const TInt16 KLanguageUkrainian = 0x756B; // uk
/** Urdu */
static const TInt16 KLanguageUrdu = 0x7572; // ur
/** Uzbek */
static const TInt16 KLanguageUzbek = 0x757A; // uz
/** Vietnamese */
static const TInt16 KLanguageVietnamese = 0x7669; // vi
/** Volapuk */
static const TInt16 KLanguageVolapuk = 0x766F; // vo
/** Wolof */
static const TInt16 KLanguageWolof = 0x776F; // wo
/** Xhosa */
static const TInt16 KLanguageXhosa = 0x7868; // xh
/** Yiddish */
static const TInt16 KLanguageYiddish = 0x7969; // yi - formerly ji
/** Yoruba */
static const TInt16 KLanguageYoruba = 0x796F; // yo
/** Zhuang */
static const TInt16 KLanguageZhuang = 0x7A61; // za
/** Chinese */
static const TInt16 KLanguageChinese = 0x7A68; // zh
/** Zulu */
static const TInt16 KLanguageZulu = 0x7A75; // zu

//***********************************************************************/
//
//   SDP database server API
//
//***********************************************************************/

/** Typedef for a TSdpAttributeID package buf */
typedef TPckgBuf<TSdpAttributeID> TSdpAttributeIDPckgBuf;
/** Handle to a service record. */
typedef TUint32 TSdpServRecordHandle;
/** Typedef for a TSdpServRecordHandle package buf */
typedef TPckgBuf<TSdpServRecordHandle> TSdpServRecordHandlePckgBuf;

NONSHARABLE_CLASS(RSdp) : public RSessionBase
/** Provides a session to the Service Discovery Database.

Used to create subsessions to database functionality. A clients must create
and connect a session, before using a RSdpDatabase subsession to access the
database.

@see RSdpDatabase */
	{
public:
	IMPORT_C RSdp();
	IMPORT_C TInt Connect();
	IMPORT_C TVersion Version() const;
	IMPORT_C void ResourceCountMarkStart();
	IMPORT_C void ResourceCountMarkEnd();
	IMPORT_C TInt ResourceCount();
	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);

private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

NONSHARABLE_CLASS(RSdpSubSession) : public RSubSessionBase
/** Base class used in the derivation of RSdpDatabase. 

The class contains basic subssession functionality. It has no user accessible
functions. */
	{
public:
	/** Opens subsession on an RSdp session
    @param aSession The session on which the subsession is being opened.
    */
	IMPORT_C virtual TInt Open(RSdp& aSession)=0;
	/** Closes subsession */
	IMPORT_C virtual void Close()=0;

private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 		
	};



class CSdpAttrValueDES;
class CSdpAttrValue;

NONSHARABLE_CLASS(RSdpDatabase) : public RSdpSubSession
/** Subsession to the SDP through which service records and their attributes can 
be added, deleted, and updated. */
	{
public:
	IMPORT_C RSdpDatabase();
	IMPORT_C TInt Open(RSdp& aSession);
	IMPORT_C void Close();
	IMPORT_C void CreateServiceRecordL(const TUUID& aUUID, TSdpServRecordHandle& aHandle);
	IMPORT_C void CreateServiceRecordL(CSdpAttrValueDES& aUUIDList, TSdpServRecordHandle& aHandle);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue& aAttrValue);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, TUint aUintValue);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, const TDesC16& aDesCValue);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, const TDesC8& aDesCValue);
	IMPORT_C void DeleteAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID);
	IMPORT_C void DeleteAttribute(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID);
	IMPORT_C void DeleteRecordL(TSdpServRecordHandle aHandle);
	IMPORT_C void DeleteRecord(TSdpServRecordHandle aHandle);
private:
	HBufC8* iBuffer; //used for synchronous requests (could have been local)
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 		
	};


class MSdpElementBuilder
/** Constructs an attribute value, or a set of attributes, from multiple data elements.

The builder interface is used for building single attribute values, and sets of attribute 
value, attribute ID pairs, as for example in a service record.

In particular, the builder interface can be implemented to receive the results from 
CSdpAgent attribute queries.

Each member function itself returns an MSdpElementBuilder interface that can 
then be used to add further elements. 

For more on the format of attribute values, see [BS1 Service Discovery Protocol 
3].

Note that the interface defines each function to leave with the error KErrGeneral.

@see CSdpAgent::AttributeRequestL() */
	{
public:
	IMPORT_C virtual MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	IMPORT_C virtual MSdpElementBuilder* BuildNilL();
	IMPORT_C virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	IMPORT_C virtual MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	IMPORT_C virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	IMPORT_C virtual MSdpElementBuilder* BuildBooleanL(TBool aBool);
	IMPORT_C virtual MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	IMPORT_C virtual MSdpElementBuilder* BuildDESL();  // Must not return NULL
	IMPORT_C virtual MSdpElementBuilder* BuildDEAL();  // ditto
	IMPORT_C virtual MSdpElementBuilder* StartListL(); // ditto
	IMPORT_C virtual MSdpElementBuilder* EndListL();
	IMPORT_C virtual MSdpElementBuilder* BuildURLL(const TDesC8& aURL);
	
    /**
 	 Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	 @param aInterface UID of the interface to return
	 @param aObject the container for another interface as specified by aInterface
	 */
	IMPORT_C void MSEB_ExtensionInterfaceL(TUid aInterface, void*& aObject);	
	};

/** Types of attribute data elements. */
enum TSdpElementType
	{
	/** Null type. */
	ETypeNil		=  0,
	/** Unsigned integer. */
	ETypeUint		=  1,
	/** Signed integer. */
	ETypeInt		=  2,
	/** UUID. */
	ETypeUUID		=  3,
	/** Text string. */
	ETypeString		=  4,
	/** Boolean. */
	ETypeBoolean	=  5,
	/** Data element sequence. */
	ETypeDES		=  6,
	/** Data element alternative. */
	ETypeDEA		=  7,
	/** URL. */
	ETypeURL		=  8,
	/** Outside SDP reserved range. */
	ETypeEncoded	= 32,	// Outside SDP reserved range
	};

class MSdpAttributeValueVisitor;

NONSHARABLE_CLASS(CSdpAttrValue) : public CBase
/** Base class for classes that encapsulate SDP attributes values.

Sub-classes of this wrap specific types of SDP service record attributes. 
For example, CSdpBoolean derives from CSdpAttrValue to encapsulate boolean 
attributes. The base class defines getters for all types: for example, it 
declares a function Bool() that returns the attribute value as a TBool. Note 
though that the function would panic unless called on actual CSdpBoolean.

A common base class allows attributes of all types to be used polymorphically. 
You can use CSdpAttrValue's Type() function to find the actual type being 
used, and upcast the pointer appropriately. */
	{
public:
	CSdpAttrValue();
	virtual ~CSdpAttrValue();


	/** Gets the attribute type.
	
	@return Attribute type */
	virtual TSdpElementType Type() const=0;


	/** Gets the size of the attribute.
	
	@return Size of the attribute in bytes */
	virtual TUint DataSize() const=0;


	// getter interface
	virtual TUint Uint() const;
	virtual TInt Int() const;
	virtual TBool DoesIntFit() const;
	virtual TInt Bool() const;
	virtual const TUUID &UUID() const;
	virtual const TPtrC8 Des() const;
	
	virtual void AcceptVisitorL(MSdpAttributeValueVisitor& aVisitor);

	IMPORT_C void Uint64(TUint64& aValue) const; 
 	IMPORT_C void Uint128(TUint64& aLo, TUint64& aHi) const;

protected:
	TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);
	};

/** Base class for classes that specify lists of attribute data elements.

It implements the MSdpElementBuilder interface to build data elements into 
an attribute value.

@see CSdpAttrValueDEA
@see CSdpAttrValueDES */
NONSHARABLE_CLASS(CSdpAttrValueList) : public CSdpAttrValue, public MSdpElementBuilder
	{
public:
	virtual ~CSdpAttrValueList();
	virtual void AcceptVisitorL(MSdpAttributeValueVisitor& aVisitor);
	virtual TUint DataSize() const;
	IMPORT_C void AppendValueL(CSdpAttrValue* aValue);

	// MSdpElementBuilder interface
	/** Adds an element of any type.
	
	@param aType Type descriptor
	@param aSizeDesc Size descriptor
	@param aData Data field
	@return Attribute value with added element */
	virtual MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	virtual MSdpElementBuilder* BuildNilL();
	virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	virtual MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	virtual MSdpElementBuilder* BuildBooleanL(TBool aBool);
	virtual MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	virtual MSdpElementBuilder* BuildDESL();
	virtual MSdpElementBuilder* BuildDEAL();
	virtual MSdpElementBuilder* StartListL();
	virtual MSdpElementBuilder* EndListL();
	virtual MSdpElementBuilder* BuildURLL(const TDesC8& aString);
	virtual MSdpElementBuilder* BuildEncodedL(const TDesC8& aString);

protected:
	CSdpAttrValueList(MSdpElementBuilder *aBuilder);
	void ConstructL();

protected:
	/** Array of attribute values contained in this CSdpAttrValueList class*/
	CArrayPtr<CSdpAttrValue> *iList;
	
private:
	MSdpElementBuilder *iParent;
	};


NONSHARABLE_CLASS(CSdpAttrValueNil) : public CSdpAttrValue
/** A null type data attribute. */
	{
public:
	IMPORT_C static CSdpAttrValueNil *NewNilL();
	virtual ~CSdpAttrValueNil();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	
private:
	CSdpAttrValueNil();
	};

/** Max size for an SDP Uint attribute value in bytes */
static const TInt KSdpMaxUintSize = 16;

NONSHARABLE_CLASS(CSdpAttrValueUint) : public CSdpAttrValue
/** A unsigned integer value of an attribute.

The value can be up to 128 bits in size. You need to choose to use either Uint() for up to 32 bit size,
Uint64() for up to 64 bit size or Uint128() for up to 128 bit size. The size can be found using DataSize().
If the incorrect function is used the code will panic. 
*/
	{
public:
	IMPORT_C static CSdpAttrValueUint* NewUintL(const TDesC8 &aUint);
	virtual ~CSdpAttrValueUint();

	IMPORT_C void SetUintValue(const TDesC8& aValue);
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	TUint Uint() const;
	virtual TBool DoesIntFit() const;
	virtual const TPtrC8 Des() const;

private:
	TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);
	CSdpAttrValueUint(const TDesC8 & aUint);
	
private:
	TBuf8<KSdpMaxUintSize> iUint;
	};

/** 
Max size for an SDP Int attribute value in bytes 
The new value is KSdpMaxIntSize
@deprecated
*/
static const TInt KMaxIntSize  = 16;

/** 
Max size for an SDP Int attribute value in bytes 
*/
static const TInt KSdpMaxIntSize  = 16;

NONSHARABLE_CLASS(CSdpAttrValueInt) : public CSdpAttrValue
/** A signed integer value of an attribute.

The value can be up to 128 bits in size. */
	{
public:
	IMPORT_C static CSdpAttrValueInt* NewIntL(const TDesC8 &aInt);
	virtual ~CSdpAttrValueInt();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	virtual TInt Int() const;
	virtual TBool DoesIntFit() const;
	virtual const TPtrC8 Des() const;
	
private:
	CSdpAttrValueInt(const TDesC8 & aInt);
	
private:
	TBuf8<KSdpMaxIntSize> iInt;
	};


NONSHARABLE_CLASS(CSdpAttrValueUUID) : public CSdpAttrValue
/** A UUID value of an attribute.

The value can be up to 128 bits in size. */
	{
public:
	IMPORT_C static CSdpAttrValueUUID* NewUUIDL(const TUUID& aUUID);
	virtual ~CSdpAttrValueUUID();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	// getters
	virtual const TUUID &UUID() const;
	virtual const TPtrC8 Des() const;
	
private:
	CSdpAttrValueUUID(const TUUID& aUUID);
	
private:
	TUUID iUUID;
	};

NONSHARABLE_CLASS(CSdpAttrValueString) : public CSdpAttrValue
/** A Text String value of an attribute.

The encoding of the string is up to the user. The interpretation of the encoding 
is enabled using facilities in SDP. */
	{
public:
	IMPORT_C static CSdpAttrValueString* NewStringL(const TDesC8& aString);
	virtual ~CSdpAttrValueString();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	virtual const TPtrC8 Des() const;
	
private:
	CSdpAttrValueString();
	void ConstructL(const TDesC8& aString);
	
private:
	HBufC8 *iBuffer;
	};

NONSHARABLE_CLASS(CSdpAttrValueBoolean) : public CSdpAttrValue
/** A Boolean value of an attribute. */
	{
public:
	IMPORT_C static CSdpAttrValueBoolean *NewBoolL(TBool aBool);
	virtual ~CSdpAttrValueBoolean();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	virtual TBool Bool() const;
	
private:
	CSdpAttrValueBoolean(TBool aBool);
	
private:
	TBool iBool;
	};

NONSHARABLE_CLASS(CSdpAttrValueDES) : public CSdpAttrValueList
/** A Data element sequence (DES) value of an attribute.

A DES can contain other values within it, including other sequences. This 
enables arbitrary tree structures to be created.

Most of the functionality is supplied by the base class CSdpAttrValueList. */
	{
public:
	IMPORT_C static CSdpAttrValueDES* NewDESL(MSdpElementBuilder* aBuilder);
	virtual TSdpElementType Type() const;
	
private:
	CSdpAttrValueDES(MSdpElementBuilder *aBuilder);
	};

NONSHARABLE_CLASS(CSdpAttrValueDEA) : public CSdpAttrValueList
/** A Data element alternative (DEA) value of an attribute: this is an attribute 
whose value is a sequence of data elements from which one data element is 
to be selected

Most of the functionality is supplied by the base class CSdpAttrValueList. */
	{
public:
	IMPORT_C static CSdpAttrValueDEA* NewDEAL(MSdpElementBuilder* aBuilder);
	virtual TSdpElementType Type() const;
	
private:
	CSdpAttrValueDEA(MSdpElementBuilder *aBuilder);
	};

NONSHARABLE_CLASS(CSdpAttrValueURL) : public CSdpAttrValue
/** A URL value of an attribute. */
	{
public:
	IMPORT_C static CSdpAttrValueURL* NewURLL(const TDesC8& aString);
	virtual ~CSdpAttrValueURL();
	virtual TSdpElementType Type() const;
	virtual const TPtrC8 Des() const;
	virtual TUint DataSize() const;
	
private:
	CSdpAttrValueURL();
	void ConstructL(const TDesC8& aString);
	
private:
	HBufC8 *iBuffer;
	};

class MSdpAttributeValueVisitor
/** Abstract interface that can be implemented to receive an enumeration of the 
values in an attribute list.

The class member functions are called by an CSdpAttrValue (or one of its sub-classes) 
object when CSdpAttrValue::AcceptVisitorL() is called.

Note that:

When AcceptVisitorL() is called on attributes that are not lists (not DEA 
or DES), only VisitAttributeValueL() is called, passing the attribute value 
object itself (i.e. *this), and the value attribute type.

When AcceptVisitorL() is called on attributes that are lists (DEA or DES), 
VisitAttributeValueL() is called for each member of the list

This implements the standard Visitor pattern.

@see CSdpAttrValue::AcceptVisitorL() */
	{
public:
	/** Called to pass an attribute value.
	
	@param aValue Attribute value
	@param aType Value type */
    virtual void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)=0;
	
	
	/** Called to indicate the start of a list of attribute values.
	
	This call is followed by a call to VisitAttributeValueL() for each attribute 
	value in the list, and concluded by a call to EndList().
	
	@param aList Attribute value list */
    virtual void StartListL(CSdpAttrValueList &aList)=0;
	
	
	/** Called to indicate the end of a list of attribute values. */
    virtual void EndListL()=0;
    
    /**
 	 Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	 @param aInterface UID of the interface to return
	 @param aObject the container for another interface as specified by aInterface
	 */
	IMPORT_C virtual void MSAVV_ExtensionInterfaceL(TUid aInterface, void*& aObject);    
	};

class SdpUtil
/**Integer Putter/Getter Utility Class 

Utility class for putting integer values into/getting integer values out of descriptors  */
	{
public:
	IMPORT_C static TUint GetUint(const TDesC8& aData);
	IMPORT_C static void PutUint(TUint8* aPtr, TInt64 aInt, TInt aNumberOfBytes);
	IMPORT_C static void PutUint64(TUint8* aPtr, const TUint64& aNumber);			
	IMPORT_C static void GetUint64(const TDesC8& aData, TUint64& aNumber);
	IMPORT_C static void PutUint128(TUint8* aPtr, const TUint64& aLo, const TUint64& aHi);	
	IMPORT_C static void GetUint128(const TDesC8& aData, TUint64& aLo, TUint64& aHi);	
	};
	
/**Integer to Descriptor Convertor

Utility class to convert EPOC integers into TDesC8's, in the correct
format for passing to the BuildIntL and BuildUintL members in SDP.
This is effectively a Big-endian variant of TPckgBuf.
*/


template<class T>
class TSdpIntBuf : public TBufC8<sizeof(T)>
	{
public:
	inline TSdpIntBuf(const T& aRef);
	};

/**Speciality of Integer to Descriptor Convertor for TUint64s*/
TEMPLATE_SPECIALIZATION class TSdpIntBuf<TUint64> :public TBufC8<sizeof(TUint64)>
	{
public:
	IMPORT_C TSdpIntBuf(const TUint64& aRef);
	};

/**Speciality of Integer to Descriptor Convertor for TUint32s*/
TEMPLATE_SPECIALIZATION class TSdpIntBuf<TUint32> : public TSdpIntBuf<TUint>
	{
public:
	IMPORT_C TSdpIntBuf(const TUint32& aRef);
	};

/**Speciality of Integer to Descriptor Convertor for TUint16s*/
TEMPLATE_SPECIALIZATION class TSdpIntBuf<TUint16> : public TSdpIntBuf<TUint>
	{
public:
	IMPORT_C TSdpIntBuf(const TUint16& aRef);
	};

/**Speciality of Integer to Descriptor Convertor for TUint8s*/
TEMPLATE_SPECIALIZATION class TSdpIntBuf<TUint8> : public TSdpIntBuf<TUint>
	{
public:
	IMPORT_C TSdpIntBuf(const TUint8& aRef);
	};



// This function has to be inline and not exported since otherwise
// the replacement of T for the class provided doesn't work.
template <class T>
inline TSdpIntBuf<T>::TSdpIntBuf(const T &aRef)
	{
	TBufC8<sizeof(T)>::DoSetLength(sizeof(T));
	SdpUtil::PutUint(TBufC8<sizeof(T)>::iBuf, TInt64(aRef), sizeof(T));
	}

//*************************************************************
//
//  SDP Agent client API
//
//*************************************************************

class MSdpAgentNotifier
/** Handles responses to Bluetooth Service Discovery Protocol queries.

Clients that make queries through CSdpAgent must implement this interface 
to handle the responses. 

@see CSdpAgent::AttributeRequestL() */
	{
public:
	/** Called when an service record request (CSdpAgent::NextRecordRequestComplete()) 
	operation completes.
	
	@param aError KErrNone if successful;
	              KErrEof if there are no more SDP records left to be read; or an SDP error.
	@param aHandle Service record for which the query was made
	@param aTotalRecordsCount Total number of matching records
	@see CSdpAgent::NextRecordRequestL() */
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)=0;

	/** Called when an attribute request (CSdpAgent::AttributeRequestL()) wants to 
	pass up a result.
	
	@param aHandle Service record for which the query was made
	@param aAttrID ID of the attribute obtained
	@param aAttrValue Attribute value obtained
	@see CSdpAgent::AttributeRequestL() */
	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)=0;

	/** Called when an attribute request (CSdpAgent::AttributeRequestL()) wants to 
	signal the completion of a attribute request.
	
	@param aHandle Service record for which the query was made
	@param aError an error
	@see CSdpAgent::AttributeRequestL() */
	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError)=0;

    /**
 	 Returns a null aObject if the extension is not implemented, or a pointer to another interface if it is.
	 @param aInterface UID of the interface to return
	 @param aObject the container for another interface as specified by aInterface
	 */
	IMPORT_C virtual void MSAN_ExtensionInterfaceL(TUid aInterface, void*& aObject);	
	};

class CSdpAgentEng;
class CSdpSearchPattern;
class CSdpAttrIdMatchList;

NONSHARABLE_CLASS(CSdpAgent) : public CBase
/** Makes Bluetooth service discovery protocol (SDP) requests to a remote device.

To make SDP requests, 

Construct a CSdpAgent object with the Bluetooth address of the remote device 
to query.

Set the classes of service that you want to query for. The classes have predefined 
UUIDs, which you specify with SetRecordFilterL(). 

Get results through NextRecordRequestL(). Information on services is stored 
as records, which can be queried in turn for attribute values for the service 
through AttributeRequestL().

A user of this class must implement MSdpAgentNotifier to receive the responses 
to queries.

Note that queries are asynchronous, although this is hidden in the API. The 
implication is that the interface functions that receive responses are only 
called when the thread's active scheduler can schedule handling of the completion 
of the query. 

@see MSdpAgentNotifier */
	{
public:
	IMPORT_C static CSdpAgent* NewL(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr);
	IMPORT_C static CSdpAgent* NewLC(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr);
	IMPORT_C ~CSdpAgent();
	IMPORT_C void SetRecordFilterL(const CSdpSearchPattern& aUUIDFilter);
	IMPORT_C void SetAttributePredictorListL(const CSdpAttrIdMatchList& aMatchList);
	IMPORT_C void NextRecordRequestL();

	IMPORT_C void AttributeRequestL(TSdpServRecordHandle aHandle, 
								   TSdpAttributeID aAttrID);
	IMPORT_C void AttributeRequestL(TSdpServRecordHandle aHandle, 
								   const CSdpAttrIdMatchList& aMatchList);
	IMPORT_C void AttributeRequestL(MSdpElementBuilder* aBuilder,
		                           TSdpServRecordHandle aHandle, 
								   TSdpAttributeID aAttrID);
	IMPORT_C void AttributeRequestL(MSdpElementBuilder* aBuilder,
		                           TSdpServRecordHandle aHandle, 
								   const CSdpAttrIdMatchList& aMatchList);
	IMPORT_C void Cancel();

private:
	CSdpAgent();
	void ConstructL(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr);


private:
	CSdpAgentEng* iAgentEngine;	
	};

NONSHARABLE_CLASS(CSdpSearchPattern) : public CBase, public MSdpElementBuilder
/** A list of Bluetooth service classes, represented as Universal Unique Identifiers 
(UUIDs), to be matched in SDP Service Search Requests.

@see CSdpAgent::SetRecordFilterL()
@see TUUID */
	{
public:
	IMPORT_C static CSdpSearchPattern* NewL();
	IMPORT_C void ConstructL();
	IMPORT_C ~CSdpSearchPattern();
	IMPORT_C TInt AddL(const TUUID& aUUID);
	IMPORT_C TInt Remove(const TUUID& aUUID);
	IMPORT_C TInt Find(const TUUID& aUUID, TInt &aPos) const;
	IMPORT_C TInt Count() const;
	IMPORT_C const TUUID At(TInt anIndex) const;
	IMPORT_C void Reset();
	/** Tests if the list is empty.
	
	@return True if the list is empty */
	IMPORT_C TBool IsEmpty();
	
private:
	CSdpSearchPattern();

private:
	// Implementation of Mbuilder interface
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	MSdpElementBuilder* BuildDESL();
	MSdpElementBuilder* StartListL();
	MSdpElementBuilder* EndListL();
	
private:
	CArrayFixFlat<TUUID>* iUUIDArray;
	};

struct TAttrRange
/** A range of attribute ID values.

This class is used in an attribute list, CSdpAttrIdMatchList, so that all 
attributes with IDs in the specified range are considered.

@see CSdpAttrIdMatchList */
	{
public:
	/** Default constructor. */
	IMPORT_C TAttrRange();
	
	
	/** Constructor with single ID.
	
	@param aAttrId The start and the end of the range are both set to aAttrId */
	IMPORT_C TAttrRange(TSdpAttributeID aAttrId);
	
	
	/** Constructor with start and end IDs.
	
	@param aStart ID for the start of the range
	@param aEnd ID for the end of the range */
	IMPORT_C TAttrRange(TSdpAttributeID aStart, TSdpAttributeID aEnd);
	
	
	/** Tests if the specified ID is either within the range.
	
	@param aAttrId ID to test
	@return True if in range, else false */
	IMPORT_C TBool IsInRange(TSdpAttributeID aAttrId) const;
	
	
	/** Tests if the specified ID is either within the range, is one less than the 
	lower bound, or one more than the upper bound.
	
	@param aAttrId ID to test
	@return True if contiguous, else false */
	IMPORT_C TBool IsContiguousWith(TSdpAttributeID aAttrId) const;
	
	
	/** Tests if the specified range is contiguous with the range.
	
	@param aRange Range to test
	@return True if contiguous, else false */
	IMPORT_C TBool IsContiguousWith(TAttrRange aRange) const;

public:
	/** ID of the start of the range */
	TSdpAttributeID iStart;
	/** ID of the end of the range */
	TSdpAttributeID iEnd;
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};


/** Greatest range possible for attribute IDs.
 
Useful for including all attributes in a CSdpAttrIdMatchList
@see CSdpAttrIdMatchList*/
#define KAttrRangeAll TAttrRange(0, KMaxTUint16)


class CSdpServRecord;
class TElementEncoder;
class MAttributeMatchHandler;

NONSHARABLE_CLASS(CSdpAttrIdMatchList) : public CBase, public MSdpElementBuilder
/** SDP Attribute ID Match list.

Holds a list of Attribute IDs to be retrieved from a remote device in an Service 
Attribute Request. Set in terms of ranges of attributes, each one specified 
through a TAttrRange. */
	{
public:
	IMPORT_C static CSdpAttrIdMatchList* NewL();
	IMPORT_C static CSdpAttrIdMatchList* NewL(const CSdpAttrIdMatchList& aAttrMatchList);
	IMPORT_C ~CSdpAttrIdMatchList();
	IMPORT_C void AddL(TAttrRange aRange);
	IMPORT_C void RemoveL(TAttrRange aRange);
	TInt Find(TSdpAttributeID aAttrId, TInt &aPos) const;
	
	// Do not use this - it gives an off-by-one error for aPos
	// unless the attribute is the first one in a contiguous range.
	// Use InMatchListRange() instead
	IMPORT_C TBool InMatchList(TSdpAttributeID aAttrId, TInt &aPos) const; 
	


	/** Tests if the specified attribute ID is in the list.
	
	@param aAttrId Attribute ID to test
	@return ETrue if the attribute is in the list, else EFalse */
	IMPORT_C TBool InMatchList(TSdpAttributeID aAttrId) const;


	/** Gets the number of separate ranges of attribute IDs in the list.
	
	@return Number of ranges of attribute IDs in the list */
	IMPORT_C TInt Count() const;

	void FindAttributesL(CSdpServRecord &aRec, MAttributeMatchHandler &aHandler) const;
	
	/**
	This method is for internal sub-system use only, it is not to be used otherwise.
	@internalTechnology
	@released
	*/
    IMPORT_C TUint EncodeL(TElementEncoder aEncoder) const;
    
    
	/** Tests if the specified attribute ID is in the list.
	
	@param aAttrId Attribute ID to test
	@param aRange Position of the range of IDs in the list which contains aAttrId, or 0 if not found
	@return ETrue if the attribute is in the list, else EFalse */
	IMPORT_C TBool InMatchListRange(TSdpAttributeID aAttrId, TInt &aRange) const; 
    

private:
	// Implementation of MSdpElementBuilder interface
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	MSdpElementBuilder* BuildDESL();
	MSdpElementBuilder* StartListL();
	MSdpElementBuilder* EndListL();
	TInt FindInOrContiguousWith(TSdpAttributeID aAttrId, TInt &aPos) const;

private:
	CSdpAttrIdMatchList();
	CArrayFix<TAttrRange>* iList;
	
	__DECLARE_TEST;
	};


// interface via NetDB
// includes parser and builder to use it


/** Cannot bind to specifed sockets protocol, as already bound. */
const static TInt KErrSdpAlreadyBound = -6400;
/** Remote device gave unknown error. */
const static TInt KErrSdpPeerError = -6401;
/** Local device is not connected. */
const static TInt KErrSdpClientNotConnected = -6402;
/** Invalid/unsupported SDP version. */
const static TInt KErrSdpUnsupportedVersion = -6403;
/** Invalid Service Record Handle. */
const static TInt KErrSdpBadRecordHandle = -6404;
/** Invalid Continuation State. */
const static TInt KErrSdpBadContinuationState = -6405;
/** SDP server rejected the request. */
const static TInt KErrSdpServerRejectedRequest = -6406;
/** Request buffer was ill-formed. */
const static TInt KErrSdpBadRequestBufferLength = -6407;
/** Result buffer was ill-formed. */
const static TInt KErrSdpBadResultBufferLength = -6408;
/** UUID entry was ill-formed. */
const static TInt KErrSdpBadUUIDLength = -6409;
/** Response was ill-formed. */
const static TInt KErrSdpBadResultData = -6410;
/** SDP database is full. */
const static TInt KErrSdpDatabaseFull = -6411;


template <class T> class CSdpStackFix;

NONSHARABLE_CLASS(CElementParser) : public CBase
/** Parser for SDP attribute values.

The class parses an input buffer containing an attribute value into its constituent 
data elements. It calls an MSdpElementBuilder object each time a data element 
is decoded.

This parser can parse incrementally, and be fed new data as it arrives.

@see MSdpElementBuilder */
	{
public:
NONSHARABLE_CLASS(CLinearBuf) : public CBase
/*Buffer used for a buffered parse.*/
	{
public:
	static CLinearBuf* NewLC(TInt aExpandSize);
	~CLinearBuf();
	void AppendL(const TDesC8& aData);
	const TPtrC8 Ptr() const;
	void Consume(TInt aLength);
	TInt Size() const;

private:
	CLinearBuf();
	void ConstructL(TInt aExpandSize);

	CBufFlat* iBuf;
	TInt iStartOffset;
	};

public:
	IMPORT_C static CElementParser* NewL(MSdpElementBuilder* aBuilder);
	IMPORT_C ~CElementParser();
	IMPORT_C TInt ParseElementsL(const TDesC8& aData);
	IMPORT_C TBool BufferedParseL(const TDesC8& aData);
	IMPORT_C void Reset();
	IMPORT_C void Reset(MSdpElementBuilder* aBuilder);
	IMPORT_C void SetBuilder(MSdpElementBuilder* aBuilder);
	IMPORT_C MSdpElementBuilder* Builder();

private:
	void ConstructL();
	CElementParser(MSdpElementBuilder* aBuilder);
	static TBool IsComposite(TUint8 aType);
	void CloseListsL();

	MSdpElementBuilder* iBuilder;
	CSdpStackFix<TInt>* iListStack;
	CLinearBuf* iRemainderBuf;
	};

_LIT(KSdpClientPanic,"SDPServer");
enum TSdpClientPanic
	{
	ESdpBadRequest = 0,
	ESdpBadDescriptor = 1,
	ESdpBadSubSessionHandle = 6,
	ESdpBadSubSessionRemove = 7,
	ESdpNonExistantRecordHandle = 12,
	};

#endif
