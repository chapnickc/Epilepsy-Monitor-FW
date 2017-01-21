import logging

from pyOCD.board import MbedBoard


logging.basicConfig(level=logging.INFO)

board = MbedBoard.chooseBoard()
target = board.target
flash = board.flash

target.resume()


target.halt()

print("pc: 0x%X" % target.readCoreRegister("pc"))

target.resume()

board.uninit()
