/** @file

  Copyright (c) 2011 - 2020, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PREPI_H_
#define _PREPI_H_

#include <PiPei.h>

#include <Library/PcdLib.h>
#include <Library/ArmLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/SerialPortLib.h>
#include <Library/ArmPlatformLib.h>

/* some BGRA8888 color definitions */
#define FB_BGRA8888_BLACK 0xff000000
#define FB_BGRA8888_WHITE 0xffffffff
#define FB_BGRA8888_CYAN 0xff00ffff
#define FB_BGRA8888_BLUE 0xff0000ff
#define FB_BGRA8888_SILVER 0xffc0c0c0
#define FB_BGRA8888_YELLOW 0xffffff00
#define FB_BGRA8888_ORANGE 0xffffa500
#define FB_BGRA8888_RED 0xffff0000
#define FB_BGRA8888_GREEN 0xff00ff00

/* PSHOLD */
#define PSHOLD_ADRESS               0xFC4AB000

/* KPSS regs */
#define KPSS_BASE                   0xF9000000
#define APCS_KPSS_WDT_BASE          (KPSS_BASE + 0x00017000)
#define APCS_KPSS_WDT_EN            (APCS_KPSS_WDT_BASE + 0x8)

/* MDP regs */
//#define MDP_BASE                    (0xFD900000)
//#define REG_MDP(off)                (MDP_BASE + (off))

#define MDP_CTL_0_BASE				      (0xFD900000 + 0x2000) //0xfd900600??
#define CTL_START                   0x1C

#define MDP_HW_REV                  (0xFD900000 + 0x0100)

// From mdp5.h
#define MDSS_MDP_REV(major, minor, step) \
        ((((major) & 0x000F) << 28) |    \
         (((minor) & 0x0FFF) << 16) |    \
         ((step)   & 0xFFFF))

#define MDSS_MDP_HW_REV_100    MDSS_MDP_REV(1, 0, 0) /* 8974 v1.0 */
#define MDSS_MDP_HW_REV_101    MDSS_MDP_REV(1, 1, 0) /* 8x26 v1.0 */
#define MDSS_MDP_HW_REV_101_1  MDSS_MDP_REV(1, 1, 1) /* 8x26 v2.0, 8926 v1.0 */
#define MDSS_MDP_HW_REV_102    MDSS_MDP_REV(1, 2, 0) /* 8974 v2.0 */
#define MDSS_MDP_HW_REV_102_1  MDSS_MDP_REV(1, 2, 1) /* 8974 v3.0 (Pro) */
#define MDSS_MDP_HW_REV_103    MDSS_MDP_REV(1, 3, 0) /* 8084 v1.0 */
#define MDSS_MDP_HW_REV_105    MDSS_MDP_REV(1, 5, 0) /* 8994 v1.0 */
#define MDSS_MDP_HW_REV_106    MDSS_MDP_REV(1, 6, 0) /* 8916 v1.0 */
#define MDSS_MDP_HW_REV_107    MDSS_MDP_REV(1, 7, 0) /* 8996 v1.0 */
#define MDSS_MDP_HW_REV_108    MDSS_MDP_REV(1, 8, 0) /* 8939 v1.0 */
#define MDSS_MDP_HW_REV_109    MDSS_MDP_REV(1, 9, 0) /* 8994 v2.0 */
#define MDSS_MDP_HW_REV_110    MDSS_MDP_REV(1, 10, 0) /* 8992 v1.0 */
#define MDSS_MDP_HW_REV_111    MDSS_MDP_REV(1, 11, 0) /* 8956 v1.0 */
#define MDSS_MDP_HW_REV_112    MDSS_MDP_REV(1, 12, 0) /* 8952 v1.0 */
#define MDSS_MDP_HW_REV_114    MDSS_MDP_REV(1, 14, 0) /* 8937 v1.0 */
#define MDSS_MDP_HW_REV_116    MDSS_MDP_REV(1, 16, 0) /* msm8953 */
#define MDSS_MDP_HW_REV_115    MDSS_MDP_REV(1, 15, 0) /* msm8917 v1.0 */
#define MDSS_MDP_HW_REV_200    MDSS_MDP_REV(2, 0, 0) /* 8092 v1.0 */

extern UINT64  mSystemMemoryEnd;

EFI_STATUS
EFIAPI
MemoryPeim (
  IN EFI_PHYSICAL_ADDRESS  UefiMemoryBase,
  IN UINT64                UefiMemorySize
  );

EFI_STATUS
EFIAPI
PlatformPeim (
  VOID
  );

// Either implemented by PrePiLib or by MemoryInitPei
VOID
BuildMemoryTypeInformationHob (
  VOID
  );

// Initialize the Architecture specific controllers
VOID
ArchInitialize (
  VOID
  );

VOID
EFIAPI
ProcessLibraryConstructorList (
  VOID
  );

// Initialize early GIC
EFI_STATUS
EFIAPI
QGicPeim (
  VOID
  );

#endif /* _PREPI_H_ */
