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
    announcing_file_metadata_change_state = 4,
    announcing_file_metadata_change_with_more_state = 5,
    sending_new_file_metadata_state = 6,
    sending_new_file_metadata_with_more_state = 7,
    sending_file_metadata_change_state = 8,
    sending_file_metadata_change_with_more_state = 9,
    sending_new_file_state = 10,
    sending_new_file_with_more_alpha_state = 11,
    sending_new_file_with_more_beta_state = 12,
    acknowledging_new_file_state = 13,
    acknowledging_new_file_with_more_state = 14,
    acknowledging_file_metadata_change_state = 15,
    acknowledging_file_metadata_change_with_more_state = 16,
    promoting_new_file_metadata_state = 17,
    promoting_new_file_metadata_with_more_state = 18,
    receiving_file_metadata_change_state = 19,
    receiving_file_metadata_change_with_more_state = 20,
    sending_fake_file_state = 21,
    sending_fake_file_with_more_alpha_state = 22,
    sending_fake_file_with_more_beta_state = 23,
    broadcasting_all_received_state = 24,
    broadcasting_all_received_with_more_alpha_state = 25,
    broadcasting_all_received_with_more_beta_state = 26,
    broadcasting_all_metadata_received_state = 27,
    exit_state = 28
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
    status_116 = 116,
    status_117 = 117,
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
    status_141 = 141,
    status_142 = 142,
    status_145 = 145,
    status_146 = 146,
    status_147 = 147,
    status_150 = 150,
    status_160 = 160,
    status_161 = 161,
    status_162 = 162,
    status_164 = 164,
    status_165 = 165,
    status_166 = 166,
    status_170 = 170,
    status_171 = 171,
    status_172 = 172,
    status_173 = 173,
    status_174 = 174,
    status_175 = 175,
    status_177 = 177,
    status_178 = 178,
    status_200 = 200,
    status_210 = 210,
    status_300 = 300,
    status_304 = 304,
    status_320 = 320,
    status_324 = 324,
    status_410 = 410,
    status_420 = 420,
    status_500 = 500
} status_t;

typedef enum {
    NULL_event = 0,
    new_local_file_event = 1,
    new_local_file_with_more_event = 2,
    local_file_metadata_change_event = 3,
    local_file_metadata_change_with_more_event = 4,
    received_heartbeat_event = 5,
    all_nodes_replied_event = 6,
    timing_offset_elapsed_event = 7,
    all_nodes_have_file_event = 8,
    all_nodes_have_all_files_event = 9,
    all_nodes_have_all_metadata_changes_with_more_event = 10,
    file_send_tick_event = 11,
    received_file_data_event = 12,
    node_only_sends_heartbeats_event = 13,
    all_nodes_have_file_with_more_event = 14,
    all_metadata_received_event = 15,
    all_data_received_event = 16,
    interrupt_event = 17
} event_t;

typedef enum {
    NULL_action = 0,
    send_heartbeat_action = 1,
    add_new_file_to_queue_action = 2,
    send_file_data_action = 3,
    ignore_action = 4,
    store_file_data_action = 5,
    send_fake_file_data_action = 6,
    exit_action = 7
} action_t;

//  ---------------------------------------------------------------------------
//  State machine API

