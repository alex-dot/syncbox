/**
 * \file        tarmuft_fsm - tarmuft state machine engine
 * \brief       Provides a finite state machine API.  
 * \author      Alexander Herr
 * \date        2016
 * \copyright   GNU Public License v3 or higher. 
 *
 *  ** WARNING *************************************************************
 *  THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
 *  your changes at the next build cycle. This is great for temporary printf
 *  statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
 *  for commits are:
 *
 *   * The XML model used for this code generation: tarmuft_fsm.xml, or
 *   * The code generation script that built this file: fsm_c.gsl
 *  ************************************************************************
 */

#ifndef TARMUFT_FSM_H_
#define TARMUFT_FSM_H_

#ifdef __cplusplus
extern "C" {
#endif

//  ---------------------------------------------------------------------------
//  State machine constants

typedef enum {
    NULL_state = 0,
    ready_state = 1,
    announcing_new_file_state = 2,
    announcing_new_file_with_more_state = 3,
    sending_new_file_metadata_state = 4,
    sending_new_file_metadata_with_more_state = 5,
    sending_new_file_state = 6,
    sending_new_file_with_more_state = 7,
    acknowledging_new_file_state = 8,
    acknowledging_new_file_with_more_state = 9,
    promoting_new_file_metadata_state = 10,
    promoting_new_file_metadata_with_more_state = 11,
    sending_fake_file_state = 12,
    sending_fake_file_with_more_state = 13,
    broadcasting_all_received_state = 14,
    broadcasting_all_received_with_more_state = 15,
    exit_state = 16
} state_t;

typedef enum {
    NULL_status = 0,
    status_100 = 100,
    status_110 = 110,
    status_111 = 111,
    status_112 = 112,
    status_113 = 113,
    status_114 = 114,
    status_115 = 115,
    status_120 = 120,
    status_121 = 121,
    status_122 = 122,
    status_124 = 124,
    status_125 = 125,
    status_126 = 126,
    status_130 = 130,
    status_131 = 131,
    status_132 = 132,
    status_134 = 134,
    status_135 = 135,
    status_136 = 136,
    status_140 = 140,
    status_142 = 142,
    status_145 = 145,
    status_150 = 150,
    status_200 = 200,
    status_210 = 210,
    status_300 = 300,
    status_304 = 304,
    status_410 = 410,
    status_420 = 420,
    status_500 = 500
} status_t;

typedef enum {
    NULL_event = 0,
    new_local_file_event = 1,
    new_local_file_with_more_event = 2,
    received_heartbeat_event = 3,
    all_nodes_replied_event = 4,
    timing_offset_elapsed_event = 5,
    file_send_tick_event = 6,
    received_file_data_event = 7,
    all_nodes_have_file_event = 8,
    node_only_sends_heartbeats_event = 9,
    all_data_received_event = 10,
    interrupt_event = 11
} event_t;

typedef enum {
    NULL_action = 0,
    send_heartbeat_action = 1,
    send_file_data_action = 2,
    ignore_action = 3,
    add_new_file_to_queue_action = 4,
    store_file_data_action = 5,
    send_fake_file_data_action = 6,
    exit_action = 7
} action_t;

//  ---------------------------------------------------------------------------
//  State machine API

static inline event_t
  get_event_by_status_code(status_t status) {
    if ( status == status_100 ) {
        return node_only_sends_heartbeats_event;
    } else if ( status == status_110 ) {
        return received_heartbeat_event;
    } else if ( status == status_111 ) {
        return received_heartbeat_event;
    } else if ( status == status_112 ) {
        return file_send_tick_event;
    } else if ( status == status_113 ) {
        return all_data_received_event;
    } else if ( status == status_114 ) {
        return received_heartbeat_event;
    } else if ( status == status_115 ) {
        return received_heartbeat_event;
    } else if ( status == status_120 ) {
        return received_heartbeat_event;
    } else if ( status == status_121 ) {
        return received_heartbeat_event;
    } else if ( status == status_122 ) {
        return all_nodes_replied_event;
    } else if ( status == status_124 ) {
        return received_heartbeat_event;
    } else if ( status == status_125 ) {
        return received_heartbeat_event;
    } else if ( status == status_126 ) {
        return all_nodes_replied_event;
    } else if ( status == status_130 ) {
        return received_heartbeat_event;
    } else if ( status == status_131 ) {
        return received_heartbeat_event;
    } else if ( status == status_132 ) {
        return timing_offset_elapsed_event;
    } else if ( status == status_134 ) {
        return received_heartbeat_event;
    } else if ( status == status_135 ) {
        return received_heartbeat_event;
    } else if ( status == status_136 ) {
        return timing_offset_elapsed_event;
    } else if ( status == status_140 ) {
        return received_heartbeat_event;
    } else if ( status == status_142 ) {
        return all_nodes_have_file_event;
    } else if ( status == status_145 ) {
        return received_heartbeat_event;
    } else if ( status == status_150 ) {
        return received_heartbeat_event;
    } else if ( status == status_200 ) {
        return received_file_data_event;
    } else if ( status == status_210 ) {
        return received_file_data_event;
    } else if ( status == status_300 ) {
        return new_local_file_event;
    } else if ( status == status_304 ) {
        return new_local_file_with_more_event;
    } else if ( status == status_410 ) {
        return received_heartbeat_event;
    } else if ( status == status_420 ) {
        return received_heartbeat_event;
    } else if ( status == status_500 ) {
        return interrupt_event;
    } else {
        return NULL_event;
    }
}

static inline bool
  check_event(state_t state, event_t event, status_t status) {
    if ( state == ready_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_120 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_121 ) {
            return true;
        } else if ( event == all_nodes_replied_event && status == status_122 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_125 ) {
            return true;
        } else if ( event == all_nodes_replied_event && status == status_126 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_131 ) {
            return true;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_135 ) {
            return true;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_new_file_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_111 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return true;
        } else if ( event == received_file_data_event && status == status_210 ) {
            return true;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_new_file_with_more_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return true;
        } else if ( event == received_file_data_event && status == status_210 ) {
            return true;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == acknowledging_new_file_state ) {
        if ( event == received_heartbeat_event && status == status_130 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_120 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_121 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == acknowledging_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_134 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_125 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == promoting_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && status == status_130 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_131 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_134 ) {
            return true;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == promoting_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_134 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_135 ) {
            return true;
        } else if ( event == timing_offset_elapsed_event && status == status_136 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_fake_file_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_110 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_111 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return true;
        } else if ( event == received_file_data_event && status == status_200 ) {
            return true;
        } else if ( event == all_data_received_event && status == status_113 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_fake_file_with_more_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_145 ) {
            return true;
        } else if ( event == received_file_data_event && status == status_200 ) {
            return true;
        } else if ( event == all_data_received_event && status == status_113 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == broadcasting_all_received_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_110 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_111 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return true;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == broadcasting_all_received_with_more_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_145 ) {
            return true;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == exit_state ) {
        if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

static inline action_t
  get_action(state_t state, event_t event, status_t status) {
    if ( state == ready_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_120 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_121 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_replied_event && status == status_122 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_125 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_replied_event && status == status_126 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_131 ) {
            return send_heartbeat_action;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_135 ) {
            return send_heartbeat_action;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_new_file_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_111 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return send_heartbeat_action;
        } else if ( event == received_file_data_event && status == status_210 ) {
            return ignore_action;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return send_heartbeat_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_new_file_with_more_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return send_heartbeat_action;
        } else if ( event == received_file_data_event && status == status_210 ) {
            return ignore_action;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return send_heartbeat_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == acknowledging_new_file_state ) {
        if ( event == received_heartbeat_event && status == status_130 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_120 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_121 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == acknowledging_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_134 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_125 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == promoting_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && status == status_130 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_131 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_134 ) {
            return send_heartbeat_action;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == promoting_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_134 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_135 ) {
            return send_heartbeat_action;
        } else if ( event == timing_offset_elapsed_event && status == status_136 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_fake_file_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_110 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_111 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return send_heartbeat_action;
        } else if ( event == received_file_data_event && status == status_200 ) {
            return store_file_data_action;
        } else if ( event == all_data_received_event && status == status_113 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return send_heartbeat_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_fake_file_with_more_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_145 ) {
            return send_heartbeat_action;
        } else if ( event == received_file_data_event && status == status_200 ) {
            return store_file_data_action;
        } else if ( event == all_data_received_event && status == status_113 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_410 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_420 ) {
            return send_heartbeat_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == broadcasting_all_received_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_fake_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_110 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_111 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_140 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == broadcasting_all_received_with_more_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_fake_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_145 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_have_file_event && status == status_142 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == exit_state ) {
        if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else {
        return NULL_action;
    }
}

static inline status_t
  get_heartbeat_status(state_t state, event_t event, status_t received_status) {
    if ( state == ready_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return status_120;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_124;
        } else if ( event == received_heartbeat_event && received_status == status_120 ) {
            return status_121;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return status_125;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return status_124;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_124;
        } else if ( event == received_heartbeat_event && received_status == status_121 ) {
            return status_120;
        } else if ( event == all_nodes_replied_event && received_status == status_122 ) {
            return status_130;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return status_124;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_124;
        } else if ( event == received_heartbeat_event && received_status == status_125 ) {
            return status_124;
        } else if ( event == all_nodes_replied_event && received_status == status_126 ) {
            return status_134;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return status_134;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_134;
        } else if ( event == received_heartbeat_event && received_status == status_131 ) {
            return status_130;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return status_110;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return status_134;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_134;
        } else if ( event == received_heartbeat_event && received_status == status_135 ) {
            return status_134;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return status_114;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_new_file_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_200;
        } else if ( event == received_heartbeat_event && received_status == status_111 ) {
            return status_110;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return status_110;
        } else if ( event == received_file_data_event && received_status == status_210 ) {
            return status_110;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return status_150;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_150;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_110;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_110;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return status_410;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return status_420;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_new_file_with_more_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_200;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return status_114;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return status_114;
        } else if ( event == received_file_data_event && received_status == status_210 ) {
            return status_114;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return status_110;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_110;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_114;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_114;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return status_410;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return status_420;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == acknowledging_new_file_state ) {
        if ( event == received_heartbeat_event && received_status == status_130 ) {
            return status_131;
        } else if ( event == received_heartbeat_event && received_status == status_120 ) {
            return status_121;
        } else if ( event == received_heartbeat_event && received_status == status_121 ) {
            return status_121;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return status_125;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_121;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_121;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == acknowledging_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_134 ) {
            return status_131;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return status_125;
        } else if ( event == received_heartbeat_event && received_status == status_125 ) {
            return status_125;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_125;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_125;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == promoting_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && received_status == status_130 ) {
            return status_131;
        } else if ( event == received_heartbeat_event && received_status == status_131 ) {
            return status_131;
        } else if ( event == received_heartbeat_event && received_status == status_134 ) {
            return status_135;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return status_111;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_131;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_131;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == promoting_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_134 ) {
            return status_135;
        } else if ( event == received_heartbeat_event && received_status == status_135 ) {
            return status_135;
        } else if ( event == timing_offset_elapsed_event && received_status == status_136 ) {
            return status_115;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_135;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_135;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_fake_file_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_210;
        } else if ( event == received_heartbeat_event && received_status == status_110 ) {
            return status_111;
        } else if ( event == received_heartbeat_event && received_status == status_111 ) {
            return status_111;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return status_111;
        } else if ( event == received_file_data_event && received_status == status_200 ) {
            return status_111;
        } else if ( event == all_data_received_event && received_status == status_113 ) {
            return status_140;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_111;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_111;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_420;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return status_410;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return status_420;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_fake_file_with_more_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_210;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return status_115;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return status_115;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return status_115;
        } else if ( event == received_file_data_event && received_status == status_200 ) {
            return status_115;
        } else if ( event == all_data_received_event && received_status == status_113 ) {
            return status_145;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_115;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_115;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_420;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return status_410;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return status_420;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == broadcasting_all_received_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_210;
        } else if ( event == received_heartbeat_event && received_status == status_110 ) {
            return status_140;
        } else if ( event == received_heartbeat_event && received_status == status_111 ) {
            return status_140;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return status_140;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return status_150;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_150;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_140;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_140;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == broadcasting_all_received_with_more_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_210;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return status_145;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return status_145;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return status_145;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return status_150;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_150;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_145;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_145;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == exit_state ) {
        if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else {
        return NULL_status;
    }
}

static inline state_t
  get_new_state(state_t state, event_t event, status_t received_status) {
    if ( state == ready_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return announcing_new_file_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_120 ) {
            return acknowledging_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_121 ) {
            return announcing_new_file_state;
        } else if ( event == all_nodes_replied_event && received_status == status_122 ) {
            return sending_new_file_metadata_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_125 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == all_nodes_replied_event && received_status == status_126 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_131 ) {
            return sending_new_file_metadata_state;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return sending_new_file_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_135 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return sending_new_file_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_new_file_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_111 ) {
            return sending_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return sending_new_file_state;
        } else if ( event == received_file_data_event && received_status == status_210 ) {
            return sending_new_file_state;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return ready_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return ready_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_new_file_with_more_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return sending_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return sending_new_file_with_more_state;
        } else if ( event == received_file_data_event && received_status == status_210 ) {
            return sending_new_file_with_more_state;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return sending_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return sending_new_file_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return ready_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == acknowledging_new_file_state ) {
        if ( event == received_heartbeat_event && received_status == status_130 ) {
            return promoting_new_file_metadata_state;
        } else if ( event == received_heartbeat_event && received_status == status_120 ) {
            return acknowledging_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_121 ) {
            return acknowledging_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return acknowledging_new_file_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return acknowledging_new_file_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == acknowledging_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_134 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_125 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == promoting_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && received_status == status_130 ) {
            return promoting_new_file_metadata_state;
        } else if ( event == received_heartbeat_event && received_status == status_131 ) {
            return promoting_new_file_metadata_state;
        } else if ( event == received_heartbeat_event && received_status == status_134 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return sending_fake_file_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return promoting_new_file_metadata_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return promoting_new_file_metadata_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == promoting_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_134 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_135 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == timing_offset_elapsed_event && received_status == status_136 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_fake_file_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_fake_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_110 ) {
            return sending_fake_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_111 ) {
            return sending_fake_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return sending_fake_file_state;
        } else if ( event == received_file_data_event && received_status == status_200 ) {
            return sending_fake_file_state;
        } else if ( event == all_data_received_event && received_status == status_113 ) {
            return broadcasting_all_received_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_fake_file_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_fake_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return ready_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_fake_file_with_more_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == received_file_data_event && received_status == status_200 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == all_data_received_event && received_status == status_113 ) {
            return broadcasting_all_received_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_fake_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_410 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_420 ) {
            return ready_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == broadcasting_all_received_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return broadcasting_all_received_state;
        } else if ( event == received_heartbeat_event && received_status == status_110 ) {
            return broadcasting_all_received_state;
        } else if ( event == received_heartbeat_event && received_status == status_111 ) {
            return broadcasting_all_received_state;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return broadcasting_all_received_state;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return ready_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return broadcasting_all_received_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return broadcasting_all_received_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == broadcasting_all_received_with_more_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return broadcasting_all_received_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return broadcasting_all_received_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return broadcasting_all_received_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return broadcasting_all_received_with_more_state;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return sending_fake_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return sending_fake_file_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return broadcasting_all_received_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return broadcasting_all_received_with_more_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == exit_state ) {
        if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else {
        return NULL_state;
    }
}

#ifdef __cplusplus
}
#endif

#endif  // TARMUFT_FSM_H_
