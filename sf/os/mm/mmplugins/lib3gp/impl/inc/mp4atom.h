// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _MP4ATOM_H
#define _MP4ATOM_H

#include <3gplibrary/mp4config.h>
#include <3gplibrary/mp4lib.h>
#include "mp4list.h"
#include "filewriter.h" /* Async buffering filewriter */
#include "metadatafilewriter.h" /* Async buffering filewriter */
#include "asyncfileparser.h" /*Async file reader for parsing */

#define ATOMTYPE_FTYP 0x66747970 /* 'ftyp' */
#define ATOMTYPE_MOOV 0x6d6f6f76 /* 'moov' */
#define ATOMTYPE_MDAT 0x6d646174 /* 'mdat' */
#define ATOMTYPE_FREE 0x66726565 /* 'free' */
#define ATOMTYPE_SKIP 0x736b6970 /* 'skip' */
#define ATOMTYPE_UDTA 0x75647461 /* 'udta' */
#define ATOMTYPE_MVHD 0x6d766864 /* 'mvhd' */
#define ATOMTYPE_IODS 0x696f6473 /* 'iods' */
#define ATOMTYPE_TRAK 0x7472616b /* 'trak' */
#define ATOMTYPE_TKHD 0x746b6864 /* 'tkhd' */
#define ATOMTYPE_MDIA 0x6d646961 /* 'mdia' */
#define ATOMTYPE_MDHD 0x6d646864 /* 'mdhd' */
#define ATOMTYPE_HDLR 0x68646c72 /* 'hdlr' */
#define ATOMTYPE_MINF 0x6d696e66 /* 'minf' */
#define ATOMTYPE_VMHD 0x766d6864 /* 'vmhd' */
#define ATOMTYPE_SMHD 0x736d6864 /* 'smhd' */
#define ATOMTYPE_DINF 0x64696e66 /* 'dinf' */
#define ATOMTYPE_DREF 0x64726566 /* 'dref' */
#define ATOMTYPE_TREF 0x74726566 /* 'tref' */
#define ATOMTYPE_STBL 0x7374626c /* 'stbl' */
#define ATOMTYPE_STTS 0x73747473 /* 'stts' */
#define ATOMTYPE_CTTS 0x63747473 /* 'ctts' */
#define ATOMTYPE_STSS 0x73747373 /* 'stss' */
#define ATOMTYPE_STSD 0x73747364 /* 'stsd' */
#define ATOMTYPE_STSZ 0x7374737a /* 'stsz' */
#define ATOMTYPE_STZ2 0x73747a32 /* 'stz2' */
#define ATOMTYPE_STSC 0x73747363 /* 'stsc' */
#define ATOMTYPE_STCO 0x7374636f /* 'stco' */
#define ATOMTYPE_CO64 0x636f3634 /* 'co64' */
#define ATOMTYPE_MP4V 0x6d703476 /* 'mp4v' */
#define ATOMTYPE_MP4A 0x6d703461 /* 'mp4a' */
#define ATOMTYPE_MP4S 0x6d703473 /* 'mp4s' */
#define ATOMTYPE_S263 0x73323633 /* 's263' */
#define ATOMTYPE_D263 0x64323633 /* 'd263' */
#define ATOMTYPE_BITR 0x62697472 /* 'bitr' */
#define ATOMTYPE_SAMR 0x73616d72 /* 'samr' */
#define ATOMTYPE_SAWB 0x73617762 /* 'sawb' */
#define ATOMTYPE_DAMR 0x64616d72 /* 'damr' */
#define ATOMTYPE_ESD  0x65736473 /* 'esds' */
#define ATOMTYPE_URL  0x75726c20 /* 'url ' */
#define ATOMTYPE_URN  0x75726e20 /* 'urn ' */
#define ATOMTYPE_UUID 0x75756964 /* 'uuid' */
#define ATOMTYPE_EDTS 0x65647473 /* 'edts' */
#define ATOMTYPE_META 0x6d657461 /* 'meta' */
/* AVC Additions */
#define ATOMTYPE_AVCC 0x61766343 /* 'avcC' */
#define ATOMTYPE_BTRT 0x62747274 /* 'btrt' */
#define ATOMTYPE_AVC1 0x61766331 /* 'avc1' */
#define ATOMTYPE_M4DS 0x6d346473 /* 'm4ds' */
#define ATOMTYPE_SDTP 0x73647470 /* 'sdtp' */
#define ATOMTYPE_AVCP 0x61766370 /* 'avcp' */
/* QCELP 13K Additions */
#define ATOMTYPE_SQCP 0x73716370 /* 'sqcp' */
#define ATOMTYPE_DQCP 0x64716370 /* 'dqcp' */
/* inline Addition */
#define ATOMTYPE_ID32 0x49443332 /* 'ID32' */

#define HANDLERTYPE_ODSM 0x6f64736d /* 'odsm' */
#define HANDLERTYPE_VIDE 0x76696465 /* 'vide' */
#define HANDLERTYPE_SOUN 0x736f756e /* 'soun' */



#define FTYP_SIZE    24     /* Size of FTYP box in bytes */

#define READBUFSIZE  8192   /* Size of file reading buffer */
#define WRITEBUFSIZE 2048   /* Size of file writing buffer */

