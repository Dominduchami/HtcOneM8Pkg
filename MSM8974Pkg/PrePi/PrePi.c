/** @file

  Copyright (c) 2011-2017, ARM Limited. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>

#include <Library/CacheMaintenanceLib.h>
#include <Library/DebugAgentLib.h>
#include <Library/IoLib.h>
#include <Library/PrePiLib.h>
#include <Library/PrintLib.h>
#include <Library/PrePiHobListPointerLib.h>
#include <Library/TimerLib.h>
#include <Library/PerformanceLib.h>
#include <Library/FramebufferHelperLib.h>

#include <Ppi/GuidedSectionExtraction.h>
#include <Ppi/ArmMpCoreInfo.h>
#include <Ppi/SecPerformance.h>

#include "PrePi.h"

UINT64  mSystemMemoryEnd = FixedPcdGet64 (PcdSystemMemoryBase) +
                           FixedPcdGet64 (PcdSystemMemorySize) - 1;

/**
  SEC main routine.
  @param[in]  UefiMemoryBase  Start of the PI/UEFI memory region
  @param[in]  StacksBase      Start of the stack
  @param[in]  StartTimeStamp  Timer value at start of execution
**/
STATIC
VOID
PrePiMain (
  IN  UINTN   UefiMemoryBase,
  IN  UINTN   StacksBase,
  IN  UINT64  StartTimeStamp
  )
{
  EFI_HOB_HANDOFF_INFO_TABLE  *HobList;
  EFI_STATUS                  Status;
  CHAR8                       Buffer[100];
  UINTN                       CharCount;
  UINTN                       StacksSize;
  FIRMWARE_SEC_PERFORMANCE    Performance;

  /* Disable Watchdog, if it was enabled by first bootloader. */
	MmioWrite32(APCS_KPSS_WDT_EN, 0);

  // Initialize the architecture specific bits
  ArchInitialize ();

  // Reconfigure the framebuffer based on PCD
  ReconfigFb();

  // Paint screen to black
  PaintScreen(FB_BGRA8888_BLACK);

#if DISPLAY_ENABLE_AUTOREFRESH == 1
  // Enable autorefresh
  DisplayAutorefresh(ENABLE);
#else
  // Refresh once
  MdpRefresh();
#endif

  // Initialize the Serial Port
  SerialPortInitialize ();
  CharCount = AsciiSPrint (
                Buffer,
                sizeof (Buffer),
                "UEFI firmware (version %s built at %a on %a)\n\r",
                (CHAR16 *)PcdGetPtr (PcdFirmwareVersionString),
                __TIME__,
                __DATE__
                );
  SerialPortWrite ((UINT8 *)Buffer, CharCount);

  DEBUG((
        EFI_D_INFO | EFI_D_LOAD,
        "UEFI Memory Base = 0x%p, Stack Base = 0x%p\n",
        UefiMemoryBase,
        StacksBase
    ));

  // Initialize the Debug Agent for Source Level Debugging
  InitializeDebugAgent (DEBUG_AGENT_INIT_POSTMEM_SEC, NULL, NULL);
  SaveAndSetDebugTimerInterrupt (TRUE);

  // Declare the PI/UEFI memory region
  HobList = HobConstructor (
              (VOID *)UefiMemoryBase,
              FixedPcdGet32 (PcdSystemMemoryUefiRegionSize),
              (VOID *)UefiMemoryBase,
              (VOID *)StacksBase // The top of the UEFI Memory is reserved for the stacks
              );
  PrePeiSetHobList (HobList);

  // Initialize MMU and Memory HOBs (Resource Descriptor HOBs)
  Status = MemoryPeim (UefiMemoryBase, FixedPcdGet32 (PcdSystemMemoryUefiRegionSize));
  ASSERT_EFI_ERROR (Status);

  // Initialize GIC
  Status = QGicPeim();
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Failed to configure GIC\n"));
    CpuDeadLoop();
  }
  DEBUG((EFI_D_INFO | EFI_D_LOAD, "GIC configured\n"));

  // Create the Stacks HOB
  StacksSize = PcdGet32 (PcdCPUCorePrimaryStackSize);

  BuildStackHob (StacksBase, StacksSize);

  // TODO: Call CpuPei as a library
  BuildCpuHob (ArmGetPhysicalAddressBits (), PcdGet8 (PcdPrePiCpuIoSize));

  // Store timer value logged at the beginning of firmware image execution
  Performance.ResetEnd = GetTimeInNanoSecond (StartTimeStamp);

  // Build SEC Performance Data Hob
  BuildGuidDataHob (&gEfiFirmwarePerformanceGuid, &Performance, sizeof (Performance));

  // Set the Boot Mode
  SetBootMode (ArmPlatformGetBootMode ());

  // Initialize Platform HOBs (CpuHob and FvHob)
  Status = PlatformPeim ();
  ASSERT_EFI_ERROR (Status);

  // Now, the HOB List has been initialized, we can register performance information
  PERF_START (NULL, "PEI", NULL, StartTimeStamp);

  // SEC phase needs to run library constructors by hand.
  ProcessLibraryConstructorList ();

  // Assume the FV that contains the SEC (our code) also contains a compressed FV.
  Status = DecompressFirstFv ();
  ASSERT_EFI_ERROR (Status);

  // Load the DXE Core and transfer control to it
  Status = LoadDxeCoreFromFv (NULL, 0);
  ASSERT_EFI_ERROR (Status);
}

VOID
CEntryPoint (
  IN  UINTN  MpId,
  IN  UINTN  UefiMemoryBase,
  IN  UINTN  StacksBase
  )
{
  UINT64  StartTimeStamp;

  // Initialize the platform specific controllers
  ArmPlatformInitialize (MpId);

  StartTimeStamp = 0;

  // Data Cache enabled on Primary core when MMU is enabled.
  ArmDisableDataCache ();
  // Invalidate instruction cache
  ArmInvalidateInstructionCache ();
  // Enable Instruction Caches on all cores.
  ArmEnableInstructionCache ();

  // Wait the Primary core has defined the address of the Global Variable region (event: ARM_CPU_EVENT_DEFAULT)
  ArmCallWFE ();

  PrePiMain (UefiMemoryBase, StacksBase, StartTimeStamp);

  // DXE Core should always load and never return
  ASSERT (FALSE);
}
