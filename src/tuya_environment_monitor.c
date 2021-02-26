/**
 * @File: tuya_environment_monitor.c 
 * @Author: shiliu.yang@tuya.com 
 * @Last Modified time: 2021-01-19 
 * @Description: 
 */

#include "tuya_dp_process.h"
#include "tuya_environment_monitor.h"
#include "tuya_iot_com_api.h"
#include "uni_log.h"
#include "tuya_uart.h"
#include "BkDriverUart.h"
#include "tuya_hal_semaphore.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uni_thread.h"
#include "soc_timer.h"
#include "soc_adc.h"

/* 传感器相关引脚定义 */
/* RS2255 IC 引脚 */
#define RS2255_A    TY_GPIOA_6
#define RS2255_B    TY_GPIOA_7

/* 火焰传感器引脚 */
#define FLAME_SENSOR_PIN    TY_GPIOA_8
/* 220V 断电 */
#define POWER_OFF_220V_PIN  TY_GPIOA_24
/* 甲醛传感器数据输入串口 */
#define CH2O_SENSOR_UART    TY_UART2
#define CH2O_BUFFER_SIZE    20
/* PM2.5 传感器数据输入串口 */
#define PM25_SENSOR_UART    TY_UART1
#define PM25_BUFFER_SIZE    20
/* 传感器预热时间 */
#define SENSOR_PREHEAT_TIME 30000U
/* 预热完成信号量，定时器 */
static SEM_HANDLE   preheat_semaphore;
static UCHAR_T      preheat_timer;
/* 烟雾传感器 单位：V */
#define SMOKE_ALARM_LIM 1.0
/* 燃气传感器 单位：V */
#define GAS_ALARM_LIM   1.0

/* ADC 采集次数 */
#define ADC_DATA_LEN    4 //ADC 单次采集次数
static  tuya_adc_dev_t  tuya_adc;

static VOID adc_init(VOID)
{
    tuya_adc.priv.pData = Malloc(ADC_DATA_LEN * sizeof(USHORT_T));
    memset(tuya_adc.priv.pData, 0, ADC_DATA_LEN*sizeof(USHORT_T));
    tuya_adc.priv.data_buff_size = ADC_DATA_LEN; //设置数据缓存个数
}

 /***********************************************************
 *   Function:  get_adc_value
 *   Input:     none
 *   Output:    adc_value : 采集到的 adc 值
 *   Return:    none
 *   Notice:    得到 adc 采集的电压值
 ***********************************************************/
VOID get_adc_value(OUT USHORT_T* adc_value) 
{
    INT_T ret;

    if (adc_value == NULL) {
        PR_ERR("pm25_adc_value is NULL");
        return;
    }
    memset(tuya_adc.priv.pData, 0, ADC_DATA_LEN*sizeof(USHORT_T));
    ret = tuya_hal_adc_init(&tuya_adc);
    if (ret != OPRT_OK) {
        PR_ERR("ADC init error : %d ", ret);
        return;
    }

    ret = ret = tuya_hal_adc_value_get(ADC_DATA_LEN, adc_value); 
    if (ret != OPRT_OK) {
        PR_ERR("ADC get value error : %d ", ret);
    }

    tuya_hal_adc_finalize(&tuya_adc);
    return;
}

 /***********************************************************
 *   Function:  ch2o_check_sum
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    甲醛数据校验和
 ***********************************************************/
static UCHAR_T ch2o_check_sum(UCHAR_T *data, UCHAR_T len)
{
    UCHAR_T i, tempq = 0;
    data += 1; //指向data[1]

    for(i=0; i<(len-2); i++)
    {
        tempq += *data;
        data++;
    }

    tempq = (~tempq) + 1;

    return (tempq);
}

 /***********************************************************
 *   Function:  get_pm25_sensor_value
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    得到并上传 pm2.5 的数据
 ***********************************************************/
