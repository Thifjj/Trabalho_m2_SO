// #include "ntrap.h"
// #include "ntimer.h"
// #include "scheduler.h"
// #include "uart.h"

// void trap_handler(uint64_t *frame){
//     uint64_t scause;

//     /* Ler o CSR scause */
//     asm volatile("csrr %0, scause" : "=r"(scause));

//     /* Pegando os valores necessarios para verificar interrupcao*/ 
//     uint64_t is_interrupt = scause & (1ULL << 63);
//     uint64_t exception_code = scause & ~(1ULL << 63);

//     /* Verificar se é uma interrupção E se é o Supervisor Timer Interrupt) */
//     if (is_interrupt && exception_code == 5) {
        
//         /* Chamar timer_next() para programar o próximo pulso e limpar a interrupção atual */
//         timer_next();
        
//         /* Chamar o escalonador para forçar a troca de contexto preemptiva */
//         schedule_from_trap(frame);
        
//         return; /* Sai da função para não cair no loop infinito abaixo */
//     }

//     uart_print("Unhandled trap\n");

//     while (1);
// }

#include "ntrap.h"
#include "ntimer.h"
#include "scheduler.h"
#include "uart.h"

extern void uart_print_uint(uint64_t val); // Garantindo o acesso à função de impressão

void trap_handler(uint64_t *frame){
    uint64_t scause, sepc, stval;

/* Ler os CSRs essenciais para depuração */
    asm volatile("csrr %0, scause" : "=r"(scause));
    asm volatile("csrr %0, sepc"   : "=r"(sepc));
    asm volatile("csrr %0, stval"  : "=r"(stval));

    uint64_t is_interrupt = scause & (1ULL << 63);
    uint64_t exception_code = scause & ~(1ULL << 63);

    /* Tratamento do Timer (Interrupção código 5) */
    if (is_interrupt && exception_code == 5) {
        timer_next();
        schedule_from_trap(frame);
        return; 
    }

    /* === NOVA ABORDAGEM: RELATÓRIO DE CRASH (PANIC) === */
    uart_print("\n[KERNEL PANIC] EXCECAO NAO TRATADA!\n");
    
    uart_print("SCAUSE (Motivo) : ");
    uart_print_uint(exception_code);
    uart_print("\n");

    uart_print("SEPC (Instrucao): ");
    uart_print_uint(sepc);
    uart_print("\n");

    uart_print("STVAL (Endereco): ");
    uart_print_uint(stval);
    uart_print("\n");

    /* Trava o sistema para podermos ler a tela */
    while (1);
}