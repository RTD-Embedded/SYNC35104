/**
 * @file ubxproto.h
 * @brief Interface for interpreting the U-Blox protocol
 */

#ifndef UBXPROTO_H_INCLUDED
#define UBXPROTO_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

/**
 * @brief First synchronization character of UBX
 */
#define UBX_SYNC_CHAR1			(0xb5)

/**
 * @brief Second synchronization character of UBX
 */
#define UBX_SYNC_CHAR2			(0x62)

 /**
  * @brief Size of UBX message header in bytes
  * @details
  * The header contains the two synchronization characters (2 bytes), the message
  * class (1 byte), the message ID (1 byte) and the payload length (2 bytes)
  */
#define UBX_HEADER_SIZE				(6)

#define UBX_HEADER_SYNC_OFFSET		(0)
#define UBX_HEADER_CLASS_OFFSET		(2)
#define UBX_HEADER_ID_OFFSET		(3)
#define UBX_HEADER_LENGTH_OFFSET	(4)

#define UBX_CHKSUM_SIZE				(2)

#define UBX_OVERHEAD_SIZE (UBX_HEADER_SIZE + UBX_CHKSUM_SIZE)

/**
 * @brief Maximum length of NMEA messages in bytes
 */
#define NMEA_MAX_LENGTH			(82)

/**
 * @brief U-blox message structure
 */
typedef struct {
	/** @brief Synchronization characters */
	uint8_t sync_char[2];
	/** @brief Message class */
	uint8_t msg_class;
	/** @brief Message identifier */
	uint8_t msg_id;
	/** @brief Payload length */
	uint16_t msg_len;
	/** @brief Checksum */
	uint8_t chksum[2];
} ubx_msg_t;

/**
 * @brief U-Blox protocols
 */
typedef enum {
	/** @brief No protocol */
	UBX_PROTO_NONE,
	/** @brief NMEA protocol */
	UBX_PROTO_NMEA,
	/** @brief UBX protocol */
	UBX_PROTO_UBX,
	/** @brief RTCM protocol */
	UBX_PROTO_RTCM,
} ubx_protocol_type_t;

/**
 * @brief U-Blox message classes 
 */
typedef enum {
	/**
	 * @brief Navigation results 
	 * @details 
	 * Position, Speed, Time, Acc, Heading, DOP, SVs used
	 */
	UBX_MSG_CLASS_NAV = 0x01, 
	
	UBX_MSG_CLASS_RXM = 0x02, //Receiver Manager Messages : Satellite Status, RTC Status
	UBX_MSG_CLASS_INF = 0x04, // Information Messages : Printf - Style Messages, with IDs such as Error, Warning, Notice
	UBX_MSG_CLASS_ACK = 0x05, // Ack / Nack Messages : as replies to CFG Input Messages
	
	/**
	 * @brief Configuration input messages 
	 * @details
	 * Set Dynamic Model, Set DOP Mask, Set Baud Rate, etc.
	 */
	UBX_MSG_CLASS_CFG = 0x06,
	
	/**
	 * @brief Monitoring messages 
	 * @details 
	 * Comunication Status, CPU Load, Stack Usage, Task Status
	 */
	UBX_MSG_CLASS_MON = 0x0A,
	UBX_MSG_CLASS_AID = 0x0B, // AssistNow Aiding Messages : Ephemeris, Almanac, other A - GPS data input
	
	/**
	 * @brief Timing Messages 
	 * @details
	 * Timepulse Output, Timemark Results
	 */
	UBX_MSG_CLASS_TIM = 0x0D, 
	
	/**
	 * @brief External Sensor Fusion Messages
	 * @details
	 * External sensor measurements and status information
	 */
	UBX_MSG_CLASS_ESF = 0x10,
	UBX_NUM_MSG_CLASSES = 17
} ubx_msg_class_t;

typedef enum {
	UBX_MSG_NAV_AOPSTATUS = 0x60,
	UBX_MSG_NAV_ATT = 0x05,
	UBX_MSG_NAV_CLOCK = 0x22,
	UBX_MSG_NAV_DGPS = 0x31,
	UBX_MSG_NAV_DOP = 0x04,
	UBX_MSG_NAV_EOE = 0x61,
	UBX_MSG_NAV_GEOFENCE = 0x39,
	UBX_MSG_NAV_HPPOSECEF = 0x13,
	UBX_MSG_NAV_HPPOSLLH = 0x14,
	UBX_MSG_NAV_NMI = 0x28,
	UBX_MSG_NAV_ODO = 0x09,
	UBX_MSG_NAV_ORB = 0x34,
	UBX_MSG_NAV_POSECEF = 0x01,
	UBX_MSG_NAV_POSLLH = 0x02,
	UBX_MSG_NAV_PVT = 0x07,
	UBX_MSG_NAV_RELPOSNED = 0x3c,
	UBX_MSG_NAV_RESETODO = 0x10,
	UBX_MSG_NAV_SAT = 0x35,
	UBX_MSG_NAV_SBAS = 0x32,
	UBX_MSG_NAV_SLAS = 0x42,
	UBX_MSG_NAV_SOL = 0x06,
	UBX_MSG_NAV_STATUS = 0x03,
	UBX_MSG_NAV_SVINFO = 0x30,
	UBX_MSG_NAV_SVIN = 0x3b,
	/** @brief Beidu time solution */
	UBX_MSG_NAV_TIMEBDS = 0x24,
	/** @brief Galileo time solution */
	UBX_MSG_NAV_TIMEGAL = 0x25,
	/** @brief GLONASS time solution */
	UBX_MSG_NAV_TIMEGLO = 0x23,
	/** @brief GPS time solution */
	UBX_MSG_NAV_TIMEGPS = 0x20,
	/** @brief Leap second event information */
	UBX_MSG_NAV_TIMELS = 0x26,
	/** @brief UTC time solution */
	UBX_MSG_NAV_TIMEUTC = 0x21,
	/** @brief Velocity solution in ECEF */
	UBX_MSG_NAV_VELECEF = 0x11,
	/** @brief Velocity solution in NED frame */
	UBX_MSG_NAV_VELNED = 0x12,

} ubx_msg_nav_id_t;

