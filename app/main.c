#include <stdio.h>
#include <stdbool.h>
#include "wwconfig.h"

typedef enum {
   WW_CURRENT_RATING_60A = 60,
   WW_CURRENT_RATING_120A = 120,
   WW_CURRENT_RATING_200A = 200,
   WW_CURRENT_RATING_400A = 400,
   WW_CURRENT_RATING_600A = 600,
} WW_CurrentRating;


void main()
{
	printf("Wattwatchers config library test application.\n");

	open_config();

	/* Load current config values from config file */
	iterate_config();

	/* Update confg values for second CT */
	int ct_key = 0;
	struct ct_settings new_settings;
	new_settings.current_rating = WW_CURRENT_RATING_60A;
  new_settings.amp_cal_offset = 91;
  new_settings.voltage_cal_offset = 49;
 	new_settings.calibrated = false;
  new_settings.lens = true;
  new_settings.country_code = "AU";
	update_config(ct_key,&new_settings);

 /* Re-load new config values from config file */
	iterate_config();

	close_config();
}