#define BLOCK_LIMIT  200  /* Max number of metadata blocks to keep in memory */

#define MAXSAMPLESPERSECOND  150 /* magic number used to try validate input data to parser from possibly corrupted file */


#define METADATAFILE_VIDEO_STTS_SAMPLE_COUNT     0
#define METADATAFILE_VIDEO_STTS_SAMPLE_DELTA     1
#define METADATAFILE_VIDEO_STSZ_ENTRY_SIZE       2
#define METADATAFILE_VIDEO_STCO_CHUNK_OFFSET     3
#define METADATAFILE_VIDEO_STSS_SAMPLE_NUMBER    4
#define METADATAFILE_AUDIO_STTS_SAMPLE_COUNT     5
#define METADATAFILE_AUDIO_STTS_SAMPLE_DELTA     6
#define METADATAFILE_AUDIO_STSZ_ENTRY_SIZE       7
#define METADATAFILE_AUDIO_STCO_CHUNK_OFFSET     8
#define METADATAFILE_VIDEO_SDTP_SAMPLE_DEPENDENCY 9

/* Multiple Sample Entry Inclusion */
#define STSDMAXSAMPLEENTRYCOUNT 10   /* defined the maximum number of sample entried allowed in a Sample Description Box */


typedef struct atomHeader
{
  mp4_u32  size;
  mp4_u32  type;
  mp4_u64  largeSize;
  mp4_u8   extendedType[16];
  mp4_u8   version;
  mp4_u8   flags[3];
} atomHeader;


typedef struct fileTypeAtom
{
  atomHeader  *atomhdr;
  mp4_u32     majorBrand;
  mp4_u32     minorVersion;
  mp4_u32     *compatibleBrands;
} fileTypeAtom;


typedef struct movieHeaderAtom
{
  atomHeader      *atomhdr;
  mp4_u32         creationTime;
  mp4_u32         modificationTime;
  mp4_u32         timeScale;
  mp4_u32         duration;
  mp4_u32         nextTrackID;
  mp4_u64         creationTime64;
  mp4_u64         modificationTime64;
  mp4_u64         duration64;
} movieHeaderAtom;


typedef struct objectDescriptorAtom
{
  atomHeader      *atomhdr;
  mp4_u8          tag;
  mp4_u32         sizeOfInstance;
  mp4_u16         flags;
  mp4_u8          ODProfileLevelIndication;
  mp4_u8          SceneProfileLevelIndication;
  mp4_u8          AudioProfileLevelIndication;
  mp4_u8          VisualProfileLevelIndication;
  mp4_u8          GraphicsProfileLevelIndication;
} objectDescriptorAtom;


typedef struct trackHeaderAtom
{
  atomHeader      *atomhdr;
  mp4_u32         creationTime;
  mp4_u32         modificationTime;
  mp4_u32         trackID;
  mp4_u32         reserved;
  mp4_u32         duration;
  mp4_u64         creationTime64;
  mp4_u64         modificationTime64;
  mp4_u64         duration64;
  mp4_u16         width;
  mp4_u16         height;
  mp4_bool        isAudio;
  mp4_bool        isVisual;
} trackHeaderAtom;


typedef struct mediaHeaderAtom
{
  atomHeader  *atomhdr;
  mp4_u32     creationTime;
  mp4_u32     modificationTime;
  mp4_u32     timeScale;
  mp4_u32     duration;
  mp4_u64     creationTime64;
  mp4_u64     modificationTime64;
  mp4_u64     duration64;
} mediaHeaderAtom;


typedef struct handlerAtom
{
  atomHeader    *atomhdr;
  mp4_u32       handlerType;
  mp4_u8        *name;
} handlerAtom;


typedef struct dataEntryURLAtom
{
  atomHeader  *atomhdr;
} dataEntryURLAtom;


typedef struct dataEntryURNAtom
{
  atomHeader  *atomhdr;
} dataEntryURNAtom;


typedef struct dataReferenceAtom
{
  atomHeader        *atomhdr;
  mp4_u32           entryCount;
  dataEntryURLAtom  *url;
  dataEntryURNAtom  *urn;
} dataReferenceAtom;


typedef struct dataInformationAtom
{
  atomHeader         *atomhdr;
  dataReferenceAtom  *dref;
} dataInformationAtom;


typedef struct timeToSampleAtom
{
  atomHeader  *atomhdr;
  mp4_u32      entryCount;
  mp4_u32      *sampleCount;
  mp4_i32      *sampleDelta;
} timeToSampleAtom;


typedef struct compositionTimeToSampleAtom
{
  atomHeader      *atomhdr;
  mp4_u32         entryCount;
  mp4_u32         *sampleCount;
  mp4_u32         *sampleOffset;
} compositionTimeToSampleAtom;


typedef struct ESDAtom
{
  atomHeader  *atomhdr;
  mp4_u8      esDescrTag;
  mp4_u32     size;
  mp4_u16     ESID;
  mp4_u8      flags;
  mp4_u16     dependsOnESID;
  mp4_u8      URLLength;
  mp4_u8      *URLString;
  mp4_u16     OCRESID;
  mp4_u8      decConfDescrTag;
  mp4_u32     decConfDescrSize;
  mp4_u8      objectTypeIndication;
  mp4_u8      stream;
  mp4_u32     bufferSizeDB;
  mp4_u32     maxBitrate;
  mp4_u32     avgBitrate;
  mp4_u8      decSpecificInfoTag;
  mp4_u32     decSpecificInfoSize;
  mp4_u8      *decSpecificInfo;
} ESDAtom;


