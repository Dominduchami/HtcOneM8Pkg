
#define MDP_PP_SYNC_CONFIG_VSYNC	0x004
#define MDP_PP_AUTOREFRESH_CONFIG	0x030
 
#define BIT(bit) (1 << (bit))

#define MDP_CTL_0_BASE				(0xFD900000 + 0x600)
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