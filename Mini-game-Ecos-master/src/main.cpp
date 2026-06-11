#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ===== PINOS =====
const int BTN_UP   = 32;
const int BTN_DOWN = 33;
const int BTN_A    = 25;
const int BTN_B    = 26;
const int BUZZER   = 27;

// ===== SPRITES - MAIS VISÍVEIS E DISTINTOS =====
byte maria1[8] = {
  B00100,
  B01110,
  B00100,
  B01110,
  B10100,
  B00110,
  B01001,
  B10001
};

byte maria2[8] = {
  B00100,
  B01110,
  B00100,
  B01110,
  B00101,
  B01100,
  B10010,
  B00011
};

// CARAVELA - mais visível
byte caravela[8] = {
  B00100,
  B00110,
  B01111,
  B11111,
  B11111,
  B01110,
  B01110,
  B00100
};

// CANHÃO - mais agressivo
byte canhao[8] = {
  B01110,
  B11111,
  B11111,
  B11111,
  B01111,
  B00110,
  B01110,
  B00100
};

// BANDEIRA (símbolo das revoluções) - NOVO
byte bandeira[8] = {
  B11111,
  B10001,
  B10101,
  B10001,
  B11111,
  B00100,
  B00100,
  B00000
};

// ===== ESTADOS =====
enum Estado {
  MENU,
  JOGO,
  QUIZ,
  GAMEOVER
};

Estado estadoAtual = MENU;

// ===== MENU =====
bool frameMariaMenu = false;
bool mostrarStart = true;
bool tipoObsMenu = 0;

unsigned long tFrameMenu = 0;
unsigned long tMoveMenu = 0;
unsigned long tBlinkMenu = 0;

int posObsMenu = 19;
int posObsAntMenu = 19;
int linhaObsMenu = 2;
int linhaObsAntMenu = 2;
int linhaMariaMenu = 1;
int linhaMariaAntMenu = 1;

// ===== GAMEPLAY =====
bool frameMariaJogo = false;
const int NUM_OBSTACULOS = 3;
int tipoObsJogo[NUM_OBSTACULOS];

unsigned long tFrameJogo = 0;
unsigned long tMoveJogo = 0;
unsigned long tScore = 0;
unsigned long tMoveMariaJogo = 0;  // ← NOVO: Controlar velocidade de movimento de Maria

int linhaMariaJogo = 1;
int linhaMariaAntJogo = 1;
int posObsJogo[NUM_OBSTACULOS];
int posObsAntJogo[NUM_OBSTACULOS];
int linhaObsJogo[NUM_OBSTACULOS];
int linhaObsAntJogo[NUM_OBSTACULOS];

int score = 0;
int vidas = 1;
bool quizDisponivel = false;
int proximoQuizPonto = 50;

int ultimoScoreMostrado = -1;
bool ultimoQuizMostrado = false;

int ultimoScoreAumentoVel = -1;  // NOVO: Rastreia último aumento de velocidade

bool upPressionadoAntes = false;
bool downPressionadoAntes = false;

const unsigned long velocidadeFrame = 180;
const unsigned long velocidadeMove  = 140;
const unsigned long velocidadeBlink = 450;
const unsigned long velocidadeScore = 300;
const unsigned long velocidadeMovimentoMaria = 80;  // ← NOVO: Velocidade de movimento entre linhas

unsigned long velocidadeFrameJogo = velocidadeFrame;
unsigned long velocidadeMoveJogo = velocidadeMove;
unsigned long velocidadeMovimentoMariaJogo = velocidadeMovimentoMaria;  // ← NOVO: Dinâmica

// Dificuldade progressiva mais suave
float multiplicadorDificuldade = 1.0;
float multiplicadorMaria = 1.0;  // ← NOVO: Multiplicador específico para Maria

// ===== QUIZ - FORMATADO PARA LCD 20x4 =====
struct Pergunta {
  const char* pergunta;
  const char* opcaoA;
  const char* opcaoB;
  char correta;
};