typedef struct visualSampleEntry
{
  atomHeader  *atomhdr;
  mp4_u16     dataReferenceIndex;
  mp4_u16     width;
  mp4_u16     height;
  ESDAtom     *esd;
} visualSampleEntry;


typedef struct audioSampleEntry
{
  atomHeader  *atomhdr;
  mp4_u16     dataReferenceIndex;
  mp4_u16     timeScale;
  ESDAtom     *esd;
} audioSampleEntry;


typedef struct mpegSampleEntry
{
  atomHeader  *atomhdr;
  mp4_u16     dataReferenceIndex;
  ESDAtom     *esd;
} mpegSampleEntry;


typedef struct bitrateAtom
{
  atomHeader  *atomhdr;
  mp4_u32     avgBitrate;
  mp4_u32     maxBitrate;
} bitrateAtom;


typedef struct h263SpecificAtom
{
  atomHeader  *atomhdr;
  mp4_u32     vendor;
  mp4_u8      decoderVersion;
  mp4_u8      h263Level;
  mp4_u8      h263Profile;
  bitrateAtom *bitr;
} h263SpecificAtom;


typedef struct h263SampleEntry
{
  atomHeader        *atomhdr;
  mp4_u16           dataReferenceIndex;
  mp4_u16           width;
  mp4_u16           height;
  h263SpecificAtom  *d263;
} h263SampleEntry;


typedef struct amrDecSpecStruc
{
  atomHeader  *atomhdr;
  mp4_u32     vendor;
  mp4_u8      decoderVersion;
  mp4_u16     modeSet;
  mp4_u8      modeChangePeriod;
  mp4_u8      framesPerSample;
} amrDecSpecStruc;


typedef struct amrSampleEntry
{
  atomHeader        *atomhdr;
  mp4_u16           dataReferenceIndex;
  mp4_u16           timeScale;
  amrDecSpecStruc   *damr;
} amrSampleEntry;

typedef struct avcConfigurationAtom
{
  atomHeader		*atomhdr;
  mp4_u8				*avcConfig;
  mp4_u32				avcConfigSize;
} avcConfigurationAtom;

typedef struct	mpeg4BitrateAtom
{
    atomHeader		*atomhdr;
	mp4_u32		bufferSizeDB;
	mp4_u32		maxBitRate;
	mp4_u32		avgBitrate;
} mpeg4BitrateAtom;

typedef struct mpeg4ExtensionDescriptorsAtom
{
    atomHeader		*atomhdr;
	mp4_u8		*descr;
	mp4_u32		 descrSize;
} mpeg4ExtensionDescriptorsAtom;

typedef struct avcSampleEntry
{
	atomHeader			*atomhdr;
    mp4_u16           dataReferenceIndex;
    mp4_u16           width;
    mp4_u16           height;
	avcConfigurationAtom	*avcc;
	mpeg4BitrateAtom			*btrt;
	mpeg4ExtensionDescriptorsAtom	*m4ds;
} avcSampleEntry;

typedef struct qcelpDecSpecStruc
{
  atomHeader  *atomhdr;
  mp4_u32     vendor;
  mp4_u8      decoderVersion;
  mp4_u8      framesPerSample;
} qcelpDecSpecStruc;

typedef struct qcelpSampleEntry
{
  atomHeader        *atomhdr;
  mp4_u16           dataReferenceIndex;
  mp4_u16           timeScale;
  qcelpDecSpecStruc   *dqcp;
} qcelpSampleEntry;

typedef struct avcParameterSampleEntry
{
	
} avcParameterSampleEntry;

typedef struct sampleDescriptionAtom
{
  atomHeader          *atomhdr;
  mp4_u32             entryCount;
  visualSampleEntry   *mp4v[STSDMAXSAMPLEENTRYCOUNT];
  audioSampleEntry    *mp4a[STSDMAXSAMPLEENTRYCOUNT];
  mpegSampleEntry     *mp4s[STSDMAXSAMPLEENTRYCOUNT];
  h263SampleEntry     *s263[STSDMAXSAMPLEENTRYCOUNT];
  amrSampleEntry      *samr[STSDMAXSAMPLEENTRYCOUNT];
  amrSampleEntry      *sawb[STSDMAXSAMPLEENTRYCOUNT];
  /* AVC addition */
  avcSampleEntry	*avc1[STSDMAXSAMPLEENTRYCOUNT];
  /* QCELP 13K addition */
  qcelpSampleEntry  *sqcp[STSDMAXSAMPLEENTRYCOUNT];
  avcParameterSampleEntry *avcp[STSDMAXSAMPLEENTRYCOUNT];
} sampleDescriptionAtom;


typedef struct sampleSizeAtom
{
  atomHeader  *atomhdr;
  mp4_u32     sampleSize;
  mp4_u32     sampleCount;
  mp4_u32     *entrySize;
} sampleSizeAtom;


