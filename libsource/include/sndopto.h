/*************************************************************************
Name:            MESSAGE.H

Description:

This include file defines what a message header looks like and various
message passing routines
**************************************************************************/
/*
 * System maximums
 */
#define MAX_PROCESSES       32
#define MAX_NODES           64
#define MAX_SYSTEM_MESSAGE  255
#define UNKNOWN_PROCESS     MAX_PROCESSES
#define STRING_LENGTH       81
#define MAX_NAME_LENGTH 40

/*
 * System flag defines for message.SystemFlags
 */
#define ALLOCATED_MESSAGE       1      /* If AllocMessage used */
#define EXTERNAL_MESSAGE        2
#define MULTIPLE_DATA_MESSAGE   8       /* If there are multiple data ptr's */
#define TRANSPUTER_BOOTER "TransputerBooter"

/*
 * The following structure holds all of the routing information for a
 * process located on some processor
 */
typedef struct AddressStruct
    {
    unsigned char
                node,               /* Processor node which message should go to */
                dummy,              /* Not currently used */
                process,            /* Process within node */
                dummy1;             /* Not currently used */
    }Address;

/*
 * The following is the header passed with every message.
 */
typedef struct MessageHeaderStruct
    {
    struct AddressStruct
        dest,          /* Process to which message is routed */
        source;        /* Process which message originated from */

    long int
                flags,              /* Any flags for this message */
                MessageId,         /* Identifies type of message being passed */
                MessageSize;       /* Size of data in message. Always multiple of 4 */
    } MessageHeader;

#define MESSAGE_HEADER_SIZE     (5*4)

/*
 * The following is the message.
 */
typedef struct MessageStruct
    {
    struct MessageHeaderStruct
                header;             /* Header for data being passed */
    unsigned char
                *data;              /* Actual data being passed */
    unsigned long
                *size;              /* Sizes of data buffers if more than one */
    unsigned
                SystemFlags;       /* Sets parameters for this message */
    }Message;



/*
 * Definitions for the various types of messages passed around the system
 */
struct ProcessSetupStruct
    {
    long int
        distance;           /* Distance message has travelled */
    unsigned char           /* Name of process */
        name[MAX_NAME_LENGTH];
    struct AddressStruct
        address;
    };

struct NodeSetupStruct
    {
    long int
        distance,           /* Distance message has travelled */
        NodeNumber,        /* Node number for the new node being added to system */
        SendSetupInfo;    /* If TRUE then send setup information */
    unsigned char           /* Name of new node */
        name[MAX_NAME_LENGTH];
    };

CPLUSPLUS_START
extern  void FreeMessage(struct MessageStruct *message);
CPLUSPLUS_END
/***************************************************************
Name: GENERAL    -General section describing OPTOTRAK interface

Note when passing messages to the OPTOTRAK, ASCII message types end in
COMMAND, while normal C type structures end in MESSAGE.
****************************************************************/
/*
 * Constants defining limits on certain objects in the OPTOTRAK system.
 */
#define MAX_SENSORS                     10
#define MAX_OPTOSCOPES                   4
#define MAX_RIGID_BODIES                10
#define MAX_SENSOR_NAME_LENGTH          20
#define OPTOTRAK_MAX_MARKERS            256

/*
 * Defines for different commands shared between OPTOTRAK and OPTOSCOPE
 */
#define OPTO_ERROR_REQUEST_MESSAGE      1000
#define OPTO_ERROR_REQUEST_COMMAND      1001

#define OPTO_STATUS_REQUEST_MESSAGE     1002

#define OPTO_TX_ONE_FRAME_MESSAGE       1004

#define OPTO_TX_MANY_FRAME_MESSAGE      1006

#define OPTO_LATEST_FRAME_MESSAGE       1010

#define OPTO_ERROR_MESSAGE              1050
#define OPTO_ERROR_COMMAND              1051

#define OPTOTRAK_REGISTER_MESSAGE       1102
#define OPTOTRAK_REGISTER_COMMAND       1103
struct OptotrakRegisterStruct
    {
    long int    monitor;                /* TRUE if this is a simple monitor */
                                        /* and not a controlling application */
    long int    AsciiResponse;         /* TRUE if responses to application */
    };                                  /* should be in ascii (not binary) */


/*
 * The following are message ids you could receive from either the
 * OPTOTRAK or the OPTOSCOPE
 */
#define OPTO_SUCCESSFUL_MESSAGE         1052
#define OPTO_SUCCESSFUL_COMMAND         1053

#define OPTO_UNSUCCESSFUL_MESSAGE       1054
#define OPTO_UNSUCCESSFUL_COMMAND       1055
struct OptoUnsuccessfulStruct
    {
    long int        ErrorCode;          /* Error code if not successful */
    unsigned char   message[ 80];        /* String describing the error */
    };


