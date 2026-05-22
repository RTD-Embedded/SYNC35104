#include "ubxproto.h"

static char const * const ubx_msg_class_names[UBX_NUM_MSG_CLASSES] = {
	"Unknown",
	"NAV",
	"RXM",
	"Unknown",
	"INF",
	"ACK",
	"CFG",
	"Unknown",
	"Unknown",
	"Unknown",
	"MON",
	"AID",
	"Unknown",
	"TIM",
	"Unknown",
	"Unknown",
	"ESF"
};

static char const * const ubx_msg_ack_id_names[UBX_NUM_ACK_IDS] = { "ACK-NAK", "ACK-ACK" };

char const * ubx_msg_get_class_name(uint8_t msg_class)
{
	switch (msg_class) {
	case UBX_MSG_CLASS_NAV:
	case UBX_MSG_CLASS_RXM:
	case UBX_MSG_CLASS_INF:
	case UBX_MSG_CLASS_ACK:
	case UBX_MSG_CLASS_CFG:
	case UBX_MSG_CLASS_MON:
	case UBX_MSG_CLASS_AID:
	case UBX_MSG_CLASS_TIM:
	case UBX_MSG_CLASS_ESF:
		return ubx_msg_class_names[msg_class];
		break;
	default:
		return "Unknown";
		break;
	}
}

