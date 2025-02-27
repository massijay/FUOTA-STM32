/*$Id: //dwh/bluetooth/DWC_ble154combo/firmware/rel/1.30a-SOW05PatchV6/firmware/public_inc/rfd_dev_config.h#1 $*/
/**
 ********************************************************************************
 * @file    rfd_dev_config.h
 * @brief Configurations for reduced function device and full function device 
 *
 *
 ******************************************************************************
 * @copy
 *
 *(C) COPYRIGHT 2023 SYNOPSYS, Inc. This Synopsys "product" and all associated documentation
 *are proprieytary to Synopsys, Inc. and may only be used pursuant to the terms and
 *conditions of a written license agreement with Synopsys, Inc. All other use,
 *reproduction, modification, or distribution of the Synopsys "product" or the associated
 *documentation is strictly prohibited.
 *
 *
 * THE ENTIRE NOTICE ABOVE MUST BE REPRODUCED ON ALL AUTHORIZED COPIES.
 *
 * <h2><center>&copy; (C) COPYRIGHT 2023 SYNOPSYS, INC.</center></h2>
 * <h2><center>&copy; 	ALL RIGHTS RESERVED</center></h2>
 *
 * \n\n<b>References</b>\n
 * -Documents folder .
 *
 * <b>Edit History For File</b>\n
 *  This section contains comments describing changes made to this file.\n
 *  Notice that changes are listed in reverse chronological order.\n
 * <table border>
 * <tr>
 *   <td><b> PLEASE UPDATE with when date </b></td>
 *   <td><b> PLEASE UPDATE with who made the changes </b></td>
 *   <td><b> PLEASE UPDATE WITH what, where, why the changes were made</b></td>
 * </tr>
 * </table>\n
 */


#ifndef MAC_CONTROLLER_INC_RFD_DEV_CONFIG_H_
#define MAC_CONTROLLER_INC_RFD_DEV_CONFIG_H_

#include "ll_fw_config.h"


#if (!FFD_DEVICE_CONFIG)  /* RFD Device Configuration */

#define RFD_SUPPORT_ACTIVE_SCAN						1 /* Enable\Disable :RFD supports Active Scanning Enable:1 - Disable:0 */
#define RFD_SUPPORT_ENERGY_DETECT					1 /* Enable\Disable :RFD supports Energy Detection Enable:1 - Disable:0 */
#define RFD_SUPPORT_DATA_PURGE						1 /* Enable\Disable :RFD supports Data Purge Primitive Enable:1 - Disable:0 */
#define RFD_SUPPORT_ASSOCIATION_IND_RSP				1 /* Enable\Disable :RFD supports Association Indication and Response Primitives Enable:1 - Disable:0 */
#define RFD_SUPPORT_ORPHAN_IND_RSP					1 /* Enable\Disable :RFD supports Orphan Indication and Response Primitives Enable:1 - Disable:0 */
#define RFD_SUPPORT_START_PRIM						1 /* Enable\Disable :RFD supports Start Primitive Enable:1 - Disable:0 */
#define RFD_SUPPORT_PROMISCUOUS_MODE				1 /* Enable\Disable :RFD supports Promiscuous Mode Enable:1 - Disable:0 */
#define RFD_SUPPORT_SEND_BEACON						1 /* Enable\Disable :RFD supports Sending Beacons if Coordinator Enable:1 - Disable:0 */
#define RFD_SUPPORT_PANID_CONFLICT_RSLN				1 /* Enable\Disable :RFD supports Pan Id conflict detection and resolution Enable:1 - Disable:0 */

#endif


#endif /* MAC_CONTROLLER_INC_RFD_DEV_CONFIG_H_ */
