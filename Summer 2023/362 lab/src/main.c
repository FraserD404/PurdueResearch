/**
  ******************************************************************************
  * @file    main.c
  * @author  Weili An
  * @version V1.0
  * @date    Nov 26, 2022
  * @brief   ECE 362 Lab 11 student template
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include <stdint.h>

// Uncomment only one of the following to test each step
#define STEP41
//#define STEP42
//#define STEP43
//#define STEP44

void init_usart5();

void init_usart5() {
    // TODO

    // enable the ports
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->AHBENR |= RCC_AHBENR_GPIODEN; 

    // set alternate functions
    GPIOC->MODER |= GPIO_MODER_MODER12_1;
    GPIOD->MODER |= GPIO_MODER_MODER2_1;

    // set tx and rx respectively
    GPIOC->AFR[1] |= 0x2 << 16;
    GPIOC->AFR[0] |= 0x2 << 8;

    RCC->APB1ENR |= RCC_APB1ENR_USART5EN;

    // disable uart
    USART5->CR1 &= 0;

    USART5->CR1 &= ~(0x1 << 28 | 0x1 << 12); 
    USART5->CR2 &= ~USART_CR2_STOP;
    USART5->CR1 &= ~USART_CR1_PCE;
    USART5->CR1 &= ~USART_CR1_OVER8;

    // CLEAR THE BRR
    USART5->BRR &= ~0xFFFF;

    // set the baud rate
    USART5->BRR = 0x1A1;

    USART5->CR1 |= (USART_CR1_TE | USART_CR1_RE);

    // enable uart
    USART5->CR1 |= 1; 

    // wait for acknowledgement
    while(!(((USART5 -> ISR) & USART_ISR_TEACK) && ((USART5 -> ISR) & USART_ISR_REACK))){
        // spin and do nothing
        // like me when i'm unmotivated to work ^_^
        continue; 
    }
}

#ifdef STEP41
int main(void){
    init_usart5();
    for(;;) {
        while (!(USART5->ISR & USART_ISR_RXNE)) { }
        char c = USART5->RDR;
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = c;
    }
}
#endif

#ifdef STEP42
#include <stdio.h>

// TODO Resolve the echo and carriage-return problem

int __io_putchar(int c) {
    while(!(USART5->ISR & USART_ISR_TXE));

    if(c == '\n') {
        USART5->TDR = '\r';
        
        // spin and do nothing
        while(!(USART5->ISR & USART_ISR_TXE));
    }

    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    while (!(USART5->ISR & USART_ISR_RXNE));

    char c = USART5->RDR;

    if(c == '\r'){
        c = '\n';
    }

    __io_putchar(c);

    return c;
}

int main() {
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP43
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
int __io_putchar(int c) {
    // TODO Copy from your STEP42

    while(!(USART5->ISR & USART_ISR_TXE));

    if(c == '\n') {
        USART5->TDR = '\r';
        
        // spin and do nothing
        while(!(USART5->ISR & USART_ISR_TXE));
    }

    USART5->TDR = c;
    return c;

}

int __io_getchar(void) {
    // TODO
    return line_buffer_getchar();
}

int main() {
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP44

#include <stdio.h>
#include "fifo.h"
#include "tty.h"

// TODO DMA data structures
#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0; 

void enable_tty_interrupt(void) {
    // TODO
    NVIC_EnableIRQ(USART3_8_IRQn);

    USART5->CR1 |= USART_CR1_RXNEIE;
    USART5->CR3 |= USART_CR3_DMAR; 

    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    
    DMA2->RMPCR |= DMA_RMPCR2_CH2_USART5_RX;

    DMA2_Channel2->CCR &= !DMA_CCR_EN;

    DMA2_Channel2->CMAR = (int) serfifo; 
    DMA2_Channel2->CPAR = (int) &(USART5->RDR);
    DMA2_Channel2->CNDTR = FIFOSIZE;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_DIR; 
    DMA2_Channel2 -> CCR &= ~(DMA_CCR_MSIZE | DMA_CCR_PSIZE); //set 8 bits
    DMA2_Channel2 -> CCR |= DMA_CCR_MINC; 

    DMA2_Channel2 -> CCR &= ~DMA_CCR_PINC; 
    DMA2_Channel2 -> CCR |= DMA_CCR_CIRC; 
    
    DMA2_Channel2 -> CCR |= DMA_CCR_PL; 

    DMA2_Channel2 -> CCR |= DMA_CCR_EN;
}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi");
    }

    char ch = fifo_remove(&input_fifo);
    return ch; 
}

int __io_putchar(int c) {
    // TODO Copy from step 42

    while(!(USART5->ISR & USART_ISR_TXE));

    if(c == '\n') {
        USART5->TDR = '\r';
        
        // spin and do nothing
        while(!(USART5->ISR & USART_ISR_TXE));
    }

    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()

    return (int) interrupt_getchar(); 
}

// TODO Copy the content for the USART5 ISR here
// TODO Remember to look up for the proper name of the ISR function

void USART3_4_5_6_7_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo)) {
            insert_echo_char(serfifo[seroffset]);
        }

        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}
int main() {
    init_usart5();

    enable_tty_interrupt();

    setbuf(stdin,0); // These turn off buffering; more efficient, but makes it hard to explain why first 1023 characters not dispalyed
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: "); // Types name but shouldn't echo the characters; USE CTRL-J to finish
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n"); // After, will type TWO instead of ONE
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif




/*



*/




