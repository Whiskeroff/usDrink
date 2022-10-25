/*  2022-02-02 Whiskeroff <usco@mail.ru>
    DFP library for DFPlayer Mini module
    ver. 0.1.0
*/

#define TXOS_USE 1         // 0 -> использовать SoftwareSerial, 
                           // 1 -> использовать TXOnlySerial
//#define DFP_DEBUG        // раскоментировать для вывода в консоль 
                           // диагностических сообщений
#define DFP_TIMEOUT 1000   // таймаут операций с DFPlayer, мс
#define DFP_SAMPLE_MAX 40  // количество треков на SD-карте, 
                           // используется для рандомного выбора тоста

#if (TXOS_USE == 1)
#include "TXOnlySerial.h"
#else
#include "SoftwareSerial.h"
#endif

class usDFPmini{
public :
#if (TXOS_USE == 1)
	void init (TXOnlySerial*, uint8_t busyPIN);
#else
	void init (SoftwareSerial*, uint8_t busyPIN);
#endif // TXOS_USE	
	void sendCmd (uint8_t *); 
	void play();
	void play (uint16_t index);
	void play (int index);
	void pause();
	void stop();
	void next();
	void previous();
	void setVolume(uint8_t vol );
	void tick();
	bool isBusy();
	void playRandom();
private:
#if (TXOS_USE == 1)
	TXOnlySerial* mySerial;
#else
	SoftwareSerial* mySerial;
#endif
	uint32_t _playTimer;
	bool _playFlag;
	bool _busy;
	uint8_t _busyPIN;
};