#define OPTO_DATA_BUFFER_MESSAGE        1056
#define OPTO_LATEST_BUFFER_MESSAGE      1058
#define OPTO_DATA_HEADER_SIZE             24
struct OptoDataHeaderStruct
    {
    long int DataId;            /* Identifier sent with each data packet. */
    long int StartFrameNumber; /* Collection frame number for first frame in message */
    long int NumberOfFrames;   /* Number of frames in this message */
    long int NumberOfElements; /* Number of elements in each frame */
    long int FramesLeft;        /* Number of frames left to retrieve for collection */
    long int flags;              /* Flags indicating status of buffer etc. */
    };

/*
 * These are flags and constants used by OPTOTRAK and OPTOSCOPE for any of
 * the buffer messages
 */
#define OPTO_BUFFER_OVERRUN_FLAG        0x0001
#define OPTO_FRAME_OVERRUN_FLAG         0x0002
#define OPTO_NO_PRE_FRAMES_FLAG         0x0004
/***************************************************************
Name: DATAPROP   -Details general interface to the data proprietor
****************************************************************/

/*
 * Messages specific to the OPTOTRAK data proprietor
 */
#define OPTO_LATEST_RAW_FRAME_MESSAGE   1012
#define DATAPROP_SEND_DATA_MESSAGE              2403
#define OPTO_LATEST_WAVE_FRAME_MESSAGE  1018

struct optotrak_wave_head_struct
    {
    float       centroid;        /* Calculated centroid */
    char        peak_value;      /* Peak value at peak pixel */
    char        gain;            /* Gain setting for this collection */
    char        error;           /* Error in centtroid calculation */
    char        dummy;
    char        peak_offset[ 2]; /* Pixel Number of peak     */
    char        start_pixel[ 2]; /* Pixel number of first pixel in wave */
    };

#define OPTO_TRANSFORM_DATA_MESSAGE     1016
#define OPTO_TRANSFORM_DATA_SIZE           8
struct OptoTransformDataStruct
    {
    long int markers;       /* Total # of markers to convert */
    long int FullDataInfo;  /* Data contains peak info as well */
    };


/*
 * Data ids you can receive back from the OPTOTRAK
 */
#define OPTO_TRANS_BUFFER_MESSAGE       1062
/***************************************************************
Name: ODAU       -Section detailing interfacing with the ODAU unit
****************************************************************/
#define OPTOSCOPE_MAX_CHANNELS_IN       16
#define OPTOSCOPE_MIN_CHANNEL_NUM        1
#define OPTOSCOPE_MAX_TABLE_ENTRIES     OPTOSCOPE_MAX_CHANNELS_IN + 1

#define ODAU_DIGITAL_PORT_OFF     0
#define ODAU_DIGITAL_INPB_INPA    1
#define ODAU_DIGITAL_OUTPB_INPA   2
#define ODAU_DIGITAL_OUTPB_OUTPA  3

#define OPTOSCOPE_SETUP_COLL_MESSAGE        1202
#define OPTOSCOPE_SETUP_COLL_COMMAND        1203
struct OptoscopeSetupCollStruct
    {
    long int    DataId;
    long int    FrameFrequency;
    long int    AnalogChannels;
    long int    AnalogGain;
    long int    DigitalMode;
    float       CollectionTime;
    float       PreTriggerTime;
    long int    StreamData;
    };

/*
 * The following is a global structure is passed from the OPTOSCOPE the the
 * OPTOTRAK so that it may record the required collection information.
 */
struct ScopeCollectionParmsStruct
    {
    long int    DataId;
    long int    frequency;
    long int    PacketSize;
    };

#define OPTOSCOPE_START_BUFF_MESSAGE        1206
#define OPTOSCOPE_START_BUFF_COMMAND        1207

#define OPTOSCOPE_STOP_BUFF_MESSAGE         1208
#define OPTOSCOPE_STOP_BUFF_COMMAND         1209

#define OPTOSCOPE_SHUTDOWN_MESSAGE          1212
#define OPTOSCOPE_SHUTDOWN_COMMAND          1213

struct OptoscopeSetChnlAllStruct
    {
    long int    status;
    long int    gain;
    };

struct OptoscopeSetChnlSglStruct
    {
    long int    status;
    long int    gain;
    long int    channel;
    };

struct OptoscopeSetGainAllStruct
    {
    long int    gain;
    };

struct OptoscopeSetGainSglStruct
    {
    long int    gain;
    long int    channel;
    };

#define OPTOSCOPE_STATUS_MESSAGE        1250
struct OptoscopeStatusStruct
    {
    long int    DataId;
    long int    FrameFrequency;
    long int    AnalogGain;
    long int    AnalogChannels;
    long int    DigitalMode;
    float       CollectionTime;
    float       PreTriggerTime;
    long int    StreamData;
    long int    ErrorFlags;
    };
