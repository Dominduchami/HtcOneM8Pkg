APRIORI DXE {
  INF MdeModulePkg/Core/Dxe/DxeMain.inf
  INF MdeModulePkg/Universal/PCD/Dxe/Pcd.inf

  INF hTCPkg/HardwareInitDxe/HardwareInitDxe.inf
  # INF QcomPkg/Drivers/EnvDxe/EnvDxe.inf
  INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  
  # Next, install cpu protocol and enable the interrupt controller
  INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  INF ArmPkg/Drivers/ArmGicDxe/ArmGicDxe.inf

  INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  
  INF QcomPkg/Drivers/SecurityDxe/SecurityDxe.inf
  INF QcomPkg/Drivers/SmemDxe/SmemDxe.inf
  INF QcomPkg/Drivers/DALSYSDxe/DALSYSDxe.inf
  INF QcomPkg/Drivers/HWIODxe/HWIODxe.inf
  INF QcomPkg/Drivers/ChipInfoDxe/ChipInfoDxe.inf
  INF QcomPkg/Drivers/PlatformInfoDxe/PlatformInfoDxe.inf
  
  INF QcomPkg/Drivers/SmdDxe/SmdDxe.inf
  INF QcomPkg/Drivers/ULogDxe/ULogDxe.inf
  INF QcomPkg/Drivers/NpaDxe/NpaDxe.inf
  # INF QcomPkg/Drivers/ClockDxe/ClockDxe.inf
  
  INF MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  INF QcomPkg/Drivers/PartitionDxe/PartitionDxe.inf
  INF FatPkg/EnhancedFatDxe/Fat.inf
  INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  INF QcomPkg/Drivers/MMCHSDxe/MMCHSDxe.inf
  
  INF QcomPkg/Drivers/TzDxe/TzDxe.inf
  INF QcomPkg/Drivers/VariableDxe/VariableDxe.inf
  # INF QcomPkg/Drivers/DppDxe/DppDxe.inf
  
  INF QcomPkg/Drivers/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  INF QcomPkg/Drivers/ResetRuntimeDxe/ResetRuntimeDxe.inf
  INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  INF MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  INF QcomPkg/Drivers/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  INF MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  INF MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  INF QcomPkg/Drivers/TLMMDxe/TLMMDxe.inf
  INF QcomPkg/Drivers/I2CDxe/I2CDxe.inf
  INF QcomPkg/Drivers/SPMIDxe/SPMIDxe.inf
  INF QcomPkg/Drivers/SPIDxe/SPIDxe.inf
  INF QcomPkg/Drivers/PmicDxe/PmicDxe.inf
  # INF hTCPkg/PartitionIODxe/PartitionIODxe.inf
  
  INF QcomPkg/Drivers/ButtonsDxe/ButtonsDxe.inf
  
  # INF QcomPkg/Drivers/UsbConfigDxe/UsbConfigDxe.inf
  # INF QcomPkg/Drivers/EhciPciEmulationDxe/EhciPciEmulationDxe.inf
  # INF QcomPkg/Drivers/XhciPciEmulationDxe/XhciPciEmulationDxe.inf
  # INF QcomPkg/Drivers/EhciDxe/EhciDxe.inf
  # INF QcomPkg/Drivers/XhciDxe/XhciDxe.inf
  # INF QcomPkg/Drivers/UsbBusDxe/UsbBusDxe.inf
  # INF QcomPkg/Drivers/UsbKbDxe/UsbKbDxe.inf
  # INF MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  # INF MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  # INF QcomPkg/Drivers/UsbfnDxe/UsbfnDxe.inf
  
  # INF QcomPkg/Drivers/DebugPortDxe/DebugPortDxe.inf
  # INF QcomPkg/Drivers/SerialDxe/SerialDxe.inf
  # INF QcomPkg/Drivers/SimpleTextInOutSerialDxe/SimpleTextInOutSerial.inf
  INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  INF MSM8974Pkg/Drivers/ConSplitterDxe/ConSplitterDxe.inf
  INF MSM8974Pkg/Drivers/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  # INF QcomPkg/Drivers/TsFwUpgradeDxe/TsFwUpgradeDxe.inf
  # INF QcomPkg/Drivers/AdcDxe/AdcDxe.inf
  # INF QcomPkg/Drivers/TsensDxe/TsensDxe.inf 
  # INF QcomPkg/Drivers/ChargerDxe/ChargerDxe.inf
  # INF QcomPkg/Drivers/DisplayDxe/DisplayDxe.inf
  INF MSM8974Pkg/Drivers/SimpleFbDxe/SimpleFbDxe.inf
}
