#include "MMCHS.h"

#include <Chipset/gpio.h>
#include <Library/QcomGpioTlmmLib.h>
#include <Platform/iomap.h>
#include <Platform/irqs.h>

#include <Protocol/QcomBoard.h>

/* Protocol reference */
QCOM_BOARD_PROTOCOL* mBoardProtocol = NULL;

#if 0
STATIC struct mmc_device* PlatformCallbackInitSlot (struct mmc_config_data *config)
{
  EFI_STATUS    Status;
  BIO_INSTANCE  *Instance;
  
  // initialize MMC device
  struct mmc_device *dev = mmc_init (config);
  if (dev == NULL)
    return NULL;

  // allocate instance
  Status = BioInstanceContructor (&Instance);
  if (EFI_ERROR(Status)) {
    return dev;
  }

  // set data
  Instance->MmcDev               = dev;
  Instance->BlockMedia.BlockSize = dev->card.block_size;
  Instance->BlockMedia.LastBlock = dev->card.capacity/Instance->BlockMedia.BlockSize - 1;

  // give every device a slighty different GUID
  Instance->DevicePath.Mmc.Guid.Data4[7] = config->slot;

  // Publish BlockIO
  Status = gBS->InstallMultipleProtocolInterfaces (
              &Instance->Handle,
              &gEfiBlockIoProtocolGuid,    &Instance->BlockIo,
              &gEfiDevicePathProtocolGuid, &Instance->DevicePath,
              NULL
              );
      
  return dev;
}
#endif

STATIC BIO_INSTANCE mBioTemplate = {
  BIO_INSTANCE_SIGNATURE,
  NULL, // Handle
  { // BlockIo
    EFI_BLOCK_IO_INTERFACE_REVISION,   // Revision
    NULL,                              // *Media
    MMCHSReset,                        // Reset
    MMCHSReadBlocks,                   // ReadBlocks
    MMCHSWriteBlocks,                  // WriteBlocks
    MMCHSFlushBlocks                   // FlushBlocks
  },
  { // BlockMedia
    BIO_INSTANCE_SIGNATURE,                   // MediaId
    FALSE,                                    // RemovableMedia
    TRUE,                                     // MediaPresent
    FALSE,                                    // LogicalPartition
    FALSE,                                    // ReadOnly
    FALSE,                                    // WriteCaching
    0,                                        // BlockSize
    4,                                        // IoAlign
    0,                                        // Pad
    0                                         // LastBlock
  },
  { // DevicePath
   {
      {
        HARDWARE_DEVICE_PATH, HW_VENDOR_DP,
        { (UINT8) (sizeof(VENDOR_DEVICE_PATH)), (UINT8) ((sizeof(VENDOR_DEVICE_PATH)) >> 8) },
      },
      // Hardware Device Path for Bio
      EFI_CALLER_ID_GUID // Use the driver's GUID
    },

    {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      { sizeof (EFI_DEVICE_PATH_PROTOCOL), 0 }
    }
  }
};

/*
 * Function: mmc_write
 * Arg     : Data address on card, data length, i/p buffer
 * Return  : 0 on Success, non zero on failure
 * Flow    : Write the data from in to the card
 */