// Perguntas verificadas historicamente - COMPACTADAS PARA 20 CHARS
Pergunta perguntas[9] = {
  // Revolta dos Búzios (1798)
  {"Buzios: Tipo governo", "Monarquia", "Republica", 'B'},
  {"Buzios: Quem liderou", "Alfaiates", "Fazendeiros", 'A'},
  {"Buzios: Queriam abolir", "Impostos", "Escravidão", 'B'},
  
  // Revolta dos Malês (1835)
  {"Males: Religiao", "Islamismo", "Catolicismo", 'A'},
  {"Males: Quantos", "100", "600", 'B'},
  {"Males: Buscavam", "Riqueza", "Liberdade", 'B'},
  
  // Sabinada (1837-1838)
  {"Sabinada: Local", "Bahia", "Minas", 'A'},
  {"Sabinada: Tipo", "Aristocrata", "Popular", 'B'},
  {"Sabinada: Periodo", "1837-1838", "1845-1846", 'A'}
};

int perguntaAtual = 0;
bool aPressionadoAntesQuiz = false;
bool bPressionadoAntesQuiz = false;

// ===== FUNÇÕES AUXILIARES =====
void beepCurto(int freq, int tempo) {
  tone(BUZZER, freq, tempo);
}

void beepColisao() {
  tone(BUZZER, 450, 120);
}

void beepAcerto() {
  tone(BUZZER, 1400, 120);
}

void beepErro() {
  tone(BUZZER, 300, 250);
}

void beepEntrada() {
  tone(BUZZER, 1000, 80);
  delay(120);
  tone(BUZZER, 1300, 80);
  delay(120);
  tone(BUZZER, 1600, 120);
  delay(150);
  noTone(BUZZER);
}

void criarSprites() {
  lcd.createChar(0, maria1);
  lcd.createChar(1, maria2);
  lcd.createChar(2, caravela);
  lcd.createChar(3, canhao);
  lcd.createChar(4, bandeira);
}

void printCentralizado(int linha, const char* texto) {
  int tam = strlen(texto);
  int col = (20 - tam) / 2;
  if (col < 0) col = 0;
  lcd.setCursor(col, linha);
  lcd.print(texto);
}

char fundoTela[4][21];

void setBackgroundLine(int linha, const char* texto) {
  int len = strlen(texto);
  for (int i = 0; i < 20; i++) {
    fundoTela[linha][i] = (i < len) ? texto[i] : ' ';
  }
  fundoTela[linha][20] = '\0';
  lcd.setCursor(0, linha);
  lcd.print(fundoTela[linha]);
}

char getBackgroundChar(int col, int lin) {
  if (col >= 0 && col < 20 && lin >= 0 && lin < 4) {
    return fundoTela[lin][col];
  }
  return ' ';
}

void apagarPosicao(int col, int lin) {
  if (col >= 0 && col < 20 && lin >= 0 && lin < 4) {
    lcd.setCursor(col, lin);
    lcd.print(getBackgroundChar(col, lin));
  }
}

void desenharFundoJogo() {
  setBackgroundLine(0, "                    ");
  setBackgroundLine(1, "                    ");
  setBackgroundLine(2, "                    ");
  setBackgroundLine(3, "                    ");
}

// ===== MENU =====
void desenharTelaBase() {
  lcd.clear();

  setBackgroundLine(0, " ECOS DA RESISTENCIA");
  setBackgroundLine(1, "                    ");
  setBackgroundLine(2, "                    ");
  setBackgroundLine(3, "                    ");

  lcd.setCursor(2, linhaMariaMenu);
  lcd.write(byte(0));

  lcd.setCursor(posObsMenu, linhaObsMenu);
  byte obstaculoSprite = (tipoObsMenu == 0) ? 2 : (tipoObsMenu == 1) ? 3 : 4;
  lcd.write(byte(obstaculoSprite));
}

