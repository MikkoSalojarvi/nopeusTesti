int kaikki[200];
int valoIntervalli = 1500;
volatile unsigned long aika;
volatile unsigned long viimeValoAika = 0;
volatile int kierros = 0; // monesko kierros valoilla on menossa
volatile int diodi[4]; // moninkertaistetaan interruptien määrä diodine avulla
volatile byte pisteet = 0;
volatile int nappi = 0;
int seuraavaValo = 0;
volatile unsigned long aika2;
const int tones [9] = {1956, 1136, 1432, 1700}; // (111, 147, 181, 220} {131, 147, 165, 175} ;
int num; // käytetään pisteiden muuttamisessa binääriseksi 7 segment ajuria varten
int lopetus = 1;
int ykkoset;
const byte numPins = 4;       // näytön pinnien asettamiseen
const byte pins[] = {8, 9, 10, 11}; //
const int ykkosNaytto = 6;
const int kymppiNaytto = 7;
const int kytkin = 2;
int varmistin = 0;
const int napit[4] = {3, 4, 19};
const int summeri = 13;
int vaikeus = 0;
bool ledState = 0;

void setup() {
  for (int i = 6; i < 19; i++) { //5-13 output pinnejä
    pinMode(i, OUTPUT);
  }
  for (int i = 2; i < 5; i++) {
    pinMode (i, INPUT);
  }
  pinMode (19, INPUT);
  attachInterrupt(digitalPinToInterrupt(kytkin), nappi2, RISING);
}

void nappi2 () {
  if ( aika2 < millis() - 190) { // debounce yritys, ei toimi... suoritetaan vain kun 190 ms kulunut viime suorituksesta
    varmistin = digitalRead(kytkin);
    if (varmistin == HIGH) {
      for (int i = 0; i < 4; i++) {
        diodi[i] = digitalRead(napit[i]);
      }
      if (diodi[0] != 0) { // digitalPin3,eli nappi 2, painettu
        nappi = 2;
      }
      else if (diodi[1] != 0) { // digitalPin 4, eli nappi 1
        nappi = 1;
      }
      else if (diodi[2] != 0) {
        nappi = 4;
      }
      else nappi = 3;
      tone(summeri, tones[nappi - 1], 70);
      if (nappi == kaikki[pisteet + 1]) {
        pisteet = pisteet + 1;
      }
      else { // peli loppuu, lisää ääni jos peliin tulee summeri
        detachInterrupt(digitalPinToInterrupt(kytkin));
        digitalWrite(kaikki[kierros] + 14, LOW);
        kierros = 0;
        lopetus = 1;
      }
    }
  }
  aika2 = millis();
}

void naytto() {
  if (viimeValoAika % 10 == 10 || 0 == aika % 10) {
    if (ykkoset == 1) {
      digitalWrite(kymppiNaytto, HIGH); // sammuttaa kymppien näytön
      byte num = pisteet % 10;
      for (byte i = 0; i < numPins; i++) {
        digitalWrite(pins[i], bitRead(num, i));
      }
      digitalWrite(ykkosNaytto, LOW); // ykkösten näyttö päälle
      ykkoset = 0;
    }
    else {
      digitalWrite(ykkosNaytto, HIGH); // sammuttaa ykkosten näytön
      byte num = (pisteet / 10) % 10;
      for (byte i = 0; i < numPins; i++) {
        digitalWrite(pins[i], bitRead(num, i));
      }
      digitalWrite(kymppiNaytto, LOW); // kympppien näyttö päälle
      ykkoset++;
    }
  }
}

void loop() {
  aika = millis();
  while (lopetus != 0) {
    if (lopetus == 1) {
      tone(summeri, 392); // nuotti G4
      while (aika >= viimeValoAika - 230) {
        viimeValoAika = millis();
        naytto();
      }
      tone(summeri, 262);
      while (aika >= viimeValoAika - 480) {
        viimeValoAika = millis();
        naytto();
      }
      naytto();
      noTone(summeri);
      lopetus++;
    }
    detachInterrupt(digitalPinToInterrupt(kytkin)); //Tämähän on jo irti??
    if (aika < viimeValoAika - 100) {
      nappi = 0;
      valoIntervalli = 1500;
      seuraavaValo = 14 * vaikeus;
      while ( nappi == 0) {
        naytto();
        nappi = digitalRead(kytkin);
        viimeValoAika = millis();
        if (viimeValoAika > aika2 + 750) {
          aika2 = millis();
          digitalWrite(seuraavaValo, ledState);
          ledState = !ledState;
          naytto();
        }
      }
      digitalWrite(seuraavaValo, LOW);
      for (int i = 0; i < 4; i++) {
        diodi[i] = digitalRead(napit[i]);
        naytto();
      }
      if (diodi[0] != 0) {
        vaikeus = 2;
      }
      else if (diodi[1] != 0) {
        vaikeus = 1;
      }
      else if (diodi[2] != 0) {
        vaikeus = 4;
      }
      else {
        vaikeus = 3;
      }
      pisteet = 0;
      aika = millis();
      while (aika >= viimeValoAika - 480) {
        viimeValoAika = millis();
        naytto();
      }
      nappi = 0;
      lopetus = 0;
      attachInterrupt(digitalPinToInterrupt(kytkin), nappi2, RISING);
    }
    viimeValoAika = millis();
    naytto();
    nappi = 0;
    kierros = 0;
  }
  if ( aika >= viimeValoAika + valoIntervalli || pisteet == kierros) { // aika vaihtaa lamppua
    digitalWrite(kaikki[kierros] + 14, LOW);
    kierros++;
    kaikki[kierros] = random(1, 5);
    while ( kaikki[kierros] == kaikki[kierros - 1]) {
      kaikki[kierros] = random(1, 5);
    }
    viimeValoAika = millis();
    naytto();
    seuraavaValo = kaikki[kierros] + 14;
    digitalWrite(seuraavaValo, HIGH);
    if (kierros <= 7) valoIntervalli = valoIntervalli - (80 + 5 * vaikeus);
    else if (kierros <= 37) valoIntervalli = valoIntervalli - (6 + vaikeus);
    else if (kierros <= 85) valoIntervalli = valoIntervalli - (1 + vaikeus);
    else valoIntervalli = valoIntervalli - 3;
  }
  naytto();
}
