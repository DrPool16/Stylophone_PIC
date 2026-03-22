
# Stylophone Emulator - PIC16F887

This project implements a Stylophone-like synthesizer using a PIC16F887 microcontroller, optimized to operate with a low-frequency crystal oscillator (32.768 kHz). The system uses a Hardware Abstraction Layer (HAL) architecture to scan a 24-key matrix built from PCB pads.




## 🛠 Technical Specifications
Microcontroller: PIC16F887
Clock Frequency: 32.768 kHz (LP mode)
Peripherals Used: PWM (CCP1), Timer2, GPIO
Software Architecture:
Deterministic scanning using pointer-based structures and Lookup Tables (LUT) for CPU cycle optimization
## 🎼 Tone Generation Logic
Due to the low oscillator frequency (32.768 kHz), a Lookup Table (LUT) was implemented for notes in octaves 3 and 4. This approach avoids costly floating-point operations and real-time divisions, ensuring minimal latency in key response.

PWM Frequency Calculation

For each note, the PR2 and Duty Cycle (50%) values were precomputed using:


```bash
PR2 = (Fosc / (4 * Fnote * Prescaler)) - 1
```


## 🧠 System Design Highlights
- Hardware Abstraction Layer (HAL):
The use of a Tecla structure with volatile unsigned char* port demonstrates separation between hardware mapping and application logic.
- Real-Time Behavior (Determinism):
The key scanning loop exits immediately upon detection (break), ensuring predictable and fast response.
- Resource Optimization:
Precomputed frequency values (LUT) eliminate runtime calculations, making the system viable under extreme clock constraints.
## 🔄 System Flow Diagram
<img width="807" height="1339" alt="mermaid-diagram" src="https://github.com/user-attachments/assets/b55ee6ab-d803-4a2f-b8f8-b4794787d787" />

## 🏷 GitHub Topics
- embedded-systems
- pic16f887
- mplab-x
- pwm-audio
- c-programming
- low-power

