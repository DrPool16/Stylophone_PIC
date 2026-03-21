/*
 * File:   styloapp.c
 * Author: DrPool_16
 *
 * Created on 20 de marzo de 2026, 16:56
 */


#include <xc.h>

// --- FUSES (CONFIGURACIÓN) ---
#pragma config FOSC = LP        // Oscilador de Baja Velocidad (32Khz)
#pragma config WDTE = OFF       // Watchdog Timer desactivado
#pragma config PWRTE = ON       // Power-up Timer activado
#pragma config MCLRE = ON       // Master Clear activado
#pragma config CP = OFF         // Protección de código desactivada
#pragma config BOREN = OFF      // Brown-out Reset desactivado
#pragma config LVP = OFF        // Programación de bajo voltaje desactivada

#define _XTAL_FREQ 32768      // Definimos la frecuencia de 6MHz

// Estructura de Nota (Ingeniería de datos)
typedef struct {
    uint8_t pr2_val;    // Valor del periodo
    uint16_t duty_val;  // Valor del ciclo de trabajo (10 bits)
} Nota;

/* TABLA DE NOTAS (Octava 3 y 4) 
   Calculado para 6MHz con Prescaler de 4 en Timer 2.
   Fórmula: PR2 = (6,000,000 / (4 * Fnota * 4)) - 1
*/

const Nota ESCALA[] = {
    /*OCTAVA 3*/
    {62, 250}, // Do3           (130.8 Hz)  -> Indix 0
    {58, 236}, // Do3#/Reb3     (138.6 Hz)  -> Indix 1
    {55, 223}, // Re3           (146.8 Hz)  -> Indix 2
    {52, 210}, // Re3#/Mib3     (155.6 Hz)  -> Indix 3
    {49, 199}, // Mib3          (164.8 Hz)  -> Indix 4
    {46, 188}, // Fa3           (174.6 Hz)  -> Indix 5
    {43, 177}, // Fa3#/Solb3    (184.9 Hz)  -> Indix 6
    {40, 167}, // Sol3          (196.0 Hz)  -> Indix 7
    {38, 158}, // Sol3#/Lab3    (207.6 Hz)  -> Indix 8
    {36, 149}, // La3           (220.0 Hz)  -> Indix 9
    {34, 141}, // La3#/Sib3     (233.0 Hz)  -> Indix 10
    {32, 132},  // Si3           (247.0 Hz)  -> Indix 11


    /*OCTAVA 4*/
    {30, 125}, // Do3           (261.8 Hz)  -> Indix 0
    {28, 118}, // Do3#/Reb3     (277.1 Hz)  -> Indix 1
    {27, 111}, // Re3           (293.6 Hz)  -> Indix 2
    {25, 105}, // Re3#/Mib3     (311.1 Hz)  -> Indix 3
    {24, 99},  // Mib3          (329.6 Hz)  -> Indix 4
    {22, 93},  // Fa3           (349.2 Hz)  -> Indix 5
    {21, 88},  // Fa3#/Solb3    (369.9 Hz)  -> Indix 6
    {20, 83},  // Sol3          (391.9 Hz)  -> Indix 7
    {19, 79},  // Sol3#/Lab3    (415.3 Hz)  -> Indix 8
    {18, 74},  // La3           (440.0 Hz)  -> Indix 9
    {17, 70},  // La3#/Sib3     (466.1 Hz)  -> Indix 10
    {16, 66}   // Si3           (493.8 Hz)  -> Indix 11

};
// Estructura para el mapeo de teclas (Hardware)
typedef struct {
    volatile unsigned char *port; // Puntero al registro del puerto
    uint8_t pin;                  // Bit del pin (0-7)
    uint8_t nota_idx;             // Índice de la nota en la tabla ESCALA
} Tecla;
// --- MAPEO DE LAS 22 NOTAS ---
// Aquí debes completar los 22 registros según donde soldaste cada cable
const Tecla MIS_TECLAS[] = {
    {&PORTA, 0, 0},  // Key 1 -> Do3
    {&PORTA, 1, 1},  // Key 2 -> Do#3
    {&PORTA, 2, 2},  // Key 3 -> Re3
    {&PORTA, 3, 3},  // Key 4 -> Re#3
    {&PORTA, 4, 4},  // Key 5 -> Mi3
    {&PORTA, 5, 5},  // Key 6 -> Fa3
    {&PORTC, 3, 6},  // Key 7 -> Fa#3
    {&PORTC, 4, 7},  // Key 8 -> Sol3
    {&PORTB, 0, 8},  // Key 9 -> Sol#3
    {&PORTB, 1, 9},  // Key 10 -> La3
    {&PORTB, 2, 10}, // Key 11 -> La#3
    {&PORTB, 3, 11}, // Key 12 -> Si3

    {&PORTB, 4, 12}, // Key 13 -> Do4
    {&PORTB, 5, 13}, // Key 14 -> Do#4
    {&PORTB, 6, 14}, // Key 15 -> Re3
    {&PORTB, 7, 15}, // Key 16 -> Re#4
    {&PORTD, 0, 16}, // Key 17 -> Mi4
    {&PORTD, 1, 17}, // Key 18 -> Fa4
    {&PORTD, 2, 18}, // Key 19 -> Fa#4
    {&PORTD, 3, 19}, // Key 20 -> Sol4
    {&PORTD, 4, 20}, // Key 21 -> Sol#4
    {&PORTD, 5, 21}, // Key 22 -> La4
    {&PORTD, 6, 22}, // Key 23 -> La#4
    {&PORTD, 7, 23} // Key 24 -> Si4
};


void init_stylophone(void) {
    // Configuración Digital
    ANSEL = 0;
    ANSELH = 0;

    // Limpiar puertos
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;

    // Entradas y Salidas
    TRISA = 0xFF; // Teclas Puerto A
    TRISB = 0xFF; // Teclas Puerto B
    TRISD = 0xFF; // Teclas Puerto D
    TRISC2 = 0;  // Pin RC1(CCP1) como salida.
    TRISC3 = 1;
    TRISC4 = 1;
    

    // Configuración Timer 2 para PWM
    // T2CON: Prescaler 1:1, Timer2 ON
    T2CON = 0b00000000; 
}

void sonar(int id) {
    if (id == -1) {
        CCP1CON = 0; // Silencio
        return;
    }

    PR2 = ESCALA[id].pr2_val;
    
    // Carga del Duty Cycle de 10 bits
    CCPR1L = (ESCALA[id].duty_val >> 2);          // 8 bits altos
    CCP1CONbits.DC1B = (ESCALA[id].duty_val & 3); // 2 bits bajos
    
    CCP1CONbits.CCP1M = 0x0C; // Modo PWM Activo
}

void main(void) {
    init_stylophone();

    while(1) {
        // Escaneo de teclado con prioridad
        // (Puedes mapear tus pines según tu hardware actual)
        if (RA0)      sonar(0);  // Do3
        else if (RA1) sonar(1);  // Re3
        else if (RA2) sonar(2);  // Mi3
        else if (RB0) sonar(7);  // Do4
        else if (RB1) sonar(8);  // Re4
        else if (RB2) sonar(10); // La4
        else sonar(-1);          // Silencio
    }
}