#include "ntimer.h"

//valor default de intervalo
static uint64_t tick_interval = 100000;

/* Implementação da chamada SBI set_timer via ecall */
static inline void sbi_set_timer(uint64_t stime_value) {
    asm volatile(
        "mv a0, %0\n"       // a0 recebe o tempo programado
        "li a7, 0\n"        // a7 = 0 é o ID da extensão legacy set_timer do OpenSBI
        "ecall"             // Realiza a chamada de sistema (trap) para o Machine Mode
        :
        : "r" (stime_value)
        : "a0", "a7", "memory"
    );
}

void timer_next(){
    uint64_t now;

    //ler csr time
    asm volatile("csrr %0, time" : "=r"(now));

    //Chamada SBI set_timer via ecall
    sbi_set_timer(now+tick_interval);
}

void timer_init(uint64_t interval){
    if(interval !=0){
        tick_interval = interval;
    }

    timer_next();

    //torna 1 o bit 5 (STIE) no CSR SIE
    asm volatile("csrs sie, %0" :: "r"(1ULL << 5));

    //torna 1 o bit 1 do csrs sstatus (SIE global)
    asm volatile("csrs sstatus, %0" :: "r"(1ULL << 1));
};