/**
 * @brief Acknowledged / Not acknowledged messages
 * @details
 * Acknowledge or Reject messages to UBX - CFG input messages.
 * Messages in the UBX - ACK class output the processing results to 
 * UBX - CFG and some other messages.
 */
typedef enum {
	/** @brief Not acknowledged */
	UBX_MSG_ACK_NAK = 0x00,
	/** @brief Acknowledged */
	UBX_MSG_ACK_ACK = 0x01,
	/** @brief Number of mesage IDs in ACK class */
	UBX_NUM_ACK_IDS = 2,
} ubx_msg_ack_id_t;

typedef enum {
	/** @brief Antenna configuration */
	UBX_MSG_CFG_ANT = 0x13,
	UBX_MSG_CFG_BATCH = 0x93,
	UBX_MSG_CFG_CFG = 0x09,
	UBX_MSG_CFG_DAT = 0x06,
	UBX_MSG_CFG_DGNSS = 0x70,
	UBX_MSG_CFG_DOSC = 0x61,
	UBX_MSG_CFG_ESRC = 0x60,
	UBX_MSG_CFG_GEOFENCE = 0x69,
	UBX_MSG_CFG_GNSS = 0x3E,
	UBX_MSG_CFG_HNR = 0x5C,
	UBX_MSG_CFG_INF = 0x02,
	UBX_MSG_CFG_ITFM = 0x39,
	UBX_MSG_CFG_LOGFILTER = 0x47,
	/** @brief Message configuration */
	UBX_MSG_CFG_MSG = 0x01,
	UBX_MSG_CFG_NAV5 = 0x24,
	UBX_MSG_CFG_NAVX5 = 0x23,
	UBX_MSG_CFG_NMEA = 0x17,
	UBX_MSG_CFG_ODO = 0x1E,
	UBX_MSG_CFG_PM2 = 0x3B,
	UBX_MSG_CFG_PMS = 0x86,
	UBX_MSG_CFG_PRT = 0x00,
	UBX_MSG_CFG_PWR = 0x57,
	UBX_MSG_CFG_RATE = 0x08,
	UBX_MSG_CFG_RINV = 0x34,
	/** @brief Reset receiver */
	UBX_MSG_CFG_RST = 0x04,
	UBX_MSG_CFG_RXM = 0x11,
	UBX_MSG_CFG_SBAS = 0x16,
	UBX_MSG_CFG_SLAS = 0x8D,
	UBX_MSG_CFG_SMGR = 0x62,
	UBX_MSG_CFG_TMODE2 = 0x3D,
	UBX_MSG_CFG_TMODE3 = 0x71,
	UBX_MSG_CFG_TPS = 0x31,
	UBX_MSG_CFG_TXSLOT = 0x53,
	/** @brief USB configuration */
	UBX_MSG_CFG_USB = 0x1B,
} ubx_msg_cfg_id_t;

typedef enum {
	UBX_MSG_MON_BATCH= 0x32,
	UBX_MSG_MON_GNSS = 0x28,
	UBX_MSG_MON_HW2 = 0x0b,
	UBX_MSG_MON_HW = 0x09,
	UBX_MSG_MON_IO = 0x02,
	UBX_MSG_MON_MSGPP = 0x06,
	UBX_MSG_MON_PATCH = 0x27,
	UBX_MSG_MON_RXBUF = 0x07,
	UBX_MSG_MON_RXR = 0x21,
	UBX_MSG_MON_SMGR = 0x2e,
	/** @brief Transmit buffer status */
	UBX_MSG_MON_TXBUF = 0x08,
	/** @brief Receiver and software version */
	UBX_MSG_MON_VER = 0x04,
} ubx_msg_mon_id_t;

typedef struct {
	/** @brief Synchronization character */
	char sync_char;
	/** @brief Talker identifier */
	char talker[2];
	/** @brief Message format */
	char format[3];
	/** @brief Checksum */
	char chksum[2];
} nmea_msg_t;

/**
 * @brief NMEA talkers supported by the UBX protocol
 */
typedef enum {
	/** @brief GPS, SBAS, QZSS */
	NMEA_TALKER_GPS = 0,
	/** @brief GLONASS */
	NMEA_TALKER_GLONASS = 1,
	/** @brief Galileo */
	NMEA_TALKER_GALILEO = 2,
	/** @brief Beidu */
	NMEA_TALKER_BEIDU = 3,
	NMEA_TALKER_GNSS = 4,
	NMEA_NUM_TALKERS = 5
} ubx_nmea_talker_id_t;

#ifdef __cplusplus
extern "C" {
#endif 

/**
 * @brief Retrieve UBX message class name
 * @param[in] msg_class Numeric message class
 * return Name of message class ("Unknown" if not known)
 */
char const * ubx_msg_get_class_name(uint8_t msg_class);

char const * ubx_msg_get_id_name(uint8_t msg_class, uint8_t msg_id);

char const * ubx_msg_get_description(uint8_t msg_class, uint8_t msg_id);

int ubx_msg_print_dbg(FILE * fp, ubx_msg_t *msg);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* UBXPROTO_H_INCLUDED */