/***************************************************************
Name: REALTIME   -Details advanced data properitor calls for realtime option
****************************************************************/
/*
 * These are flags which are used with rigid bodies and the determination
 * of their transformations.
 */
#define OPTOTRAK_UNDETERMINED_FLAG      0x0001
#define OPTOTRAK_STATIC_XFRM_FLAG       0x0002
#define OPTOTRAK_STATIC_RIGID_FLAG      0x0004
#define OPTOTRAK_CONSTANT_RIGID_FLAG    0x0008
#define OPTOTRAK_NO_RIGID_CALCS_FLAG    0x0010
#define OPTOTRAK_DO_RIGID_CALCS_FLAG    0x0020
#define OPTOTRAK_QUATERN_RIGID_FLAG     0x0040
#define OPTOTRAK_ITERATIVE_RIGID_FLAG   0x0080
#define OPTOTRAK_SET_QUATERN_ERROR_FLAG 0x0100
#define OPTOTRAK_SET_MIN_MARKERS_FLAG   0x0200
#define OPTOTRAK_RETURN_QUATERN_FLAG    0x1000
#define OPTOTRAK_RETURN_MATRIX_FLAG     0x2000
#define OPTOTRAK_RETURN_EULER_FLAG      0x4000
/*
 * Messages specific to the OPTOTRAK data proprietor
 */
#define OPTO_LATEST_RIGID_FRAME_MESSAGE 1014

#define OPTOTRAK_ADD_RIGID_MESSAGE      1122
struct OptotrakAddRigidStruct
    {
    long int lnRigidId;         /* Unique identifier for rigid body. */
    long int lnStartMarker;     /* Start marker for rigid body. */
    long int lnNumberOfMarkers; /* Number of markers in rigid body. */
    long int lnMinimumMarkers;  /* Minimum number of markers for calcs. */
    float    fMax3dError;       /* Max allowable quaternion error. */
    long int lnFlags;           /* Flags for this rigid body. */
    };

#define OPTOTRAK_DEL_RIGID_MESSAGE      1124
struct OptotrakDelRigidStruct
    {
    long int lnRigidId;     /* Unique identifier for rigid body. */
    };

#define OPTOTRAK_SET_RIGID_MESSAGE      1126
struct OptotrakSetRigidStruct
    {
    long int lnRigidId;            /* Unique identifier for rigid body. */
    long int lnMarkerAngle;
    long int lnMinimumMarkers;     /* Minimum number of markers for calcs. */
    float    fMax3dError;
    float    fMaxSensorError;
    float    fMax3dRmsError;
    float    fMaxSensorRmsError;
    long int lnFlags;              /* New status flags for this rigid body. */
    };

#define OPTOTRAK_ROTATE_RIGIDS_MESSAGE  1128
struct OptotrakRotateRigidsStruct
    {
    long int        lnRotationMethod; /* Flags to control xfrm rotations. */
    long int        lnRigidId;        /* Rigid body to base xfrm rotations on. */
    transformation  dtEulerXfrm;      /* XFRM to base xfrm rotations on. */
    };

#define OPTOTRAK_STOP_ROTATING_MESSAGE  1130

#define OPTOTRAK_ADD_NORMALS_MESSAGE    1138
struct OptotrakAddNormalsStruct
    {
    long int lnRigidId;         /* Unique identifier for rigid body. */
    };

#define OPTOTRAK_GET_RIG_STATUS_MESSAGE    1140

/*
 * Data ids you can receive back from the OPTOTRAK data proprietor
 */
#define OPTOTRAK_RIGID_STATUS_MESSAGE    1064
struct OptotrakRigidStatusStruct
    {
    long int lnRigidBodies;     /* Number of rigid bodies in use */
    };

#define OPTO_LATEST_RIGID_MESSAGE       1060
#define OPTO_RIGID_HEADER_SIZE            12
struct OptotrakRigidHeaderStruct
    {
    long int    NumberOfRigids;   /* number of transforms following header */
    long int    StartFrameNumber; /* frame number of 3D data used */
    long int    flags;              /* current flag settings for OPTOTRAK */
    };

union TransformationUnion
    {
    RotationTransformation          rotation;
    transformation                  euler;
    QuatTransformation              quaternion;
    };

struct OptotrakRigidStruct
    {
    long int                    RigidId;         /* rigid body id xfrm is for */
    long int                    flags;            /* flags for this rigid body */
    float                       QuaternionError; /* quat rms error for xfrm */
    float                       IterativeError;  /* iter rms error for xfrm */
    union TransformationUnion  transformation;   /* latest calculated xfrm */
    };
