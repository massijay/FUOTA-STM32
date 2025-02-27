/*
 * fuota_state_machine.h
 *
 *  Created on: Oct 25, 2024
 *      Author: Massimiliano Cristarella
 */

#ifndef INC_FUOTA_STATE_MACHINE_H_
#define INC_FUOTA_STATE_MACHINE_H_

typedef enum
{
  FUOTA_TRANSITION_FAILED = 0,
  FUOTA_TRANSITION_OK
} FuotaTransition_Result;

typedef enum
{
  STATE_IDLE,
  STATE_UPD_REQ_RECEIVED,
  STATE_UPD_DOWNLOADING,
  STATE_UPD_VERIFYING
} FuotaState;

typedef enum
{
  TRANSITION_UPD_REQUEST,
  TRANSITION_DATA_DOWNLOAD,
  TRANSITION_DOWNLOAD_FINISH,
  TRANSITION_UPD_CANCEL,
  TRANSITION_UPD_FAILED,
  TRANSITION_UPD_SUCCESS
} FuotaTransition;

FuotaTransition_Result TryTransition(FuotaState* currentState, FuotaTransition transition);

#endif /* INC_FUOTA_STATE_MACHINE_H_ */
