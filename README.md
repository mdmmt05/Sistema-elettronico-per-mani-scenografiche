# 🤲 Nato Due Volte — Sistema di Controllo delle Mani Scenografiche

<div align="center">

![Arduino](https://img.shields.io/badge/Arduino-Mega%202560%20%7C%20Nano-00878F?style=for-the-badge&logo=arduino&logoColor=white)
![RF24](https://img.shields.io/badge/RF24-nRF24L01%2B-blue?style=for-the-badge)
![Language](https://img.shields.io/badge/Linguaggio-C%2B%2B%20%2F%20Arduino-orange?style=for-the-badge)
![Event](https://img.shields.io/badge/Carnevale%20di%20Massafra-73%C2%AA%20edizione-purple?style=for-the-badge)
![License](https://img.shields.io/badge/Licenza-MIT-green?style=for-the-badge)

</div>

---

> 🇮🇹 [Italiano](#-descrizione-del-progetto) · 🇬🇧 [English](#-project-description)

---

## 🇮🇹 Descrizione del Progetto

**Nato Due Volte** è un carro carnevalesco presentato alla **73ª edizione del Carnevale di Massafra**. Il sistema descritto in questa repository gestisce due **mani scenografiche robotizzate**, ciascuna composta da cinque dita ad attuazione indipendente tramite servomotori.

Il cuore del sistema è una comunicazione wireless in radiofrequenza tra un **telecomando palmare personalizzato** e due **unità di controllo indipendenti**, una per mano. Ogni unità è basata su una shield Arduino progettata su misura che integra il modulo RF e i connettori per i servomotori. Il telecomando permette di inviare fino a **7 comandi di movimento** a ciascuna mano, selezionabili tramite una combinazione di due pressioni di pulsante.

Il progetto include firmware Arduino per entrambe le unità di controllo e per il telecomando, oltre al manuale di installazione completo.

---

## 📸 Demo

Apri i video natoduevolte.mp4 e mano.mp4 per osservare il risultato di questo progetto.

---

## 🔧 Hardware e Componenti

### Componenti per unità

| Componente | Quantità | Note |
|---|---|---|
| Arduino Mega 2560 | 2 | Unità di controllo (una per mano) |
| Arduino Nano | 1 | Telecomando |
| Modulo nRF24L01+ | 3 | Un modulo per ogni scheda |
| Shield personalizzata (Mega) | 2 | Progettata su misura — include connettori J1–J7 |
| Shield personalizzata (Nano) | 1 | Progettata su misura — include 7 pulsanti |
| Servomotori | 10 | 5 per mano, connettore 3 pin (DATA / 5V / GND) |
| Alimentatore 5V / min. 8A | 2 | Uno per unità di controllo |
| Powerbank 5V | 1 | Alimentazione telecomando via USB-C |

### Connessione servomotori (shield Mega)

| Connettore shield | Indice firmware | Dito |
|:-----------------:|:---------------:|:----:|
| J3 | 0 | Pollice |
| J4 | 1 | Indice |
| J5 | 2 | Medio |
| J6 | 3 | Anulare |
| J7 | 4 | Mignolo |

---

## 📂 Struttura della Repository

```
nato-due-volte/
├── controller_code.ino          # Firmware unità di controllo (Arduino Mega)
├── code_telecomando.ino         # Firmware telecomando (Arduino Nano)
└── manuale_di_installazione.docx  # Manuale di installazione completo (IT/EN)
```

---

## ⚙️ Installazione del Firmware

### Prerequisiti
- Arduino IDE
- Libreria **RF24** (installabile dal Library Manager dell'Arduino IDE)

### Unità di controllo (Arduino Mega)

1. Aprire `controller_code.ino` nell'Arduino IDE.
2. Alla riga `#define USE_FIRST`, impostare:
   - `true` per il **controller #1**
   - `false` per il **controller #2**
3. Selezionare **Arduino Mega or Mega 2560** come scheda target.
4. Caricare lo sketch.
5. Ripetere la procedura per la seconda scheda con il valore opposto.

> ⚠️ **Attenzione:** caricare entrambe le schede con lo stesso valore causerà la risposta di entrambe le mani allo stesso pulsante.

### Telecomando (Arduino Nano)

1. Aprire `code_telecomando.ino` nell'Arduino IDE.
2. Selezionare **Arduino Nano** come scheda target.
3. Caricare lo sketch. Nessuna configurazione aggiuntiva richiesta.

Per istruzioni complete su collegamento hardware, alimentazione e risoluzione dei problemi, consultare il **[manuale di installazione](./manuale_di_installazione.docx)**.

---

---

## 🇬🇧 Project Description

**Nato Due Volte** (*Born Twice*) is a carnival float presented at the **73rd edition of the Carnevale di Massafra**. This repository contains the firmware and documentation for the system controlling two **scenic robotic hands**, each composed of five independently actuated fingers driven by servo motors.

The system relies on wireless radio-frequency communication between a **custom handheld remote controller** and two **independent control units**, one per hand. Each unit is built around a custom-designed Arduino shield integrating the RF module and servo connectors. The remote controller allows the operator to send up to **7 movement commands** to each hand, selected via a two-button-press combination.

The repository includes Arduino firmware for both the control units and the remote controller, along with a complete bilingual installation manual.

---

## 📸 Demo

Open videos natoduevolte.mp4 and mano.mp4 to see this project in action.

---

## 🔧 Hardware & Components

### Component list

| Component | Qty | Notes |
|---|---|---|
| Arduino Mega 2560 | 2 | Control units (one per hand) |
| Arduino Nano | 1 | Remote controller |
| nRF24L01+ module | 3 | One per board |
| Custom shield (Mega) | 2 | Custom-designed — includes J1–J7 connectors |
| Custom shield (Nano) | 1 | Custom-designed — includes 7 push-buttons |
| Servo motors | 10 | 5 per hand, 3-pin connector (DATA / 5V / GND) |
| 5V / min. 8A PSU | 2 | One per control unit |
| 5V powerbank | 1 | Remote controller power via USB-C |

### Servo connections (Mega shield)

| Shield connector | Firmware index | Finger |
|:----------------:|:--------------:|:------:|
| J3 | 0 | Thumb |
| J4 | 1 | Index |
| J5 | 2 | Middle |
| J6 | 3 | Ring |
| J7 | 4 | Little |

---

## 📂 Repository Structure

```
nato-due-volte/
├── controller_code.ino            # Control unit firmware (Arduino Mega)
├── code_telecomando.ino           # Remote controller firmware (Arduino Nano)
└── manuale_di_installazione.docx  # Full installation manual (IT/EN)
```

---

## ⚙️ Firmware Installation

### Prerequisites
- Arduino IDE
- **RF24** library (installable via the Arduino IDE Library Manager)

### Control units (Arduino Mega)

1. Open `controller_code.ino` in the Arduino IDE.
2. At the `#define USE_FIRST` line, set:
   - `true` for **controller #1**
   - `false` for **controller #2**
3. Select **Arduino Mega or Mega 2560** as the target board.
4. Upload the sketch.
5. Repeat for the second board with the opposite value.

> ⚠️ **Warning:** flashing both boards with the same value will cause both hands to respond to the same button press.

### Remote controller (Arduino Nano)

1. Open `code_telecomando.ino` in the Arduino IDE.
2. Select **Arduino Nano** as the target board.
3. Upload the sketch. No additional configuration required.

For complete hardware wiring, power supply, and troubleshooting instructions, refer to the **[installation manual](./manuale_di_installazione.docx)**.

---

## 📄 Licenza / License

Distribuito sotto licenza MIT. / Distributed under the MIT License.