STATIC UINT32 mmc_write(BIO_INSTANCE *Instance, UINT64 data_addr, UINT32 data_len, VOID *in)
{
  UINT32 val = 0;
  UINT32 block_size = 0;
  UINT32 write_size = SDHCI_ADMA_MAX_TRANS_SZ;
  UINT8 *sptr = (UINT8 *)in;

  block_size = Instance->BlockMedia.BlockSize;

  ASSERT(!(data_addr % block_size));

  if (data_len % block_size)
    data_len = ROUNDUP(data_len, block_size);

  /*
  * Flush the cache before handing over the data to
  * storage driver
  */
  arch_clean_invalidate_cache_range((addr_t)in, data_len);

  /* TODO: This function is aware of max data that can be
  * tranferred using sdhci adma mode, need to have a cleaner
  * implementation to keep this function independent of sdhci
  * limitations
  */
  while (data_len > write_size) {
    val = mmc_sdhci_write(Instance->MmcDev, (VOID *)sptr, (data_addr / block_size), (write_size / block_size));
    if (val)
    {
      DEBUG((EFI_D_ERROR, "Failed Writing block @ %x\n",(UINTN)(data_addr / block_size)));
      return val;
    }
    sptr += write_size;
    data_addr += write_size;
    data_len -= write_size;
  }

  if (data_len)
    val = mmc_sdhci_write(Instance->MmcDev, (VOID *)sptr, (data_addr / block_size), (data_len / block_size));

  if (val)
    DEBUG((EFI_D_ERROR, "Failed Writing block @ %x\n",(UINTN)(data_addr / block_size)));

  return val;
}

/*
 * Function: mmc_read
 * Arg     : Data address on card, o/p buffer & data length
 * Return  : 0 on Success, non zero on failure
 * Flow    : Read data from the card to out
 */
STATIC UINT32 mmc_read(BIO_INSTANCE *Instance, UINT64 data_addr, UINT32 *out, UINT32 data_len)
{
  UINT32 ret = 0;
  UINT32 block_size;
  UINT32 read_size = SDHCI_ADMA_MAX_TRANS_SZ;
  UINT8 *sptr = (UINT8 *)out;

  block_size = Instance->BlockMedia.BlockSize;

  ASSERT(!(data_addr % block_size));
  ASSERT(!(data_len % block_size));

  /*
  * dma onto write back memory is unsafe/nonportable,
  * but callers to this routine normally provide
  * write back buffers. Invalidate cache
  * before read data from mmc.
  */
  arch_clean_invalidate_cache_range((addr_t)(out), data_len);

  /* TODO: This function is aware of max data that can be
  * tranferred using sdhci adma mode, need to have a cleaner
  * implementation to keep this function independent of sdhci
  * limitations
  */
  while (data_len > read_size) {
    ret = mmc_sdhci_read(Instance->MmcDev, (VOID *)sptr, (data_addr / block_size), (read_size / block_size));
    if (ret)
    {
      DEBUG((EFI_D_ERROR, "Failed Reading block @ %x\n",(UINTN) (data_addr / block_size)));
      return ret;
    }
    sptr += read_size;
    data_addr += read_size;
    data_len -= read_size;
  }

  if (data_len)
    ret = mmc_sdhci_read(Instance->MmcDev, (VOID *)sptr, (data_addr / block_size), (data_len / block_size));

  if (ret)
    DEBUG((EFI_D_ERROR, "Failed Reading block @ %x\n",(UINTN) (data_addr / block_size)));

  return ret;
}

EFI_STATUS
EFIAPI
MMCHSReset (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MMCHSReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  OUT VOID                          *Buffer
  )
{
  BIO_INSTANCE              *Instance;
  EFI_BLOCK_IO_MEDIA        *Media;
  UINTN                     BlockSize;
  UINTN                     RC;

  Instance  = BIO_INSTANCE_FROM_BLOCKIO_THIS(This);
  Media     = &Instance->BlockMedia;
  BlockSize = Media->BlockSize;

  if (MediaId != Media->MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (Lba > Media->LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Lba + (BufferSize / BlockSize) - 1) > Media->LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize % BlockSize != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  RC = mmc_read (Instance, (UINT64) Lba * BlockSize, Buffer, BufferSize);
  if (RC == 0)
    return EFI_SUCCESS;
  else
    return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
MMCHSWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL          *This,
  IN UINT32                         MediaId,
  IN EFI_LBA                        Lba,
  IN UINTN                          BufferSize,
  IN VOID                           *Buffer
  )
{
  BIO_INSTANCE              *Instance;
  EFI_BLOCK_IO_MEDIA        *Media;
  UINTN                      BlockSize;
  UINTN                     RC;

  Instance  = BIO_INSTANCE_FROM_BLOCKIO_THIS(This);
  Media     = &Instance->BlockMedia;
  BlockSize = Media->BlockSize;

  if (MediaId != Media->MediaId) {
    return EFI_MEDIA_CHANGED;
  }

  if (Lba > Media->LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Lba + (BufferSize / BlockSize) - 1) > Media->LastBlock) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize % BlockSize != 0) {
    return EFI_BAD_BUFFER_SIZE;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == 0) {
    return EFI_SUCCESS;
  }

  RC = mmc_write (Instance, (UINT64) Lba * BlockSize, BufferSize, Buffer);
  if (RC == 0)
    return EFI_SUCCESS;
  else
    return EFI_DEVICE_ERROR;
}

