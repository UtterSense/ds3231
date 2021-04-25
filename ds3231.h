/*   MODULE HEADER FILE: ds3231.h --------------------------------------- 
 * 
 *   Header file for building a shared library to be used by JNI for
 *   communicationg with the LKM (device driver) for the DS3231
 *   board which has chips for
 * 	 RTC
 *   EEPROM.
 ----------------------------------------------------------------------*/ 
#ifndef DS3231_H
#define DS3231_H

#include <stdbool.h>

#define ANDROID_APP      //Add this define if we are running C Code as part of Android App 
                         //to facilitate adb logging          
#ifdef ANDROID_APP
 #include <android/log.h>
 #include <jni.h>
#endif

#define LEN 10

struct DateTime {
	
	char Year[LEN];
	char Month[LEN];
	char Day[LEN];
	char Date[LEN];
	char Hour[LEN];
	char Minute[LEN];
	char Second[LEN];
	
};	

struct DateTime dt;

struct Temperature {
	char TempUpper[LEN];   //Integer part
	char TempLower[LEN];   //Decimal part
};	

struct Temperature temp;

//Timer Attributes:
int timer_int = 1; //Secs
bool timer_flg = false;

char* m_DateTimeStr = "";
char* m_TempStr = "";

//Program control flag:
int done = 0;
	

void ds3231_test(void);
char* jni_test(void);
int init_dev(void);
void close_dev(void);
int32_t read_byte(char addr);
int32_t read_data(char* elem);
int32_t write_byte(char elem, char data);
void get_DateTime(void);
void get_Temperature(void);
char* get_year(void);
char* get_month(void);
char* get_date(void);
char* get_day(void);
char* get_hour(void);
char* get_minute(void);
char* get_second(void);
void set_DateTime(void);
void set_year(char d1, char d2);
void set_month(char d1, char d2);
void set_date(char d1, char d2);
void set_day(char d1);
void set_hour(char d1, char d2);
void set_minute(char d1, char d2);
void set_second(char d1, char d2);
char*  get_temp_upper(void);
char*  get_temp_lower(void);
void TimerHandler(int sig);
void start_timer(int interval);
void stop_timer(void);
void set_timer_int(int interval);



#ifdef ANDROID_APP
	void init_callbacks(char *f_name, char *f_sig);
	void sendToJava(char *c);
#endif


#ifdef ANDROID_APP
	JNIEnv *glo_env;
	JavaVM *JVM;
	jclass glo_clazz;
	jobject glo_obj;
	char *callback_name; 
	char *callback_sig;
#endif

#endif