static inline event_t
  get_event_by_status_code(status_t status) {
    if ( status == status_100 ) {
        return received_heartbeat_event;
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
    } else if ( status == status_116 ) {
        return received_heartbeat_event;
    } else if ( status == status_117 ) {
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
    } else if ( status == status_141 ) {
        return received_heartbeat_event;
    } else if ( status == status_142 ) {
        return all_nodes_have_file_event;
    } else if ( status == status_145 ) {
        return received_heartbeat_event;
    } else if ( status == status_146 ) {
        return all_nodes_have_file_with_more_event;
    } else if ( status == status_147 ) {
        return received_heartbeat_event;
    } else if ( status == status_150 ) {
        return received_heartbeat_event;
    } else if ( status == status_160 ) {
        return received_heartbeat_event;
    } else if ( status == status_161 ) {
        return received_heartbeat_event;
    } else if ( status == status_162 ) {
        return all_nodes_replied_event;
    } else if ( status == status_164 ) {
        return received_heartbeat_event;
    } else if ( status == status_165 ) {
        return received_heartbeat_event;
    } else if ( status == status_166 ) {
        return all_nodes_replied_event;
    } else if ( status == status_170 ) {
        return received_heartbeat_event;
    } else if ( status == status_171 ) {
        return received_heartbeat_event;
    } else if ( status == status_172 ) {
        return all_nodes_have_file_event;
    } else if ( status == status_173 ) {
        return all_metadata_received_event;
    } else if ( status == status_174 ) {
        return received_heartbeat_event;
    } else if ( status == status_175 ) {
        return received_heartbeat_event;
    } else if ( status == status_177 ) {
        return all_nodes_have_all_metadata_changes_with_more_event;
    } else if ( status == status_178 ) {
        return all_nodes_have_all_metadata_changes_with_more_event;
    } else if ( status == status_200 ) {
        return received_file_data_event;
    } else if ( status == status_210 ) {
        return received_file_data_event;
    } else if ( status == status_300 ) {
        return new_local_file_event;
    } else if ( status == status_304 ) {
        return new_local_file_with_more_event;
    } else if ( status == status_320 ) {
        return local_file_metadata_change_event;
    } else if ( status == status_324 ) {
        return local_file_metadata_change_with_more_event;
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
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_100 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_120 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_160 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_164 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == received_heartbeat_event && status == status_121 ) {
            return true;
        } else if ( event == all_nodes_replied_event && status == status_122 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_125 ) {
            return true;
        } else if ( event == all_nodes_replied_event && status == status_126 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == announcing_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_161 ) {
            return true;
        } else if ( event == all_nodes_replied_event && status == status_162 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == announcing_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_165 ) {
            return true;
        } else if ( event == all_nodes_replied_event && status == status_166 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && status == status_131 ) {
            return true;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_135 ) {
            return true;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_171 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return true;
        } else if ( event == all_nodes_have_file_event && status == status_172 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == sending_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_175 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return true;
        } else if ( event == all_nodes_have_all_files_event && status == status_172 ) {
            return true;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && status == status_177 ) {
            return true;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && status == status_178 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_new_file_with_more_alpha_state ) {
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
        } else if ( event == all_nodes_have_file_with_more_event && status == status_146 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_new_file_with_more_beta_state ) {
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
        } else if ( event == all_nodes_have_file_with_more_event && status == status_146 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return true;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == acknowledging_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_170 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_160 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_161 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_164 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == acknowledging_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_174 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_164 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_165 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == receiving_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_170 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_171 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_175 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return true;
        } else if ( event == all_metadata_received_event && status == status_173 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == receiving_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_174 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_175 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return true;
        } else if ( event == all_metadata_received_event && status == status_173 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_fake_file_with_more_alpha_state ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_fake_file_with_more_beta_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_117 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == broadcasting_all_received_with_more_alpha_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_145 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_116 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == broadcasting_all_received_with_more_beta_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_116 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_117 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_147 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return true;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return true;
        } else if ( event == interrupt_event && status == status_500 ) {
            return true;
        } else {
            return false;
        }
    } else if ( state == broadcasting_all_metadata_received_state ) {
        if ( event == received_heartbeat_event && status == status_170 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_171 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_130 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_134 ) {
            return true;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return true;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return true;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_120 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_124 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_160 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_164 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == received_heartbeat_event && status == status_121 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_replied_event && status == status_122 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_125 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_replied_event && status == status_126 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == announcing_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_161 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_replied_event && status == status_162 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == announcing_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_165 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_replied_event && status == status_166 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && status == status_131 ) {
            return send_heartbeat_action;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_135 ) {
            return send_heartbeat_action;
        } else if ( event == timing_offset_elapsed_event && status == status_132 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_171 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_have_file_event && status == status_172 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == sending_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_175 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_have_all_files_event && status == status_172 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && status == status_177 ) {
            return send_heartbeat_action;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && status == status_178 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return send_heartbeat_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_new_file_with_more_alpha_state ) {
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
        } else if ( event == all_nodes_have_file_with_more_event && status == status_146 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_new_file_with_more_beta_state ) {
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
        } else if ( event == all_nodes_have_file_with_more_event && status == status_146 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == local_file_metadata_change_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == acknowledging_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_170 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_160 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_161 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_164 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == acknowledging_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_174 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_164 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_165 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == receiving_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && status == status_170 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_171 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_175 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return send_heartbeat_action;
        } else if ( event == all_metadata_received_event && status == status_173 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == receiving_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && status == status_174 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_175 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return send_heartbeat_action;
        } else if ( event == all_metadata_received_event && status == status_173 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_fake_file_with_more_alpha_state ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
    } else if ( state == sending_fake_file_with_more_beta_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_117 ) {
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
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == broadcasting_all_received_with_more_alpha_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_fake_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_115 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_145 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_116 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == broadcasting_all_received_with_more_beta_state ) {
        if ( event == file_send_tick_event && status == status_112 ) {
            return send_fake_file_data_action;
        } else if ( event == received_heartbeat_event && status == status_116 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_117 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_147 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_114 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
            return add_new_file_to_queue_action;
        } else if ( event == node_only_sends_heartbeats_event && status == status_100 ) {
            return send_heartbeat_action;
        } else if ( event == interrupt_event && status == status_500 ) {
            return exit_action;
        } else {
            return NULL_action;
        }
    } else if ( state == broadcasting_all_metadata_received_state ) {
        if ( event == received_heartbeat_event && status == status_170 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_171 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_141 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_130 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_134 ) {
            return send_heartbeat_action;
        } else if ( event == received_heartbeat_event && status == status_150 ) {
            return send_heartbeat_action;
        } else if ( event == new_local_file_event && status == status_300 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_304 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_event && status == status_320 ) {
            return add_new_file_to_queue_action;
        } else if ( event == new_local_file_with_more_event && status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_160;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_164;
        } else if ( event == received_heartbeat_event && received_status == status_100 ) {
            return status_100;
        } else if ( event == received_heartbeat_event && received_status == status_120 ) {
            return status_121;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return status_125;
        } else if ( event == received_heartbeat_event && received_status == status_160 ) {
            return status_161;
        } else if ( event == received_heartbeat_event && received_status == status_164 ) {
            return status_165;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_100;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == received_heartbeat_event && received_status == status_121 ) {
            return status_120;
        } else if ( event == all_nodes_replied_event && received_status == status_122 ) {
            return status_130;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_124;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_124;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_164;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_164;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_125 ) {
            return status_124;
        } else if ( event == all_nodes_replied_event && received_status == status_126 ) {
            return status_134;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_124;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_124;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_164;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_164;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == announcing_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_161 ) {
            return status_160;
        } else if ( event == all_nodes_replied_event && received_status == status_162 ) {
            return status_170;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_164;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_164;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_164;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_164;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == announcing_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_165 ) {
            return status_164;
        } else if ( event == all_nodes_replied_event && received_status == status_166 ) {
            return status_174;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_164;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_164;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_164;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_164;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && received_status == status_131 ) {
            return status_130;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return status_110;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_130;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_130;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_130;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_130;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_135 ) {
            return status_134;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return status_114;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_134;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_134;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_134;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_134;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_171 ) {
            return status_170;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return status_170;
        } else if ( event == all_nodes_have_file_event && received_status == status_172 ) {
            return status_150;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_170;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_170;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_170;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_170;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == sending_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_175 ) {
            return status_174;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return status_174;
        } else if ( event == all_nodes_have_all_files_event && received_status == status_172 ) {
            return status_150;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && received_status == status_177 ) {
            return status_130;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && received_status == status_178 ) {
            return status_134;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_174;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_174;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_174;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_174;
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
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_110;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_110;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_110;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
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
    } else if ( state == sending_new_file_with_more_alpha_state ) {
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
        } else if ( event == all_nodes_have_file_with_more_event && received_status == status_146 ) {
            return status_116;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_114;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_114;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_114;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
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
    } else if ( state == sending_new_file_with_more_beta_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_200;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return status_116;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return status_116;
        } else if ( event == received_file_data_event && received_status == status_210 ) {
            return status_116;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return status_110;
        } else if ( event == all_nodes_have_file_with_more_event && received_status == status_146 ) {
            return status_114;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_116;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_116;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_116;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_116;
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
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_121;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return status_125;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return status_125;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == acknowledging_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_170 ) {
            return status_171;
        } else if ( event == received_heartbeat_event && received_status == status_160 ) {
            return status_161;
        } else if ( event == received_heartbeat_event && received_status == status_161 ) {
            return status_161;
        } else if ( event == received_heartbeat_event && received_status == status_164 ) {
            return status_165;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_161;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_161;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_161;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_161;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == acknowledging_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_174 ) {
            return status_175;
        } else if ( event == received_heartbeat_event && received_status == status_164 ) {
            return status_165;
        } else if ( event == received_heartbeat_event && received_status == status_165 ) {
            return status_165;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_165;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_165;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_165;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_165;
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
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_131;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
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
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_135;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_135;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == receiving_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_170 ) {
            return status_171;
        } else if ( event == received_heartbeat_event && received_status == status_171 ) {
            return status_171;
        } else if ( event == received_heartbeat_event && received_status == status_175 ) {
            return status_171;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return status_171;
        } else if ( event == all_metadata_received_event && received_status == status_173 ) {
            return status_141;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_171;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_171;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_171;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_171;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == receiving_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_174 ) {
            return status_175;
        } else if ( event == received_heartbeat_event && received_status == status_175 ) {
            return status_175;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return status_175;
        } else if ( event == all_metadata_received_event && received_status == status_173 ) {
            return status_141;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_175;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_175;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_175;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_175;
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
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_111;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
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
    } else if ( state == sending_fake_file_with_more_alpha_state ) {
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
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_115;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
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
    } else if ( state == sending_fake_file_with_more_beta_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_210;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return status_117;
        } else if ( event == received_heartbeat_event && received_status == status_117 ) {
            return status_117;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return status_117;
        } else if ( event == received_file_data_event && received_status == status_200 ) {
            return status_117;
        } else if ( event == all_data_received_event && received_status == status_113 ) {
            return status_147;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_117;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_117;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_117;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_117;
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
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_150;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_140;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_140;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_140;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_140;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == broadcasting_all_received_with_more_alpha_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_210;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return status_145;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return status_145;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return status_145;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_150;
        } else if ( event == received_heartbeat_event && received_status == status_116 ) {
            return status_117;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_145;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_145;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_145;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_145;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == broadcasting_all_received_with_more_beta_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return status_210;
        } else if ( event == received_heartbeat_event && received_status == status_116 ) {
            return status_147;
        } else if ( event == received_heartbeat_event && received_status == status_117 ) {
            return status_147;
        } else if ( event == received_heartbeat_event && received_status == status_147 ) {
            return status_147;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_150;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return status_115;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_147;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_147;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_147;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_147;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return status_410;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return status_500;
        } else {
            return NULL_status;
        }
    } else if ( state == broadcasting_all_metadata_received_state ) {
        if ( event == received_heartbeat_event && received_status == status_170 ) {
            return status_141;
        } else if ( event == received_heartbeat_event && received_status == status_171 ) {
            return status_141;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return status_141;
        } else if ( event == received_heartbeat_event && received_status == status_130 ) {
            return status_131;
        } else if ( event == received_heartbeat_event && received_status == status_134 ) {
            return status_135;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return status_150;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return status_141;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return status_141;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return status_141;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return status_141;
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
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return announcing_file_metadata_change_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == received_heartbeat_event && received_status == status_120 ) {
            return acknowledging_new_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_124 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_160 ) {
            return acknowledging_file_metadata_change_state;
        } else if ( event == received_heartbeat_event && received_status == status_164 ) {
            return acknowledging_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == announcing_new_file_state ) {
        if ( event == received_heartbeat_event && received_status == status_121 ) {
            return announcing_new_file_state;
        } else if ( event == all_nodes_replied_event && received_status == status_122 ) {
            return sending_new_file_metadata_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == announcing_new_file_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_125 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == all_nodes_replied_event && received_status == status_126 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return announcing_new_file_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == announcing_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_161 ) {
            return announcing_file_metadata_change_state;
        } else if ( event == all_nodes_replied_event && received_status == status_162 ) {
            return sending_file_metadata_change_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == announcing_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_165 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == all_nodes_replied_event && received_status == status_166 ) {
            return sending_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return announcing_file_metadata_change_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_new_file_metadata_state ) {
        if ( event == received_heartbeat_event && received_status == status_131 ) {
            return sending_new_file_metadata_state;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return sending_new_file_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_metadata_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_metadata_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return sending_new_file_metadata_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return sending_new_file_metadata_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_new_file_metadata_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_135 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == timing_offset_elapsed_event && received_status == status_132 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_171 ) {
            return sending_file_metadata_change_state;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return sending_file_metadata_change_state;
        } else if ( event == all_nodes_have_file_event && received_status == status_172 ) {
            return ready_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_file_metadata_change_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_file_metadata_change_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return sending_file_metadata_change_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return sending_file_metadata_change_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == sending_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_175 ) {
            return sending_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return sending_file_metadata_change_with_more_state;
        } else if ( event == all_nodes_have_all_files_event && received_status == status_172 ) {
            return ready_state;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && received_status == status_177 ) {
            return sending_new_file_metadata_state;
        } else if ( event == all_nodes_have_all_metadata_changes_with_more_event && received_status == status_178 ) {
            return sending_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_file_metadata_change_with_more_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return sending_file_metadata_change_with_more_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return sending_file_metadata_change_with_more_state;
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
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return sending_new_file_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
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
    } else if ( state == sending_new_file_with_more_alpha_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == received_file_data_event && received_status == status_210 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return sending_new_file_state;
        } else if ( event == all_nodes_have_file_with_more_event && received_status == status_146 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return sending_new_file_with_more_alpha_state;
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
    } else if ( state == sending_new_file_with_more_beta_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_140 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == received_file_data_event && received_status == status_210 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == all_nodes_have_file_event && received_status == status_142 ) {
            return sending_new_file_state;
        } else if ( event == all_nodes_have_file_with_more_event && received_status == status_146 ) {
            return sending_new_file_with_more_alpha_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return sending_new_file_with_more_beta_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return sending_new_file_with_more_beta_state;
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
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return acknowledging_new_file_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
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
        } else if ( event == local_file_metadata_change_event && received_status == status_320 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == local_file_metadata_change_with_more_event && received_status == status_324 ) {
            return acknowledging_new_file_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == acknowledging_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_170 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == received_heartbeat_event && received_status == status_160 ) {
            return acknowledging_file_metadata_change_state;
        } else if ( event == received_heartbeat_event && received_status == status_161 ) {
            return acknowledging_file_metadata_change_state;
        } else if ( event == received_heartbeat_event && received_status == status_164 ) {
            return acknowledging_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return acknowledging_file_metadata_change_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return acknowledging_file_metadata_change_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return acknowledging_file_metadata_change_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return acknowledging_file_metadata_change_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == acknowledging_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_174 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_164 ) {
            return acknowledging_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_165 ) {
            return acknowledging_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return acknowledging_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return acknowledging_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return acknowledging_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return acknowledging_file_metadata_change_with_more_state;
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
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return promoting_new_file_metadata_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
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
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == receiving_file_metadata_change_state ) {
        if ( event == received_heartbeat_event && received_status == status_170 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == received_heartbeat_event && received_status == status_171 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == received_heartbeat_event && received_status == status_175 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == all_metadata_received_event && received_status == status_173 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return receiving_file_metadata_change_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == receiving_file_metadata_change_with_more_state ) {
        if ( event == received_heartbeat_event && received_status == status_174 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_175 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == all_metadata_received_event && received_status == status_173 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return receiving_file_metadata_change_with_more_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return receiving_file_metadata_change_with_more_state;
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
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return sending_fake_file_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
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
    } else if ( state == sending_fake_file_with_more_alpha_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == received_file_data_event && received_status == status_200 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == all_data_received_event && received_status == status_113 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return sending_fake_file_with_more_alpha_state;
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
    } else if ( state == sending_fake_file_with_more_beta_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_117 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == received_file_data_event && received_status == status_200 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == all_data_received_event && received_status == status_113 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return sending_fake_file_with_more_beta_state;
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
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return ready_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return broadcasting_all_received_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return broadcasting_all_received_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return broadcasting_all_received_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return broadcasting_all_received_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == broadcasting_all_received_with_more_alpha_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_115 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_145 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return sending_fake_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_116 ) {
            return sending_fake_file_with_more_beta_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return broadcasting_all_received_with_more_alpha_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == broadcasting_all_received_with_more_beta_state ) {
        if ( event == file_send_tick_event && received_status == status_112 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_116 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_117 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_147 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return sending_fake_file_state;
        } else if ( event == received_heartbeat_event && received_status == status_114 ) {
            return sending_fake_file_with_more_alpha_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return broadcasting_all_received_with_more_beta_state;
        } else if ( event == node_only_sends_heartbeats_event && received_status == status_100 ) {
            return ready_state;
        } else if ( event == interrupt_event && received_status == status_500 ) {
            return exit_state;
        } else {
            return NULL_state;
        }
    } else if ( state == broadcasting_all_metadata_received_state ) {
        if ( event == received_heartbeat_event && received_status == status_170 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == received_heartbeat_event && received_status == status_171 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == received_heartbeat_event && received_status == status_141 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == received_heartbeat_event && received_status == status_130 ) {
            return promoting_new_file_metadata_state;
        } else if ( event == received_heartbeat_event && received_status == status_134 ) {
            return promoting_new_file_metadata_with_more_state;
        } else if ( event == received_heartbeat_event && received_status == status_150 ) {
            return ready_state;
        } else if ( event == new_local_file_event && received_status == status_300 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_304 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == new_local_file_event && received_status == status_320 ) {
            return broadcasting_all_metadata_received_state;
        } else if ( event == new_local_file_with_more_event && received_status == status_324 ) {
            return broadcasting_all_metadata_received_state;
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