EFI_STATUS
EFIAPI
MMCHSFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL  *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
BioInstanceContructor (
  OUT BIO_INSTANCE** NewInstance
  )
{
  BIO_INSTANCE* Instance;

  Instance = AllocateCopyPool (sizeof(BIO_INSTANCE), &mBioTemplate);
  if (Instance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Instance->BlockIo.Media = &Instance->BlockMedia;

  *NewInstance = Instance;
  return EFI_SUCCESS;
}

// lk2nd target.c start:
struct mmc_device *dev;

static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

#define BOARD_SOC_VERSION1(soc_rev) (soc_rev >= 0x10000 && soc_rev < 0x20000)

static void set_sdc_power_ctrl(void)
{
	uint8_t tlmm_hdrv_clk = 0;
	uint32_t platform_id = 0;

  // here

	platform_id = mBoardProtocol->board_platform_id();

  dprintf(CRITICAL, "mBoard platform check??\n");

	switch(platform_id)
	{
		case MSM8274AA:
		case MSM8274AB:
		case MSM8674AA:
		case MSM8674AB:
		case MSM8974AA:
		case MSM8974AB:
			if (mBoardProtocol->board_hardware_id() == HW_PLATFORM_MTP) {
        DEBUG((EFI_D_ERROR, "Board id = HW_PLATFORM_MTP, using 10MA"));
				tlmm_hdrv_clk = TLMM_CUR_VAL_10MA;
      }
			else {
				tlmm_hdrv_clk = TLMM_CUR_VAL_16MA;
        DEBUG((EFI_D_ERROR, "Board id check, using 16MA"));
      }
      break;
		default:
			tlmm_hdrv_clk = TLMM_CUR_VAL_16MA;
      DEBUG((EFI_D_ERROR, "Using 16MA"));
	};

  // here

	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  tlmm_hdrv_clk, TLMM_HDRV_MASK, 0 },
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, 0 },
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, 0 },
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, 0 },
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, 0 },
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, 0 },
	};

	struct tlmm_cfgs sdc1_rclk_cfg[] =
	{
		{ SDC1_RCLK_PULL_CTL_OFF, TLMM_PULL_DOWN, TLMM_PULL_MASK, 0 },
	};

	/* Set the drive strength & pull control values */
  DEBUG((EFI_D_ERROR, "Set the drive strength & pull control values"));
	//tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	//tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
  // We fail here :-(
  gGpioTlmm->tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
  gGpioTlmm->tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));

	/* RCLK is supported only with 8974 pro, set rclk to pull down
	 * only for 8974 pro targets
	 */
  // already dead
	if (!mBoardProtocol->platform_is_8974()) {
		//tlmm_set_pull_ctrl(sdc1_rclk_cfg, ARRAY_SIZE(sdc1_rclk_cfg));
    DEBUG((EFI_D_ERROR, "Platform_is_8974 returned true"));
    gGpioTlmm->tlmm_set_pull_ctrl(sdc1_rclk_cfg, ARRAY_SIZE(sdc1_rclk_cfg));
  }
}

