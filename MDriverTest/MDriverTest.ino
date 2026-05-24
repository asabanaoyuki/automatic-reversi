const int PIN_SER    = D7;
const int PIN_OE     = D8;
const int PIN_LATCH  = D9;
const int PIN_CLK    = D10;

void setup() {
  Serial.begin(9600);
  pinMode( PIN_SER, OUTPUT );
  pinMode( PIN_LATCH, OUTPUT );
  pinMode( PIN_CLK, OUTPUT );
  pinMode( PIN_OE, OUTPUT );

  
  digitalWrite(PIN_OE, LOW);
  digitalWrite(PIN_LATCH, LOW);
  byte b = B01100110;
  shiftOut(PIN_SER, PIN_CLK, LSBFIRST, b);
  digitalWrite(PIN_LATCH, HIGH);
}

byte rowData = 0;
char feeld[17] = {0};


void loop() { 
if (Serial.available()) {
    char c = Serial.read();
    
    // 改行文字（\n や \r）が届いた場合は無視する（シリアルモニター対策）
    if (c == '\n' || c == '\r') return;

    // 現在の文字列の長さを取得（最大16文字まで安全にカウント）
    size_t len = strnlen(feeld, sizeof(feeld) - 1);

    if (len < 16) {
      // ── パターンA: まだ配列（最大16文字）に余裕があるとき ──
      // 単純に末尾に文字を追加して、最後にヌル文字を入れる
      feeld[len] = c;
      feeld[len + 1] = '\0';
    } 
    else {
      // ── パターンB: 既に15文字いっぱいのとき ──
      // 2文字目から末尾までを左に1つずらす（1文字目は消える）
      memmove(feeld, feeld + 1, 15); 
      // 一番最後のマス（インデックス14）に新しい文字を入れる
      feeld[15] = c;
      // 念のため最後（インデックス15）は常にヌル文字
      feeld[16] = '\0';
    }
    
    // 結果を表示
    Serial.println(feeld);

    //4桁ずつに分解
    char feeld1[5],feeld2[5],feeld3[5],feeld4[5];
    strncpy(feeld1, &feeld[0], 4);
    feeld1[4] = '\0';
    strncpy(feeld2, &feeld[4], 4);
    feeld2[4] = '\0';
    strncpy(feeld3, &feeld[8], 4);
    feeld3[4] = '\0';
    strncpy(feeld4, &feeld[12], 4);
    feeld4[4] = '\0';

    //モータへの指令に変換
    byte byteFeeld = ChangeMDriverInput(feeld1);
    Serial.println(byteFeeld, BIN);
    
    digitalWrite(PIN_LATCH, LOW);
    shiftOut(PIN_SER, PIN_CLK, LSBFIRST, byteFeeld);
    digitalWrite(PIN_LATCH, HIGH);
  }
  analogFadeToMax(PIN_OE,3000);
}


byte ChangeMDriverInput(char cinput[]){
  byte byte_feeld = 0;
  //文字の位置ごとに変換方式を変えないといけないのでFORループで回せない
  //1文字目
  if(cinput[0] == 'O'){
    byte_feeld |= 0b00;
  }else if(cinput[0] == 'A'){
    byte_feeld |= 0b01;
  }else if(cinput[0] == 'B'){
    byte_feeld |= 0b10;
  }
  //2文字目
  byte_feeld = byte_feeld << 2; 
  if(cinput[1] == 'O'){
    byte_feeld |= 0b00;
  }else if(cinput[1] == 'A'){
    byte_feeld |= 0b10;
  }else if(cinput[1] == 'B'){
    byte_feeld |= 0b01;
  }
  //3文字目
  byte_feeld = byte_feeld << 2; 
  if(cinput[2] == 'O'){
    byte_feeld |= 0b00;
  }else if(cinput[2] == 'A'){
    byte_feeld |= 0b01;
  }else if(cinput[2] == 'B'){
    byte_feeld |= 0b10;
  }
  //4文字目
  byte_feeld = byte_feeld << 2; 
  if(cinput[3] == 'O'){
    byte_feeld |= 0b00;
  }else if(cinput[3] == 'A'){
    byte_feeld |= 0b10;
  }else if(cinput[3] == 'B'){
    byte_feeld |= 0b01;
  }
  return byte_feeld;
}

// pin: PWMピン番号, durationMs: MAXまで持っていく全体の時間（ミリ秒）
void analogFadeToMax(int pin, int durationMs) {
  // 0 から 255 まで 1ずつ上げていく
  // 1ステップあたりの待ち時間を計算
  int stepDelay = durationMs / 255;

  for (int val = 0; val <= 255; val++) {
    analogWrite(pin, 255-val);
    delay(stepDelay); 
  }
}