static VOID get_pm25_sensor_value(VOID)
{
    UINT_T  buff_ret, find_head_index = 0;
    //PM2.5 数据高位，低位
    UCHAR_T pm25_data_high, pm25_data_low;
    //校验和
    UCHAR_T check_sums = 0x00;    
    //串口数据缓存区
    UCHAR_T pm25_receive_buffer[PM25_BUFFER_SIZE];
    //指向 PM2.5 数据头部
    UCHAR_T *p_pm25_value = NULL;

    memset(pm25_receive_buffer, 0, sizeof(pm25_receive_buffer));

    //读取串口数据
    //bk_uart_recv(PM25_SENSOR_UART, pm25_receive_buffer, PM25_BUFFER_SIZE, 0xFFFF);
    ty_uart_read_data(PM25_SENSOR_UART, pm25_receive_buffer, PM25_BUFFER_SIZE);

    // for (find_head_index = 0; find_head_index<PM25_BUFFER_SIZE; find_head_index++) {
    //     PR_NOTICE("pm25_receive_buffer[%d] = %02x", find_head_index, pm25_receive_buffer[find_head_index]);
    // }

    //寻找 PM2.5 传感器发送过来的头部
    for (find_head_index = 0; find_head_index<CH2O_BUFFER_SIZE; find_head_index++) {
        if (pm25_receive_buffer[find_head_index] == 0xff && pm25_receive_buffer[find_head_index+1] == 0x18 && pm25_receive_buffer[find_head_index+2] == 0x00){
            //PR_NOTICE("find head is %d", find_head_index);
            break;
        }
    }

    //本次采集数据不完整
    if (find_head_index > 11) { 
        PR_ERR("PM2.5 get uart data no complete!");
        return;
    }

    //将指针指向 PM2.5 数据中的头部
    p_pm25_value = pm25_receive_buffer + find_head_index;

    //检验和，确认读取的数据的准确性
    check_sums = ch2o_check_sum(p_pm25_value, 9);
    if (check_sums != *(p_pm25_value + 8)) {
        PR_ERR("PM2.5 check_sums error");
        return;
    }
    
    pm25_data_high = *(p_pm25_value+3);
    pm25_data_low = *(p_pm25_value+4);

    gs_environment_monitor.pm25_value = pm25_data_high * 100.0 + pm25_data_low;

    //PR_NOTICE("PM2.5: %d - %d .", pm25_data_high, pm25_data_low);

    //PR_NOTICE("PM2.5 value is : %d .", gs_environment_monitor.pm25_value);

    //上传 PM2.5 数据到涂鸦云
    updata_dp_single(gs_environment_monitor.dp_pm25_value, PROP_VALUE, gs_environment_monitor.pm25_value);

    return;
}

 /***********************************************************
 *   Function:  get_ch2o_sensor_value
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    得到并上传甲醛的数据
 ***********************************************************/
static VOID get_ch2o_sensor_value(VOID)
{
    UINT_T  buff_ret, find_head_index = 0;
    //甲醛数据高位，低位
    UCHAR_T ch2o_data_high, ch2o_data_low;
    //校验和
    UCHAR_T check_sums = 0x00;    
    //串口数据缓存区
    UCHAR_T ch2o_receive_buffer[CH2O_BUFFER_SIZE];
    //指向甲醛数据头部
    UCHAR_T *p_ch2o_value = NULL;

    memset(ch2o_receive_buffer, 0, sizeof(ch2o_receive_buffer));

    //读取串口数据
    bk_uart_recv(CH2O_SENSOR_UART, ch2o_receive_buffer, CH2O_BUFFER_SIZE, 0xFFFF);

    // for (find_head_index = 0; find_head_index<CH2O_BUFFER_SIZE; find_head_index++) {
    //     PR_NOTICE("ch2o_receive_buffer[%d] = %02x", find_head_index, ch2o_receive_buffer[find_head_index]);
    // }

    //寻找 ch2o 传感器发送过来的头部
    for (find_head_index = 0; find_head_index<CH2O_BUFFER_SIZE; find_head_index++) {
        if (ch2o_receive_buffer[find_head_index] == 0xff && \
            ch2o_receive_buffer[find_head_index+1] == 0x17 && \
            ch2o_receive_buffer[find_head_index+2] == 0x04){
            //PR_NOTICE("find head is %d", find_head_index);
            break;
        }
    }

    //本次采集数据不完整
    if (find_head_index > 11) { 
        PR_ERR("ch2o get uart data no complete!");
        return;
    }

    //将指针指向 ch2o 数据中的头部
    p_ch2o_value = ch2o_receive_buffer + find_head_index;

    //检验和，确认读取的数据的准确性
    check_sums = ch2o_check_sum(p_ch2o_value, 9);
    if (check_sums != *(p_ch2o_value + 8)) {
        PR_ERR("ch2o check_sums error");
        return;
    }
    
    ch2o_data_high = *(p_ch2o_value+4);
    ch2o_data_low = *(p_ch2o_value+5);

    gs_environment_monitor.ch2o_value = ch2o_data_high * 256 + ch2o_data_low;

    //PR_NOTICE("ch2o value is : %d .", gs_environment_monitor.ch2o_value);

    //上传 ch2o 数据到涂鸦云
    updata_dp_single(gs_environment_monitor.dp_ch2o_value, PROP_VALUE, gs_environment_monitor.ch2o_value);

    return;
}

 /***********************************************************
 *   Function:  get_smoke_sensor_value
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    得到并上传烟雾的数据，A1
 ***********************************************************/
static VOID get_smoke_sensor_value(VOID)
{
    USHORT_T    smoke_adc_value;
    FLOAT_T     smoke_volt; 

    //复用 adc 到 A1 
    tuya_gpio_write(RS2255_A, FALSE);
    tuya_gpio_write(RS2255_B, TRUE);

    tuya_hal_system_sleep(500);

    //得到烟雾传感器 ad 值
    get_adc_value(&smoke_adc_value);
    //PR_NOTICE("smoke_adc_value : %d ", smoke_adc_value);

    //计算实际电压值
    smoke_volt = (smoke_adc_value / 4095.0) * 2.4 * 2;
    //PR_NOTICE("smoke_volt : %lf ", smoke_volt);

    //判断是否到报警门限值
    if (smoke_volt >= SMOKE_ALARM_LIM) {
        gs_environment_monitor.smoke_state = ALARM;
    } else {
        gs_environment_monitor.smoke_state = NORMAL;
    }

    //上传数据
    updata_dp_single(gs_environment_monitor.dp_smoke_state, PROP_ENUM, gs_environment_monitor.smoke_state);

    //PR_NOTICE("get smoke value, updata...");
    return;
}

 /***********************************************************
 *   Function:  get_smoke_sensor_value
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    得到并上传燃气的数据，A2
 ***********************************************************/
