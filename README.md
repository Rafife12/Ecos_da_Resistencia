# 🎮 Ecos da Resistência - Mini-Game Educativo Embarcado

> Um jogo interativo desenvolvido em **ESP32** que ensina a história das revoluções populares da Bahia (1798-1838) através de gamificação.

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Arduino](https://img.shields.io/badge/platform-ESP32-red.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Status](https://img.shields.io/badge/status-production-brightgreen.svg)](#status)

---

## 📋 Sobre o Projeto

**Ecos da Resistência** é um console educativo embarcado que combina:

- 🎯 **Mecânica de Jogo**: Endless runner (estilo Flappy Bird)
- 📚 **Conteúdo Histórico**: 9 questões sobre revoluções baianas
- 🎨 **Hardware**: ESP32 + LCD 20x4 + 4 Botões + Buzzer
- 🧠 **Pedagogia**: Game-Based Learning com dificuldade progressiva

### Personagens e Temática

- **Protagonista**: Maria Felipa de Oliveira (Heroína baiana da Independência)
- **Inimigos**: Caravelas e Canhões (símbolos coloniais)
- **Obstáculo Especial**: Bandeira (símbolo das revoluções)

### Conteúdo Histórico Coberto

1. **Revolta dos Búzios (1798)** - Movimento republicano e abolicionista
2. **Revolta dos Malês (1835)** - Maior insurreição urbana de escravizados
3. **Sabinada (1837-1838)** - Movimento popular baiano

---

## 🛠️ Requisitos

### Hardware Necessário

| Componente | Quantidade | Especificação |
|-----------|-----------|---------------|
| **ESP32** | 1x | DevKit V1 (dual-core 32-bit) |
| **LCD 20x4** | 1x | Com interface I2C (backpack PCF8574) |
| **Botões** | 4x | Push-button tátil com pull-up |
| **Buzzer** | 1x | Passivo (PWM) |
| **Transistor** | 1x | BC548 NPN |
| **Capacitores** | 4x | 100nF cerâmico |
| **Resistores** | 2x | 220Ω e 1kΩ |
| **Cabo USB** | 1x | Para programação |

### Software Necessário

- ✅ Arduino IDE 2.0+
- ✅ ESP32 Board Package
- ✅ Biblioteca: LiquidCrystal_I2C
- ✅ Wire (nativa, já incluída)

---

## 📥 Instalação Passo a Passo

### 1️⃣ Instalar Arduino IDE

1. Acesse: https://www.arduino.cc/en/software
2. Baixe a versão 2.0+ para seu sistema operacional
3. Instale normalmente (Next → Next → Finish)

### 2️⃣ Adicionar Suporte ESP32

**No Arduino IDE:**

```
Arquivo → Preferências
```

Na caixa de diálogo, procure por: **"URLs adicionais de gerenciadores de placa"**

Cole esta URL:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Clique **OK**.

Agora vá para:
```
Ferramentas → Placa → Gerenciador de Placa
```

Procure por: **"esp32"**

Selecione e clique em **Instalar** (v2.0+)

### 3️⃣ Instalar Biblioteca LiquidCrystal_I2C

**No Arduino IDE:**

```
Ferramentas → Gerenciar Bibliotecas
```

Procure por: **"LiquidCrystal_I2C"**

Selecione a biblioteca de **Frank de Brabander**

Clique em **Instalar**

### 4️⃣ Configurar Placa e Porta

**Ferramentas → Placa → ESP32 Dev Module**

**Ferramentas → Porta → COM?** (identifique qual porta seu ESP32 está conectado)

**Ferramentas → Upload Speed → 115200**

---

## 🎮 Como Compilar e Fazer Upload

### Opção A: Usando o Código Completo

1. **Clone o repositório:**
```bash
git clone https://github.com/seu-usuario/Ecos-da-Resistencia.git
cd Ecos-da-Resistencia
```

2. **Abra o arquivo principal:**
```
Arduino IDE → Arquivo → Abrir
```

Navegue até: `ECOS_RESISTENCIA_v2.0_FINAL.ino`

3. **Verifique o código:**
```
Clique em ✓ (Verificar)
```

Aguarde "Compilação concluída com sucesso"

4. **Faça o Upload:**
```
Clique em → (Upload)
```

Aguarde a mensagem "Upload concluído"

### Opção B: Copiar e Colar o Código

1. **Crie um novo sketch:**
```
Arquivo → Novo
```

2. **Copie todo o código do arquivo:**
`ECOS_RESISTENCIA_v2.0_FINAL.ino`

3. **Cole no Arduino IDE** e faça o upload

---

## ⚙️ Configuração de Hardware

### Conexões Recomendadas

```
ESP32 PIN        →  COMPONENTE
═══════════════════════════════════════

GPIO 32          →  Botão UP (pull-up)
GPIO 33          →  Botão DOWN (pull-up)
GPIO 25          →  Botão A (pull-up)
GPIO 26          →  Botão B (pull-up)
GPIO 27          →  Buzzer (via transistor)

GPIO 21 (SDA)    →  LCD I2C (SDA)
GPIO 22 (SCL)    →  LCD I2C (SCL)

GND              →  GND (todos)
3.3V             →  VCC (LCD e pull-ups)
5V               →  VCC Buzzer (fonte externa)
```

### Diagrama de Blocos

```
┌─────────────────────────────────┐
│          ESP32 DevKit           │
├─────────────────────────────────┤
│ GPIO32 ──→ BTN_UP               │
│ GPIO33 ──→ BTN_DOWN             │
│ GPIO25 ──→ BTN_A                │
│ GPIO26 ──→ BTN_B                │
│ GPIO27 ──→ BUZZER (transistor)  │
│ GPIO21 ──→ LCD I2C (SDA)        │
│ GPIO22 ──→ LCD I2C (SCL)        │
└─────────────────────────────────┘
         ↓
    ┌──────────┐
    │ LCD 20x4 │
    └──────────┘
```

---

## 🎮 Como Jogar

### Controles

| Botão | Função |
|-------|--------|
| **UP** (GPIO 32) | Mover Maria para cima |
| **DOWN** (GPIO 33) | Mover Maria para baixo |
| **A** (GPIO 25) | Iniciar / Responder A no quiz |
| **B** (GPIO 26) | Responder B no quiz |

### Fluxo do Jogo

1. **Menu** → Pressione **A** para iniciar
2. **Gameplay** → Desvie dos obstáculos
3. **Colisão** → Vai para o **Quiz** (se quiz disponível)
4. **Quiz** → Responda corretamente para ganhar +1 vida
5. **Game Over** → Quando perde todas as vidas

### Dificuldade

- Começa normal em 0 pontos
- **+8% a cada 15 pontos** (velocidade aumenta)
- **Quiz aparece a cada 300 pontos**
- Limite máximo: 60ms (não fica impossível)

---

## 📊 Progressão de Dificuldade

| Score | Multiplicador | Velocidade |
|-------|---------------|-----------|
| 0-14 | 1.00x | 140ms |
| 15 | 1.15x | 122ms |
| 30 | 1.30x | 108ms |
| 45 | 1.45x | 97ms |
| 60 | 1.60x | 88ms |
| 75+ | 1.75x+ | 80ms+ |

---

## 📝 Estrutura do Código

### Arquivos Principais

```
Ecos-da-Resistencia/
├── ECOS_RESISTENCIA_v2.0_FINAL.ino    # Código principal
├── README.md                           # Este arquivo
├── HARDWARE_DIAGRAMA.pdf               # Diagrama de conexões
├── DOCUMENTACAO/
│   ├── QUIZ_PERGUNTAS.txt             # Base de dados do quiz
│   ├── ANALISE_DIFICULDADE.txt        # Análise da curva
│   └── MELHORIAS_v2.md                # Histórico de versões
└── ARTIGO_ACADEMICO.pdf               # Artigo científico
```

### Seções do Código

```cpp
// ===== PINOS =====
// Definição dos GPIO

// ===== SPRITES =====
// Caracteres personalizados do LCD

// ===== ESTADOS =====
// Máquina de Estados Finita (MENU, JOGO, QUIZ, GAMEOVER)

// ===== QUIZ =====
// Base de dados com 9 perguntas históricas

// ===== GAMEPLAY =====
// Lógica principal do jogo

// ===== SETUP / LOOP =====
// Inicialização e loop principal
```

---

## 🧪 Teste no Simulador (Wokwi)

**Para testar antes de fazer upload no hardware:**

1. Acesse: https://wokwi.com/
2. Crie um novo projeto para ESP32
3. Cole o código
4. Clique em **RUN**
5. O LCD simulado mostrará o jogo em tempo real

---

## 📚 Perguntas do Quiz

### Revolta dos Búzios (1798)

- "Qual foi o tipo de governo na Revolta de Buzios?" 
  - Resposta: **Republica**

- "Quem liderou na Revolta de Buzios?"
  - Resposta: **Alfaiates**

- "Quem queriam abolir na Revolta de Buzios?"
  - Resposta: **Escravidao**

### Revolta dos Malês (1835)

- "Qual era a Religiao na Revoltad dos Males?"
  - Resposta: **Islamismo**

- "Quantos participantes estavam envolvidos na Revolta dos Males?"
  - Resposta: **600**

- "O que eles buscavam na Revolta dos Males?"
  - Resposta: **Liberdade**

### Sabinada (1837-1838)

- "Qual foi o local da Sabinada?"
  - Resposta: **Bahia**

- "Qual foi o Tipo da Sabinada?"
  - Resposta: **Popular**

- "A Sabinada buscava separar"
  - Resposta: **Bahia**

---

## 🐛 Troubleshooting

### Problema: "Board not found"
**Solução:** 
- Instale o driver CH340 (ESP32 USB)
- Reconecte o cabo USB
- Verifique a porta em Ferramentas → Porta

### Problema: "LiquidCrystal_I2C not found"
**Solução:**
- Vá em Ferramentas → Gerenciar Bibliotecas
- Procure e instale "LiquidCrystal_I2C"

### Problema: "Erro de compilação"
**Solução:**
- Verifique se a placa está selecionada: **ESP32 Dev Module**
- Verifique a velocidade de upload: **115200**

### Problema: "LCD preto, sem aparecer nada"
**Solução:**
- Ajuste o potenciômetro de contraste no LCD
- Verifique se o endereço I2C é 0x27 (pode ser 0x3F)

### Problema: "Botões não funcionam"
**Solução:**
- Verifique os GPIOs: 32, 33, 25, 26
- Teste com um multímetro se há continuidade

---

## 🔧 Modificações Personalizadas

### Mudar Dificuldade

No código, procure por:
```cpp
multiplicadorDificuldade += 0.15;  // 15% a cada 15 pontos
```

Opções:
- Mais fácil: `0.08` (8%)
- Mais difícil: `0.20` (20%)

### Mudar Pontos para Quiz

Procure por:
```cpp
proximoQuizPonto = 300;  // Quiz a cada 300 pontos
```

### Adicionar Novas Perguntas

Na seção de QUIZ, adicione mais questões ao array:
```cpp
{"Sua pergunta aqui", "Opção A", "Opção B", 'A'}
```

---

## 📖 Referências e Créditos

### Autores do Projeto

- Gabriel Carvalho Silva (UNIJORGE)
- João Barreiro Gidi (UNIJORGE)
- Lucca Iago Olímpio Santos Batista (UNIJORGE)
- **Matheus Gomes da Silva** (UNIJORGE)
- Rafael Souza da Silva (UNIJORGE)

### Referências Académicas

- **Deterding, S. et al. (2011)**: Gamification concepts
- **Kapp, K. M. (2012)**: Game-Based Learning theory
- **Gee, J. P. (2003)**: Video games and learning
- **Tavares, L. H. D. (2001)**: História da Bahia
- **Reis, J. J. (2003)**: Revolta dos Malês

### Licenças

- Hardware: Open Source (CC BY-SA)
- Software: MIT License
- Conteúdo Educacional: CC BY 4.0

---

## 🤝 Contribuindo

Sugestões de melhorias:

1. **Novas perguntas** sobre história baiana
2. **Novos sprites/obstáculos**
3. **Otimizações de performance**
4. **Suporte para mais idiomas**
5. **Documentação em outros idiomas**

Para contribuir:
1. Fork o repositório
2. Crie uma branch (`git checkout -b feature/nova-funcionalidade`)
3. Commit suas mudanças (`git commit -m 'Add nova-funcionalidade'`)
4. Push para a branch (`git push origin feature/nova-funcionalidade`)
5. Abra um Pull Request

---

## 📜 Licença

Este projeto é licenciado sob a **MIT License** - veja o arquivo [LICENSE](LICENSE) para detalhes.

---

## 📞 Suporte

Dúvidas? Abra uma **Issue** no GitHub!

---

## 🎯 Status

✅ **Produção** - Versão 2.0 estável e testada
✅ **Hardware** - Totalmente funcional
✅ **Software** - Código otimizado e sem bugs
✅ **Educacional** - Conteúdo histórico verificado

---

**Desenvolvido com ❤️ para educar e entreter**

*Ecos da Resistência - Aprendizado da História Baiana através de um Mini Game Interativo Embarcado*
