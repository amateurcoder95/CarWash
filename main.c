#include "Includes.h" 
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)
//END CONFIG
// Main function
void ADCinit(void);
void displayState1(void);
void displayState2(void);
void displayState3(void);
void displayState4(void);
unsigned int ReadADC(void);
unsigned int ADCvalue = 0;
unsigned int f, h_alarm = 0, m_alarm = 0, s_alarm = 0;
unsigned int voltage;
unsigned int state = 1;
unsigned int switch_state = 0;

void main(void) {
    TRISA = 0xff;
    TRISC = 0X00;
    TRISD = 0X00;
    InitLCD(); // Initialize LCD	
    InitI2C(); // Initialize i2c pins
    state = 1;
    // Khoi tao gia mac dinh
    //__delay_ms(10000) // Tre 1s  
    Set_DS1307_RTC_Time(PM_Time, 2, 30, 30);
    // kh?i t?o ngày tháng m?c ??nh
    Set_DS1307_RTC_Date(04, 04, 16, Tuesday);
    DisplayFirstTimeToLCD(Get_DS1307_RTC_Time());
    while (1) {
        switch (state) {
            case 1:
                displayState1();
                break;
            case 2:
                displayState2();
                break;
            case 3:
                displayState3();
                break;
            case 4:
                displayState4();
                break;
        }
    }
}

void ADCinit(void)// adc
{
    ADCON1bits.ADCS2 = 0, ADCON0bits.ADCS1 = 0, ADCON0bits.ADCS0 = 0;
    CHS2 = 0, CHS1 = 0, CHS0 = 0;
    ADCON1bits.ADFM = 1;
    ADCON1bits.PCFG = 0XE;
    ADCON0bits.ADON = 1;
}
//lay nhiet do

unsigned int ReadADC(void) {
    unsigned int nhietdo = 0;
    ADCON0bits.GO_nDONE = 1;
    while (ADCON0bits.GO_nDONE);
    nhietdo = ADRESH * 256 + ADRESL;
    return (nhietdo);
}
//Trang thai 1: Hien thi thoi gian, ngay thang

void displayState1(void) {
    while (1) {
        // Hien thi thoi gian
        DisplayTimeToLCD(Get_DS1307_RTC_Time());
        ADCinit();
        ADCvalue = ReadADC();
        //Hien thi ngày tháng t? giây 0->giây 40
        if (Get_DS1307_RTC_Time()[0] > 40) {
            WriteCommandToLCD(0xc0);
            DisplayDateOnLCD(Get_DS1307_RTC_Date());
        }
        //Hi?n th? nhi?t ?? t? giây 40->59
        if (Get_DS1307_RTC_Time()[0] < 40) {

            voltage = (int) (5000.0f / 1023 * ADCvalue);
            f = voltage / 10;
            WriteCommandToLCD(0xc0);
            WriteStringToLCD("TEMP: ");
            WriteTempToLCD(f);
            WriteCommandToLCD(0xc8);
            WriteDataToLCD(0xdf);
            WriteStringToLCD("C");
        }
        //WriteCommandToLCD(0xc0);
        //Nh?n nút chuy?n tr?ng thái
        if (!RA1) {
            state++;
            if (state == 5) {
                state = 1;
            }
            break;
        }
        //Báo th?c
        if (h_alarm == (int) Get_DS1307_RTC_Time()[2] && m_alarm == (int) Get_DS1307_RTC_Time()[1]) {
            RD0 = 0;
        }
        //T?t báo th?c
        if (!RA4) {
            RD0 = 1;
            h_alarm = 0;
            m_alarm = 0;
        }

    }
}
//Tnangj thái 2: S?a th?i gian