static VOID get_gas_sensor_value(VOID)
{
    USHORT_T    gas_adc_value = 0;
    FLOAT_T     gas_volt; 

    //通过 RS2255 复用 adc 到 A2 
    tuya_gpio_write(RS2255_A, TRUE);
    tuya_gpio_write(RS2255_B, FALSE);

    tuya_hal_system_sleep(500);

    get_adc_value(&gas_adc_value);
    //PR_NOTICE("gas_adc_value : %d ", gas_adc_value);

    gas_volt = (gas_adc_value / 4095.0) * 2.4 * 2;
    //PR_NOTICE("gas_volt : %lf ", gas_volt);

    if (gas_volt >= GAS_ALARM_LIM) {
        gs_environment_monitor.gas_state = ALARM;
    } else {
        gs_environment_monitor.gas_state = NORMAL;
    }

    updata_dp_single(gs_environment_monitor.dp_gas_state, PROP_ENUM, gs_environment_monitor.gas_state);
    
    //PR_NOTICE("get gas value, updata...");
    return;
}

 /***********************************************************
 *   Function:  get_smoke_sensor_value
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    得到并上传火焰的数据
 ***********************************************************/
static VOID get_flame_sensor_value(VOID)
{
    
    if (FALSE == tuya_gpio_read(FLAME_SENSOR_PIN)) {
        gs_environment_monitor.flame_state = ALARM;
        /* 检测到火焰，拉低220V控制引脚，断电 */
        tuya_gpio_write(POWER_OFF_220V_PIN, FALSE);
    } else {
        gs_environment_monitor.flame_state = NORMAL;
    }

    updata_dp_single(gs_environment_monitor.dp_flame_state, PROP_ENUM, gs_environment_monitor.flame_state);

    return;
}

 /***********************************************************
 *   Function:  acquire_data_task
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    获取传感器数据任务
 ***********************************************************/
VOID acquire_data_task(VOID)
{
    //等待预热完成
    tuya_hal_semaphore_wait(preheat_semaphore);

    while (1) {
        get_ch2o_sensor_value();
        get_flame_sensor_value();
        get_gas_sensor_value();
        get_pm25_sensor_value();
        get_smoke_sensor_value();
        tuya_hal_system_sleep(500);
    }
}

VOID preheat_semaphore_post_task(VOID)
{
    //预热完成，释放信号量
    tuya_hal_semaphore_post(preheat_semaphore);
    //关闭预热软件定时器
    opSocSWTimerStop(preheat_timer);

    //预热完成，上传预热完成数据到涂鸦云
    gs_environment_monitor.preheat_state = false;
    updata_dp_single(gs_environment_monitor.dp_preheat, PROP_BOOL, gs_environment_monitor.preheat_state);
}

 /***********************************************************
 *   Function:  air_box_device_init
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    空气盒子设备初始化
 ***********************************************************/
VOID air_box_device_init(VOID)
{
    INT_T opRet = OPRT_OK;

    /* 火焰传感器相关外设初始化 */
    tuya_gpio_inout_set(FLAME_SENSOR_PIN, TRUE);

    tuya_gpio_inout_set(POWER_OFF_220V_PIN, FALSE);
    /* 启动时，拉高220V控制引脚，通电 */
    tuya_gpio_write(POWER_OFF_220V_PIN, TRUE);

    /* ADC 复用，相关引脚初始化 */ 
    tuya_gpio_inout_set(RS2255_A, FALSE);
    tuya_gpio_inout_set(RS2255_B, FALSE);
    adc_init();

    /* 甲醛传感器使用 uart2 接收数据，在函数 app_init() 里面已经修改完成波特率 */
    
    /* pm2.5 传感器 串口 初始化 */
    ty_uart_init(PM25_SENSOR_UART, TYU_RATE_9600, TYWL_8B, TYP_NONE, TYS_STOPBIT1, (PM25_BUFFER_SIZE * SIZEOF(UCHAR_T)), TRUE);

    /* 创建信号量 */
    opRet = tuya_hal_semaphore_create_init(&preheat_semaphore, 0, 1);
    if (opRet != OPRT_OK) {
        PR_ERR("creat preheat semaphore error : %d", opRet);
    }

    /* 预热 30s 后释放信号量，开始采集传感器数据 */
    opSocSWTimerStart(preheat_timer, SENSOR_PREHEAT_TIME, preheat_semaphore_post_task); 

    tuya_hal_thread_create(acquire_data_task, "acquire sensor data", 512 * 4, NULL, TRD_PRIO_2, NULL);
}
