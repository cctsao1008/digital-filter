#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <time.h>
#include <string.h>

#define DATE_COUNT   10001

typedef struct {
    float input_t1, input_t2, input_t3, input_t4;
    float output_t1, output_t2, output_t3, output_t4;
} filter_data_t;

filter_data_t filter_data ={0.0f};

/*  4th Order Low Pass Filter for 100 Hz Data
 *  cheby2(4,60,12.5/50)
 */
 
#define B0  0.001893594048567f
#define B1 -0.002220262954039f
#define B2  0.003389066536478f
#define B3 -0.002220262954039f
#define B4  0.001893594048567f

#define A1 -3.362256889209355f
#define A2  4.282608240117919f
#define A3 -2.444765517272841f
#define A4  0.527149895089809f

float iir_filter(float input, filter_data_t * fd)
{
    float output;

    output = B0 * input + 
	         B1 * fd->input_t1 + 
			 B2 * fd->input_t2 + 
			 B3 * fd->input_t3 + 
			 B4 * fd->input_t4 - 
			 A1 * fd->output_t1 - 
			 A2 * fd->output_t2 - 
			 A3 * fd->output_t3 - 
			 A4 * fd->output_t4;

    fd->input_t4 = fd->input_t3;
    fd->input_t3 = fd->input_t2;
    fd->input_t2 = fd->input_t1;
    fd->input_t1 = input;

    fd->output_t4 = fd->output_t3;
    fd->output_t3 = fd->output_t2;
    fd->output_t2 = fd->output_t1;
    fd->output_t1 = output;

    return output;
}

int msleep(unsigned long milisec)
{
    struct timespec req = {0};
    time_t sec = (int)(milisec / 1000);
    milisec = milisec - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = milisec * 1000000L;

    while(nanosleep(&req,&req) == (-1))
         continue;
    return 1;
}

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    FILE *fd = NULL;
    uint16_t count = 0;
    char *substr;
    char tmp_buf[DATE_COUNT*10] = {'\n'};
    
    float raw_data[DATE_COUNT] = {0.0f};
    float filted_data[DATE_COUNT] = {0.0f};

	/* Load raw data */
	{
		fd = fopen("sensor.csv", "r");
		fread(tmp_buf, 10, DATE_COUNT, fd);
		fclose(fd);
    
		fd = fopen("sensor_filted.csv", "w"); 

		substr = strtok(tmp_buf, "\r\n");
 
		while (substr != NULL && count != DATE_COUNT) {
			raw_data[count++] = atof(substr);
			//printf("float data = %f\n", atof(tok));
			substr = strtok(NULL, "\r\n");
		}

		printf("raw data[%d] = %f\n", 0, raw_data[0]);
		printf("raw data[%d] = %f\n", DATE_COUNT - 1, raw_data[DATE_COUNT - 1]);
    
		printf("count = %d\n", count);

		// check point
		system("pause");
	}
	
	count = 0;
	
	for(;;) {
		#if 0
		msleep(10);
		#endif
		
		/* read sensor data */
		
		/* do filtering */
		filted_data[count] = iir_filter(raw_data[count], &filter_data);
		
		/* log filted data */
		fprintf(fd,"%f\n", filted_data[count]);
		count++;
		
		printf("count = %d\n", count);
		
		/* check end of data */
		if(count == DATE_COUNT)
		    break;
	}

	fclose(fd);

	printf("finished!!\n");

	return 0;
}