static void target_mmc_sdhci_init(void)
{
	struct mmc_config_data config = {0};
	uint32_t soc_ver = 0;

	dprintf(CRITICAL, "target_mmc_sdhci_init()\n");

	soc_ver = mBoardProtocol->board_soc_version();

	/*
	 * 8974 v1 fluid devices, have a hardware bug
	 * which limits the bus width to 4 bit.
	 */
	switch(mBoardProtocol->board_hardware_id())
	{
		if (mBoardProtocol->platform_is_8974() && BOARD_SOC_VERSION1(soc_ver)) {
      config.bus_width = DATA_BUS_WIDTH_4BIT;
      dprintf(CRITICAL, "Fluid, data bus is 4bit\n");
    }
    else {
      config.bus_width = DATA_BUS_WIDTH_8BIT;
      dprintf(CRITICAL, "Fluid, data bus is 8bit\n");
    }
    break;
		default:
      dprintf(CRITICAL, "NOT Fluid, data bus is 8bit\n");
			config.bus_width = DATA_BUS_WIDTH_8BIT;
	};

	/* Trying Slot 1*/
	config.slot = 1;
	/*
	 * For 8974 AC platform the software clock
	 * plan recommends to use the following frequencies:
	 * 200 MHz --> 192 MHZ
	 * 400 MHZ --> 384 MHZ
	 * only for emmc (slot 1)
	 */
	if (mBoardProtocol->platform_is_8974ac()) {
    dprintf(CRITICAL, "PLatform is AC\n");
		config.max_clk_rate = MMC_CLK_192MHZ;
		config.hs400_support = 1;
	} else {
    dprintf(CRITICAL, "Platform is not-AC\n");
		config.max_clk_rate = MMC_CLK_200MHZ;
	}
	config.sdhc_base = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base = mmc_pwrctl_base[config.slot - 1];
	config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];

	if (!(dev = mmc_init(&config))) {
    dprintf(CRITICAL, "EMMC init failed!!!, looping\n");
    for(;;) {};
		/* Trying Slot 2 next */
		config.slot = 2;
		config.max_clk_rate = MMC_CLK_200MHZ;
		config.sdhc_base = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base = mmc_pwrctl_base[config.slot - 1];
		config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];

		if (!(dev = mmc_init(&config))) {
			dprintf(CRITICAL, "mmc/sd init failed!");
			ASSERT(0);
		}
	}

	/*
	 * MMC initialization is complete, read the partition table info
	 */
	/*if (partition_read_table()) {
		dprintf(CRITICAL, "Error reading the partition table info\n");
		ASSERT(0);
	}*/
}

//VOID MmcSdhciInit(INIT_SLOT_CB InitSlot)
VOID TargetMmcSdhciInit()
{
  DEBUG((EFI_D_ERROR, "target_init()\n"));
  /*
	 * Set drive strength & pull ctrl for
	 * emmc
	 */
  DEBUG((EFI_D_ERROR, "// Start 'emmcdxe' code //\n"));
  DEBUG((EFI_D_ERROR, "Set emmc drive strength & pull ctrl\n"));
	set_sdc_power_ctrl();

  DEBUG((EFI_D_ERROR, "target_mmc_sdhci_init()\n"));
  target_mmc_sdhci_init();

  // Init SD card slot
  /*if (InitSlot(&config) == NULL) {
      DEBUG((DEBUG_ERROR, "Can't initialize mmc slot %u\n", config.slot));
  }*/
}

// lk2nd target.c end

EFI_STATUS
EFIAPI
MMCHSInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{ 
  // Locate Qualcomm Board Protocol
  EFI_STATUS    Status = gBS->LocateProtocol(
    &gQcomBoardProtocolGuid,
    NULL,
    (VOID *)&mBoardProtocol
  );
  ASSERT_EFI_ERROR(Status);

  DEBUG((EFI_D_ERROR, "qcomBoard located, initing emmc/sd\n"));//

  // let the target register MMC devices
  //TargetMmcSdhciInit (PlatformCallbackInitSlot);
  TargetMmcSdhciInit();

  DEBUG((EFI_D_ERROR, "EMMC init end, loop forever\n"));// hmm
  for(;;) {};

  return Status;
}
