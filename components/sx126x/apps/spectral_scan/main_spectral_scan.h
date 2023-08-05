/*!
 * @file      main_spectral_scan.h
 *
 * @brief     Spectral-scan example for SX126x chip
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

#ifndef MAIN_SPECTRAL_SCAN_H
#define MAIN_SPECTRAL_SCAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*!
 * @brief first channel frequency, in Hz
 */
#ifndef FREQ_START_HZ
#define FREQ_START_HZ 915000000
#endif

/*!
 * @brief number of channels to scan (default: 200kHz between each channel)
 */
#ifndef NB_CHAN
#define NB_CHAN 30
#endif

/*!
 * @brief number of scan points for each frequency scan in statistics mode
 */
#ifndef NB_SCAN
#define NB_SCAN 100
#endif

/*!
 * @brief number of seconds between 2 scans in the thread
 */
#ifndef PACE_S
#define PACE_S 1
#endif

/*!
 * @brief width between each channel
 */
#ifndef WIDTH_CHAN_HZ
#define WIDTH_CHAN_HZ 10000 /* 200kHz channels */
#endif

/*!
 * @brief RSSI spectral scan scale
 */
#ifndef RSSI_TOP_LEVEL_DBM
#define RSSI_TOP_LEVEL_DBM 0        // 0 dBm
#endif
#ifndef RSSI_BOTTOM_LEVEL_DBM
#define RSSI_BOTTOM_LEVEL_DBM -128  // -128 dBm
#endif
#ifndef RSSI_SCALE
#define RSSI_SCALE 4                // 4 dBm
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

#ifdef __cplusplus
}
#endif

#endif  // MAIN_SPECTRAL_SCAN_H

/* --- EOF ------------------------------------------------------------------ */
