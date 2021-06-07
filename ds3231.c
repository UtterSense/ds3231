/*   MODULE: ds3231.c ------------------------------------------------- 
 *   Source file for building a shared library to be used by JNI for
 *   communicationg with the LKM (device driver) for the DS3231
 *   board which has chips for
 *   RTC
 *   EEPROM.
 *   
 ----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>               //Communication facility between kernel and user space
#include <signal.h>
#include "ds3231.h"
									

//Define approrpiate logging function protoptye
//logf will be the alias for both Android App and normal Linux builds
#ifdef ANDROID_APP
	#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "** NATIVE C++ **: ", __VA_ARGS__))
#else
    #define LOGI(...) printf(__VA_ARGS__)
#endif 



#define BUFFER_LENGTH 256               ///< The buffer length for messages 
static char receive[BUFFER_LENGTH] = {0};     ///< The receive buffer from the LKM

int res;
  
//File pointer for file commands  
int fd;

//Flag for logging:
//bool logging = true;

int done = 0;
int timer_int = 1;
bool timer_flg = false;


//NB: This handler is working for the CTL-C keyboard signal
static void ctrl_c_handler(int n, siginfo_t *info,void *unused)
{
   if(n == SIGINT) //Check it is the right user ID
   {
      LOGI("\nWe have received the CTRL-C signal!\n");
   }   
   done = 1; //This will stop program
}



void TimerHandler(int sig)
{
    //printf("Timer call index: %d\n",timer_cnt);
    get_DateTime();
    get_Temperature();
    if(timer_flg)
    {   
       alarm(timer_int);
    }
      
    
} 

void start_timer(int interval)
{
   
    set_timer_int(interval);
    timer_flg = true; 
    signal(SIGALRM, TimerHandler);
    alarm(timer_int);
   
   
}//start_timer
   
   

void stop_timer(void)
{
   timer_flg = false;
   
   
}//stop_timer   

 

void set_timer_int(int interval)
{
   timer_int = interval;
   
}//set_timer_int   

  
void ds3231_test(void)
{
	int num = 5;
	char *str = "Hello from ds3231!";
	
	LOGI("Integer number: %d, String %s\n", num, str);


}//ds3231_test

char* jni_test(void)
{
	//Test to check we can interface with Java space
	return "Communicating with DS3231 shared library OK!\n";
	 	

}//jni_test



//Open the device driver
int init_dev(void)
{
	
   //Install CTRL-C Interrupt handler for program control:
   struct sigaction  ctrl;
   
   
   ctrl.sa_flags = (SA_SIGINFO | SA_RESETHAND);
   ctrl.sa_sigaction = ctrl_c_handler;
   if( sigaction(SIGINT,&ctrl,NULL) == -1)
   {
      printf("Problem setting up signal handler for SIGINTn");
   }   
   else
   {
      //printf("Signal handler for SIGINT set up OK!\n");
   }
   
   
   //LOGI("Opening the ds3231 device driver\n");
		
   //Open driver:
   fd = open("/dev/ds3231", O_RDWR);             // Open the device with read/write access
   if (fd < 0)
   {
		LOGI("Failed to open the device driver...");
		LOGI("The error number: %d",errno);
		LOGI("Error description: %s",strerror(errno));
				
		return errno;
   }
   //LOGI("Opened device driver OK\n");
  	
	
   return 0;
    	
	
}//init_dev


int32_t read_byte(char addr)
{
   //Read value back from kernel driver
   //Define address to read from:
   receive[0] = addr;   
   res = read(fd, receive,BUFFER_LENGTH);
   if (res < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("The received message is: %s\n", receive);
   
   return 0;
     
}  


int32_t read_data(char* elem)
{
   //Read value back from kernel driver
   //Define address to read from:
   strcpy(receive,elem);
   //res = read(fd, receive,BUFFER_LENGTH);
   res = read(fd, receive,strlen(receive));
   if (res < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   //printf("The received message is: %s: Len: %d\n", receive,strlen(receive));
   return 0;
}  


int32_t write_byte(char elem, char data)
{
  //Read value back from kernel driver
   //Define address to read from:
   char buf[3];
   buf[0] = elem;
   buf[1] = data;
   res = write(fd, buf,strlen(buf));
   if (res < 0){
      perror("Failed to write to the device.");
      return errno;
   }          
   return 0;	  
}  


void get_DateTime()
{
  
   strcpy(dt.Year,get_year());
   strcpy(dt.Month,get_month());
   strcpy(dt.Date,get_date());
   strcpy(dt.Day,get_day());
   strcpy(dt.Hour,get_hour());
   strcpy(dt.Minute,get_minute());
   strcpy(dt.Second,get_second());
   
    
   LOGI("Current Date and Time:\n");
   LOGI("%s, %s %s, 20%s | %s:%s:%s\n",dt.Day,dt.Date,dt.Month,dt.Year,dt.Hour,dt.Minute,dt.Second);
  #ifdef ANDROID_APP
	  sendToJava("Current Date and Time:\n");
	  sendToJava(dt.Day);
	  sendToJava(", ");
	  sendToJava(dt.Date);
	  sendToJava(", ");
	  sendToJava(dt.Month);
	  sendToJava(", 20");
	  sendToJava(dt.Year);
	  sendToJava(" | ");
	  sendToJava(dt.Hour);
	  sendToJava(":");
	  sendToJava(dt.Minute);
	  sendToJava(":");
	  sendToJava(dt.Second);
	  sendToJava("\n");
  #endif
  
}  

void get_Temperature()
{
   strcpy(temp.TempUpper,get_temp_upper());
   LOGI("Current DS3231 on-board temp:\n");
   strcpy(temp.TempLower,get_temp_lower());
   LOGI("%s.%s (C)\n",temp.TempUpper,temp.TempLower);
   #ifdef ANDROID_APP
	  sendToJava("Current DS3231 on-board temp:\n");
	  sendToJava(temp.TempUpper);
	  sendToJava(".");
	  sendToJava(temp.TempLower);
	  sendToJava(" (C)\n");
  #endif
   

}

char* get_year()
{
   read_data("yr");
   return receive;
  
}  

char* get_month()
{
   read_data("mo");
   return receive;
}  

char* get_date()
{
   read_data("Da");
   return receive;
}  

char* get_day()
{
   read_data("da");
   return receive;
}  



char* get_hour()
{
   read_data("ho");
   return receive;
}  

char* get_minute()
{
   read_data("mi");
   return receive;
}  

char* get_second()
{
   read_data("se");
   return receive;
}  


int32_t getHMSTimestamp()
{
	int32_t ts_hr,ts_min,ts_sec;
	//Return the total timestamp (secs) for hrs/mins/secs
	read_data("ho");
   ts_hr =  60*60*atoi(receive);
   read_data("mi");
   ts_min =  60*atoi(receive);
   read_data("se");
   ts_sec =  atoi(receive);
      
	return ts_hr+ts_min+ts_sec; 
	
}//getHMSTimestamp	


char* get_temp_upper()
{
   read_data("tu");
   return receive;
}

char* get_temp_lower()
{
   read_data("tl");
   return receive;
}




void set_year(char d1, char d2)
{
      //Example: **20; d1=0x02, d2=0x00
      char byte = (d1 << 4) | d2;
      write_byte('Y',byte);
}	


void set_month(char d1, char d2)
{
   char byte = (d1 << 4) | d2;  
   write_byte('M',byte);
   
}	


void set_day(char d1)
{
    write_byte('d',d1);
  
}	


void set_date(char d1, char d2)
{
    char byte = (d1 << 4) | d2;  
    write_byte('D',byte);
}	


void set_hour(char d1, char d2)
{
    //Set hour (24 hr mode)
    char byte = (d1 << 4) | d2;
    write_byte('h',byte);
}	


void set_minute(char d1, char d2)
{
    char byte = (d1 << 4) | d2;
    write_byte('m',byte);
}	


void set_second(char d1, char d2)
{
    char byte = (d1 << 4) | d2;
    write_byte('s',byte);
}	

 

void set_DateTime()
{
		
	set_year(0x02,0x00);
	usleep(100);
	set_month(0x00,0x04);
	usleep(100);
	set_day(0x02);
	usleep(100);
	set_date(0x02,0x07);
	usleep(100);
	set_hour(0x01,0x04);
	
}


	
//Close the device driver	
void close_dev(void)
{
    LOGI("Closing the ds3231 device driver\n");
	close(fd);
	
}//close_dev
	
	


//---------------------------------------------------------------------
//DEFINE METHODS FOR JAVA CALLBACK HERE (If Android App realisation):
#ifdef ANDROID_APP
void init_callbacks(char *f_name, char *f_sig)
{
	//strcpy(callback_name,f_name);
	callback_name = f_name;
	callback_sig = f_sig;
	LOGI("Set callback strings OK!");
	LOGI("Callback name: %s",callback_name);
	LOGI("Callback signature: %s",callback_sig);
}
	
void sendToJava(char *c)
{
	
	//LOGI("JNIEnv pointer 0x%p",glo_env);
	//LOGI("JVM pointer 0x%p",JVM);
	//LOGI("Global class 0x%p",&glo_clazz);
	//LOGI("Global object 0x%p",&glo_obj);
    
	    
    jstring js = (*glo_env)->NewStringUTF(glo_env,c);
    jclass clazz = (*glo_env)->GetObjectClass(glo_env,glo_obj);
	jmethodID mid_cb = (*glo_env)->GetMethodID(glo_env,clazz,callback_name, callback_sig);
    (*glo_env)->CallVoidMethod(glo_env, glo_obj,mid_cb,js); //OK with this
      
    
	LOGI("Data sent to Java!");
	
	
}	

#endif	
//---------------------------------------------------------------------
