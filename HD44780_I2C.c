//service display HD44780 I2C - PCF8574T.
   
#define F_CPU 16000000UL
/*
zdefiniowanie wejsciem przetwornika ADC bedzie wyprowadzenie PC0, PC1
tutaj podpinamy termistor (termistory)
*/

/*
#define ADCIN PC0 //Nie wykorzystywane poniewaz wejscie ADC definiujemy za pomoca
ADMUX z podaniem numeru wejscia 0,1,2,3 Atmega 328 ma 6 wejsc ADC ale dwa sa zajete
na SCL i SDA do wyswietlacza HD44780
*/
#include <avr/io.h>
#include "lcdvtwi.h"
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>


//Function run in - main function. Read state entry ADC (PC0, PC1, PC2, PC3)
//adctouse - number entry converter ADC from which read value

int ADCsingleREAD(uint8_t adctouse)
{
    int ADCval;
	ADCval = 0;
	ADCL = 0;
    ADMUX = adctouse; // use #1 ADC
	//ADMUX = 0;  // use #1 ADC
    ADMUX |= (1 << REFS0);	// use AVcc as the reference
    ADMUX &= ~(1 << ADLAR);	// clear for 10 bit resolution
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	// 128 prescale for 8Mhz
    ADCSRA |= (1 << ADEN);    // Enable the ADC

    ADCSRA |= (1 << ADSC);    // Start the ADC conversion

    while(ADCSRA & (1 << ADSC));   

    ADCval = ADCL;
    ADCval = (ADCH << 8) + ADCval;    // ADCH is read so ADC can be updated again
    return ADCval;
}

int main(void)
{
	//Pin PD0, PD1, PD2,  PD3 port D set jako output
	DDRD |= (1<<PD0);
	DDRD |= (1<<PD1);
	DDRD |= (1<<PD2);
	DDRD |= (1<<PD3);

	//initiation display
	lcdvtwi_init();
	lcdvtwi_led_on();

	//clear display
	lcdvtwi_clear();

    int ADC0value;
	int ADC1value;
	int ADC2value;
	char temperatura_celsjusz_adc0;
	char temperatura_celsjusz_adc1;

	static char info1;

	int licznik;
	licznik = 0;
	
	//loop without end
    while (1)
    {
		++licznik;
		
		static char adc_po_konwersjia[15];
		static char adc_po_konwersjib[15];
		static char adc_po_konwersjic[15];

		switch(licznik){
			case 1:

			//activation function read  - number port input ADC 0
    		ADC0value = ADCsingleREAD(0);
        	// ADCvalue now contains an 10bit ADC read

			//clear display
			lcdvtwi_clear();
				
			//Display data.
			//static char adc_po_konwersjia[15];
			dtostrf(ADC0value,3,0, adc_po_konwersjia);

			lcdvtwi_set_cursor(0,0);
			lcdvtwi_write_text("ADC0 ");
			lcdvtwi_write_text(adc_po_konwersjia);

			if(ADC0value > 700){

				lcdvtwi_write_text(" START1");
				lcdvtwi_set_cursor(0,1);
				lcdvtwi_write_text("temp. ");
				lcdvtwi_write_text(">30.00 C");

				//short circuit relay controlled by PD0
				//Pin PD0 set low state to turn on relay
				PORTD &=~(1<<PD0);
				//info1 = "POMPA";
				//info2 = "START";
			}else{
				lcdvtwi_write_text(" STOP1");
				lcdvtwi_set_cursor(0,1);
				lcdvtwi_write_text("temp. ");
				lcdvtwi_write_text("<30.00 C");
				//Pin PD0 set jako high state
				//necessary to turn off relay 
			
				PORTD |= (1<<PD0);
				//info1 = " STOP";
			}

			break;
			case 2:
			//activation function read  - number port input ADC 1
			ADC1value = ADCsingleREAD(1);
			//czyscimy wyswietlacz
			lcdvtwi_clear();
			//Wyswietlenie danych na wyswietlaczu. Odbywa sie w petli
			//static char adc_po_konwersjia[15];
			dtostrf(ADC1value,3,0, adc_po_konwersjib);

			lcdvtwi_set_cursor(0,0);
			lcdvtwi_write_text("ADC1 ");
			lcdvtwi_write_text(adc_po_konwersjib);

			if(ADC1value > 700){
				lcdvtwi_write_text(" START2");
				lcdvtwi_set_cursor(0,1);
				lcdvtwi_write_text("temp. ");
				lcdvtwi_write_text(">40.00 C");

				//Zwarcie przekaznika sterowanego przez PD0
				//Nozka PD0 ustawiona jako stan niski po to zeby WLACZYC przekaznik
				PORTD &=~(1<<PD1);
				//info1 = "POMPA";
				//info2 = "START";
			}else{
				lcdvtwi_write_text(" STOP2");
				lcdvtwi_set_cursor(0,1);
				lcdvtwi_write_text("temp. ");
				lcdvtwi_write_text("<30.00 C");
				//Nozka PD0 ustawiona jako stan wysoki
				//To jest konieczne aby WYLACZYC przekaznik gdy wartosc z przetwornika ADC
				//nie pozwala na zalaczenie przekaznika
				PORTD |= (1<<PD1);
				//info1 = " STOP";
			}

			break;
			case 3:
		
			//Uruchomienie funkcji odczytujacej z przekazaniem numeru wejscia ADC 2
			ADC2value = ADCsingleREAD(3);
			//czyscimy wyswietlacz
			lcdvtwi_clear();
			//Wyswietlenie danych na wyswietlaczu. Odbywa sie w petli
			//static char adc_po_konwersjia[15];
			dtostrf(ADC2value,3,0, adc_po_konwersjic);

			lcdvtwi_set_cursor(0,0);
			lcdvtwi_write_text("ADC2 ");
			lcdvtwi_write_text(adc_po_konwersjic);

			if(ADC2value > 700){
				lcdvtwi_write_text(" START3");
				lcdvtwi_set_cursor(0,1);
				lcdvtwi_write_text("temp. ");
				lcdvtwi_write_text(">30.00 C");

				//Zwarcie przekaznika sterowanego przez PD0
				//Wyprowadzenie PD0 ustawiona jako stan niski po to zeby WLACZYC przekaznik
				PORTD &=~(1<<PD2);
				//info1 = "POMPA";
				//info2 = "START";
			}else{
				lcdvtwi_write_text(" STOP3");
				lcdvtwi_set_cursor(0,1);
				lcdvtwi_write_text("temp. ");
				lcdvtwi_write_text("<30.00 C");
				//Wyprowadzenie PD0 ustawiona jako stan wysoki
				//To jest konieczne aby WYLACZYC przekaznik gdy wartosc z przetwornika ADC
				//nie pozwala na zalaczenie przekaznika
				PORTD |= (1<<PD2);
				//info1 = " STOP";
			}

			break;
			case 4:
			//czyscimy wyswietlacz
			lcdvtwi_clear();
			lcdvtwi_set_cursor(0,0);
			lcdvtwi_write_text("ADC3 brak czujnika");
			break;
		}
		
		_delay_ms(100);

		if(licznik==3){
			licznik = 0;
		}

    }

}

