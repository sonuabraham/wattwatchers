#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include "wwconfig.h"

char *WW_CONFIG_FILE = "/home/sonu/wattwatchers/config/wattwatchers.cfg";
struct config_t conf;
struct config_t *config;

#define DEBUG

static int get_root_element_count(config_t *config, char *name, config_setting_t *config_element);
static int get_element_count(config_setting_t *config, char *name, config_setting_t *config_element);
static int get_config_int(config_setting_t *setting, char *name);
static int get_config_bool(config_setting_t *setting, char *name);
static const char *get_config_string(config_setting_t *setting, char *name);

/*
* Function open_config. Open the configuraation file.
*/
void open_config(void)
{
	if (strlen(WW_CONFIG_FILE) == 0)
	{
		fprintf(stderr, "No configuration file specified.\n");
		exit(1);
	}

	config = &conf;
	config_init(config);

	int loaded_config = config_read_file(config, WW_CONFIG_FILE);
	if (loaded_config != 1)
	{
		fprintf(stderr, "Error reading config file %s. Error on line %d: %s\n", config_error_file(config), config_error_line(config), config_error_text(config));
		config_destroy(config);
	}
}

/* Function ct_iterate. Iterate thru all the current tranformers in the configuraation
*  and print all the configuration elements. The configuration elements can be used to
*  perform other calculations including power measurements.
*/

static void ct_iterate(int auditorInt, struct ct_settings settings, struct config_setting_t *auditor_element)
{

	/*
	* Loop through current transformers.
	*/
	struct config_setting_t conf_cts;
	struct config_setting_t *config_cts = &conf_cts;
	int cts_count = get_element_count(auditor_element, "ct", config_cts);
	if (config_cts == NULL)
	{
		fprintf(stdout, "No CTs defined for auditor %d.\n", auditorInt);
		return;
	}
#ifdef DEBUG
	printf("Number of CTs within auditor %d: %d\n", auditorInt, cts_count);
#endif

	int ctsInt;
	for(ctsInt = 0; ctsInt < cts_count; ctsInt++)
	{
		struct config_setting_t *cts_element = config_setting_get_elem(config_cts, ctsInt);
		if (cts_element == NULL)
		{
			continue;
		}

		struct ct_settings cts_settings;
		memcpy(&cts_settings, &settings, sizeof(cts_settings));
		cts_settings.pointer = cts_element;

		struct ct_settings *ctsPtr = NULL;
		ctsPtr = (struct ct_settings *) malloc(sizeof(struct ct_settings));

		memcpy(ctsPtr, &cts_settings, sizeof(struct ct_settings));
		ctsPtr->pointer = cts_element;

		struct config_setting_t *server_setting = NULL;

		int cts_enabled = get_config_bool(cts_element, "enabled");
		if (cts_enabled == 0)
		{
			fprintf(stdout, "CT %d is not enabled.\n", ctsInt);
			continue;
		}
		else if (cts_enabled > 0)
		{
			cts_settings.enabled = cts_enabled;
			cts_settings.current_rating = get_config_int(cts_element,"current_rating");
			cts_settings.amp_cal_offset = get_config_int(cts_element,"amp_cal_offset");
			cts_settings.voltage_cal_offset = get_config_int(cts_element,"voltage_cal_offset");
			cts_settings.calibrated = get_config_bool(cts_element,"calibrated");
			cts_settings.lens = get_config_bool(cts_element,"lens");
			cts_settings.country_code = get_config_string(cts_element, "country_code");

#ifdef DEBUG
			printf("auditor[%d].ct[%d].enabled = %d\n", auditorInt, ctsInt, cts_settings.enabled);
			printf("auditor[%d].ct[%d].current_rating = %d\n", auditorInt, ctsInt, cts_settings.current_rating);
			printf("auditor[%d].ct[%d].amp_cal_offset = %d\n", auditorInt, ctsInt, cts_settings.amp_cal_offset);
			printf("auditor[%d].ct[%d].voltage_cal_offset = %d\n", auditorInt, ctsInt, cts_settings.voltage_cal_offset);
			printf("auditor[%d].ct[%d].calibrated = %d\n", auditorInt, ctsInt, cts_settings.calibrated);
			printf("auditor[%d].ct[%d].lens = %d\n", auditorInt, ctsInt, cts_settings.lens);
			printf("auditor[%d].ct[%d].country_code = %s\n", auditorInt, ctsInt, cts_settings.country_code);
#endif

		/*
		* Loop through URLs.
		*/
		struct config_setting_t *conf_serverURLs = config_setting_lookup(cts_element, "servers.URL");
		if (conf_serverURLs == NULL)
		{
			fprintf(stdout, "No URLs defined for server %d using schema %d.\n", ctsInt, auditorInt);
		}
		else
		{
			int config_serverURLs = config_setting_length(conf_serverURLs);
			int l;
			for(l = 0; l < config_serverURLs; l++)
			{
				struct config_setting_t *serverURL_element = config_setting_get_elem(conf_serverURLs, l);
				if (serverURL_element == NULL)
				{
					continue;
				}


				ctsPtr->URL[l] = config_setting_get_string(serverURL_element);
				#ifdef DEBUG
					printf("auditor[%d].ct[%d].serverURI[%d] = %s\n", auditorInt, ctsInt, l,ctsPtr->URL[l]);
				#endif
			}

		}
	}
}
}

