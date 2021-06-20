#pragma once


struct ct_settings
{
	int enabled;
  int current_rating;
  int amp_cal_offset;
  int voltage_cal_offset;
  int calibrated;
  int lens;
  const char *country_code;
	struct config_setting_t *pointer;
	const char *URL[21];
};

void open_config(void);
void iterate_config(void);
void update_config(int key,struct ct_settings *settings);
void close_config(void);
