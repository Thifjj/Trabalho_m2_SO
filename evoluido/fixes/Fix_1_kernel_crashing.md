### 1. O Problema da Sobrescrita do Ponteiro Base (`a1`) no Assembly
**Por que dava problema:**
Na linguagem Assembly do RISC-V (definida pela ABI - Application Binary Interface), os parâmetros das funções são passados nos registradores `a0` a `a7`. Quando a função `context_switch(void *old, void *new)` é chamada:
* O registrador `a0` carrega o endereço de memória do contexto antigo (`old`).
* O registrador **`a1` carrega o endereço de memória do novo contexto (`new`)**.

Dentro do seu arquivo `context.S`, você usava o `a1` como ponteiro base para ler os registradores salvos na memória (ex: `ld ra, 0(a1)`). Se você carregasse os registradores na ordem exata (de `x1` a `x31`), em um dado momento você faria `ld a1, 80(a1)`. 
Nessa linha, você instruía a CPU a: "leia o valor salvo na memória e substitua o próprio registrador `a1`". Como a `Task 2` tinha acabado de ser criada, o valor salvo era `0`. Ao tentar executar a instrução seguinte (ex: `ld a2, 88(a1)`), a CPU tentava ler do endereço de memória `0 + 88`. Novamente, isso causava um acesso inválido à memória e provocava o reinício do microkernel.

**Por que a modificação resolveu:**
A solução foi uma simples mudança de ordem. Ao movermos a restauração dos registradores `a0` e `a1` para o **final** do código, garantimos que o endereço de memória base (que estava em `a1`) fosse preservado até que todos os outros 29 registradores tivessem sido lidos com segurança. Somente após a CPU carregar todo o restante, ela finalmente atualizou o seu próprio ponteiro, executando o `ret` (return) com sucesso para dentro da task.

### Arquivos Modificados

1. **`kernel/context.S`**: A função em Assembly `context_switch` foi reorganizada para que as instruções `ld a0, 72(a1)` e `ld a1, 80(a1)` fossem as últimas coisas executadas antes do `ret`, evitando que o ponteiro base da memória fosse corrompido durante a restauração do contexto.