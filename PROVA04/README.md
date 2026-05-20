Projeto Final Integrado 

Disciplinas Envolvidas 

* **Sistemas Operacionais Embarcados**

* **Internet das Coisas (IoT)**

Objetivo do Projeto 

Sua equipe deverá desenvolver uma aplicação embarcada utilizando a placa **ESP32**, integrando os conceitos de ambas as disciplinas. O projeto deve simular uma situação real do cotidiano, demonstrando de forma clara a coleta, o processamento e o envio de dados. 

---

## Requisitos do Projeto

1. Requisitos de Internet das Coisas (IoT) 

O projeto deve obrigatoriamente contemplar comunicação com algum serviço IoT, incluindo pelo menos uma das opções abaixo: 

* Envio de dados para a nuvem (ex: MQTT, HTTP, Firebase, Thingspeak, Blynk, etc.); 


* Dashboard para visualização dos dados em tempo real; 


* Aplicativo ou interface web para monitoramento; 


* Controle remoto do dispositivo (ex: ligar/desligar saídas via celular ou web). 


> 
> **Exemplos de aplicações sugeridas:** * Monitoramento de temperatura e umidade com dashboard online * Sistema de irrigação inteligente controlado via app * Monitoramento de nível de reservatório com envio para nuvem * Automação residencial com controle remoto 
> 
> 

2. Requisitos de Sistemas Embarcados (FreeRTOS) 

O sistema deve utilizar obrigatoriamente os seguintes recursos do FreeRTOS: 

* Tasks (tarefas concorrentes); 


* Software Timers; 


* Filas (Queues). 



Além disso, deve conter pelo menos **um** dos seguintes mecanismos de sincronização e comunicação: 

* Semáforos; 


* Task Notifications; 


* Event Groups. 



3. Requisitos de Hardware Mínimo 

O projeto deve conter no mínimo a seguinte estrutura física: 

* 1 ou mais entradas analógicas (sensores ou leitura de tensão); 


* 2 entradas digitais; 


* 1 display de 7 segmentos; 


* 2 saídas digitais (LED, relé, buzzer, etc.). 



*Nota: É permitido utilizar componentes adicionais (sensores, módulos, atuadores), mediante disponibilidade e aprovação prévia dos professores.* 

4. Requisitos de Engenharia 

Além da implementação funcional, o desenvolvimento deve demonstrar: 

* Organização do sistema estruturada em múltiplas tarefas; 


* Separação clara de responsabilidades (leitura de dados, processamento e comunicação); 


* Uso adequado e correto de recursos de sincronização; 


* Integração eficiente com os serviços de IoT escolhidos. 



---

Entregáveis 

Cada equipe deverá apresentar no dia da avaliação: 

* Protótipo funcional do projeto; 


* Demonstração prática do funcionamento; 


* Explicação detalhada da arquitetura do sistema; 


* Justificativa técnica para as escolhas de desenvolvimento; 


* Demonstração em tempo real da integração com o serviço IoT. 



---

Observações Importantes 

* O projeto deve estar totalmente funcional até a data final estabelecida. 


* Cada integrante da equipe deve participar ativamente da apresentação oral. 


* Projetos sem comunicação IoT não serão aceitos (com exceção feita aos grupos formados exclusivamente por alunos que não cursam a disciplina de IoT). 


* Projetos sem o uso de FreeRTOS não serão aceitos (com exceção feita aos grupos formados exclusivamente por alunos que não cursam a disciplina de SOE). 



---

Cronograma do Projeto 

| Semana | Datas | Sistemas Operacionais Embarcados (Quarta-feira) | Internet das Coisas (Quinta-feira) |
| --- | --- | --- | --- |
| **15** | 20/05 e 21/05 | Formação das equipes e entrega do tema | Aula normal |
| **16** | 27/05 e 28/05 | Evento CyberSATC | Avaliação teórica |
| **17** | 03/06 e 04/06 | Desenvolvimento do projeto em laboratório | Desenvolvimento do projeto em laboratório |
| **18** | 10/06 e 11/06 | Desenvolvimento do projeto em laboratório | Desenvolvimento do projeto em laboratório |
| **19** | 17/06 e 18/06 | Desenvolvimento do projeto em laboratório | Desenvolvimento do projeto em laboratório |
| **20** | 24/06 e 25/06 | Ajustes finais e validação do sistema | Apresentação final dos projetos |



---

Critérios de Avaliação 

| Critério de Avaliação | Pontuação Máxima |
| --- | --- |
| Tema coerente com a realidade do cotidiano | 1.0 |
| Utilização correta do hardware mínimo exigido | 1.0 |
| Apresentação oral e desenvoltura (avaliação individual) | 2.0 |
| Funcionamento prático conforme a proposta enviada | 3.0 |
| Uso correto das ferramentas de programação (FreeRTOS + IoT) | 3.0 |
| **Total** | **10,0** |