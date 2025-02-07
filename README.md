![image](https://github.com/user-attachments/assets/f2a5c9b8-6208-4723-8f46-1d74be421827)


# 🛠️ Projeto: Interfaces de Comunicação Serial com RP2040

## 📑 Sumário
- [🎯 Objetivos](#-objetivos)
- [📋 Descrição do Projeto](#-descrição-do-projeto)
- [⚙️ Funcionalidades Implementadas](#%EF%B8%8F-funcionalidades-implementadas)
- [🛠️ Requisitos do Projeto](#%EF%B8%8F-requisitos-do-projeto)
- [📂 Estrutura do Repositório](#-estrutura-do-reposit%C3%A1rio)
- [🖥️ Como Compilar](#%EF%B8%8F-como-compilar)
- [🤝 Contribuições](#-contribui%C3%A7%C3%B5es)
- [📽️ Demonstração em Vídeo](#%EF%B8%8F-demonstra%C3%A7%C3%A3o-em-v%C3%ADdeo)
- [📜 Licença](#-licen%C3%A7a)
- [💡 Considerações Finais](#-considera%C3%A7%C3%B5es-finais)

## 🎯 Objetivos
- Compreender o funcionamento e a aplicação de comunicação serial em microcontroladores.
- Aplicar na prática os conceitos de UART e I2C.
- Manipular LEDs comuns e LEDs endereçáveis WS2812.
- Implementar soluções para botões com interrupções e debounce.
- Desenvolver um projeto que combina hardware e software.

## 📋 Descrição do Projeto
Este projeto utiliza a placa BitDogLab com os seguintes componentes:
- Matriz 5x5 de LEDs WS2812 (GPIO 7)
- LED RGB (GPIOs 11, 12 e 13)
- Botão A (GPIO 5)
- Botão B (GPIO 6)
- Display SSD1306 via I2C (GPIO 14 e GPIO 15)

## ⚙️ Funcionalidades Implementadas
1. **Modificação da Biblioteca `font.h`:**
   - Adição de caracteres minúsculos criativos.

2. **Entrada de Caracteres via Serial Monitor:**
   - Exibição de cada caractere no display SSD1306.
   - Exibição de símbolos na matriz 5x5 para números entre 0 e 9.

3. **Interação com Botões:**
   - **Botão A:** Alterna o estado do LED Verde RGB com registro no display e Serial Monitor.
   - **Botão B:** Alterna o estado do LED Azul RGB com registro no display e Serial Monitor.

## 🛠️ Requisitos do Projeto
- **Uso de Interrupções:** Controlar os eventos dos botões.
- **Debouncing:** Implementação via software.
- **Controle de LEDs:** Utilização dos LEDs comuns e da matriz WS2812.
- **Uso do Display SSD1306:** Exibição de fontes maiúsculas e minúsculas.
- **Comunicação via UART:** Envio de informações ao Serial Monitor.
- **Organização do Código:** Código estruturado e comentado.

## 📂 Estrutura do Repositório
```
├── DisplayC.c           # Código principal do projeto
├── font.h               # Biblioteca com fontes modificadas
├── README.md            # Este arquivo
└── ...                  # Demais arquivos necessários
```

## 🖥️ Como Compilar
1. Clone o repositório:
   ```bash
   git clone <URL-do-repositório>
   ```
2. Navegue até o diretório do projeto:
   ```bash
   cd <nome-do-diretório>
   ```
3. Compile o projeto com seu ambiente de desenvolvimento configurado para o RP2040.
4. Carregue o código na placa BitDogLab.

## 🧑‍💻 Autor
**Atenilton Santos de Souza Júnior**

## 📝 Descrição
Tarefa apresentada ao Cepedi como parte dos critérios de avaliação do curso EmbarcaTech em Software e Sistemas Embarcados, com foco na aplicação prática de comunicação serial e integração de hardware com o microcontrolador RP2040.

## 🤝 Contribuições
Este projeto foi desenvolvido por **Atenilton Santos de Souza Júnior**.
Contribuições são bem-vindas! Siga os passos abaixo para contribuir:
1. Fork este repositório.
2. Crie uma nova branch:
   ```bash
   git checkout -b minha-feature
   ```
3. Faça suas modificações e commit:
   ```bash
   git commit -m 'Minha nova feature'
   ```
4. Envie suas alterações:
   ```bash
   git push origin minha-feature
   ```
5. Abra um Pull Request.

## 📽️ Demonstração em Vídeo
- Link para o vídeo demonstrativo: [YouTube/Google Drive](#)

## 📜 Licença
Este projeto está licenciado sob a [MIT License](LICENSE).

## 💡 Considerações Finais
Este projeto oferece uma ótima oportunidade para consolidar conhecimentos sobre comunicação serial, manipulação de hardware e desenvolvimento com microcontroladores. Certifique-se de seguir todos os requisitos e manter um código limpo e bem comentado.