void atualizarAnimacaoMenu() {
  if (millis() - tFrameMenu >= velocidadeFrame) {
    tFrameMenu = millis();
    frameMariaMenu = !frameMariaMenu;
  }

  if (millis() - tMoveMenu >= velocidadeMove) {
    tMoveMenu = millis();

    apagarPosicao(2, linhaMariaAntMenu);
    apagarPosicao(posObsAntMenu, linhaObsAntMenu);

    posObsMenu--;

    if (posObsMenu < 0) {
      posObsMenu = 19;
      tipoObsMenu = (tipoObsMenu + 1) % 3;

      if (linhaObsMenu == 2) {
        linhaObsMenu = 1;
        linhaMariaMenu = 2;
      } else {
        linhaObsMenu = 2;
        linhaMariaMenu = 1;
      }
    }

    lcd.setCursor(2, linhaMariaMenu);
    lcd.write(frameMariaMenu ? byte(0) : byte(1));

    lcd.setCursor(posObsMenu, linhaObsMenu);
    byte obstaculoSprite = (tipoObsMenu == 0) ? 2 : (tipoObsMenu == 1) ? 3 : 4;
    lcd.write(byte(obstaculoSprite));

    posObsAntMenu = posObsMenu;
    linhaObsAntMenu = linhaObsMenu;
    linhaMariaAntMenu = linhaMariaMenu;
  }

  if (millis() - tBlinkMenu >= velocidadeBlink) {
    tBlinkMenu = millis();
    mostrarStart = !mostrarStart;

    lcd.setCursor(0, 3);
    lcd.print("                    ");

    if (mostrarStart) {
      printCentralizado(3, "A = INICIAR");
    }
  }
}

void animacaoEntrada() {
  lcd.clear();
  printCentralizado(1, "CARREGANDO...");
  beepEntrada();
}

void resetarMenu() {
  frameMariaMenu = false;
  mostrarStart = true;
  tipoObsMenu = 0;

  posObsMenu = 19;
  posObsAntMenu = 19;
  linhaObsMenu = 2;
  linhaObsAntMenu = 2;
  linhaMariaMenu = 1;
  linhaMariaAntMenu = 1;

  tFrameMenu = millis();
  tMoveMenu = millis();
  tBlinkMenu = millis();

  desenharTelaBase();
}

void resetObstaculosJogo() {
  for (int i = 0; i < NUM_OBSTACULOS; i++) {
    posObsJogo[i] = 19 - i * 5;
    posObsAntJogo[i] = posObsJogo[i];
    linhaObsJogo[i] = random(1, 4);
    
    bool linhaValida = true;
    for (int j = 0; j < i; j++) {
      if (linhaObsJogo[i] == linhaObsJogo[j]) {
        linhaValida = false;
        break;
      }
    }
    if (!linhaValida) {
      linhaObsJogo[i] = (linhaObsJogo[i] % 3) + 1;
    }
    
    linhaObsAntJogo[i] = linhaObsJogo[i];
    tipoObsJogo[i] = random(0, 3);
  }
}

void loopMenu() {
  atualizarAnimacaoMenu();

  if (digitalRead(BTN_A) == LOW) {
    delay(180);
    animacaoEntrada();
    estadoAtual = JOGO;

    frameMariaJogo = false;
    resetObstaculosJogo();

    linhaMariaJogo = 1;
    linhaMariaAntJogo = 1;

    score = 0;
    ultimoScoreAumentoVel = -1;  // Reseta rastreador de aumento de velocidade
    vidas = 1;
    quizDisponivel = true;   // ✅ Quiz ativado na primeira colisão
    proximoQuizPonto = 50;  // Quiz a cada 50 pontos (mais tempo pra ver velocidade)

    multiplicadorDificuldade = 1.0;
    velocidadeFrameJogo = velocidadeFrame;
    velocidadeMoveJogo = velocidadeMove;

    ultimoScoreMostrado = -1;
    ultimoQuizMostrado = !quizDisponivel;

    upPressionadoAntes = false;
    downPressionadoAntes = false;

    tFrameJogo = millis();
    tMoveJogo = millis();
    tScore = millis();
    tMoveMariaJogo = millis();  // ← NOVO: Inicializar movimento de Maria

    lcd.clear();
    desenharFundoJogo();
  }
}