typedef struct sampleToChunkAtom
{
  atomHeader  *atomhdr;
  mp4_u32     entryCount;
  mp4_u32     *firstChunk;
  mp4_u32     *samplesPerChunk;
  mp4_u32     *sampleDescriptionIndex;
} sampleToChunkAtom;


typedef struct chunkOffsetAtom
{
  atomHeader  *atomhdr;
  mp4_u32     entryCount;
  mp4_u32     *chunkOffset;
} chunkOffsetAtom;

typedef struct chunkOffset64Atom
{
  atomHeader  *atomhdr;
  mp4_u32     entryCount;
  mp4_u64     *chunkOffset;
} chunkOffset64Atom;


typedef struct syncSampleAtom
{
  atomHeader  *atomhdr;
  mp4_u32     entryCount;
  mp4_u32     *sampleNumber;
} syncSampleAtom;


typedef struct shadowSyncSampleAtom
{
  atomHeader  *atomhdr;
} shadowSyncSampleAtom;

typedef struct sampleDependency
{
	mp4_u8  sDependsOn;
	mp4_u8  sIsDependentOn;
	mp4_u8  sHasRedundancy;
} sampleDependency;

typedef struct sampleDependencyAtom
{
	atomHeader        *atomhdr;
	mp4_u32           sampleCount;
	sampleDependency  *dep;
} sampleDependencyAtom;

typedef struct sampleTableAtom
{
  atomHeader                    *atomhdr;
  timeToSampleAtom              *stts;
  compositionTimeToSampleAtom   *ctts;
  sampleDescriptionAtom         *stsd;
  sampleSizeAtom                *stsz;
  sampleToChunkAtom             *stsc;
  mp4_bool 						is32BitOffsets;
  union 
  {
  	chunkOffsetAtom             *stco;
  	chunkOffset64Atom           *stco64;
  };
  syncSampleAtom                *stss;
  shadowSyncSampleAtom          *stsh;
  sampleDependencyAtom          *sdtp;
} sampleTableAtom;


typedef struct videoMediaHeaderAtom
{
  atomHeader  *atomhdr;
} videoMediaHeaderAtom;


typedef struct soundMediaHeaderAtom
{
  atomHeader  *atomhdr;
} soundMediaHeaderAtom;


typedef struct mediaInformationAtom
{
  atomHeader            *atomhdr;
  videoMediaHeaderAtom  *vmhd;
  soundMediaHeaderAtom  *smhd;
  dataInformationAtom   *dinf;
  sampleTableAtom       *stbl;
} mediaInformationAtom;


typedef struct mediaAtom
{
  atomHeader            *atomhdr;
  mediaHeaderAtom       *mdhd;
  handlerAtom           *hdlr;
  mediaInformationAtom  *minf;
} mediaAtom;


typedef struct trackReferenceAtom
{
  atomHeader  *atomhdr;
} trackReferenceAtom;


typedef struct editListContainerAtom
{
  atomHeader  *atomhdr;
} editListContainerAtom;


typedef struct copyrightAtom
{
  void *p;
} copyrightAtom;

typedef struct userDataAtom
{
  atomHeader    *atomhdr;
  mp4_u64       atomcontentloc;   //used for parsing
  mp4_u32       atomcontentsize;  //used for parsing and composing
  mp4_u8        *contentdata;     //used for composing
} userDataAtom;

typedef struct trackAtom
{
  atomHeader             *atomhdr;
  trackHeaderAtom        *tkhd;
  trackReferenceAtom     *tref;
  editListContainerAtom  *edts;
  mediaAtom              *mdia;
  userDataAtom           *udta;
} trackAtom;

typedef struct ID32Atom
{
  atomHeader *atomhdr;
  mp4_u64    atomcontentloc;
  mp4_u16    language; // top bit is padding, remaining 15 bits is Packed ISO-639-2/T language code 
} ID32Atom;

typedef struct metaAtom
{
  atomHeader  *atomhdr;
  handlerAtom *hdlr;
  ID32Atom    *ID32;
} metaAtom;

typedef struct movieAtom
{
  atomHeader            *atomhdr;
  movieHeaderAtom       *mvhd;
  trackAtom             *trakAudio; // audio
  trackAtom             *trakVideo; // video
  objectDescriptorAtom  *iods;
  userDataAtom          *udta;
  metaAtom              *meta;
} movieAtom;


typedef struct mediaDataAtom
{
  atomHeader  *atomhdr;
  mp4_u8      *data;
} mediaDataAtom;


