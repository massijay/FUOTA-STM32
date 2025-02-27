/*
 * fuota_state_machine.c
 *
 *  Created on: Oct 25, 2024
 *      Author: Massimiliano Cristarella
 */

#include "fuota_state_machine.h"

/**
  * @brief  Perform the specified transition if it's allowed
  * @param  currentState: Pointer to the current state of the machine. If the transition is successful, its value is updated
  * @param  transition: Transition to attempt
  * @retval FuotaTransition_Result: Result enum indicating whether the transition was successful or not
  */
FuotaTransition_Result TryTransition(FuotaState* currentState, FuotaTransition transition)
{
  FuotaTransition_Result result = FUOTA_TRANSITION_FAILED;
  switch (*currentState)
  {
    case STATE_IDLE:
      switch (transition)
      {
        case TRANSITION_UPD_REQUEST:
          *currentState = STATE_UPD_REQ_RECEIVED;
          result = FUOTA_TRANSITION_OK;
          break;
        default:
          break;
      }
      break;
    case STATE_UPD_REQ_RECEIVED:
      switch (transition)
      {
        case TRANSITION_UPD_CANCEL:
          *currentState = STATE_IDLE;
          result = FUOTA_TRANSITION_OK;
          break;
        case TRANSITION_DATA_DOWNLOAD:
          *currentState = STATE_UPD_DOWNLOADING;
          result = FUOTA_TRANSITION_OK;
          break;
        case TRANSITION_UPD_REQUEST:
          // State does not change
          result = FUOTA_TRANSITION_OK;
          break;
        default:
          break;
      }
      break;
    case STATE_UPD_DOWNLOADING:
      switch (transition)
      {
        case TRANSITION_UPD_CANCEL:
          *currentState = STATE_IDLE;
          result = FUOTA_TRANSITION_OK;
          break;
        case TRANSITION_DATA_DOWNLOAD:
          // State does not change
          result = FUOTA_TRANSITION_OK;
          break;
        case TRANSITION_DOWNLOAD_FINISH:
          *currentState = STATE_UPD_VERIFYING;
          result = FUOTA_TRANSITION_OK;
          break;
        case TRANSITION_UPD_FAILED:
          *currentState = STATE_IDLE;
          result = FUOTA_TRANSITION_OK;
          break;
        default:
          break;
      }
      break;
    case STATE_UPD_VERIFYING:
      switch (transition)
      {
        case TRANSITION_UPD_FAILED:
          *currentState = STATE_IDLE;
          result = FUOTA_TRANSITION_OK;
          break;
        case TRANSITION_UPD_SUCCESS:
          *currentState = STATE_IDLE;
          result = FUOTA_TRANSITION_OK;
          break;
        default:
          break;
      }
      break;
    default:
      // State not implemented
      break;
  }
  return result;
}