char const * ubx_msg_get_id_name(uint8_t msg_class, uint8_t msg_id)
{
	switch (msg_class) {
	case UBX_MSG_CLASS_NAV:
		switch (msg_id) {
		case UBX_MSG_NAV_AOPSTATUS: return "NAV-AOPSTATUS";
		case UBX_MSG_NAV_ATT: return "NAV-ATT";
		case UBX_MSG_NAV_CLOCK: return "NAV-CLOCK";
		case UBX_MSG_NAV_DGPS: return "NAV-DGPS";
		case UBX_MSG_NAV_DOP: return "NAV-DOP";
		case UBX_MSG_NAV_EOE: return "NAV-EOE";
		case UBX_MSG_NAV_GEOFENCE: return "NAV-GEOFENCE";
		case UBX_MSG_NAV_HPPOSECEF: return "NAV-HPPOSECEF";
		case UBX_MSG_NAV_HPPOSLLH: return "NAV-HPPOSLLH";
		case UBX_MSG_NAV_NMI: return "NAV-NMI";
		case UBX_MSG_NAV_ODO: return "NAV-ODO";
		case UBX_MSG_NAV_ORB: return "NAV-ORB";
		case UBX_MSG_NAV_POSECEF: return "NAV-POSECEF";
		case UBX_MSG_NAV_POSLLH: return "NAV-POSLLH";
		case UBX_MSG_NAV_PVT: return "NAV-PVT";
		case UBX_MSG_NAV_RELPOSNED: return "NAV-RELPOSNED";
		case UBX_MSG_NAV_RESETODO: return "NAV-RESETODO";
		case UBX_MSG_NAV_SAT: return "NAV-SAT";
		case UBX_MSG_NAV_SBAS: return "NAV-SBAS";
		case UBX_MSG_NAV_SLAS: return "NAV-SLAS";
		case UBX_MSG_NAV_SOL: return "NAV-SOL";
		case UBX_MSG_NAV_STATUS: return "NAV-STATUS";
		case UBX_MSG_NAV_SVINFO: return "NAV-SVINFO";
		case UBX_MSG_NAV_SVIN: return "NAV-SVIN";
		case UBX_MSG_NAV_TIMEBDS: return "NAV-TIMEBDS";
		case UBX_MSG_NAV_TIMEGAL: return "NAV-TIMEGAL";
		case UBX_MSG_NAV_TIMEGLO: return "NAV-TIMEGLO";
		case UBX_MSG_NAV_TIMEGPS: return "NAV-TIMEGPS";
		case UBX_MSG_NAV_TIMELS: return "NAV-TIMELS";
		case UBX_MSG_NAV_TIMEUTC: return "NAV-TIMEUTC";
		case UBX_MSG_NAV_VELECEF: return "NAV-VELECEF";
		case UBX_MSG_NAV_VELNED: return "NAV-VELNED";
		default:
			return "NAV-Unknown";
		}
	case UBX_MSG_CLASS_ACK:
		switch (msg_id) {
		case UBX_MSG_ACK_NAK:
		case UBX_MSG_ACK_ACK:
			return ubx_msg_ack_id_names[msg_id];
		default:
			return "ACK-Unknown";
		}
	case UBX_MSG_CLASS_CFG:
		switch (msg_id) {
		case UBX_MSG_CFG_ANT: return "CFG-ANT";
		case UBX_MSG_CFG_BATCH:	return "CFG-BATCH";
		case UBX_MSG_CFG_CFG: return "CFG-CFG";
		case UBX_MSG_CFG_DAT: return "CFG-DAT";
		case UBX_MSG_CFG_DGNSS: return "CFG-DGNSS";
		case UBX_MSG_CFG_DOSC: return "CFG-DOSC";
		case UBX_MSG_CFG_ESRC: return "CFG-ESRC";
		case UBX_MSG_CFG_GEOFENCE: return "CFG-GEOFENCE";
		case UBX_MSG_CFG_GNSS: return "CFG-GNSS";
		case UBX_MSG_CFG_HNR: return "CFG-HNR";
		case UBX_MSG_CFG_INF: return "CFG-INF";
		case UBX_MSG_CFG_ITFM: return "CFG-ITFM";
		case UBX_MSG_CFG_LOGFILTER: return "CFG-LOGFILTER";
		case UBX_MSG_CFG_MSG: return "CFG-MSG";
		case UBX_MSG_CFG_NAV5: return "CFG-NAV5";
		case UBX_MSG_CFG_NAVX5: return "CFG-NAVX5";
		case UBX_MSG_CFG_NMEA: return "CFG-NMEA";
		case UBX_MSG_CFG_ODO: return "CFG-ODO";
		case UBX_MSG_CFG_PM2: return "CFG-PM2";
		case UBX_MSG_CFG_PMS: return "CFG-PMS";
		case UBX_MSG_CFG_PRT: return "CFG-PRT";
		case UBX_MSG_CFG_PWR: return "CFG-PWR";
		case UBX_MSG_CFG_RATE: return "CFG-RATE";
		case UBX_MSG_CFG_RINV: return "CFG-RINV";
		case UBX_MSG_CFG_RST: return "CFG-RST";
		case UBX_MSG_CFG_RXM: return "CFG-RXM";
		case UBX_MSG_CFG_SBAS: return "CFG-SBAS";
		case UBX_MSG_CFG_SLAS: return "CFG-SLAS";
		case UBX_MSG_CFG_SMGR: return "CFG-SMGR";
		case UBX_MSG_CFG_TMODE2: return "CFG-TMODE2";
		case UBX_MSG_CFG_TMODE3: return "CFG-TMODE3";
		case UBX_MSG_CFG_TXSLOT: return "CFG-TXSLOT";
		case UBX_MSG_CFG_USB: return "CFG-USB";
		default:
			return "CFG-Unknown";
		}
	case UBX_MSG_CLASS_MON:
		switch (msg_id) {
		case UBX_MSG_MON_BATCH: return "MON-BATCH";
		case UBX_MSG_MON_GNSS: return "MON-GNSS";
		case UBX_MSG_MON_HW2: return "MON-HW2";
		case UBX_MSG_MON_HW: return "MON-HW";
		case UBX_MSG_MON_IO: return "MON-IO";
		case UBX_MSG_MON_MSGPP: return "MON-MSGPP";
		case UBX_MSG_MON_PATCH: return "MON-PATCH";
		case UBX_MSG_MON_RXBUF: return "MON-RXBUF";
		case UBX_MSG_MON_RXR: return "MON-RXR";
		case UBX_MSG_MON_SMGR: return "MON-SMGR";
		case UBX_MSG_MON_TXBUF: return "MON-TXBUF";
		case UBX_MSG_MON_VER: return "MON-VER";
		default:
			return "MON-Unknown";
		}
			
	case UBX_MSG_CLASS_RXM:
	case UBX_MSG_CLASS_INF:
	case UBX_MSG_CLASS_AID:
	case UBX_MSG_CLASS_TIM:
	case UBX_MSG_CLASS_ESF:
		return ubx_msg_class_names[msg_class];
		break;
	default:
		return "Unknown";
		break;
	}
}