/* Function update_config. This function can be used to update a particulare current
* transformer setting with key as the index value.
*/

void update_config(int key, struct ct_settings *settings)
{

	struct config_setting_t conf_auditors;
	struct config_setting_t *config_auditors = &conf_auditors;
	int auditor_count = get_root_element_count(config, "auditor", config_auditors);

	struct config_setting_t *auditor_element = config_setting_get_elem(config_auditors, 0);
	if (auditor_element== NULL)
	{
		return;
	}

	struct config_setting_t conf_cts;
	struct config_setting_t *config_cts = &conf_cts;
	int server_count = get_element_count(auditor_element, "ct", config_cts);
	if (config_cts == NULL)
	{
		return;
	}

	struct config_setting_t *cts_element = config_setting_get_elem(config_cts, key);
	if (cts_element == NULL)
	{
		return;
	}

	struct ct_settings *ctsPtr = NULL;
	ctsPtr = (struct ct_settings *) malloc(sizeof(struct ct_settings));
	memcpy(ctsPtr, &settings, sizeof(struct ct_settings));
	ctsPtr->pointer = cts_element;

	config_setting_set_int(config_setting_lookup(ctsPtr->pointer, "current_rating"), settings->current_rating);
	config_setting_set_int(config_setting_lookup(ctsPtr->pointer, "amp_cal_offset"), settings->amp_cal_offset);
	config_setting_set_int(config_setting_lookup(ctsPtr->pointer, "voltage_cal_offset"), settings->voltage_cal_offset);
	config_setting_set_bool(config_setting_lookup(ctsPtr->pointer, "calibrated"), settings->calibrated);
	config_setting_set_bool(config_setting_lookup(ctsPtr->pointer, "lens"), settings->lens);
	config_setting_set_string(config_setting_lookup(ctsPtr->pointer, "country_code"), settings->country_code);
	config_write_file(config, WW_CONFIG_FILE);

	free(ctsPtr);

}
/* Function iterate_config . This function can be used to iterate through al the auditors.
*  The function also iterates thru all the current transformers and prints the settings.
*/

void iterate_config(void)
{
	/*
	* Loop through schemas.
	*/
	struct config_setting_t conf_auditors;
	struct config_setting_t *config_auditors = &conf_auditors;
	int auditor_count = get_root_element_count(config, "auditor", config_auditors);

#ifdef DEBUG
	printf("Number of auditors: %d\n", auditor_count);
#endif

	int auditorInt;
	for(auditorInt = 0; auditorInt < auditor_count; auditorInt++)
	{
		struct config_setting_t *auditor_element = config_setting_get_elem(config_auditors, auditorInt);
		if (auditor_element== NULL)
		{
			continue;
		}
		struct ct_settings settings;
		ct_iterate(auditorInt, settings, auditor_element);
	}
}

/*
* Function get_root_element_count.Gets the root element count.
*/

static int get_root_element_count(config_t *config, char *name, config_setting_t *config_element)
{
	config_setting_t *conf_element = config_lookup(config, name);
	if (conf_element == NULL)
	{
		fprintf(stderr, "No %s found in configuration file.\n", name);
		exit(1);
	}
	else
	{
		*config_element = *conf_element;
		return config_setting_length(config_element);
	}
}

/*
* Function get_element_count:
*	* Returns number (int) of elements in list 'name' in configuration setting 'config_setting'.
*	* Updates pointer '*config_element' to point to element 'name'.
*
* config_setting_t *config : pointer to parsed config setting
* char *name : pointer to name of element
* config_setting_t *conf_element : pointer to element
*/
static int get_element_count(config_setting_t *config_setting, char *name, config_setting_t *config_element)
{
	config_setting_t *conf_element = config_setting_lookup(config_setting, name);
	if (conf_element == NULL)
	{
		fprintf(stderr, "No %s found in configuration file for this schema.\n", name);
		return 0;
	}
	else {
		*config_element = *conf_element;
		return config_setting_length(config_element);
	}
}

/*
* Function get_config_int looks up the integer value of 'name'
*  in the configuration setting 'setting' and returns the integer.
* -1 is returned if 'name' does not exist.
*/
static int get_config_int(config_setting_t *setting, char *name)
{
	config_setting_t *setting_pointer = NULL;
	setting_pointer = config_setting_lookup(setting, name);
	if (setting_pointer != NULL)
	{
		return config_setting_get_int(setting_pointer);
	}
	else
	{
		return -1;
	}
}

/*
* Function get_config_bool looks up the boolean value of 'name'
*  in the configuration setting 'setting' and returns it as an integer.
* -1 is returned if 'name' does not exist.
*/
static int get_config_bool(config_setting_t *setting, char *name)
{
	config_setting_t *setting_pointer = NULL;
	setting_pointer = config_setting_lookup(setting, name);
	if (setting_pointer != NULL)
	{
		return config_setting_get_bool(setting_pointer);
	}
	else
	{
		return -1;
	}
}

/*
* Function get_config_string.Gets the configuration string using the
* name provided as argument.
*/

static const char *get_config_string(config_setting_t *setting, char *name)
{
	config_setting_t *setting_pointer = NULL;
	setting_pointer = config_setting_lookup(setting, name);
	if (setting_pointer != NULL)
	{
		return config_setting_get_string(setting_pointer);
	}
	else
	{
		return NULL;
	}
}

/*
* Function close config.Destroys the config
*/

void close_config(void)
{
	config_destroy(config);
}
