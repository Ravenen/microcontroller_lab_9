from itertools import chain
from typing import Union

import serial
from PyQt5 import QtWidgets

from constants import *
from crc16 import *
from design import design
from misc import serial_ports
from model.listener import Listener


class COMUI(QtWidgets.QMainWindow, design.Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.port: Union[serial.Serial, None] = None
        self.listener = None
        self.init_elements()
        self.current_text_edit = None
        self.current_indicator = None

        self.bytes_to_check_buffer = ''
        self.crc_buffer = ''
        self.buffer = ''
        self.bytes_to_check_buffer_iterator = 0
        self.crc_table = generate_table()
        self.is_data_damaged = False

        self.texts = [self.slaveAText0, self.slaveBText0, self.slaveAText1, self.slaveBText1, self.slaveAText2,
                      self.slaveBText2, self.slaveAText3, self.slaveBText3, self.slaveAText4, self.slaveBText4]
        self.indicators = [self.slaveAIndicator0, self.slaveBIndicator0, self.slaveAIndicator1, self.slaveBIndicator1,
                           self.slaveAIndicator2, self.slaveBIndicator2, self.slaveAIndicator3, self.slaveBIndicator3,
                           self.slaveAIndicator4, self.slaveBIndicator4]

    def init_elements(self):
        self.comComboBox.addItems(serial_ports())
        self.openComButton.clicked.connect(self.connect)
        self.getSlaveA.clicked.connect(self.ask_slave(SLAVE_A_ADDRESS))
        self.getSlaveB.clicked.connect(self.ask_slave(SLAVE_B_ADDRESS))
        self.auto_2.clicked.connect(self.ask_automode)

    def connect(self):
        try:
            self.port = serial.Serial(self.comComboBox.currentText(), RS232_SPEED,
                                      parity=serial.PARITY_NONE,
                                      stopbits=serial.STOPBITS_ONE,
                                      bytesize=serial.EIGHTBITS)
            self.openComButton.setStyleSheet(f"background-color: {COLOR_GREEN};")
            self.openComButton.setText('Підімкнено')
            self.listen_port()
        except Exception as e:
            print(e)

    def listen_port(self):
        self.listener = Listener(self.port, self.receive_message)
        self.listener.start()

    def ask_slave(self, slave_address):
        def execute():
            self.current_text_edit = chain([self.slaveTextEdit])
            self.current_indicator = chain([self.slaveTextIndicator])
            self.slaveTextIndicator.setStyleSheet(f"background-color: {COLOR_BLUE};")
            self.send_message(slave_address)

        return execute

    def ask_automode(self):
        for indicator in self.indicators:
            indicator.setStyleSheet(f"background-color: {COLOR_BLUE};")
        self.current_text_edit = chain(self.texts)
        self.current_indicator = chain(self.indicators)
        self.send_message(AUTO_MODE_ADDRESS)

    def send_message(self, message):
        if self.port:
            self.port.write(chr(message).encode('utf-8'))

    def receive_message(self, message):
        # print(message)

        if self.bytes_to_check_buffer_iterator < BYTES_TO_CHECK:
            char = message.decode("utf-8")
            self.buffer += char
            self.bytes_to_check_buffer += char
            self.bytes_to_check_buffer_iterator += 1

        elif self.bytes_to_check_buffer_iterator >= BYTES_TO_CHECK:
            self.crc_buffer += chr(int.from_bytes(message, "big"))
            self.bytes_to_check_buffer_iterator += 1

        if self.bytes_to_check_buffer_iterator == BYTES_TO_CHECK + CRC_BYTES:
            crc = generate_crc(self.bytes_to_check_buffer + self.crc_buffer, self.crc_table)
            self.is_data_damaged = self.is_data_damaged or crc
            self.bytes_to_check_buffer_iterator = 0
            self.crc_buffer = ''
            self.bytes_to_check_buffer = ''

        if message == EOT:
            try:
                next(self.current_text_edit).setText(self.buffer)
                next(self.current_indicator).setStyleSheet(f"background-color: "
                                                           f"{COLOR_RED if self.is_data_damaged else COLOR_GREEN};")
            except StopIteration as e:
                print(e.value)

            self.is_data_damaged = False
            self.buffer = ''
            self.bytes_to_check_buffer = ''
            self.bytes_to_check_buffer_iterator = 0
            self.crc_buffer = ''
