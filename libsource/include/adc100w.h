#ifdef __cplusplus
#define PREF1 extern "C"
#else
#define PREF1
#endif


#if defined (WIN32)
#define HUGE
#define PREF2 __declspec(dllexport) __stdcall
#elif defined (WINDOWS)
#define HUGE		huge
#define PREF2 _export far pascal
#else
#define HUGE		huge
#define PREF2 far pascal
#endif

#define MAX_ADC100_UNITS 	3
#define MAX_ADC100_VALUE	4095
#define ZERO_ADC100_VALUE	2048

#ifndef CM_DEFINED
#define CM_DEFINED
typedef enum {CM_AVERAGE, CM_MINIMUM, CM_MAXIMUM, CM_SUM, CM_MAX} COMBINATION_METHOD;
#endif

  PREF1 short PREF2 adc100_get_driver_version (void);

  PREF1 void PREF2 adc100_apply_fix (unsigned short fix_no, unsigned short value);

  PREF1 short PREF2 adc100_open_unit (short port);

  PREF1 short PREF2 adc100_set_unit (short port);

  PREF1 void PREF2 adc100_close_unit (short port);

  PREF1 short PREF2 adc100_get_unit_info (char * str, short str_lth, short line, short port);

  PREF1 void PREF2 adc100_set_range (short range_a, short range_b);

  PREF1 void PREF2 adc100_set_trigger (	unsigned short enabled,
					unsigned short auto_trigger,
					unsigned short auto_ms,
					unsigned short channel,
					unsigned short dir,
					unsigned short threshold,
					unsigned short delay);

  PREF1 unsigned long PREF2 adc100_set_interval (
						unsigned long us_for_block,
						unsigned long ideal_no_of_samples,
						short channel);

  PREF1 unsigned long PREF2 adc100_get_values (unsigned short HUGE * buffer_a, unsigned short HUGE * buffer_b, unsigned long no_of_values);

  PREF1 unsigned long PREF2 adc100_get_times_and_values (long HUGE * times, unsigned short HUGE * buffer_a, unsigned short HUGE * buffer_b, unsigned long no_of_values);

  PREF1 short PREF2 adc100_get_value (unsigned short channel);

  PREF1 unsigned long PREF2 adc100_get_combined_values
	(
	unsigned short channel,
	COMBINATION_METHOD  mode,   /* Combination modes (CM_XXX) */
	unsigned short readings_for_channels/* No of readings */
	);



