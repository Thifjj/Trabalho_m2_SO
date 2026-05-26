#include "ntrap.h"
#include "ntimer.h"
#include "scheduler.h"
#include "uart.h"

void trap_handler(uint64_t *frame){
    uint64_t scause;

    /* Ler o CSR scause */
    asm volatile("csrr %0, scause" : "=r"(scause));

    /* Pegando os valores necessarios para verificar interrupcao*/ 
    uint64_t is_interrupt = scause & (1ULL << 63);
    uint64_t exception_code = scause & ~(1ULL << 63);

    /* Verificar se é uma interrupção E se é o Supervisor Timer Interrupt) */
    if (is_interrupt && exception_code == 5) {
        
        /* Chamar timer_next() para programar o próximo pulso e limpar a interrupção atual */
        timer_next();
        
        /* Chamar o escalonador para forçar a troca de contexto preemptiva */
        schedule_from_trap(frame);
        
        return; /* Sai da função para não cair no loop infinito abaixo */
    }

    uart_print("Unhandled trap\n");

    while (1);
}