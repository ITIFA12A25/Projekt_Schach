QT = core
QT += network
QT += httpserver

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Bishop.cpp \
        Board.cpp \
        Game.cpp \
        GameRepository.cpp \
        King.cpp \
        Knight.cpp \
        MatchmakingService.cpp \
        Pawn.cpp \
        Persistence.cpp \
        Player.cpp \
        Queen.cpp \
        Rook.cpp \
        UserService.cpp \
        apiserver.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Bishop.h \
    Board.h \
    Cell.h \
    Game.h \
    GameRepository.h \
    GameStatus.h \
    GameStatusUtils.h \
    King.h \
    Knight.h \
    MatchmakingService.h \
    Move.h \
    MoveValidator.h \
    Pawn.h \
    Persistence.h \
    Piece.h \
    Player.h \
    Position.h \
    Queen.h \
    Rook.h \
    UserService.h \
    apiserver.h

DISTFILES += \
    resources/rest-ui/index.html

RESOURCES += \
    resources.qrc