/***************************************************************
Name: ADMIN      -Section detailing interfacing with the OPTOTRAK Administrator
****************************************************************/
/*
 * Defines for commands to pass to the OPTOTRAK Administrator.
 */
#define OPTOTRAK_MODIFY_SYSTEM_MESSAGE  1108
#define OPTOTRAK_MODIFY_SYSTEM_COMMAND  1109
struct OptotrakModifySystemStruct
    {
    long int    threshold;
    long int    MarkerFrequency;
    float       DutyCycle;
    float       voltage;
    long int    MinimumGain;
    };

#define OPTOTRAK_SETUP_COLL_MESSAGE     1110
#define OPTOTRAK_SETUP_COLL_COMMAND     1111

/*
 * Flag definitions used in the OPTOTRAK_SETUP_COLL_MESSAGE.
 * Note: don't use the flag 0x1000 as it is taken by the constant
 *       OPTOTRAK_REALTIME_PRESENT_FLAG.
 */
#define OPTOTRAK_NO_INTERPOLATION_FLAG  0x0001
#define OPTOTRAK_FULL_DATA_FLAG         0x0002
#define OPTOTRAK_PIXEL_DATA_FLAG        0x0004
#define OPTOTRAK_MARKER_BY_MARKER_FLAG  0x0008
#define OPTOTRAK_ECHO_CALIBRATE_FLAG    0x0010
#define OPTOTRAK_BUFFER_RAW_FLAG        0x0020
#define OPTOTRAK_NO_FIRE_MARKERS_FLAG   0x0040
#define OPTOTRAK_STATIC_THRESHOLD_FLAG  0x0080
#define OPTOTRAK_WAVEFORM_DATA_FLAG     0x0100
#define OPTOTRAK_AUTO_DUTY_CYCLE_FLAG   0x0200
#define OPTOTRAK_EXTERNAL_CLOCK_FLAG    0x0400
#define OPTOTRAK_EXTERNAL_TRIGGER_FLAG  0x0800
#define OPTOTRAK_GET_NEXT_FRAME_FLAG    0x2000

/*
 * The following flags are set by the OPTOTRAK system itself.
 * They indicate (1) if the system has revision D/E Sensors,
 *               (2) if the system can perform real-time rigid bodies.
 *               (3) if the markers are on in the system.
 */
#define OPTOTRAK_REVISIOND_FLAG         0x80000000
#define OPTOTRAK_RIGID_CAPABLE_FLAG     0x08000000
#define OPTOTRAK_MARKERS_ACTIVE         0x04000000

struct OdauSetupStruct
{
    long int    OdauFrequency;
    long int    OdauChannels;
    long int    OdauGain;
    long int    OdauFlags;
};

struct OptotrakSetupCollStruct
    {
    long int    DataId;
    long int    NumMarkers;
    long int    FrameFrequency;
    float       CollectionTime;
    float       PreTriggerTime;
    long int    StreamData;
    long int    flags;
    long int    OdauFrequency;
    long int    OdauChannels;
    long int    OdauGain;
    long int    OdauFlags;
    };


#define OPTOTRAK_STOP_FIRING_MESSAGE    1112

#define OPTOTRAK_START_FIRING_MESSAGE   1114
#define OPTOTRAK_START_FIRING_COMMAND   1115

#define OPTOTRAK_START_BUFF_MESSAGE     1116
#define OPTOTRAK_START_BUFF_COMMAND     1117

#define OPTOTRAK_STOP_BUFF_MESSAGE      1118
#define OPTOTRAK_STOP_BUFF_COMMAND      1119

#define OPTOTRAK_SHUTDOWN_MESSAGE       1120
#define OPTOTRAK_SHUTDOWN_COMMAND       1121

#define OPTOTRAK_UNREGISTER_MESSAGE     1132
#define OPTOTRAK_UNREGISTER_COMMAND     1133

#define OPTOTRAK_TRIGGER_COLLECTION_MESSAGE 1134
#define OPTOTRAK_TRIGGER_COLLECTION_COMMAND 1135

#define OPTOTRAK_STROBER_TABLE_MESSAGE      1142
struct OptotrakStroberTableStruct
    {
    long int    Port1;
    long int    Port2;
    long int    Port3;
    long int    Port4;
    };

/*
 * Messages you can receive from the adminstrator.
 */
#define OPTOTRAK_STATUS_MESSAGE         1154
#define OPTOTRAK_STATUS_COMMAND         1155

/*
 * Definitions for mode
 */
#define OPTOTRAK_NOTHING_MODE           0   /* No data generation */
#define OPTOTRAK_IDLING_MODE            1   /* Raw data generation in progress */
#define OPTOTRAK_GENERATING_MODE        2   /* Reconstructed data generation in progress */
#define OPTOTRAK_COLLECTING_MODE        3   /* Data collection in progress */
/*
 * The following structure is returned when asking for status
 * all the required system parameters. The only thing not included is the
 * file bufferring information.
 */
