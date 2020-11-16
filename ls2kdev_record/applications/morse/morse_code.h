/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2018-10-17     flybreak     the first version
* 2020-10-12     anyang       using for loongxin2k1000
*/

#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <rtthread.h>


#define MORSE_DIAN        100
#define MORSE_HUA         300
#define MORSE_ZIFU_KONG   3000
#define MORSE_CI_KONG     7000
#define MORSE_FINSH       0
#define MORSE_START       1


//字母
static const rt_uint16_t morse_kong[2]  = {MORSE_CI_KONG, MORSE_ZIFU_KONG};                                    //'  '
static const rt_uint16_t morse_A[3]  = {MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG};                               //.-
static const rt_uint16_t morse_B[5]  = {MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};       //-...
static const rt_uint16_t morse_C[5]  = {MORSE_HUA, MORSE_DIAN, MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG};        //-.-.
static const rt_uint16_t morse_D[4]  = {MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};                   //-..
static const rt_uint16_t morse_E[2]  = {MORSE_DIAN, MORSE_ZIFU_KONG};                                          //.
static const rt_uint16_t morse_F[5]  = {MORSE_DIAN, MORSE_DIAN, MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG};       //..-.
static const rt_uint16_t morse_G[4]  = {MORSE_HUA, MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG};                    //--.
static const rt_uint16_t morse_H[5]  = {MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};      //....
static const rt_uint16_t morse_I[3]  = {MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};                              //..
static const rt_uint16_t morse_J[5]  = {MORSE_DIAN, MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};         //.---
static const rt_uint16_t morse_K[4]  = {MORSE_HUA, MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG};                    //-.-
static const rt_uint16_t morse_L[5]  = {MORSE_DIAN, MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};       //.-..
static const rt_uint16_t morse_M[3]  = {MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};                                //--
static const rt_uint16_t morse_N[3]  = {MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG};                               //-.
static const rt_uint16_t morse_O[4]  = {MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};                     //---
static const rt_uint16_t morse_P[5]  = {MORSE_DIAN, MORSE_HUA, MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG};        //.--.
static const rt_uint16_t morse_Q[5]  = {MORSE_HUA, MORSE_HUA, MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG};         //--.-
static const rt_uint16_t morse_R[4]  = {MORSE_DIAN, MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG};                   //.-.
static const rt_uint16_t morse_S[4]  = {MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};                  //...
static const rt_uint16_t morse_T[2]  = {MORSE_HUA, MORSE_ZIFU_KONG};                                           //-
static const rt_uint16_t morse_U[4]  = {MORSE_DIAN, MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG};                   //..-
static const rt_uint16_t morse_V[5]  = {MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG};       //...-
static const rt_uint16_t morse_W[4]  = {MORSE_DIAN, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};                    //.--
static const rt_uint16_t morse_X[5]  = {MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG};        //-..-
static const rt_uint16_t morse_Y[5]  = {MORSE_HUA, MORSE_DIAN, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};         //-.--
static const rt_uint16_t morse_Z[5]  = {MORSE_HUA, MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};        //--..

//数字
static const rt_uint16_t morse_0[6]  = {MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};       //-----
static const rt_uint16_t morse_1[6]  = {MORSE_DIAN, MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};      //.----
static const rt_uint16_t morse_2[6]  = {MORSE_DIAN, MORSE_DIAN, MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};     //..---
static const rt_uint16_t morse_3[6]  = {MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_HUA, MORSE_HUA, MORSE_ZIFU_KONG};    //...--
static const rt_uint16_t morse_4[6]  = {MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG};   //....-
static const rt_uint16_t morse_5[6]  = {MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};  //.....
static const rt_uint16_t morse_6[6]  = {MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};   //-....
static const rt_uint16_t morse_7[6]  = {MORSE_HUA, MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};    //--...
static const rt_uint16_t morse_8[6]  = {MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG};     //---..
static const rt_uint16_t morse_9[6]  = {MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG};      //----.


#endif