#include <PiDxe.h>
#include <Library/LKEnvLib.h>
#include <Library/QcomTargetMmcSdhciLib.h>
#include <Library/QcomGpioTlmmLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Chipset/mmc_sdhci.h>
#include <Platform/iomap.h>
#include <Platform/irqs.h>

#include <Protocol/QcomBoard.h>

/* Protocol reference */
QCOM_BOARD_PROTOCOL* mBoardProtocol = NULL;

/* checks */
/* Check for 8974 chip */
int platform_is_8974(void)
{
	uint32_t platform = mBoardProtocol->board_platform_id();
	int ret = 0;

	switch(platform)
	{
		case APQ8074:
		case MSM8274:
		case MSM8674:
		case MSM8974:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

/* Check for 8974Pro AC chip */
int platform_is_8974ac(void)
{
	uint32_t platform = mBoardProtocol->board_platform_id();
	int ret = 0;

	switch(platform)
	{
		case MSM8974AC:
		case MSM8674AC:
		case MSM8274AC:
		case APQ8074AC:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

/* Check for 8974Pro chip */
int platform_is_8974Pro(void)
{
	uint32_t platform = mBoardProtocol->board_platform_id();
	int ret = 0;

	switch(platform)
	{
		case APQ8074AA:
		case APQ8074AB:
		case APQ8074AC:

		case MSM8274AA:
		case MSM8274AB:
		case MSM8274AC:

		case MSM8674AA:
		case MSM8674AB:
		case MSM8674AC:

		case MSM8974AA:
		case MSM8974AB:
		case MSM8974AC:

			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}
/* end */

static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

#define BOARD_SOC_VERSION1(soc_rev) (soc_rev >= 0x10000 && soc_rev < 0x20000)

VOID LibQcomTargetMmcSdhciInit(INIT_SLOT_CB InitSlot)
{
    uint32_t reg = 0;
    uint32_t soc_ver = 0;
    uint8_t cmd;
    uint8_t dat;
    uint8_t tlmm_hdrv_clk;
    struct mmc_config_data config = {0};

    uint32_t platform_id;

    EFI_STATUS Status;

    // Locate Qualcomm Board Protocol
    Status = gBS->LocateProtocol(
      &gQcomBoardProtocolGuid,
      NULL,
      (VOID *)&mBoardProtocol
    );

    DEBUG((DEBUG_ERROR, "Reading board id\n"));
    // https://github.com/WOA-Project/MSM8974Pkg/commit/279651948ca50b1a4b0e0db7dfa85ad817ac5dc0

    platform_id = mBoardProtocol->board_platform_id();

    switch(platform_id)
    {
      case MSM8274AA:
      case MSM8274AB:
      case MSM8674AA:
      case MSM8674AB:
      case MSM8974AA:
      case MSM8974AB:
        if (mBoardProtocol->board_hardware_id() == HW_PLATFORM_MTP) {
          tlmm_hdrv_clk = TLMM_CUR_VAL_10MA;
          DEBUG((DEBUG_ERROR, "Tlmm current 10MA\n"));
        }
        else {
          tlmm_hdrv_clk = TLMM_CUR_VAL_16MA;
          DEBUG((DEBUG_ERROR, "Tlmm current 16MA\n"));
        }
        break;
      default:
        tlmm_hdrv_clk = TLMM_CUR_VAL_16MA;
        DEBUG((DEBUG_ERROR, "Tlmm current 16MA\n"));
    };

    cmd = TLMM_CUR_VAL_8MA;
    dat = TLMM_CUR_VAL_8MA;
    reg = SDC1_HDRV_PULL_CTL;

    /* Drive strength configs for sdc pins */
    struct tlmm_cfgs sdc1_hdrv_cfg[] =
    {
      { SDC1_CLK_HDRV_CTL_OFF,  tlmm_hdrv_clk, TLMM_HDRV_MASK, reg },
      { SDC1_CMD_HDRV_CTL_OFF,  cmd, TLMM_HDRV_MASK, reg },
      { SDC1_DATA_HDRV_CTL_OFF, dat, TLMM_HDRV_MASK, reg },
    };

    /* Pull configs for sdc pins */
    struct tlmm_cfgs sdc1_pull_cfg[] =
    {
      { SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, reg },
      { SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, reg },
      { SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, reg },
    };

    struct tlmm_cfgs sdc1_rclk_cfg[] =
    {
      { SDC1_RCLK_PULL_CTL_OFF, TLMM_PULL_DOWN, TLMM_PULL_MASK, reg },
    };

    /* Set the drive strength & pull control values */
    gGpioTlmm->tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
    gGpioTlmm->tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));

    /* RCLK is supported only with 8974 pro, set rclk to pull down
    * only for 8974 pro targets
    */
    if (!platform_is_8974()) {
      if(platform_is_8974Pro) {
        DEBUG((DEBUG_ERROR, "Detected PRO target, set rclk to pull down\n"));
        gGpioTlmm->tlmm_set_pull_ctrl(sdc1_rclk_cfg, ARRAY_SIZE(sdc1_rclk_cfg));
      }
    }

    // Init eMMC slot
    // https://github.com/msm8916-mainline/lk2nd/blob/c3780f1f541bf86d72ffa9b8f970acc6cb65a2f3/target/msm8974/init.c#L359

    DEBUG((DEBUG_ERROR, "Start init of emmc slot\n"));
    DEBUG((DEBUG_ERROR, "Read SOC version\n"));

	  soc_ver = mBoardProtocol->board_soc_version();

    /*
	 * 8974 v1 fluid devices, have a hardware bug
	 * which limits the bus width to 4 bit.
	 */
    switch(mBoardProtocol->board_hardware_id())
    {
      case HW_PLATFORM_FLUID:
        if (platform_is_8974() && BOARD_SOC_VERSION1(soc_ver)) {
          config.bus_width = DATA_BUS_WIDTH_4BIT;
          DEBUG((DEBUG_ERROR, "4Bit bus\n"));
        }
        else {
          config.bus_width = DATA_BUS_WIDTH_8BIT;
          DEBUG((DEBUG_ERROR, "8bit bus\n"));
        }
        break;
      default:
        config.bus_width = DATA_BUS_WIDTH_8BIT;
    };
  

    /* Try slot 1 */
    config.slot = 1;
    config.hs200_support = 1;
    /*
    * For 8974 AC platform the software clock
    * plan recommends to use the following frequencies:
    * 200 MHz --> 192 MHZ
    * 400 MHZ --> 384 MHZ
    * only for emmc (slot 1)
    */
    if (platform_is_8974ac()) {
      config.max_clk_rate = MMC_CLK_192MHZ;
      config.hs400_support = 1;
      config.hs200_support = 0;
      DEBUG((DEBUG_ERROR, "HS400, AC, 192MHZ\n"));
    } else {
      config.max_clk_rate = MMC_CLK_200MHZ;
      config.hs400_support = 0;
      DEBUG((DEBUG_ERROR, "HS200\n"));
    }

    config.sdhc_base = mmc_sdhci_base[config.slot - 1];
    config.pwrctl_base = mmc_pwrctl_base[config.slot - 1];
    config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];

    // Init SD card slot
    if (InitSlot(&config) == NULL) {
        DEBUG((DEBUG_ERROR, "Can't initialize mmc slot %u\n", config.slot));
    }
}