typedef struct sampleTable
{
  mp4_u32    currentChunk;

  /* Decoding time to sample */
  mp4_u32    sttsEntryCount;
  mp4_u32    *sttsSampleCount;
  mp4_u32    *sttsSampleDelta;
  mp4_u32    sttsMaxEntryCount;
  mp4_u32    sttsCurrentEntryCount;

  /* Sample size */
  mp4_u32    stszSampleSize;
  mp4_u32    stszSampleCount;
  mp4_u32    *stszEntrySize;
  mp4_u32    stszMaxSampleCount;
  mp4_u32    stszCurrentSampleCount;

  /* Sample to chunk */
  mp4_u32    stscEntryCount;
  mp4_u32    *stscFirstChunk;
  mp4_u32    *stscSamplesPerChunk;
  mp4_u32    *stscSampleDescriptionIndex;
  mp4_u32    stscMaxEntryCount;
  mp4_u32    stscCurrentEntryCount;

  /* Chunk offset */
  mp4_u32    stcoEntryCount;
  mp4_u64    *stcoChunkOffset;
  mp4_u32    stcoMaxEntryCount;
  mp4_u32    stcoCurrentEntryCount;
  mp4_bool   stcoNeed64Bits;
    
  /* Sync sample */
  mp4_u32    stssEntryCount;
  mp4_u32    *stssSampleNumber;
  mp4_u32    stssMaxEntryCount;
  mp4_u32    stssCurrentEntryCount;
  /* Sample Dependency */
  mp4_u32    sdtpEntryCount;
  mp4_u8    *sdtpSampleDependency;
  mp4_u32    sdtpMaxEntryCount;
  mp4_u32    sdtpCurrentEntryCount;
} sampleTable;


typedef struct MP4HandleStruct
{
  mp4_u8        sourceType;                /* Determines the type of the source file (CAF or RFile64) */
  void          *file;                     /* FILE * of possible file */
  void          *fs;                       /* File server */
  void          *rfile;                    /* File handle */
  ContentAccess::CData *cfile;             /* used when reading via the CAF and not the file based interfaces */
  mp4_i32		cafError;				   /* variable to store possible CAF error to be reported to client */
  void          *tmpfile;                  /* FILE * of temporary output file */
  void          *metaDataFile[NUM_MDF];    /* File handles for metadata files */
  MP4FileName   metaDataFileName[NUM_MDF]; /* Names of the temporary meta data files */
  MP4FileName   fileName;                  /* Name of the file */
  MP4FileName   tmpFileName;               /* Name of the temporary file */
  mp4_u64       bytesInTmpFile;            /* Number of bytes in temporary file */
  mp4_u64       mediaDataBytes;            /* Number of media data bytes */
  mp4_bool      ftypWritten;               /* Has FTYP been written */
  mp4_u32       flags;                     /* Composing flags */
  mp4_u32       type;                      /* Type of audio/video */
  list_s        *mem;                      /* Memory list */
  mp4_u32       position;                  /* Current position in memory */
  mp4_u32       absPosition;               /* Current absolute position in memory */
  mp4_u64       lastAccessedPosInFile;     /* Last accessed position in a file */
  mp4_u8        *buf;                      /* Temporary buffer space */
  mp4_u8        *diskReadBuf;              /* Memory for file buffering */
  mp4_u32       diskReadPos;               /* Reading position in the file */
  mp4_u64       diskReadBufStart;          /* Position of the start of the buffer */
  mp4_u32       diskReadBufPos;            /* Reading position inside buffer */
  mp4_u32       diskReadSize;              /* Number of bytes in buffer */
  mp4_u8        *diskWriteBuf;             /* Memory for file buffering */
  mp4_bool      ftypRead;                  /* FTYP atom has been read */
  fileTypeAtom  *ftyp;                     /* Pointer to file type atom structure */
  mp4_bool      metaDataComplete;          /* Metadata has been completeley read */
  movieAtom     *moov;                     /* Pointer to movie atom structure */
  mp4_u32       videoSampleNum;            /* Current video sample number */
  mp4_u64       videoFrameOffset;          /* Absolute position of current video frame */
  mp4_u32       videoFrameSize;            /* Size of current video frame in bytes */
  mp4_bool      videoLast;                 /* Has last video sample been read */
  mp4_u32       videoTimeScale;            /* Video timescale */
  mp4_u64       videoDuration;             /* Duration of video when composing */
  mp4_u16       videoWidth;                /* Video image width */
  mp4_u16       videoHeight;               /* Video image height */
  mp4_u32       videoMaxBitrate;           /* MPEG-4 video maximum bitrate */
  mp4_u32       videoAvgBitrate;           /* MPEG-4 video average bitrate */
  mp4_u8        *videoDecSpecificInfo;     /* MPEG-4 video DecoderSpecificInfo */
  mp4_u32       videoDecSpecificInfoSize;  /* MPEG-4 video DecoderSpecificInfo size in bytes */
  sampleTable   *videoSampleTable;         /* Video sample table */
  mp4_u8        videoLevel;                /* Video codec level, default is H.263 level 10 */
  mp4_u32       audioSampleNum;            /* Current audio sample number */
  mp4_u32       audioSampleSize;           /* Size of current audio sample in bytes */
  mp4_u64       audioSampleOffset;         /* Absolute position of current audio frame (in MP4 file) */
  mp4_u32       audioFrameCount;           /* Number of audio frames when composing */
  mp4_u32       audioTimeScale;            /* Audio timescale */
  mp4_u64       audioDuration;             /* Duration of audio when composing */
  mp4_bool      audioLast;                 /* Has last audio sample been read */
  mp4_u8        *audioDecSpecificInfo;     /* MPEG audio DecoderSpecificInfo */
  mp4_u32       audioDecSpecificInfoSize;  /* MPEG audio DecoderSpecificInfo size in bytes */
  sampleTable   *audioSampleTable;         /* Audio sample table */
  mp4_u8        audioFramesPerSample;      /* AMR frames per sample value */
  mp4_u16       audioModeSet;              /* AMR mode set */
  mp4_u32       metaDataBlocks;            /* Number of metadata blocks in memory */
  mp4_bool      metaDataOnDisk;            /* Is (a part of) metadata on the disk */
  mp4_u32       metaDataSize;              /* Size of metadata */
  mp4_u32		videoSampleEntryIndex;	   /* SampleEntry index of the video frame being read */
  mp4_u32		audioSampleEntryIndex;     /* SampleEntry index of the audio frame being read */
  mp4_u8        avcNalUnitLengthSize;      /* byte size of the length field */
  mp4_bool      generate3G2;               /* 3GPP2 File Format Generation is Selected: default:FALSE (Generate 3GP)*/
  mp4_bool      generateMP4;               /* MPEG-4 File Format Generation is Selected: defailt:FALSE (Generate 3GP)*/
  mp4_bool      qcelpStoredAsMPEGAudio;    /* QCELP 13K is registered as MPEG4 Audio: default: FALSE (in QCELPSampleEntry)*/
  mp4_u64       audioMediaDataSize;        /* Total byte size of the written audio data. This is used to calculate the max and avg bitrates */
  mp4_u32       ES_DescriptorSize;         /* size of the ES_Descriptor inside the ESDS box */ 

  // composeToBuffer:
  mp4_u8 *composeBuffer;
  mp4_bool bufferWrite;
  mp4_u32 bytesProgressed;
  mp4_u32 ftypdelta;
  mp4_u32 *composedSize;

  CFileWriter*  filewriter;                /* Async buffering filewriter */
  CMetaDataFileWriter* metadatafilewriter;  /* Async buffering metadata temp file writer */

  mp4_bool      LastWriteDataCalled;       /* When TRUE, it indicates that ParseWriteData() function is called for the last time */ 
  mp4_bool      FileHandleFromOutside;     /* When TRUE file handle is passed to library instead of file name, don't close file in end */

  mp4_bool      metaDataFileEmpty[NUM_MDF]; /* when TRUE it indicates that metadata file has been read and rest of data is in readbuffer of metadatafilewriter */
  
  userDataAtom* moovUDTA;   // used to compose MOOV UDTA (User Data) 
  userDataAtom* audioUDTA;  // used to compose AudioTrack UDTA (User Data) 
  userDataAtom* videoUDTA;  // used to compose VideoTrack UDTA (User Data)
  
  TDriveNumber   fileHandleDrive;  /* Used when we get file handle to library from outside. Used to indicate which drive are metadata temp files written.*/
  
  // Custom buffer size additions:
  mp4_u32 mediaWriteBufferSize;
  mp4_u32 metaWriteBufferSize;
  mp4_u32 writeBufferMaxCount;
  mp4_u32 readBufferSize;
  
  // Async parsing observer
  M3GPMP4LibAsyncObserver* asyncObserver;
  CFileAsyncParser* asyncReader;
  M3GPMP4LibAsyncTempFileRemoverObserver* tempFileRemoverObserver;
  
  void *file32Duplicate; //this is a RFile64 duplicate of the RFile used in Open 
} MP4HandleStruct;