char const * ubx_msg_get_description(uint8_t msg_class, uint8_t msg_id)
{
	switch (msg_class) {
	case UBX_MSG_CLASS_NAV:
		switch (msg_id) {
		case UBX_MSG_NAV_AOPSTATUS: return "AssistNow Autonomous status";
		case UBX_MSG_NAV_ATT: return "Attitude solution";
		case UBX_MSG_NAV_CLOCK: return "Clock solution";
		case UBX_MSG_NAV_DGPS: return "DGPS data used for NAV";
		case UBX_MSG_NAV_DOP: return "Dilution of precision";
		case UBX_MSG_NAV_EOE: return "End of epoch";
		case UBX_MSG_NAV_GEOFENCE: return "Geofencing status";
		case UBX_MSG_NAV_HPPOSECEF: return "High precision position solution in ECEF";
		case UBX_MSG_NAV_HPPOSLLH: return "High precision geodetic position solution";
		case UBX_MSG_NAV_NMI: return "Navigation message cross-check information";
		case UBX_MSG_NAV_ODO: return "Odometer solution";
		case UBX_MSG_NAV_ORB: return "GNSS orbit database info";
		case UBX_MSG_NAV_POSECEF: return "Position solution in ECEF";
		case UBX_MSG_NAV_POSLLH: return "Geodetic position solution";
		case UBX_MSG_NAV_PVT: return "Navigation position velocity time solution";
		case UBX_MSG_NAV_RELPOSNED: return "Relative positioning information in NED frame";
		case UBX_MSG_NAV_RESETODO: return "Reset odometer";
		case UBX_MSG_NAV_SAT: return "Satellite information";
		case UBX_MSG_NAV_SBAS: return "SBAS status data";
		case UBX_MSG_NAV_SLAS: return "QZSS L1S SLAS status data";
		case UBX_MSG_NAV_SOL: return "Navigation solution information";
		case UBX_MSG_NAV_STATUS: return "Receiver navigation status";
		case UBX_MSG_NAV_SVINFO: return "Space vehicle information";
		case UBX_MSG_NAV_SVIN: return "Survey-in data";
		case UBX_MSG_NAV_TIMEBDS: return "BeiDou time solution";
		case UBX_MSG_NAV_TIMEGAL: return "Galileo time solution";
		case UBX_MSG_NAV_TIMEGLO: return "GLONASS time solution";
		case UBX_MSG_NAV_TIMEGPS: return "GPS time solution";
		case UBX_MSG_NAV_TIMELS: return "Leap second event information";
		case UBX_MSG_NAV_TIMEUTC: return "UTC time solution";
		case UBX_MSG_NAV_VELECEF: return "Velocitiy solution in ECEF";
		case UBX_MSG_NAV_VELNED: return "Velocity solution in NED frame";
		default:
			return "Unknown navigation message";
		}
	default:
		return "Unknown message";
	}
}

int ubx_msg_print_dbg(FILE * fp, ubx_msg_t *msg)
{
	fprintf(fp, "%s (%02X %02x), %s, L: %d", 
		ubx_msg_get_id_name(msg->msg_class, msg->msg_id), 
		msg->msg_class, msg->msg_id,
		ubx_msg_get_description(msg->msg_class, msg->msg_id),
		msg->msg_len
	);
	
	fprintf(fp, "\n");
	return 0;
}