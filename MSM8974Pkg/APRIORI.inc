APRIORI DXE {
  INF MdeModulePkg/Core/Dxe/DxeMain.inf
  INF MdeModulePkg/Universal/PCD/Dxe/Pcd.inf

  INF MSM8974Pkg/hTCPkg/HardwareInitDxe/HardwareInitDxe.inf
  # INF MSM8974Pkg/QcomDrivers/EnvDxe/EnvDxe.inf
  INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  
  # Next, install cpu protocol and enable the interrupt controller
  INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  INF ArmPkg/Drivers/ArmGicDxe/ArmGicDxe.inf

  INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  
  INF MSM8974Pkg/QcomDrivers/SecurityDxe/SecurityDxe.inf
  INF MSM8974Pkg/QcomDrivers/SmemDxe/SmemDxe.inf
  INF MSM8974Pkg/QcomDrivers/DALSYSDxe/DALSYSDxe.inf
  INF MSM8974Pkg/QcomDrivers/HWIODxe/HWIODxe.inf
  INF MSM8974Pkg/QcomDrivers/ChipInfoDxe/ChipInfoDxe.inf
  INF MSM8974Pkg/QcomDrivers/PlatformInfoDxe/PlatformInfoDxe.inf
  
  INF MSM8974Pkg/QcomDrivers/SmdDxe/SmdDxe.inf
  INF MSM8974Pkg/QcomDrivers/ULogDxe/ULogDxe.inf
  INF MSM8974Pkg/QcomDrivers/NpaDxe/NpaDxe.inf
  # INF MSM8974Pkg/QcomDrivers/ClockDxe/ClockDxe.inf
  
  INF MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  INF MSM8974Pkg/QcomDrivers/PartitionDxe/PartitionDxe.inf
  INF FatPkg/EnhancedFatDxe/Fat.inf
  INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  INF MSM8974Pkg/QcomDrivers/MMCHSDxe/MMCHSDxe.inf
  
  INF MSM8974Pkg/QcomDrivers/TzDxe/TzDxe.inf
  INF MSM8974Pkg/QcomDrivers/VariableDxe/VariableDxe.inf
  # INF MSM8974Pkg/QcomDrivers/DppDxe/DppDxe.inf
  
  INF MSM8974Pkg/QcomDrivers/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  INF MSM8974Pkg/QcomDrivers/ResetRuntimeDxe/ResetRuntimeDxe.inf
  INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  INF MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  INF MSM8974Pkg/QcomDrivers/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  INF MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  INF MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  INF MSM8974Pkg/QcomDrivers/TLMMDxe/TLMMDxe.inf
  INF MSM8974Pkg/QcomDrivers/I2CDxe/I2CDxe.inf
  INF MSM8974Pkg/QcomDrivers/SPMIDxe/SPMIDxe.inf
  INF MSM8974Pkg/QcomDrivers/SPIDxe/SPIDxe.inf
  INF MSM8974Pkg/QcomDrivers/PmicDxe/PmicDxe.inf
  # INF MSM8974Pkg/hTCPkg/PartitionIODxe/PartitionIODxe.inf
  
  INF MSM8974Pkg/QcomDrivers/ButtonsDxe/ButtonsDxe.inf
  
  # INF MSM8974Pkg/QcomDrivers/UsbConfigDxe/UsbConfigDxe.inf
  # INF MSM8974Pkg/QcomDrivers/EhciPciEmulationDxe/EhciPciEmulationDxe.inf
  # INF MSM8974Pkg/QcomDrivers/XhciPciEmulationDxe/XhciPciEmulationDxe.inf
  # INF MSM8974Pkg/QcomDrivers/EhciDxe/EhciDxe.inf
  # INF MSM8974Pkg/QcomDrivers/XhciDxe/XhciDxe.inf
  # INF MSM8974Pkg/QcomDrivers/UsbBusDxe/UsbBusDxe.inf
  # INF MSM8974Pkg/QcomDrivers/UsbKbDxe/UsbKbDxe.inf
  # INF MdeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
  # INF MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
  # INF MSM8974Pkg/QcomDrivers/UsbfnDxe/UsbfnDxe.inf
  
  # INF MSM8974Pkg/QcomDrivers/DebugPortDxe/DebugPortDxe.inf
  # INF MSM8974Pkg/QcomDrivers/SerialDxe/SerialDxe.inf
  # INF MSM8974Pkg/QcomDrivers/SimpleTextInOutSerialDxe/SimpleTextInOutSerial.inf
  INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  INF MSM8974Pkg/Drivers/ConSplitterDxe/ConSplitterDxe.inf
  INF MSM8974Pkg/Drivers/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  # INF MSM8974Pkg/QcomDrivers/TsFwUpgradeDxe/TsFwUpgradeDxe.inf
  # INF MSM8974Pkg/QcomDrivers/AdcDxe/AdcDxe.inf
  # INF MSM8974Pkg/QcomDrivers/TsensDxe/TsensDxe.inf 
  # INF MSM8974Pkg/QcomDrivers/ChargerDxe/ChargerDxe.inf
  # INF MSM8974Pkg/QcomDrivers/DisplayDxe/DisplayDxe.inf
  INF MSM8974Pkg/Drivers/SimpleFbDxe/SimpleFbDxe.inf
}