void displayState2(void) {
    ClearLCDScreen();
    //Ba bi?n h,m,s l?u l?i th?i gian hi?n t?i
    unsigned int h;
    unsigned int m;
    unsigned int s;
    h = (int) Get_DS1307_RTC_Time()[2];
    m = (int) Get_DS1307_RTC_Time()[1];
    s = (int) Get_DS1307_RTC_Time()[0];
    //B?m nút chuy?n tr?ng thái
    if (!RA1) {
        state++;
        if (state == 5) {
            state = 1;
        }
    }
    //S?a th?i gian
    while (1) {
        WriteCommandToLCD(0x80);
        WriteStringToLCD("  SETTING TIME");
        WriteCommandToLCD(0xc0);
        WriteTempToLCD(h);
        WriteStringToLCD(":");
        WriteTempToLCD(m);
        WriteStringToLCD(":");
        WriteTempToLCD(s);
        if (!RA1) break;
        while (1) {
            //Khi nút switch ???c b?m s? chuy?n qua các ch? ?? ?i?u ch?nh khác
            if (RA4 == 0) {
                switch_state++;
                if (switch_state == 4) {
                    break;
                }
            }
            //Chuy?n tr?ng thái
            if (RA1 == 0) {
                state = 3;
                break;
            }
            switch (switch_state) {
                case 1:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("   CHANGE HOUR     ");
                    if (RA2 == 0 && h < 24) {
                        h++;
                        WriteCommandToLCD(0xc0);
                        WriteTempToLCD(h);
                    }
                    if (RA3 == 0 && h > 0) {
                        h--;
                        WriteCommandToLCD(0xc0);
                        WriteTempToLCD(h);
                    }
                    break;

                case 2:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("   CHANGE MINUTE   ");
                    if (RA2 == 0 && m < 60) {
                        m++;
                        WriteCommandToLCD(0xc0 + 3);
                        WriteTempToLCD(m);
                    }
                    if (RA3 == 0 && m > 0) {
                        m--;
                        WriteCommandToLCD(0xc0 + 3);
                        WriteTempToLCD(m);
                    }
                    break;
                case 3:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("  CHANGE SECOND  ");
                    if (RA2 == 0) {
                        s++;
                        WriteCommandToLCD(0xc0 + 6);
                        WriteTempToLCD(s);
                    }
                    if (RA3 == 0 && s > 0) {
                        s--;
                        WriteCommandToLCD(0xc0 + 6);
                        WriteTempToLCD(s);
                    }
                    break;
            }
            //L?u l?i th?i gian v?a thay ??i
            Set_DS1307_RTC_Time(PM_Time, h, m, s);
        }
    }

}
//Tr?ng thái 3: S?a ngày

void displayState3(void) {
    ClearLCDScreen();
    //kh?i t?o các bi?n l?u th? ngày tháng n?m
    unsigned int d;
    unsigned int m;
    unsigned int y;
    unsigned int w;
    d = (int) Get_DS1307_RTC_Date()[1];
    m = (int) Get_DS1307_RTC_Date()[2];
    y = (int) Get_DS1307_RTC_Date()[3];
    w = (int) Get_DS1307_RTC_Date()[0];
    if (!RA1) {
        state++;
        if (state == 5) {
            state = 1;
        }
    }
    while (1) {
        WriteCommandToLCD(0x80);
        WriteStringToLCD("  SETTING DATE   ");
        WriteCommandToLCD(0xc0);
        WriteTempToLCD(d);
        WriteStringToLCD(":");
        WriteTempToLCD(m);
        WriteStringToLCD(":");
        WriteTempToLCD(y);
        WriteStringToLCD(" ");
        switch (w) {
            case Monday: WriteStringToLCD("MON");
                break;
            case Tuesday: WriteStringToLCD("TUE");
                break;
            case Wednesday: WriteStringToLCD("WED");
                break;
            case Thursday: WriteStringToLCD("THU");
                break;
            case Friday: WriteStringToLCD("FRI");
                break;
            case Saturday: WriteStringToLCD("SAT");
                break;
            case Sunday: WriteStringToLCD("SUN");
                break;

            default: WriteStringToLCD("???");
                break;
        }

        if (!RA1) break;
        while (1) {
            if (RA4 == 0) {
                switch_state++;
                if (switch_state == 4) {
                    break;
                }
            }
            if (RA1 == 0) {
                state = 4;
                break;
            }
            switch (switch_state) {
                case 1:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("  CHANGE DAY   ");
                    if (RA2 == 0 && d < 31) {
                        d++;
                        WriteCommandToLCD(0xc0);
                        WriteTempToLCD(d);
                    }
                    if (RA3 == 0 && d > 0) {
                        d--;
                        WriteCommandToLCD(0xc0);
                        WriteTempToLCD(d);
                    }
                    break;

                case 2:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("   CHANGE MONTH    ");
                    if (RA2 == 0 && m < 12) {
                        m++;
                        WriteCommandToLCD(0xc0 + 3);
                        WriteTempToLCD(m);
                    }
                    if (RA3 == 0 && m > 0) {
                        m--;
                        WriteCommandToLCD(0xc0 + 3);
                        WriteTempToLCD(m);
                    }
                    break;
                case 3:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("  CHANGE YEAR  ");
                    if (RA2 == 0) {
                        y++;
                        WriteCommandToLCD(0xc0 + 6);
                        WriteTempToLCD(y);
                    }
                    if (RA3 == 0 && y > 0) {
                        y--;
                        WriteCommandToLCD(0xc0 + 6);
                        WriteTempToLCD(y);
                    }
                    break;
                case 4:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD(" CHANGE WEEK DAY  ");
                    if (RA2 == 0 && w <= 7) {
                        w++;
                    }
                    if (RA3 == 0 && w >= 1) {
                        w--;
                    }
                    WriteCommandToLCD(0xc0 + 9);
                    switch (w) {
                        case Monday: WriteStringToLCD("MON");
                            break;
                        case Tuesday: WriteStringToLCD("TUE");
                            break;
                        case Wednesday: WriteStringToLCD("WED");
                            break;
                        case Thursday: WriteStringToLCD("THU");
                            break;
                        case Friday: WriteStringToLCD("FRI");
                            break;
                        case Saturday: WriteStringToLCD("SAT");
                            break;
                        case Sunday: WriteStringToLCD("SUN");
                            break;
                        default: WriteStringToLCD("???");
                            break;
                    }
                    break;
            }
            Set_DS1307_RTC_Date(d, m, y, w);
        }
    }

}
//Tr?ng thái 4: H?n gi?

