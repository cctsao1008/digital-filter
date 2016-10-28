/**
 * @file main.c
 *
 * IIR/FIR Filter implementation
 *
 * @author Ricardo <tsao.ricardo@iac.com.tw>
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <time.h>
#include <string.h>

#define DATE_COUNT   10001

#define IIR

typedef struct {
    float input_t1, input_t2, input_t3, input_t4;
    
    #if !defined(IIR)
    // FIR
    float input_t5, input_t6, input_t7, input_t8;
    float input_t9, input_t10, input_t11, input_t12;
    #endif
    
    float output_t1, output_t2, output_t3, output_t4;
} filter_data_t;

filter_data_t fd ={0.0f};

#if defined(IIR)
/*  4th Order Low Pass Filter for 100 Hz Data
 *  cheby2(4,60,12.5/50), IIR Filter
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

float cheby2_filter(float input, filter_data_t * fd)
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

#else // FIR
/*  12th Order Low Pass Filter for 100 Hz Data
 *  fir1(12,12.5/50), FIR Filter
 */

#define B0  -0.004342695995163f
#define B1  -0.006523576737266f
#define B2   0.000000000000000f
#define B3   0.041455122121951f
#define B4   0.125395346860321f
#define B5   0.216112849045374f
#define B6   0.255805909409564f
#define B7   0.216112849045374f
#define B8   0.125395346860321f
#define B9   0.041455122121951f
#define B10  0.000000000000000f
#define B11 -0.006523576737266f
#define B12 -0.004342695995163f

float fir1_filter(float input, filter_data_t * fd)
{
    float output;

    output = B0 * input + 
             B1 * fd->input_t1 + 
             B2 * fd->input_t2 + 
             B3 * fd->input_t3 + 
             B4 * fd->input_t4 +
             B5 * fd->input_t5 +
             B6 * fd->input_t6 +
             B7 * fd->input_t7 +
             B8 * fd->input_t8 +
             B9 * fd->input_t9 +
             B10 * fd->input_t10 +
             B11 * fd->input_t11 +
             B12 * fd->input_t12;;

    fd->input_t12 = fd->input_t11;
    fd->input_t11 = fd->input_t10;
    fd->input_t10 = fd->input_t9;
    fd->input_t9 = fd->input_t8;
    fd->input_t8 = fd->input_t7;
    fd->input_t7 = fd->input_t6;
    fd->input_t6 = fd->input_t5;
    fd->input_t5 = fd->input_t4;
    fd->input_t4 = fd->input_t3;
    fd->input_t3 = fd->input_t2;
    fd->input_t2 = fd->input_t1;
    fd->input_t1 = input;

    return output;
}
#endif

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

float raw_data[DATE_COUNT] = {0.0f};

void sensor_data_write(float data)
{
    FILE *file = NULL;
    uint16_t count = 0;
    char *substr;
    char tmp_buf[DATE_COUNT*10] = {'\n'};

    /* Load raw data */
    {
        file = fopen("raw_data.csv", "r");
        fread(tmp_buf, 10, DATE_COUNT, file);
        fclose(file);

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
}

float sensor_data_read(void)
{
    static uint16_t count = 0;
    return raw_data[count++];
}

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
    FILE *file = NULL;
    uint16_t count = 0;
    
    float filtered_data[DATE_COUNT] = {0.0f};
    float data;
    
    sensor_data_write(0);
    
    file = fopen("filtered_data.csv", "w+");
    
    for(;;) {		
        /* read sensor data */
        data = sensor_data_read();

        /* do filtering */
        #if defined(IIR)
        filtered_data[count] = cheby2_filter(data, &fd);
        #else // FIR
        filtered_data[count] = fir1_filter(data, &fd);
        #endif
        
        /* log filtered data */
        fprintf(file,"%f\n", filtered_data[count]);
        count++;
        
        printf("count = %d\n", count);
        
        /* check end of data */
        if(count == DATE_COUNT)
            break;
            
        #if 0
        msleep(10);
        #endif
    }

    fclose(file);

    printf("finished!!\n");

    return 0;
}