// ===== GAMEPLAY =====
void desenharHUD(bool forcar = false) {
  if (!forcar && score == ultimoScoreMostrado && quizDisponivel == ultimoQuizMostrado) {
    return;
  }

  char linha[21];
  snprintf(linha, sizeof(linha), "PTS:%-4d VIDA:%d Q:%s", score, vidas, quizDisponivel ? "ON " : "OFF");

  lcd.setCursor(0, 0);
  lcd.print(linha);

  int tam = strlen(linha);
  for (int i = tam; i < 20; i++) {
    lcd.print(" ");
  }

  ultimoScoreMostrado = score;
  ultimoQuizMostrado = quizDisponivel;
}

void atualizarScore() {
  if (millis() - tScore >= velocidadeScore) {
    tScore = millis();
    score++;
    
    if (!quizDisponivel && score >= proximoQuizPonto) {
      quizDisponivel = true;
      proximoQuizPonto = score + 50;
    }

    if (score > ultimoScoreAumentoVel + 20) {
      ultimoScoreAumentoVel = score;
      multiplicadorDificuldade += 0.20;  // AUMENTADO: Era 0.08 (+8%), agora 0.20 (+20%)
      
      velocidadeMoveJogo = max(60UL, (unsigned long)(velocidadeMove / multiplicadorDificuldade));
      velocidadeFrameJogo = max(80UL, (unsigned long)(velocidadeFrame / multiplicadorDificuldade));
    }
  }
}

void desenharMaria() {
  if (linhaMariaJogo != linhaMariaAntJogo) {
    apagarPosicao(2, linhaMariaAntJogo);
  }

  lcd.setCursor(2, linhaMariaJogo);
  lcd.write(frameMariaJogo ? byte(0) : byte(1));

  linhaMariaAntJogo = linhaMariaJogo;
}

void moverObstaculo() {
  if (millis() - tMoveJogo >= velocidadeMoveJogo) {
    tMoveJogo = millis();

    for (int i = 0; i < NUM_OBSTACULOS; i++) {
      apagarPosicao(posObsAntJogo[i], linhaObsAntJogo[i]);
      posObsJogo[i]--;

      if (posObsJogo[i] < 0) {
        posObsJogo[i] = 19 + random(0, 3);
        int novaLinha;
        do {
          novaLinha = random(1, 4);
          bool linhaValida = true;
          for (int j = 0; j < i; j++) {
            if (novaLinha == linhaObsJogo[j]) {
              linhaValida = false;
              break;
            }
          }
          if (linhaValida) break;
        } while (true);
        
        linhaObsJogo[i] = novaLinha;
        tipoObsJogo[i] = random(0, 3);
      }

      if (posObsJogo[i] >= 0 && posObsJogo[i] < 20) {
        lcd.setCursor(posObsJogo[i], linhaObsJogo[i]);
        byte obstaculoSprite = (tipoObsJogo[i] == 0) ? 2 : (tipoObsJogo[i] == 1) ? 3 : 4;
        lcd.write(byte(obstaculoSprite));
      }

      posObsAntJogo[i] = posObsJogo[i];
      linhaObsAntJogo[i] = linhaObsJogo[i];
    }
  }
}

void animarMaria() {
  if (millis() - tFrameJogo >= velocidadeFrameJogo) {
    tFrameJogo = millis();
    frameMariaJogo = !frameMariaJogo;

    lcd.setCursor(2, linhaMariaJogo);
    lcd.write(frameMariaJogo ? byte(0) : byte(1));
  }
}

