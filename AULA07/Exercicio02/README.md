uma task lê o valor da porta analógica do potenciômetro e grava na memoria
**POT1** → GPIO 34 (entrada analógica principal)
usando filas
se chegar no quinto parâmetro gravado na memoria e não esvaziar, trava a task emissora para aguardar a receptora descarregar a fila
outra task vai ler o valor gravado na memoria e expõe um valor proporcional na saída serial

3.3v
12 bits

depois de fazer o exercicio01
jogar o valor no display de 7seg
de 0 ~ 99

é necessário fazer mux entre os 2 transistores
https://github.com/TheJohnnyX/plataforma_embarcados