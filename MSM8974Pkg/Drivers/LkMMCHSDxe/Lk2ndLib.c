#include <Library/LKEnvLib.h>
#include "MMCHS.h"
#include <Library/QcomBoardLib.h>
#include <Chipset/smem.h>
#include <Chipset/board.h>
#include <Library/QcomTargetBoardLib.h>
#include <Library/QcomGpioTlmmLib.h>
#include <Platform/iomap.h>
#include <Platform/irqs.h>

#include <Lk2nd/gpio.h>

#define SDC3_GPIO_FUNC_NUM      2

struct mmc_device *dev;

static uint32_t mmc_sdc_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE, MSM_SDC3_BASE, MSM_SDC4_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

static void set_sdc_power_ctrl(uint32_t slot)
{
	if (slot < 1 || slot > 3) {
		dprintf(CRITICAL, "Invalid SDC slot %u\n", slot);

		return;
	}

	dprintf(CRITICAL, "Setting up SDC power and drive strength for slot %u\n", slot);

	if (slot < 3) {
		uint32_t reg = (slot == 1 ? SDC1_HDRV_PULL_CTL : SDC2_HDRV_PULL_CTL);
		uint8_t tlmm_hdrv_clk = 0;
		uint32_t platform_id = 0;

		platform_id = gBoard->board_platform_id();

		switch(platform_id)
		{
			case MSM8274AA:
			case MSM8274AB:
			case MSM8674AA:
			case MSM8674AB:
			case MSM8974AA:
			case MSM8974AB:
				if (gBoard->board_hardware_id() == HW_PLATFORM_MTP)
					tlmm_hdrv_clk = TLMM_CUR_VAL_10MA;
				else
					tlmm_hdrv_clk = TLMM_CUR_VAL_16MA;
				break;
			default:
				tlmm_hdrv_clk = TLMM_CUR_VAL_16MA;
		};

		/* Drive strength configs for sdc pins */
		struct tlmm_cfgs sdc1_hdrv_cfg[] =
		{
			{ SDC1_CLK_HDRV_CTL_OFF,  tlmm_hdrv_clk, TLMM_HDRV_MASK, reg },
			{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, reg },
			{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, reg },
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
		if (!platform_is_8974())
			gGpioTlmm->tlmm_set_pull_ctrl(sdc1_rclk_cfg, ARRAY_SIZE(sdc1_rclk_cfg));
	} else {
		/* slot 3 is used via a GPIO pinmux. Use GPIO to set it up. */

		uint32_t sdhc3_gpio_pins[] = { 35, 36, 37, 38, 39, 40 };

		for (unsigned int i = 0; i < ARRAY_SIZE(sdhc3_gpio_pins); i++)
			gGpioTlmm->gpio_tlmm_config(sdhc3_gpio_pins[i], SDC3_GPIO_FUNC_NUM, GPIO_INPUT, GPIO_NO_PULL, GPIO_8MA, GPIO_ENABLE);
	}
}

#define BOARD_SOC_VERSION1(soc_rev) (soc_rev >= 0x10000 && soc_rev < 0x20000)

static void target_mmc_sdhci_init(void)
{
	struct mmc_config_data config = {0};
	uint32_t soc_ver = 0;

    // TODO
	soc_ver = gBoard->board_soc_version();

	/*
	 * 8974 v1 fluid devices, have a hardware bug
	 * which limits the bus width to 4 bit.
	 */
	switch(gBoard->board_hardware_id())
	{
		case HW_PLATFORM_FLUID:
			if (gBoard->platform_is_8974() && BOARD_SOC_VERSION1(soc_ver))
				config.bus_width = DATA_BUS_WIDTH_4BIT;
			else
				config.bus_width = DATA_BUS_WIDTH_8BIT;
			break;
		default:
			config.bus_width = DATA_BUS_WIDTH_8BIT;
	};

	/* Trying Slot 1*/
	config.slot = 1;
	set_sdc_power_ctrl(config.slot);
	/*
	 * For 8974 AC platform the software clock
	 * plan recommends to use the following frequencies:
	 * 200 MHz --> 192 MHZ
	 * 400 MHZ --> 384 MHZ
	 * only for emmc (slot 1)
	 */
	if (gBoard->platform_is_8974ac()) {
		config.max_clk_rate = MMC_CLK_192MHZ;
		config.hs400_support = 1;
	} else {
		config.max_clk_rate = MMC_CLK_200MHZ;
	}
	config.sdhc_base = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base = mmc_sdc_base[config.slot - 1];
	config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];

	if (!(dev = mmc_init(&config))) {
		/* Trying Slot 2 next */
		config.slot = 2;
		set_sdc_power_ctrl(config.slot);
		config.max_clk_rate = MMC_CLK_200MHZ;
		config.sdhc_base = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base = mmc_sdc_base[config.slot - 1];
		config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];

		if (!(dev = mmc_init(&config))) {
			dprintf(CRITICAL, "mmc init failed!");
            for(;;) {};
			ASSERT(0);
		}
	}
}

#if 0
void *target_mmc_device(void)
{
	return (void *) dev;
}

struct mmc_device *target_get_sd_mmc(void)
{
	struct mmc_config_data config;
	uint32_t sd_mmc_slot_num = lk2nd_device_get_sd_mmc_slot_num();

	if (sd_mmc_slot_num < 2 || sd_mmc_slot_num > 3) {
		dprintf(CRITICAL, "Invalid SD MMC slot %u\n", sd_mmc_slot_num);

		return NULL;
	}

	dprintf(INFO, "Selected SDC slot %u\n", sd_mmc_slot_num);

	set_sdc_power_ctrl(sd_mmc_slot_num);

	config.slot          = sd_mmc_slot_num;
	config.bus_width     = DATA_BUS_WIDTH_4BIT;
	config.max_clk_rate  = MMC_CLK_200MHZ;
	config.sdhc_base     = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base   = mmc_sdc_base[config.slot - 1];
	config.pwr_irq       = mmc_sdc_pwrctl_irq[config.slot - 1];
	config.hs400_support = 0;

	return mmc_init(&config);
}
#endif


void target_init()
{
    dprintf(CRITICAL, "target_init()\n");
    dprintf(CRITICAL, "Set drive strength & pull ctrl\n");
    /*
	 * Set drive strength & pull ctrl for
	 * emmc
	 */
	set_sdc_power_ctrl(1);

    dprintf(CRITICAL, "target_mmc_sdhci_init\n");
	target_mmc_sdhci_init();
}