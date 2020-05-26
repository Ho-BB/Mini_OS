

#ifndef __H743_MPU__
#define __H743_MPU__

#include "inc/typedef.h"

typedef struct h743_mpu{
  u32 TYPE;
  u32 CTRL;
  u32 RNR;
  u32 RBAR;
  u32 RASR;
} h743_mpu_t;

#define MPU              ((volatile h743_mpu_t *) 0xE000ED90)

#define MPU_BACKGROUND_REGION (1 << 2)
#define MPU_HF_NMI            (1 << 1)
#define MPU_REGIONS           (1 << 0)

#define MPU_SUBREGION0        (1 << 8)
#define MPU_SUBREGION1        (1 << 9)
#define MPU_SUBREGION2        (1 << 10)
#define MPU_SUBREGION3        (1 << 11)
#define MPU_SUBREGION4        (1 << 12)
#define MPU_SUBREGION5        (1 << 13)
#define MPU_SUBREGION6        (1 << 14)
#define MPU_SUBREGION7        (1 << 15)

typedef enum h743_mpu_region_size
  {
   region_size_32B   = (4 << 1),
   region_size_64B   = (5 << 1),
   region_size_128B  = (6 << 1),
   region_size_256B  = (7 << 1),
   region_size_512B  = (8 << 1),
   region_size_1KB   = (9 << 1),
   region_size_2KB   = (10 << 1),
   region_size_4KB   = (11 << 1),
   region_size_8KB   = (12 << 1),
   region_size_16KB  = (13 << 1),
   region_size_32KB  = (14 << 1),
   region_size_64KB  = (15 << 1),
   region_size_128KB = (16 << 1),
   region_size_256KB = (17 << 1),
   region_size_512KB = (18 << 1),
   region_size_1MB   = (19 << 1),
   region_size_2MB   = (20 << 1),
   region_size_4MB   = (21 << 1),
   region_size_8MB   = (22 << 1),
   region_size_16MB  = (23 << 1),
   region_size_32MB  = (24 << 1),
   region_size_64MB  = (25 << 1),
   region_size_128MB = (26 << 1),
   region_size_256MB = (27 << 1),
   region_size_512MB = (28 << 1),
   region_size_1GB   = (29 << 1),
   region_size_2GB   = (30 << 1),
   region_size_4GB   = (31 << 1)
  }h743_mpu_region_size_e;

typedef enum h743_mpu_access_permission
  {
   no_access = (0 << 24),
   rw_privileged = (1 << 24),
   rw_privileged_ro_unprivileged = (2 << 24),
   full_access = (3 << 24),
   ro_privileged = (5 << 24),
   ro_privileged_ro_unprivileged = (6 << 24)
  }h743_mpu_access_permission_e;

typedef enum h743_mpu_texcb
  {
   strongly_ordered        = 0,
   shared_device           = (1 << 16),
   outerWT_innerWT_noWA    = (1 << 17),
   outerWB_innerWB_noWA    = (1 << 17) | (1 << 16),
   outerNC_innerNC         = (1 << 19),
   outerWB_innerWB_RWA     = (1 << 19) | (1 << 17) | (1 << 16),
   nonshared_device        = (2 << 19),

   /* BB = 00 and AA = 00 redundant */
   outerNC_innerWBRWA      = (4 << 19) | (1 << 16),
   outerNC_innerWTnoWA     = (4 << 19) | (1 << 17),
   outerNC_innerWBnoWA     = (4 << 19) | (1 << 17) | (1 << 16),

   outerWBRWA_innerNC      = (5 << 19),
   /* BB = 01 and AA = 01 redundant */
   outerWBRWA_innerWTnoWA  = (5 << 19) | (1 << 17),
   outerWBRWA_innerWBnoWA  = (5 << 19) | (1 << 17) | (1 << 16),
   
   outerWTnoWA_innerNC     = (6 << 19),
   outerWTnoWA_innerWBRWA  = (6 << 19) | (1 << 16),
   /* BB = 10 and AA = 10 redundant */
   outerWTnoWA_innerWBnoWA = (6 << 19) | (1 << 17) | (1 << 16),

   outerWBnoWA_innerNC     = (7 << 19),
   outerWBnoWA_innerWBRWA  = (7 << 19) | (1 << 16),
   outerWBnoWA_innerWTnoWA = (7 << 19) | (1 << 17)
   /* BB = 11 and AA = 11 redundant */
  }h743_mpu_texcb_e;

typedef struct h743_mpu_conf{
  u32 region;
  u32 address;
  h743_mpu_region_size_e size;
  bool executable;
  h743_mpu_access_permission_e access_permission;
  bool shareable;
  h743_mpu_texcb_e texcb;
} h743_mpu_conf_t;

#define h743_mpu_init_conf(conf)				\
  conf = (h743_mpu_conf_t) { .region = 0,			\
			     .address = 0x00000000,		\
			     .size = region_size_4GB,		\
			     .executable = TRUE,		\
			     .access_permission = full_access,	\
			     .shareable = TRUE,			\
			     .texcb = strongly_ordered		\
  };

void h743_mpu_conf(h743_mpu_conf_t *conf);

#define h743_mpu_enable(bits)			\
  MPU->CTRL |= bits

#define h743_mpu_disable(bits)			\
  MPU->CTRL &= ~(bits)

#define h743_mpu_enable_region(region)		\
  MPU->RNR = region;				\
  MPU->RASR |= 1

#define h743_mpu_disable_region(region)		\
  MPU->RNR = region;				\
  MPU->RASR &= ~1

#define h743_mpu_enable_subregions(region, subregions)	\
  MPU->RNR = region;					\
  MPU->RASR &= ~subregions

#define h743_mpu_disable_subregions(region, subregions)	\
  MPU->RNR = region;					\
  MPU->RASR |= subregions

#endif