void mostrarQuiz();
void verificarColisao() {
  for (int i = 0; i < NUM_OBSTACULOS; i++) {
    if (posObsJogo[i] == 2 && linhaObsJogo[i] == linhaMariaJogo) {
      beepColisao();

      if (quizDisponivel) {
        perguntaAtual = random(0, 9);

        aPressionadoAntesQuiz = true;
        bPressionadoAntesQuiz = true;

        estadoAtual = QUIZ;
        mostrarQuiz();
      } else {
        vidas--;
        if (vidas > 0) {
          lcd.clear();
          desenharFundoJogo();
          for (int j = 0; j < NUM_OBSTACULOS; j++) {
            apagarPosicao(posObsJogo[j], linhaObsJogo[j]);
          }
          resetObstaculosJogo();
          ultimoScoreMostrado = -1;
          ultimoQuizMostrado = !quizDisponivel;
          desenharHUD(true);
        } else {
          estadoAtual = GAMEOVER;
        }
      }
      return;
    }
  }
}

void lerControlesJogo() {
  bool upAtual = (digitalRead(BTN_UP) == LOW);
  bool downAtual = (digitalRead(BTN_DOWN) == LOW);

  // Movimento com velocidade progressiva
  if (millis() - tMoveMariaJogo >= velocidadeMovimentoMariaJogo) {
    tMoveMariaJogo = millis();
    
    if (upAtual && linhaMariaJogo > 1) {
      linhaMariaJogo--;
      tone(BUZZER, 900, 35);
    }

    if (downAtual && linhaMariaJogo < 3) {
      linhaMariaJogo++;
      tone(BUZZER, 900, 35);
    }
  }

  upPressionadoAntes = upAtual;
  downPressionadoAntes = downAtual;
}

void loopJogo() {
  lerControlesJogo();
  atualizarScore();
  desenharHUD();
  desenharMaria();
  moverObstaculo();
  animarMaria();
  verificarColisao();
}

void mostrarQuiz() {
  lcd.clear();

  // Quebrar pergunta em 2 linhas se necessário
  const char* pergunta = perguntas[perguntaAtual].pergunta;
  int len = strlen(pergunta);
  
  if (len <= 20) {
    // Pergunta cabe em 1 linha
    lcd.setCursor(0, 0);
    lcd.print(pergunta);
    lcd.setCursor(0, 1);
    lcd.print("                    ");
  } else {
    // Pergunta precisa de 2 linhas - quebrar no espaço mais próximo
    int breakPoint = 20;
    
    // Encontrar o último espaço antes de 20 caracteres
    for (int i = 20; i > 0; i--) {
      if (pergunta[i] == ' ') {
        breakPoint = i;
        break;
      }
    }
    
    // Linha 0: primeira parte
    lcd.setCursor(0, 0);
    for (int i = 0; i < breakPoint && i < 20; i++) {
      lcd.print(pergunta[i]);
    }
    // Preencher com espaços
    for (int i = breakPoint; i < 20; i++) {
      lcd.print(" ");
    }
    
    // Linha 1: segunda parte
    lcd.setCursor(0, 1);
    int startLine2 = breakPoint;
    while (startLine2 < len && pergunta[startLine2] == ' ') {
      startLine2++; // Pular espaços
    }
    
    int charsLine2 = 0;
    for (int i = startLine2; i < len && charsLine2 < 20; i++) {
      lcd.print(pergunta[i]);
      charsLine2++;
    }
    // Preencher com espaços
    for (int i = charsLine2; i < 20; i++) {
      lcd.print(" ");
    }
  }

  // Linha 2: Opção A (max 20 chars)
  char linhaA[21];
  snprintf(linhaA, sizeof(linhaA), "A: %s", 
           perguntas[perguntaAtual].opcaoA);
  lcd.setCursor(0, 2);
  lcd.print(linhaA);

  // Linha 3: Opção B (max 20 chars)
  char linhaB[21];
  snprintf(linhaB, sizeof(linhaB), "B: %s", 
           perguntas[perguntaAtual].opcaoB);
  lcd.setCursor(0, 3);
  lcd.print(linhaB);
}

