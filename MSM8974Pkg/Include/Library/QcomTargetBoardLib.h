#ifndef __LIBRARY_QCOM_TARGET_BOARD_LIB_H__
#define __LIBRARY_QCOM_TARGET_BOARD_LIB_H__

//#include <Chipset/board.h>

void target_detect(struct board_data *);
void target_baseband_detect(struct board_data *);
int platform_is_8974(void);
int platform_is_8974Pro(void);
int platform_is_8974ac(void);

#endif
