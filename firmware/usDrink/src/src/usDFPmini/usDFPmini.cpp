/*  2022-02-02 Whiskeroff <whiskeroff@mail.ru>
    DFP library for DFPlayer Mini module
    ver. 0.1.0
*/

#include <Arduino.h>
#include "usDFPmini.h"

// инициализация плеера
#if (TXOS_USE == 1)
void usDFPmini::init (TXOnlySerial* theSerial, uint8_t busyPIN) {
#else
void usDFPmini::init (SoftwareSerial* theSerial, uint8_t busyPIN) {
#endif	
	mySerial = theSerial;
	_playTimer = millis();
	//_playTimeout = 999;
	_playFlag = false;
	_busy = false;
	_busyPIN = busyPIN; // 0 - если пин BUSY не подключен или не используется, номер PIN если BUSY подключен
	if (_busyPIN != 0) pinMode(_busyPIN, INPUT);
#ifdef DFP_DEBUG	
	Serial.print("Use DFPlayer mini! BUSY on D");
	Serial.println(_busyPIN);
#endif // DFP_DEBUG
}

void usDFPmini::tick() {
	uint8_t busyState;
#ifdef DFP_DEBUG	
	uint32_t playStarted;
#endif // DFP_DEBUG
	if (_busyPIN == 0) return;  // переработать
	busyState = !digitalRead(_busyPIN);
	if (!_busy && busyState && _playFlag) {
#ifdef DFP_DEBUG	
		playStarted = millis() - _playTimer;
		Serial.print(", started in ");
		Serial.print(playStarted);
		Serial.print("ms");
#endif // DFP_DEBUG
		_busy = true;
		_playTimer = millis();
		 return;
	}
	if (_busy && !busyState && _playFlag) {
	_busy = false;
	_playFlag = false;
#ifdef DFP_DEBUG	
	playStarted = millis() - _playTimer;
	Serial.print(", finished in ");
	Serial.print(playStarted);
	Serial.println("ms.");
#endif // DFP_DEBUG
	return;
	}
  if ((!_busy) && (!busyState) && (millis() > (_playTimer + DFP_TIMEOUT)) && _playFlag) {
#ifdef DFP_DEBUG	
	  Serial.println("... not started (timeout)");
#endif // DFP_DEBUG
    _playFlag = false;
  }
}

// проверка занят ли плеер
boolean usDFPmini::isBusy() {
  tick();
  // переработать на плай флаг
  if (_busyPIN == 0) return false; // если пин BUSY не подключен, вернуть false
  return _busy || _playFlag ; // вернуть true если 
}
 
// проиграть случайный семпл 
void usDFPmini::playRandom() {
	randomSeed(micros());
	play(uint16_t(random(1, DFP_SAMPLE_MAX)));
}

// послать произвольную команду
void usDFPmini::sendCmd (uint8_t *cmd) {
	uint8_t checksum = 0;
	cmd[4] = 1;
	for (int i=2; i<8; i++) {
		checksum += cmd[i];
	}
	cmd[8] = (uint8_t)~checksum;
	for (int i=0; i<10; i++) { //send cmd
		(*mySerial).write (cmd[i]);
	}
}

//
void usDFPmini::play() {
	static uint8_t play_cmd [10] = { 0X7E, 0xFF, 0x06, 0X0D, 00, 00, 00, 0xFE, 0xee, 0XEF};
	_playTimer = millis();
	_playFlag = true;
	_busy = false;
	sendCmd (play_cmd);
}

//
void usDFPmini::play(uint16_t index) {
	static uint8_t play_cmd [10] = { 0X7E, 0xFF, 0x06, 0X03, 00, 00, 00, 0xFE, 0xee, 0XEF};
	play_cmd[5] = (uint8_t)(index >> 8);
	play_cmd[6] = (uint8_t)(index);
	_playTimer = millis();
	_playFlag = true;
	_busy = false;
	sendCmd (play_cmd);
#ifdef DFP_DEBUG	
	Serial.print("Play track ");
	Serial.print(index);
#endif // DFP_DEBUG
}

//
void usDFPmini::setVolume(uint8_t vol) {
	static uint8_t play_cmd [10] = { 0X7E, 0xFF, 0x06, 0X06, 00, 00, 15, 0xFE, 0xee, 0XEF};
	play_cmd[6] = (uint8_t)(vol);
	sendCmd (play_cmd);
}

//
void usDFPmini::play(int index) {
	play ((uint16_t)index);
}

//
void usDFPmini::pause() {
	static uint8_t pause_cmd [] = { 0X7E, 0xFF, 0x06, 0X0E, 00, 00, 00, 0xFE, 0xED, 0XEF};
	sendCmd (pause_cmd);
}

//
void usDFPmini::stop() {
	static uint8_t stop_cmd [] = { 0X7E, 0xFF, 0x16, 0X0E, 00, 00, 00, 0xFE, 0xED, 0XEF};
	sendCmd (stop_cmd);
}

//
void usDFPmini::next () {
	static uint8_t next_cmd [] = { 0X7E, 0xFF, 0x06, 0X01, 00, 00, 00, 0xFE, 0xFA, 0XEF};
	sendCmd(next_cmd);
}

//
void usDFPmini::previous() {
	static uint8_t previous_cmd [] = { 0X7E, 0xFF, 0x06, 0X02, 00, 00, 00, 0xFE, 0xF9, 0XEF};
	sendCmd (previous_cmd);
}