typedef MP4HandleStruct *MP4HandleImp;


mp4_i32 metaDataAvailable(MP4HandleImp handle);
mp4_i32 readFTYP(MP4HandleImp handle, fileTypeAtom *ftyp);
mp4_i32 readMetaData(MP4HandleImp handle);
mp4_i32 readMoov(MP4HandleImp handle, movieAtom *moov);
mp4_i32 readAtomHeader(MP4HandleImp handle, atomHeader *ah);
mp4_i32 readFullAtomHeader(MP4HandleImp handle, atomHeader *ah);
mp4_i32 readMVHD(MP4HandleImp handle, movieHeaderAtom *mvhd);
mp4_i32 readIODS(MP4HandleImp handle, objectDescriptorAtom *iods);
mp4_i32 readTRAK(MP4HandleImp handle, trackAtom *trak);
mp4_i32 readUnknown(MP4HandleImp handle);
mp4_i32 readTKHD(MP4HandleImp handle, trackHeaderAtom *tkhd);
mp4_i32 readTREF(MP4HandleImp handle, trackReferenceAtom *tref);
mp4_i32 readEDTS(MP4HandleImp handle, editListContainerAtom *edts);
mp4_i32 readMDIA(MP4HandleImp handle, mediaAtom *mdia);
mp4_i32 readMDHD(MP4HandleImp handle, mediaHeaderAtom *mdhd);
mp4_i32 readHDLR(MP4HandleImp handle, handlerAtom *hdlr);
mp4_i32 readMINF(MP4HandleImp handle, mediaInformationAtom *minf);
mp4_i32 readVMHD(MP4HandleImp handle, videoMediaHeaderAtom *vmhd);
mp4_i32 readSMHD(MP4HandleImp handle, soundMediaHeaderAtom *smhd);
mp4_i32 readDINF(MP4HandleImp handle, dataInformationAtom *dinf);
mp4_i32 readDREF(MP4HandleImp handle, dataReferenceAtom *dref);
mp4_i32 readURL(MP4HandleImp handle, dataEntryURLAtom *url);
mp4_i32 readURN(MP4HandleImp handle, dataEntryURNAtom *urn);
mp4_i32 readSTBL(MP4HandleImp handle, sampleTableAtom *stbl);
mp4_i32 readSTTS(MP4HandleImp handle, timeToSampleAtom *stts);
mp4_i32 readCTTS(MP4HandleImp handle, compositionTimeToSampleAtom *ctts);
mp4_i32 readSTSS(MP4HandleImp handle, syncSampleAtom *stss);
mp4_i32 readSTSD(MP4HandleImp handle, sampleDescriptionAtom *stsd);
mp4_i32 readSTSZ(MP4HandleImp handle, sampleSizeAtom *stsz);
mp4_i32 readSTZ2(MP4HandleImp handle, sampleSizeAtom *stsz);
mp4_i32 readSTSC(MP4HandleImp handle, sampleToChunkAtom *stsc);
mp4_i32 readSTCO(MP4HandleImp handle, chunkOffsetAtom *stco);
mp4_i32 readCO64(MP4HandleImp handle, chunkOffset64Atom *stco64);
mp4_i32 readMP4V(MP4HandleImp handle, visualSampleEntry *mp4v);
mp4_i32 readMP4A(MP4HandleImp handle, audioSampleEntry *mp4a);
mp4_i32 readMP4S(MP4HandleImp handle, mpegSampleEntry *mp4s);
mp4_i32 readS263(MP4HandleImp handle, h263SampleEntry *s263);
mp4_i32 readSAMR(MP4HandleImp handle, amrSampleEntry *samr);
mp4_i32 readSAWB(MP4HandleImp handle, amrSampleEntry *sawb);
mp4_i32 readESD(MP4HandleImp handle, ESDAtom *esd);
mp4_i32 readD263(MP4HandleImp handle, h263SpecificAtom *d263);
mp4_i32 readBITR(MP4HandleImp handle, bitrateAtom *bitr);
mp4_i32 readDAMR(MP4HandleImp handle, amrDecSpecStruc *damr);
mp4_i32 readMeta(MP4HandleImp handle, metaAtom *meta);
/* avc inclusions*/
mp4_i32 readAVC1(MP4HandleImp handle, avcSampleEntry *avc1);
mp4_i32 readSDTP(MP4HandleImp handle, sampleDependencyAtom *sdtp,  mp4_i32 sample_count);
/* QCELP 13k inclusions */
mp4_i32 readSQCP(MP4HandleImp handle, qcelpSampleEntry *sqcp);
mp4_i32 readDQCP(MP4HandleImp handle, qcelpDecSpecStruc *dqcp);
/* inline related */
mp4_i32 readID32(MP4HandleImp handle, ID32Atom *ID32);

