#include <LinkedList.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial ss(10,11); //RX, TX
DFRobotDFPlayerMini mp3;

int numberOfFiles = 0;

LinkedList<int> numbersSelected = LinkedList<int>();

const int numRows = 4;       // numero di righe
const int numCols = 4;       // numero di colonne
const int debounceTime = 20; // millisecondi necessari allo switch per diventare stabile

// Mappa dei caratteri sul tastierino
const char keymap[numRows][numCols] = 
{
  { '1', '2', '3', 'A'  } ,
  { '4', '5', '6', 'B'  } ,
  { '7', '8', '9', 'C'  } ,
  { '*', '0', '#', 'D'  }
};

// Definisco i pin connessi alle righe o alle colonne
const int rowPins[numRows] = {5, 4, 3, 2}; // Pin di arduino connessi ai pin 1,2,3 e 4 delle righe del keypad
const int colPins[numCols] = {9, 8, 7, 6}; // Pin di arduino connessi ai pin 5,6,7 e 8 delle colonne del keypad

void setup()
{
  Serial.begin(9600);
  for (int row = 0; row < numRows; row++)
  {
    pinMode(rowPins[row],INPUT);       // Imposta i pun delle righe come input
    digitalWrite(rowPins[row],HIGH);   // Imposta le righe a HIGH (inattiva) - abilita le resistenze di pull-ups
  }
  for (int column = 0; column < numCols; column++)
  {
    pinMode(colPins[column],OUTPUT);     // Setta i pin delle colonne come input
    digitalWrite(colPins[column],HIGH);  // Imposta le colonne a HIGH (inattiva) - abilita le resistenze di pull-ups
  }

  ss.begin(9600);
  Serial.println("Running...");
  if (!mp3.begin(ss)) {
    Serial.println("Controlla cavi");
    while(true); 
  }

  mp3.volume(25);
  numberOfFiles = mp3.readFileCounts();
  numberOfFiles = mp3.readFileCounts();
}

void loop()
{
  char key = getKey();
  int number = 0;
  int listSize = 0;
  if( key != 0) 
  {       
    // se il carattere non è 0 allora ho premuto un pulsante
    if(key != '#') {
      if(key == 'A'){
        mp3.volumeUp();
      } else if(key == 'B') {
        mp3.volumeDown();
      } else {
        Serial.print("Hai premuto: ");
        Serial.println(key);
        numbersSelected.add(key); 
      }
    } else {
        number = 0;
        listSize = numbersSelected.size();
        for (int h = 0; h < listSize; h++) {

          // Get value from list
          int val = numbersSelected.get(h);
          Serial.print(val);
          Serial.print(" ");
          val = val - 48;
          number = number + val*pow(10,listSize-h-1);
          val = 0;
        }
        if(listSize > 2) {
          number = number + 1;  // questa cosa è veramente molto strana
        }
        Serial.print("Hai inserito il valore: ");
        Serial.println(number);
        numbersSelected.clear();

        Serial.println(numberOfFiles);
        
        if(number > 0 && number<=numberOfFiles-1) {
          mp3.playMp3Folder(number);
        } else {
          mp3.playMp3Folder(0000);
        }
    }
  }
}

// funzione che restituisce il tasto premuto o 0 se nessun tasto è digitato 
char getKey()
{
  char key = 0;                                  // 0 indica nessun tasto premuto

  for(int column = 0; column < numCols; column++)
  {
    digitalWrite(colPins[column],LOW);         // Attiva la colonna corrente per la lettura.
    for(int row = 0; row < numRows; row++)     // Scorro tutte le righe cercano un tasto premuto
    {
      if(digitalRead(rowPins[row]) == LOW)     // E' premuto il tasto
      {
        delay(debounceTime);                     // attendo che il segnale si stabilizzi (vedi annotazione)
        while(digitalRead(rowPins[row]) == LOW); // attendo che il tasto sia rilasciato
        key = keymap[row][column];               // Memorizzo quale sia il tasto premuto
      }
    }
    digitalWrite(colPins[column],HIGH);     // Rendo inattiva la colonna.
  }
  return key;  // restituisce il tasto premuto 0 altrimenti
}
