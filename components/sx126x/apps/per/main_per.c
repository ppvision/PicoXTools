/*!
 * @file      main_per.c
 *
 * @brief     Packet Error Rate (PER) example for SX126x chip
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2022. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>

#include "apps_common.h"
#include "sx126x.h"
#include "main_per.h"
#include "smtc_hal_mcu.h"
#include "smtc_hal_dbg_trace.h"
#include "uart_init.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

#if( RECEIVER == 1 )
const char* mode = "Receiver";
#else
const char* mode = "Transmitter";
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

static sx126x_hal_context_t* context;

static uint8_t buffer[PAYLOAD_LENGTH];

static uint16_t nb_ok         = 0;
static uint16_t nb_rx_timeout = 0;
static uint16_t nb_rx_error   = 0;

static uint8_t rolling_counter = 0;

static uint16_t per_index      = 0x00;
static bool     first_pkt_flag = false;

static uint8_t per_msg[PAYLOAD_LENGTH];

static uint32_t rx_timeout = RX_TIMEOUT_VALUE;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

uint32_t smtc_board_led_get( uint32_t led_mask );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

/**
 * @brief Main application entry point.
 */
int main( void )
{
    smtc_hal_mcu_init( );
    uart_init();

    HAL_DBG_TRACE_INFO( "===== SX126x PER example - %s =====\n\n", mode );
    apps_common_sx126x_print_version_info( );

    apps_common_shield_init( );
    context = apps_common_sx126x_get_context( );

    apps_common_sx126x_init( ( void* ) context );

    apps_common_sx126x_radio_init( ( void* ) context );

    sx126x_set_dio_irq_params(
        context, SX126X_IRQ_ALL,
        SX126X_IRQ_TX_DONE | SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT | SX126X_IRQ_HEADER_ERROR | SX126X_IRQ_CRC_ERROR,
        SX126X_IRQ_NONE, SX126X_IRQ_NONE );
    sx126x_clear_irq_status( context, SX126X_IRQ_ALL );

    for( int i = 1; i < PAYLOAD_LENGTH; i++ )
    {
        buffer[i] = i;
    }
    // Adjust RX_TIMEOUT regarding to time on air value
    rx_timeout += get_time_on_air_in_ms( );

#if( RECEIVER == 1 )
    sx126x_set_rx( context, rx_timeout );
    memcpy( per_msg, &buffer[1], PAYLOAD_LENGTH - 1 );
#else
    buffer[0] = 0;
    sx126x_write_buffer( context, 0, buffer, PAYLOAD_LENGTH );
    sx126x_set_tx( context, 0 );
#endif

    while( per_index < NB_FRAME )
    {
        apps_common_sx126x_irq_process( context );
    }

    if( per_index > NB_FRAME )  // The last validated packet should not be counted in this case
    {
        nb_ok--;
    }
    /* Display PER*/
    HAL_DBG_TRACE_PRINTF( "PER = %d \n", 100 - ( ( nb_ok * 100 ) / NB_FRAME ) );

    HAL_DBG_TRACE_PRINTF( "Final PER index: %d \n", per_index );
    HAL_DBG_TRACE_PRINTF( "Valid reception amount: %d \n", nb_ok );
    HAL_DBG_TRACE_PRINTF( "Timeout reception amount: %d \n", nb_rx_timeout );
    HAL_DBG_TRACE_PRINTF( "CRC Error reception amount: %d \n", nb_rx_error );

    while( 1 )
    {
    }
}

void on_tx_done( void )
{
    apps_common_sx126x_handle_post_tx( );
    LL_mDelay( TX_TO_TX_DELAY_IN_MS );

    buffer[0]++;
    HAL_DBG_TRACE_INFO( "Counter value: %d\n", buffer[0] );
    sx126x_write_buffer( context, 0, buffer, PAYLOAD_LENGTH );

    apps_common_sx126x_handle_pre_tx( );
    sx126x_set_tx( context, 0 );
}

void on_rx_done( void )
{
    uint8_t size;

    apps_common_sx126x_handle_post_rx( );

    apps_common_sx126x_receive( context, buffer, &size, PAYLOAD_LENGTH );

    if( memcmp( &buffer[1], per_msg, PAYLOAD_LENGTH - 1 ) == 0 )
    {
        // Let's start counting after the first received packet
        if( first_pkt_flag == true )
        {
            uint8_t rolling_counter_gap = ( uint8_t ) ( buffer[0] - rolling_counter );
            nb_ok++;
            per_index += rolling_counter_gap;
            if( rolling_counter_gap > 1 )
            {
                HAL_DBG_TRACE_WARNING( "%d packet(s) missed\n", ( rolling_counter_gap - 1 ) );
            }
            rolling_counter = buffer[0];
        }
        else
        {
            first_pkt_flag  = true;
            rolling_counter = buffer[0];
        }
        HAL_DBG_TRACE_INFO( "Counter value: %d, PER index: %d\n", buffer[0], per_index );
    }
    if( per_index < NB_FRAME )  // Re-start Rx only if the expected number of frames is not reached
    {
        apps_common_sx126x_handle_pre_rx( );
        sx126x_set_rx( context, rx_timeout );
    }
}

void on_rx_timeout( void )
{
    apps_common_sx126x_handle_post_rx( );

    // Let's start counting after the first received packet
    if( first_pkt_flag == true )
    {
        nb_rx_timeout++;
    }

    apps_common_sx126x_handle_pre_rx( );
    sx126x_set_rx( context, rx_timeout );
}

void on_rx_error( void )
{
    apps_common_sx126x_handle_post_rx( );

    // Let's start counting after the first received packet
    if( first_pkt_flag == true )
    {
        nb_rx_error++;
    }

    apps_common_sx126x_handle_pre_rx( );
    sx126x_set_rx( context, rx_timeout );
}