mp4_i32 freeFTYP(fileTypeAtom *ftyp);
mp4_i32 freeMOOV(movieAtom *moov);
mp4_i32 freeAtomHeader(atomHeader *atomhdr);
mp4_i32 freeMVHD(movieHeaderAtom *mvhd);
mp4_i32 freeTRAK(trackAtom *trak);
mp4_i32 freeTKHD(trackHeaderAtom *tkhd);
mp4_i32 freeTREF(trackReferenceAtom *tref);
mp4_i32 freeEDTS(editListContainerAtom *edts);
mp4_i32 freeMDIA(mediaAtom *mdia);
mp4_i32 freeMDHD(mediaHeaderAtom *mdhd);
mp4_i32 freeHDLR(handlerAtom *hdlr);
mp4_i32 freeMINF(mediaInformationAtom *minf);
mp4_i32 freeVMHD(videoMediaHeaderAtom *vmhd);
mp4_i32 freeSMHD(soundMediaHeaderAtom *smhd);
mp4_i32 freeDINF(dataInformationAtom *dinf);
mp4_i32 freeDREF(dataReferenceAtom *dref);
mp4_i32 freeURL(dataEntryURLAtom *url);
mp4_i32 freeURN(dataEntryURNAtom *urn);
mp4_i32 freeSTBL(sampleTableAtom *stbl);
mp4_i32 freeSTTS(timeToSampleAtom *stts);
mp4_i32 freeCTTS(compositionTimeToSampleAtom *ctts);
mp4_i32 freeSTSD(sampleDescriptionAtom *stsd);
mp4_i32 freeMP4V(visualSampleEntry *mp4v);
mp4_i32 freeESD(ESDAtom *esd);
mp4_i32 freeMP4A(audioSampleEntry *mp4a);
mp4_i32 freeMP4S(mpegSampleEntry *mp4s);
mp4_i32 freeS263(h263SampleEntry *s263);
mp4_i32 freeD263(h263SpecificAtom *d263);
mp4_i32 freeBITR(bitrateAtom *bitr);
mp4_i32 freeSAMR(amrSampleEntry *samr);
mp4_i32 freeSAWB(amrSampleEntry *sawb);
mp4_i32 freeDAMR(amrDecSpecStruc *damr);
mp4_i32 freeSTSZ(sampleSizeAtom *stsz);
mp4_i32 freeSTSC(sampleToChunkAtom *stsc);
mp4_i32 freeSTCO(chunkOffsetAtom *stco);
mp4_i32 freeSTCO64(chunkOffset64Atom *stco64);
mp4_i32 freeSTSS(syncSampleAtom *stss);
mp4_i32 freeSTSH(shadowSyncSampleAtom *stsh);
mp4_i32 freeIODS(objectDescriptorAtom *iods);
mp4_i32 readUDTA(MP4HandleImp handle, userDataAtom *udta);
mp4_i32 freeUDTA(userDataAtom *udta);
mp4_i32 freeSDTP(sampleDependencyAtom* sdtp);
mp4_i32 freeMETA(metaAtom *meta);
/* The following and AVC related */
mp4_i32 freeAVC1(avcSampleEntry *avc1);
mp4_i32 freeM4DS(mpeg4ExtensionDescriptorsAtom *m4ds);
mp4_i32 freeBTRT(mpeg4BitrateAtom *btrt);
mp4_i32 freeAVCC(avcConfigurationAtom *avcc);
/* QCELP 13K related */
mp4_i32 freeSQCP(qcelpSampleEntry *sqcp);
mp4_i32 freeDQCP(qcelpDecSpecStruc *dqcp);
/* inline related */
mp4_i32 freeID32(ID32Atom *ID32);

