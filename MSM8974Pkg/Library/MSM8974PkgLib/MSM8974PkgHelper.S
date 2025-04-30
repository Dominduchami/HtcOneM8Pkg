#
#  Copyright (c) 2018, Linaro Limited. All rights reserved.
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
#

#include <AsmMacroLib.h>
#include <Library/ArmLib.h>

ASM_FUNC(ArmPlatformPeiBootAction)
  //Disable Strict alignment checking & Enable Instruction cache
  mrc     p15, 0, r0, c1, c0, 0
  bic     r0, r0, #0x00002300     /* clear bits 13, 9:8 (--V- --RS) */
  bic     r0, r0, #0x00000005     /* clear bits 0, 2 (---- -C-M) */
  bic     r0, r0, #0x00000002     /* Clear bit 1 (Alignment faults) */
  orr     r0, r0, #0x00001000     /* set bit 12 (I) enable I-Cache */
  mcr     p15, 0, r0, c1, c0, 0
  // Enable NEON register in case folks want to use them for optimizations (CopyMem)
  mrc     p15, 0, r0, c1, c0, 2
  orr     r0, r0, #0x00f00000   // Enable VPF access (V* instructions)
  mcr     p15, 0, r0, c1, c0, 2
  mov     r0, #0x40000000       // Set EN bit in FPEXC
  mcr     p10,#0x7,r0,c8,c0,#0  // msr     FPEXC,r0 in ARM assembly
  bx    lr

//UINTN
//ArmPlatformGetCorePosition (
//  IN UINTN MpId
//  );
ASM_FUNC(ArmPlatformGetCorePosition)
  and   r1, r0, #ARM_CORE_MASK
  and   r0, r0, #ARM_CLUSTER_MASK
  add   r0, r1, r0, LSR #7
  bx    lr

//UINTN
//ArmPlatformGetPrimaryCoreMpId (
//  VOID
//  );
ASM_FUNC(ArmPlatformGetPrimaryCoreMpId)
  MOV32  (r0, FixedPcdGet32 (PcdArmPrimaryCore))
  bx    lr

//UINTN
//ArmPlatformIsPrimaryCore (
//  IN UINTN MpId
//  );
ASM_FUNC(ArmPlatformIsPrimaryCore)
  MOV32  (r1, FixedPcdGet32 (PcdArmPrimaryCoreMask))
  and   r0, r0, r1
  MOV32  (r1, FixedPcdGet32 (PcdArmPrimaryCore))
  cmp   r0, r1
  moveq r0, #1
  movne r0, #0
  bx    lr