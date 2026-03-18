Utilizando C++ no ESP32 Desenvolva a tividade abaixo:
Um estacionamento de carros possui apenas um portão para a entrada e saida de automóveis.
Seu proprietário pretende instalar um sistema de informação luminosa de liberação ou impedimento de passagem e um portão automatico que abre ou fecha segundo as condições á seguir:
sensor de carro aguardando para entrar,
sensor de carro entrando,
display 7seg.
Vaga 1
Vaga 2
Vaga 3
Vaga 4
Vaga 5
O portão deverá abrir sempre que houver carro querendo entrar, somente fechando quando sair do sensor interno, e abre quando estiver querendo sair,
somente fechando quando sair do sensor externo desacionar.

A cada carro que passar primeiro no sensor de externo e após no sensor interno,
o contador e vagas deverá decrementar em uma unidade, e quando chegar a zero vagas, não pode mais abrir o portão para entrada

Considere uma saida para abrir e outra para fechar, e, que o controle do portão é feito de forma independente por um hardware interno.