mp4_i32 determineVideoLength(MP4HandleImp handle, mp4_u32 *videolength);
mp4_i32 determineFrameRate(MP4HandleImp handle, mp4_double *framerate);
mp4_i32 determineVideoType(MP4HandleImp handle, mp4_u32 *videotype);
mp4_i32 determineVideoResolution(MP4HandleImp handle, mp4_u32 *videowidth, mp4_u32 *videoheight);
mp4_i32 determineVideoTimeScale(MP4HandleImp handle, mp4_u32 *timescale);
mp4_i32 determineAudioLength(MP4HandleImp handle, mp4_u32 *audiolength);
mp4_i32 determineAudioType(MP4HandleImp handle, mp4_u32 *audiotype);
mp4_i32 determineAudioFramesPerSample(MP4HandleImp handle, mp4_u8 *framespersample);
mp4_i32 determineAudioTimeScale(MP4HandleImp handle, mp4_u32 *timescale);
mp4_i32 determineAudioAverageBitRate(MP4HandleImp handle, mp4_u32 *averagebitrate);
mp4_i32 determineStreamSize(MP4HandleImp handle, mp4_u32 *streamsize);
mp4_i32 determineStreamAverageBitRate(MP4HandleImp handle, mp4_u32 *streamaveragebitrate, mp4_u32 streamsize);
mp4_i32 advanceVideoFrame(MP4HandleImp handle, trackAtom *trak);
mp4_i32 resolveVideoSampleOffset(MP4HandleImp handle, sampleTableAtom *stbl);
mp4_i32 resolveVideoSampleSize(MP4HandleImp handle, sampleSizeAtom *stsz);
mp4_i32 fetchVideoFrame(MP4HandleImp handle, trackAtom *trak, mp4_u8 *buffer, mp4_u32 buffersize, mp4_u32 *framesize, mp4_u32 *framenumber, mp4_bool *keyframe, mp4_u32 *timestamp2);
mp4_i32 isVideoFrameKeyFrame(MP4HandleImp handle, trackAtom *trak, mp4_bool *keyframe);
mp4_i32 convertVideoSampleToTime(MP4HandleImp handle, mediaAtom *mdia, mp4_u32 *framenumber, mp4_u32 *timestamp2);
mp4_i32 advanceAudioSample(MP4HandleImp handle, trackAtom *trak);
mp4_i32 resolveAudioSampleOffset(MP4HandleImp handle, sampleTableAtom *stbl);
mp4_i32 resolveAudioSampleSize(MP4HandleImp handle, sampleSizeAtom *stsz);
mp4_i32 fetchAudioSample(MP4HandleImp handle, trackAtom *trak, mp4_u8 *buffer, mp4_u32 buffersize, mp4_u32 *framesize, mp4_u32 *timestamp, mp4_u32 *returnedframes, mp4_u32 *timestamp2);
mp4_i32 convertAudioSampleToTime(MP4HandleImp handle, mediaAtom *mdia, mp4_u32 *timestamp, mp4_u32 *timestamp2);
mp4_i32 convertTimeToSample(MP4HandleImp handle, trackAtom *trak, mp4_u32 position, mp4_u32 *sample);
mp4_i32 goToVideoSample(MP4HandleImp handle, trackAtom *trak, mp4_u32 sample);
mp4_i32 goToAudioSample(MP4HandleImp handle, trackAtom *trak, mp4_u32 sample);
mp4_i32 findVideoKeyFrame(MP4HandleImp handle, trackAtom *trak, mp4_u32 sample, mp4_u32 *newsample);
mp4_i32 fetchAudioSampleAsync(MP4HandleImp handle, trackAtom *trak, mp4_u8 *buffer, mp4_u32* buffersize);
mp4_i32 fetchVideoFrameAsync(MP4HandleImp handle, trackAtom *trak, mp4_u8 *buffer, mp4_u32* buffersize);
mp4_i64 getChunkOffset(sampleTableAtom *stbl, mp4_u32 index); 
#endif

 
// End of File
