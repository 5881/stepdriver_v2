/*
 * Такттовый генератор контроллера шагового двигателя.
 * v2.0
 * Александр Белый 2026
 * https://t.me/candidum5881
 */

/**********************************************************************
 * Секция include и defines
**********************************************************************/
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/assert.h>
#include <libopencm3/stm32/flash.h>
//Частоту на обмотках двигателя задавать здесь в герцах
#define OUTPUT_FREQ 77
#define FCPU 8e6
#define RATIO 32
#define PERIOD (int)(FCPU/RATIO/OUTPUT_FREQ)
#define OC (int)(PERIOD/2)


static void rcc_clock_setup_in_hse_8mhz(void){
        /* Enable internal high-speed oscillator. */
        rcc_osc_on(RCC_HSI);
        rcc_wait_for_osc_ready(RCC_HSI);
 
        /* Select HSI as SYSCLK source. */
        rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSICLK);
 
        /* Enable external high-speed oscillator 8MHz. */
        rcc_osc_on(RCC_HSE);
        rcc_wait_for_osc_ready(RCC_HSE);
        rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);
 
        /*
         * Set prescalers for AHB, ADC, APB1, APB2.
         * Do this before touching the PLL (TODO: why?).
         */
        rcc_set_hpre(RCC_CFGR_HPRE_SYSCLK_NODIV);    /* Set. 8MHz Max. 72MHz */
		rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV2);  /* Set. 4MHz Max. 14MHz */
		rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_NODIV);    /* Set. 8MHz Max. 36MHz */
		rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV); 
        /*
         * Sysclk runs with 24MHz -> 0 waitstates.
         * 0WS from 0-24MHz
         * 1WS from 24-48MHz
         * 2WS from 48-72MHz
         */
        flash_set_ws(FLASH_ACR_LATENCY_0WS);
        /* Set the peripheral clock frequencies used */
        rcc_ahb_frequency = 8000000;
        rcc_apb1_frequency = 8000000;
        rcc_apb2_frequency = 8000000;
}

static void control_setup(void){
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
	              GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
	gpio_clear(GPIOA,GPIO12);
}

void timer1_init(){
	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO8);
	//gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO8);
	// End of family specific
	rcc_periph_clock_enable(RCC_TIM1);
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
				   TIM_CR1_DIR_UP);
	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);
	//timer_set_prescaler(TIM1, 24);
	timer_enable_oc_output(TIM1, TIM_OC1);
	timer_enable_break_main_output(TIM1);
	//5714 & 2857 for 24MHz
	/*F=F_MCU/TIM1_PERIOD
	 *OC_VAKUE=TIM1_PERIOD/2
	 *ROT_SPEED=F/SNEP_ON_TURN
	 *F_OUT=F/32=F_MCU/TIM1_PERIOD/32
	 *1905 & 952 -> 131Hz
	 */
	timer_set_period(TIM1, PERIOD);
	timer_set_oc_value(TIM1, TIM_OC1, OC);
	timer_enable_counter(TIM1);
};



	
void main(){
	//rcc_clock_setup_in_hse_8mhz_out_24mhz();
	rcc_clock_setup_in_hse_8mhz();
	//control_setup();
	timer1_init();
	while (1){
		//for(uint32_t i=0;i<0xfffff;i++)__asm__("nop");
		//gpio_toggle(GPIOB,GPIO12);
		};
}

