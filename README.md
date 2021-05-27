# AVR microcontroller lab 9

## Task

1. Спроектувати принципову схему системи обміну даними між мікроконтролерною мережею (один ATmega2560 та два ATmega328P) на основі інтерфейсу RS-485 та персональним комп’ютером;
2. Розробити протокол обміну між мікроконтролерами «master» та «slave» мережі на основі RS-485, а також між клієнтською програмою Windows та МК «master» (RS-232); У протоколі мають бути наявні контрольні суми CRC (таблиця 2).
3. Для МК «master» та «slave» написати програмний код, що реалізовуватиме поставлену задачу:

   - 1й Slave надсилає «Прізвище Ім’я По-батькові» студента;
   - 2й Slave надсилає «Дату народження» студента;

   Продублювати 5 раз ці повідомлення з внесеними в них спотвореннями згідно варіанту завдання.

4. Розробити клієнтську програму (С++, C#, JavaScript) та написати для неї програмний код, що реалізовуватиме зв’язок між комп’ютером та МК «master» і відображатиме та аналізуватиме отримані дані з МК «slave» на наявність помилок передачі.

**Параметри:**  
**RS-485**: швидкість передачі 9600 Бод  
**RS-232**: швидкість передачі 7200 Бод  
**Aдреси МК**: Slave1 = 88, Slave2 = 52  
**CRC**: CRC-16/CCIT-ZERO (таблична реалізація) для кожних 4 байтів інформації

**Перелік спотворень:**

1. без спотворень
2. 5й розряд найстаршого байту даних Slave1
3. 5й розряд наймолодшого байту даних Slave2
4. 1й та 3й розряд 4го байту даних Slave2
5. 0, 4, 5 розряди 7го байту даних Slave1

### Специфікації алгоритмів CRC

| Name             | Poly   | Init   | RefIn | RefOut | XorOut |
| ---------------- | ------ | ------ | ----- | ------ | ------ |
| CRC-16/CCIT-ZERO | 0x1021 | 0x0000 | false | false  | 0x0000 |

## Getting Started

### Prerequisites

- Install [VS Code](https://code.visualstudio.com/Download)
- Install [PlatformIO](https://platformio.org/install/ide?install=vscode)
- Install [Proteus](https://www.labcenter.com/)
- Install [Python 3.8+](https://www.python.org/downloads/release/python-382/)

### Installing

- Clone repository into your system

```
git clone https://github.com/Ravenen/microcontroller_lab_9.git
```

- Activate Python virtual environment

  - Windows

  ```
  GUI\venv\Scripts\activate
  ```

  - Unix

  ```
  source GUI/venv/Scripts/activate
  ```

- Install requirements from `requirements.txt`

```
pip install -r GUI/requirements.txt
```

### Executing program

- Build a project in `code` folder via PlatformIO builder
- Run the `GUI/app.py` file
- Open Proteus project, configure COMPIM with COM port and speed, and run the simulation

## Authors

[Vitaliy Pavliyk (@ravenen)](https://github.com/Ravenen)