#ifdef SHELL

#include <stdio.h>
#include "fifo.h"
#include "tty.h"

// TODO DMA data structures
#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0; 

void enable_tty_interrupt(void) {
    // TODO
    NVIC_EnableIRQ(USART3_8_IRQn);

    USART5->CR1 |= USART_CR1_RXNEIE;
    USART5->CR3 |= USART_CR3_DMAR; 

    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    
    DMA2->RMPCR |= DMA_RMPCR2_CH2_USART5_RX;

    DMA2_Channel2->CCR &= !DMA_CCR_EN;

    DMA2_Channel2->CMAR = (int) serfifo; 
    DMA2_Channel2->CPAR = (int) &(USART5->RDR);
    DMA2_Channel2->CNDTR = FIFOSIZE;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_DIR; 
    DMA2_Channel2 -> CCR &= ~(DMA_CCR_MSIZE | DMA_CCR_PSIZE); //set 8 bits
    DMA2_Channel2 -> CCR |= DMA_CCR_MINC; 

    DMA2_Channel2 -> CCR &= ~DMA_CCR_PINC; 
    DMA2_Channel2 -> CCR |= DMA_CCR_CIRC; 
    
    DMA2_Channel2 -> CCR |= DMA_CCR_PL; 

    DMA2_Channel2 -> CCR |= DMA_CCR_EN;
}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi");
    }

    char ch = fifo_remove(&input_fifo);
    return ch; 
}

int __io_putchar(int c) {
    // TODO Copy from step 42

    while(!(USART5->ISR & USART_ISR_TXE));

    if(c == '\n') {
        USART5->TDR = '\r';
        
        // spin and do nothing
        while(!(USART5->ISR & USART_ISR_TXE));
    }

    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()

    return (int) interrupt_getchar(); 
}

// TODO Copy the content for the USART5 ISR here
// TODO Remember to look up for the proper name of the ISR function

void USART3_4_5_6_7_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo)) {
            insert_echo_char(serfifo[seroffset]);
        }

        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}
int main() {
    init_usart5();

    enable_tty_interrupt();

    setbuf(stdin,0); // These turn off buffering; more efficient, but makes it hard to explain why first 1023 characters not dispalyed
    setbuf(stdout,0);
    setbuf(stderr,0);

    /*
    printf("Enter your name: "); // Types name but shouldn't echo the characters; USE CTRL-J to finish
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n"); // After, will type TWO instead of ONE
    for(;;) {
        char c = getchar();
        putchar(c);
    }
    */

    command_shell();
}
#endif