struct SystemCollectionParmsStruct
    {
    long int                                BufferSize;
    long int                                NumSensors;
    long int                                NumOptoscopes;
    long int                                NumControllers;
    long int                                CollectionType;
    struct OptotrakSetupCollStruct       CollParms;
    struct OptotrakModifySystemStruct    SysParms;
    struct ScopeCollectionParmsStruct    ScopeData[ MAX_OPTOSCOPES];
    struct AddressStruct                   WorkerAddress;
    };

#define OPTOTRAK_STATUS_CHANGED_MESSAGE 1158
#define OPTOTRAK_STATUS_CHANGED_COMMAND 1159
/*****************************************************************

Name:               CENTPROD.H


*****************************************************************/
#define CENTPROD_INCLUDE

#define MAX_BUFFER_SIZE                 512
#define OPTIMAL_PEAK                    200

#ifndef CENTROID_OK
#define CENTROID_OK                     0
#define CENTROID_WAVEFORM_TOO_WIDE      1
#define CENTROID_PEAK_TOO_SMALL         2
#define CENTROID_PEAK_TOO_LARGE         3
#define CENTROID_WAVEFORM_OFF_DEVICE    4
#define CENTROID_FELL_BEHIND            5
#define CENTROID_LAST_CENTROID_BAD      6
#define CENTROID_BUFFER_OVERFLOW        7
#define CENTROID_MISSED_CCD             8
#endif

/*****************************************************************
External Variables and Routines
*****************************************************************/
struct  CentroidBufferStruct
{
    float       centroid;             /* Calculated centroid */
    char        Peak;                 /* Peak value 0 to 255 */
    char        gain;                 /* Gain setting for this collection */
    char        ErrorCode;            /* Error in centtroid calculation */
    char        PeakNibble;           /* Low Nibble of Peak value (Revd only) */
};
/*************************************************************************
OPTOLIB     -Describes calls to OPTOLIB routines.

**************************************************************************/

/**************************************************************************
  Defines
**************************************************************************/

#define DEFAULT_LINK_ADDRESS 784

/*
 * The error codes that exist
 */
#define OPTO_NO_ERROR_CODE          0
#define OPTO_SYSTEM_ERROR_CODE      1000
#define OPTO_USER_ERROR_CODE        2000

/*
 * Flags for controlling the setup of the message passing layer on the
 * PC side.
 */
#define OPTO_LOG_ERRORS_FLAG          0x0001
#define OPTO_SECONDARY_HOST_FLAG      0X0002
#define OPTO_ASCII_RESPONSE_FLAG      0X0004

/*
 * Constants for raw files which can be converted.
 */
#define OPTOTRAK_RAW    1
#define ANALOG_RAW        2

/*
 * Constants for modes in which files can be opened.
 */
#define OPEN_READ       1
#define OPEN_WRITE      2

/*
 * Maximum constants for the processes we will have to keep track
 * of in the optoappl software.
 */
#define MAX_OPTOTRAKS         1
#define MAX_DATA_PROPRIETORS  1
#define MAX_ODAUS             4
#define MAX_PROCESS_ADDRESSES MAX_OPTOTRAKS + MAX_DATA_PROPRIETORS + MAX_ODAUS

/*
 * Constants for keeping track of whick process the application wants to
 * communicate with.
 */
#define OPTOTRAK        0
#define DATA_PROPRIETOR 1
#define ODAU1           2
#define ODAU2           3
#define ODAU3           4
#define ODAU4           5

/*
 * Flags for controlling the put message modules.
 */
#define OPTO_PROCESS_BITS       0x000F
#define OPTO_NO_REPLY_FLAG      0x0010


/**************************************************************************
 External Variables
**************************************************************************/

/*
 * String for returning error messages.
 */
extern char
    szNDErrorString[];

/**************************************************************************
 Routine Definitions
**************************************************************************/

CPLUSPLUS_START

/*
 * Routines in assembler module.
 */
boolean    SendBlock( int offset, void  *buffer, unsigned size );

boolean     ReceiveBlock( int offset, void  *buffer, unsigned size );

int     DSEG( void );

/*
 * Message Based Interface prototypes.
 */
extern int
    OptoGetError( int nSourceProcess, char *pszErrorStr, int *pnErrorCode ),
    OptoSetupMessageSystem( unsigned int uOffset, unsigned int uFlags ),
    OptoInputMessageAvailable( void ),
    OptoGetMessageData( void *pData, double dTimeoutTime ),
    OptoPutMessage( int nDestination, unsigned int uMessageId,
                    void *pMessageData, unsigned int uDataSize,
                    void *pRetPtr ),
    OptoShutdownMessageSystem( void );