void continuarJogoAposQuiz() {
  beepAcerto();

  vidas++;
  quizDisponivel = false;
  proximoQuizPonto = score + 50;
  estadoAtual = JOGO;

  lcd.clear();
  desenharFundoJogo();

  for (int i = 0; i < NUM_OBSTACULOS; i++) {
    apagarPosicao(posObsJogo[i], linhaObsJogo[i]);
  }
  resetObstaculosJogo();

  linhaMariaAntJogo = linhaMariaJogo;

  ultimoScoreMostrado = -1;
  ultimoQuizMostrado = !quizDisponivel;

  tFrameJogo = millis();
  tMoveJogo = millis();
  tScore = millis();
  tMoveMariaJogo = millis();  // ← NOVO

  desenharHUD(true);
}

void errarQuiz() {
  beepErro();
  delay(250);

  quizDisponivel = false;
  proximoQuizPonto = score + 50;
  vidas--;
  if (vidas > 0) {
    estadoAtual = JOGO;
    lcd.clear();
    desenharFundoJogo();
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
      apagarPosicao(posObsJogo[i], linhaObsJogo[i]);
    }
    resetObstaculosJogo();
    for (int i = 0; i < NUM_OBSTACULOS; i++) {
      linhaObsAntJogo[i] = linhaObsJogo[i];
    }
    linhaMariaAntJogo = linhaMariaJogo;
    ultimoScoreMostrado = -1;
    ultimoQuizMostrado = !quizDisponivel;
    tFrameJogo = millis();
    tMoveJogo = millis();
    tScore = millis();
    tMoveMariaJogo = millis();  // ← NOVO
    desenharHUD(true);
  } else {
    estadoAtual = GAMEOVER;
  }
}

void responderQuiz(char resposta) {
  if (resposta == perguntas[perguntaAtual].correta) {
    continuarJogoAposQuiz();
  } else {
    errarQuiz();
  }
}

void loopQuiz() {
  bool aAtual = (digitalRead(BTN_A) == LOW);
  bool bAtual = (digitalRead(BTN_B) == LOW);

  if (!aAtual) {
    aPressionadoAntesQuiz = false;
  }

  if (!bAtual) {
    bPressionadoAntesQuiz = false;
  }

  if (aAtual && !aPressionadoAntesQuiz) {
    aPressionadoAntesQuiz = true;
    responderQuiz('A');
  }

  if (bAtual && !bPressionadoAntesQuiz) {
    bPressionadoAntesQuiz = true;
    responderQuiz('B');
  }
}

// ===== GAMEOVER =====
void gameOver() {
  lcd.clear();

  printCentralizado(0, "FIM DA LUTA!");
  printCentralizado(1, "Score Final:");

  char scoreTexto[10];
  snprintf(scoreTexto, sizeof(scoreTexto), "%d PTS", score);
  printCentralizado(2, scoreTexto);

  printCentralizado(3, "Pressione A");

  while (digitalRead(BTN_A) == LOW) {
    delay(50);
  }
  while (digitalRead(BTN_A) == HIGH) {
    delay(50);
  }

  estadoAtual = MENU;
  multiplicadorDificuldade = 1.0;
  multiplicadorMaria = 1.0;  // ← Resetar Maria
  resetarMenu();
}

// ===== SETUP =====
void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();

  criarSprites();
  randomSeed(analogRead(34));
  resetarMenu();
}

// ===== LOOP PRINCIPAL =====
void loop() {
  switch (estadoAtual) {
    case MENU:
      loopMenu();
      break;

    case JOGO:
      loopJogo();
      break;

    case QUIZ:
      loopQuiz();
      break;

    case GAMEOVER:
      gameOver();
      break;
  }
}
