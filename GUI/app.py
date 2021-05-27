from PyQt5 import QtWidgets

from model.comui import COMUI
from misc import *


def main():
    app = QtWidgets.QApplication(sys.argv)
    window = COMUI()
    window.show()
    app.exec_()


if __name__ == '__main__':
    main()