/*
 * Routine Based Interface prototypes.
 */
extern int
    TransputerLoadSystem( char *pszNifFile ),
    TransputerInitializeSystem( unsigned int uFlags ),
    TransputerShutdownSystem( void );

extern int
    OptotrakLoadCameraParameters( char *pszCamFile ),
    OptotrakSetStroberPortTable( int nPort1,
                                 int nPort2,
                                 int nPort3,
                                 int nPort4 ),
    OptotrakSetupCollectionFromFile( char *pszCollectFile ),
    OptotrakSetupCollection( int nMarkers,
                             int nFrameFrequency,
                             int nMarkerFrequency,
                             int nThreshold,
                             int nMinimumGain,
                             int nStreamData,
                             float fDutyCycle,
                             float fVoltage,
                             float fCollectionTime,
                             float fPreTriggerTime,
                             int nFlags ),
    OptotrakActivateMarkers( void ),
    OptotrakDeActivateMarkers( void ),
    OdauSetupCollection( int nOdauId,
                         int nChannels,
                         int nGain,
                         int nDigitalMode,
                         int nFrameFrequency,
                         int nStreamData,
                         float fCollectionTime,
                         float fPreTriggerTime ),
    OptotrakGetStatus( int *pnNumSensors,
                       int *pnNumOdaus,
                       int *pnNumRigidBodies,
                       int *pnMarkers,
                       int *pnFrameFrequency,
                       int *pnMarkerFrequency,
                       int *pnThreshold,
                       int *pnMinimumGain,
                       int *pnStreamData,
                       float *pfDutyCycle,
                       float *pfVoltage,
                       float *pfCollectionTime,
                       float *pfPreTriggerTime,
                       int *pnFlags ),
    OdauGetStatus( int nOdauId,
                   int *pnChannels,
                   int *pnGain,
                   int *pnDigitalMode,
                   int *pnFrameFrequency,
                   int *pnStreamData,
                   float *pfCollectionTime,
                   float *pfPreTriggerTime,
                   int *pnFlags );

extern int
    RigidBodyAdd( int nRigidBodyId,
                  int nStartMarker,
                  int nNumMarkers,
                  float *pRigidCoordinates,
                  float *pNormalCoordinates,
                  int nFlags ),
    RigidBodyAddFromFile( int nRigidBodyId,
                          int nStartMarker,
                          char *pszRigFile,
                          int nFlags ),
    RigidBodyChangeSettings( int nRigidBodyId,
                             int nMinMarkers,
                             int nMaxMarkersAngle,
                             float fMax3dError,
                             float fMaxSensorError,
                             float fMax3dRmsError,
                             float fMaxSensorRmsError,
                             int nFlags ),
    RigidBodyDelete( int nRigidBodyId ),
    RigidBodyChangeFOR( int nRigidId,
                        int nRotationMethod );

extern int
    RequestLatest3D( void ),
    RequestLatestRaw( void ),
    RequestLatestTransforms( void ),
    RequestLatestOdauRaw( int nOdauId ),
    DataIsReady( void ),
    ReceiveLatestData( unsigned int *uFrameNumber,
                       unsigned int *uElements,
                       unsigned int *uFlags,
                       void *pDataDest ),
    DataGetLatest3D( unsigned int *puFrameNumber,
                     unsigned int *puElements,
                     unsigned int *puFlags,
                     void *pDataDest ),
    DataGetLatestRaw( unsigned int *puFrameNumber,
                      unsigned int *puElements,
                      unsigned int *puFlags,
                      void *pDataDest ),
    DataGetLatestTransforms( unsigned int *puFrameNumber,
                             unsigned int *puElements,
                             unsigned int *puFlags,
                             void *pDataDest ),
    DataGetLatestOdauRaw( int nOdauId,
                          unsigned int *puFrameNumber,
                          unsigned int *puElements,
                          unsigned int *puFlags,
                          void *pDataDest ),
    DataReceiveLatest3D( unsigned int *puFrameNumber,
                         unsigned int *puElements,
                         unsigned int *puFlags,
                         Position3d *pDataDest ),
    DataReceiveLatestRaw( unsigned int *puFrameNumber,
                          unsigned int *puElements,
                          unsigned int *puFlags,
                          void *pDataDest ),
    DataReceiveLatestTransforms( unsigned int *puFrameNumber,
                                 unsigned int *puElements,
                                 unsigned int *puFlags,
                                 void *pDataDest ),
    DataReceiveLatestOdauRaw( unsigned int *puFrameNumber,
                              unsigned int *puElements,
                              unsigned int *puFlags,
                              unsigned int *pDataDest );