void displayState4(void) {
    ClearLCDScreen();
    unsigned int h;
    unsigned int m;
    unsigned int s;
    h = 0;
    m = 0;
    s = 0;
    if (!RA1) {
        state++;
        if (state == 5) {
            state = 1;
        }
    }
    while (1) {
        WriteCommandToLCD(0x80);
        WriteStringToLCD("     ALARM     ");
        WriteCommandToLCD(0xc0);
        WriteTempToLCD(h);
        WriteStringToLCD(":");
        WriteTempToLCD(m);
        WriteStringToLCD(":");
        WriteTempToLCD(s);
        if (!RA1) {
            DisplayFirstTimeToLCD(Get_DS1307_RTC_Time());
            //DisplayDateOnLCD(Get_DS1307_RTC_Date());
            break;
        }
        while (1) {
            if (RA4 == 0) {
                switch_state++;
                if (switch_state == 4) {
                    break;
                }
            }
            if (RA1 == 0) {
                state = 1;
                break;
            }
            switch (switch_state) {
                case 1:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("    HOUR     ");
                    if (RA2 == 0 && h < 24) {
                        h++;
                        WriteCommandToLCD(0xc0);
                        WriteTempToLCD(h);
                    }
                    if (RA3 == 0 && h > 0) {
                        h--;
                        WriteCommandToLCD(0xc0);
                        WriteTempToLCD(h);
                    }
                    break;

                case 2:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("     MINUTE    ");
                    if (RA2 == 0 && m < 60) {
                        m++;
                        WriteCommandToLCD(0xc0 + 3);
                        WriteTempToLCD(m);
                    }
                    if (RA3 == 0 && m > 0) {
                        m--;
                        WriteCommandToLCD(0xc0 + 3);
                        WriteTempToLCD(m);
                    }
                    break;
                case 3:
                    WriteCommandToLCD(0x80);
                    WriteStringToLCD("    SECOND   ");
                    if (RA2 == 0) {
                        s++;
                        WriteCommandToLCD(0xc0 + 6);
                        WriteTempToLCD(s);
                    }
                    if (RA3 == 0 && s > 0) {
                        s--;
                        WriteCommandToLCD(0xc0 + 6);
                        WriteTempToLCD(s);
                    }
                    break;
            }
            h_alarm = h;
            m_alarm = m;
            s_alarm = s;
        }
    }

}
