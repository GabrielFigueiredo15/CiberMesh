💻 Projeto Rede IoT com ESP-NOW

Projeto da disciplina de Sistemas Ciberfísicos Colaborativos

👥 Integrantes da Equipe

* Lucas Azevedo de Oliveira
* Gabriel Catter Figueiredo de Alencar

Identificação da Equipe / Nó: equipe02

📚 Curso

Ciência da Computação

🏫 Disciplina

Sistemas Ciberfísicos Colaborativos

📖 Sobre o Projeto

Este projeto tem como objetivo desenvolver uma rede descentralizada de sistemas ciberfísicos utilizando microcontroladores ESP32 e o protocolo de comunicação sem fio ESP-NOW.
A proposta busca captar dados ambientais de temperatura e umidade, propagá-los dinamicamente entre os nós da rede e conectar essa rede local à nuvem por meio de um gateway integrado ao Adafruit IO via MQTT.

🎯 Objetivo Geral

Desenvolver uma rede descentralizada de sistemas ciberfísicos utilizando ESP32 e o protocolo ESP-NOW.

✅ Objetivos Específicos

* Captar dados ambientais de temperatura e umidade através de sensores
* Propagar os dados dinamicamente na rede em formato JSON
* Evitar o looping infinito de retransmissões através de lógica de descarte de mensagens duplicadas
* Conectar a rede local à nuvem por meio de um nó gateway com MQTT
* Integrar os dados à plataforma Adafruit IO

🧠 Funcionalidades do Sistema

📡 Camada de Borda
Nós ESP32 que se comunicam ponto-a-ponto via ESP-NOW, sem necessidade de roteador central.

🔁 Lógica de Deduplicação
Algoritmo de recepção que descarta mensagens repetidas com base em histórico de timestamps, evitando ecos e broadcast storms.

🌡 Leitura de Sensores
Leitura periódica de temperatura e umidade via sensor DHT11, a cada 5 segundos.

☁️ Camada de Nuvem
Nó gateway que conecta a rede local à internet via Wi-Fi e publica os dados no Adafruit IO via MQTT.

📊 Visualização em Tempo Real
Dados exibidos em widgets/dashboards no Adafruit IO, atualizados em tempo real.

💡 Sinalização Visual
LED indicador para sinalizar transmissão e recepção de pacotes na rede.

🛠 Metodologia Utilizada

1. Definição da arquitetura da rede (borda e nuvem)
2. Implementação da comunicação ponto-a-ponto via ESP-NOW
3. Estruturação dos dados em formato JSON
4. Implementação da lógica de deduplicação
5. Configuração do gateway e integração MQTT com Adafruit IO
6. Testes e validação da rede

🏗 Arquitetura do Sistema

![Arquitetura do sistema](docs/images/architecture_diagram.png)

📦 Estrutura de Dados (JSON)

```json
{
  "id": "equipe02",
  "contador": 42,
  "temperatura": 27.5,
  "umidade": 63.2
}
```

📂 Estrutura do Repositório

```
/docs
    Relatório do Projeto
    Diagramas
    Imagens

/node
    Firmware do nó sensor (ESP-NOW + DHT11)

/gateway
    Firmware do nó gateway (ESP-NOW + Wi-Fi + MQTT)
```

📌 Entregáveis

* Relatório Técnico
* Arquitetura do Sistema
* Firmware dos Nós (Borda e Gateway)
* Integração com Adafruit IO
* Apresentação dos Resultados

🔌 Protótipo

![Protótipo montado](docs/images/prototype.png)

📊 Resultado no Adafruit IO

![Dashboard Adafruit IO](docs/images/adafruit_dashboard.png)

🎓 Público-Alvo

* Estudantes de Sistemas Ciberfísicos e IoT
* Pesquisadores em redes de sensores sem fio
* Entusiastas de automação e monitoramento ambiental

🚀 Ferramentas Utilizadas

* Arduino IDE
* ESP32
* Adafruit IO
* Draw.io

📅 Organização do Projeto

O repositório será organizado utilizando:

* Issues
* Commits organizados
* Versionamento de entregáveis

🔭 Trabalhos Futuros

* Migração do roteamento manual para o protocolo ESP-Mesh
* Substituição do DHT11 por sensores mais precisos (BME280, SHT31)
* Inclusão de sensores de qualidade do ar (MQ-135, ENS160) e luminosidade (BH1750)
* Expansão para uma estação meteorológica completa

📚 Referências

* ADAFRUIT. Adafruit IO MQTT API Documentation, 2026.
* ESPRESSIF SYSTEMS. ESP-NOW User Guide, 2026.
* SILVA, J. R. Redes de Sensores sem Fio para IoT. Revista Brasileira de IoT, v. 7, n. 2, p. 34-42, 2021.

📖 Projeto de Sistemas Ciberfísicos

Projeto acadêmico desenvolvido na disciplina de Sistemas Ciberfísicos Colaborativos do curso de Ciência da Computação — UNIFOR, 2026.