extern int
    DataBufferInitializeFile( unsigned int uDataId,
                              char *pszFileName ),
    DataBufferInitializeMem( unsigned int uDataId,
                             void *pMemory ),
    DataBufferStart( void ),
    DataBufferStop( void ),
    DataBufferSpoolData( unsigned int *puSpoolStatus ),
    DataBufferWriteData( unsigned int *puRealtimeData,
                         unsigned int *puSpoolComplete,
                         unsigned int *puSpoolStatus ),
    DataBufferAbortSpooling( void );

extern int
    FileConvert( char *pszInputFilename, char *pszOutputFilename,
                 unsigned int uFileType ),
    FileOpen( char *pszFilename, unsigned int uFileId,
              unsigned int uFileMode, int *pnItems, int *pnSubItems,
              long int *plnFrames, float *pfFrequency,
              char *pszComments, void **pFileHeader ),
    FileRead( unsigned int uFileId, long int lnStartFrame,
              unsigned int uNumberOfFrames, void *pDataDest ),
    FileWrite( unsigned int uFileId, long int lnStartFrame,
               unsigned int uNumberOfFrames, void *pDataDest ),
    FileClose( unsigned int uFileId );

#ifdef PACK_UNPACK
extern unsigned
    PackAddress( struct AddressStruct *p, unsigned char *pchBuff,
                 unsigned uBuffSize ),
    UnPackAddress( struct AddressStruct *p, unsigned char *pchBuff,
                   unsigned uBuffSize ),
    PackMessageHeader( struct MessageHeaderStruct *p, unsigned char *pchBuff,
                       unsigned uBuffSize ),
    UnPackMessageHeader( struct MessageHeaderStruct *p, unsigned char *pchBuff,
                         unsigned uBuffSize ),
    PackMessage( struct MessageStruct *p, unsigned char *pchBuff,
                 unsigned uBuffSize ),
    UnPackMessage( struct MessageStruct *p, unsigned char *pchBuff,
                   unsigned uBuffSize ),
    PackProcessSetup( struct ProcessSetupStruct *p, unsigned char *pchBuff,
                      unsigned uBuffSize ),
    UnPackProcessSetup( struct ProcessSetupStruct *p, unsigned char *pchBuff,
                        unsigned uBuffSize ),
    PackNodeSetup( struct NodeSetupStruct *p, unsigned char *pchBuff,
                   unsigned uBuffSize ),
    UnPackNodeSetup( struct NodeSetupStruct *p, unsigned char *pchBuff,
                     unsigned uBuffSize ),
    PackOptotrakRegister( struct OptotrakRegisterStruct *p,
                          unsigned char *pchBuff,
                          unsigned uBuffSize ),
    UnPackOptotrakRegister( struct OptotrakRegisterStruct *p,
                            unsigned char *pchBuff,
                            unsigned uBuffSize ),
    PackOptoUnsuccessful( struct OptoUnsuccessfulStruct *p,
                          unsigned char *pchBuff,
                          unsigned uBuffSize ),
    UnPackOptoUnsuccessful( struct OptoUnsuccessfulStruct *p,
                            unsigned char *pchBuff,
                            unsigned uBuffSize ),
    PackOptoDataHeader( struct OptoDataHeaderStruct *p,
                        unsigned char *pchBuff,
                        unsigned uBuffSize ),
    UnPackOptoDataHeader( struct OptoDataHeaderStruct *p,
                          unsigned char *pchBuff,
                          unsigned uBuffSize ),
    PackOptotrak_wave_head_( struct optotrak_wave_head_struct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    UnPackOptotrak_wave_head_( struct optotrak_wave_head_struct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    PackOptoTransformData( struct OptoTransformDataStruct *p,
                           unsigned char *pchBuff,
                           unsigned uBuffSize ),
    UnPackOptoTransformData( struct OptoTransformDataStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    PackOptoscopeSetupColl( struct OptoscopeSetupCollStruct *p,
                            unsigned char *pchBuff,
                            unsigned uBuffSize ),
    UnPackOptoscopeSetupColl( struct OptoscopeSetupCollStruct *p,
                              unsigned char *pchBuff,
                              unsigned uBuffSize ),
    PackScopeCollectionParms( struct ScopeCollectionParmsStruct *p,
                              unsigned char *pchBuff,
                              unsigned uBuffSize ),
    UnPackScopeCollectionParms( struct ScopeCollectionParmsStruct *p,
                                unsigned char *pchBuff,
                                unsigned uBuffSize ),
    PackOptoscopeSetChnlAll( struct OptoscopeSetChnlAllStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    UnPackOptoscopeSetChnlAll( struct OptoscopeSetChnlAllStruct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    PackOptoscopeSetChnlSgl( struct OptoscopeSetChnlSglStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    UnPackOptoscopeSetChnlSgl( struct OptoscopeSetChnlSglStruct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    PackOptoscopeSetGainAll( struct OptoscopeSetGainAllStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    UnPackOptoscopeSetGainAll( struct OptoscopeSetGainAllStruct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    PackOptoscopeSetGainSgl( struct OptoscopeSetGainSglStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    UnPackOptoscopeSetGainSgl( struct OptoscopeSetGainSglStruct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    PackOptoscopeStatus( struct OptoscopeStatusStruct *p,
                         unsigned char *pchBuff,
                         unsigned uBuffSize ),
    UnPackOptoscopeStatus( struct OptoscopeStatusStruct *p,
                           unsigned char *pchBuff,
                           unsigned uBuffSize ),
    PackOptotrakAddRigid( struct OptotrakAddRigidStruct *p,
                          unsigned char *pchBuff,
                          unsigned uBuffSize ),
    UnPackOptotrakAddRigid( struct OptotrakAddRigidStruct *p,
                            unsigned char *pchBuff,
                            unsigned uBuffSize ),
    PackOptotrakDelRigid( struct OptotrakDelRigidStruct *p,
                          unsigned char *pchBuff,
                          unsigned uBuffSize ),
    UnPackOptotrakDelRigid( struct OptotrakDelRigidStruct *p,
                            unsigned char *pchBuff,
                            unsigned uBuffSize ),
    PackOptotrakSetRigid( struct OptotrakSetRigidStruct *p,
                          unsigned char *pchBuff,
                          unsigned uBuffSize ),
    UnPackOptotrakSetRigid( struct OptotrakSetRigidStruct *p,
                            unsigned char *pchBuff,
                            unsigned uBuffSize ),
    PackOptotrakRotateRigids( struct OptotrakRotateRigidsStruct *p,
                              unsigned char *pchBuff,
                              unsigned uBuffSize ),
    UnPackOptotrakRotateRigids( struct OptotrakRotateRigidsStruct *p,
                                unsigned char *pchBuff,
                                unsigned uBuffSize ),
    PackOptotrakAddNormals( struct OptotrakAddNormalsStruct *p,
                            unsigned char *pchBuff,
                            unsigned uBuffSize ),
    UnPackOptotrakAddNormals( struct OptotrakAddNormalsStruct *p,
                              unsigned char *pchBuff,
                              unsigned uBuffSize ),
    PackOptotrakRigidStatus( struct OptotrakRigidStatusStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    UnPackOptotrakRigidStatus( struct OptotrakRigidStatusStruct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    PackOptotrakRigidHeader( struct OptotrakRigidHeaderStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    UnPackOptotrakRigidHeader( struct OptotrakRigidHeaderStruct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    PackOptotrakRigid( struct OptotrakRigidStruct *p,
                       unsigned char *pchBuff,
                       unsigned uBuffSize ),
    UnPackOptotrakRigid( struct OptotrakRigidStruct *p,
                         unsigned char *pchBuff,
                         unsigned uBuffSize ),
    PackOptotrakModifySystem( struct OptotrakModifySystemStruct *p,
                              unsigned char *pchBuff,
                              unsigned uBuffSize ),
    UnPackOptotrakModifySystem( struct OptotrakModifySystemStruct *p,
                                unsigned char *pchBuff,
                                unsigned uBuffSize ),
    PackOdauSetup( struct OdauSetupStruct *p,
                   unsigned char *pchBuff,
                   unsigned uBuffSize ),
    UnPackOdauSetup( struct OdauSetupStruct *p,
                     unsigned char *pchBuff,
                     unsigned uBuffSize ),
    PackOptotrakStroberTable( struct OptotrakStroberTableStruct *p,
                              unsigned char *pchBuff,
                              unsigned uBuffSize ),
    PackOptotrakSetupColl( struct OptotrakSetupCollStruct *p,
                           unsigned char *pchBuff,
                           unsigned uBuffSize ),
    UnPackOptotrakStroberTable( struct OptotrakStroberTableStruct *p,
                                unsigned char *pchBuff,
                                unsigned uBuffSize ),
    UnPackOptotrakSetupColl( struct OptotrakSetupCollStruct *p,
                             unsigned char *pchBuff,
                             unsigned uBuffSize ),
    PackSystemCollectionParms( struct SystemCollectionParmsStruct *p,
                               unsigned char *pchBuff,
                               unsigned uBuffSize ),
    UnPackSystemCollectionParms( struct SystemCollectionParmsStruct *p,
                                 unsigned char *pchBuff,
                                 unsigned uBuffSize ),
    PackCentroidBuffer( struct CentroidBufferStruct *p,
                        unsigned char *pchBuff,
                        unsigned uBuffSize ),
    UnPackCentroidBuffer( struct CentroidBufferStruct *p,
                          unsigned char *pchBuff,
                          unsigned uBuffSize );
#endif
CPLUSPLUS_END
