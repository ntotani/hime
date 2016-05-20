if [ "$1" = "mac" ]; then
    USR_PATH=/System/Library/Frameworks/Python.framework/Versions/2.7
    BIN_PATH=$USR_PATH/bin/python2.7
elif [ "$1" = "linux" ]; then
    USR_PATH=/usr
    BIN_PATH=$USR_PATH/bin/python
else
    echo 'please input platform'
    exit 1
fi

if [ "$2" = "python" ]; then
    $BIN_PATH test.py
else
    g++ -shared -o hime.so -std=c++1y -lboost_python -lpython2.7 -I../../include -I$USR_PATH/include/python2.7 -DPIC -fPIC ../../src/master.cpp ../../src/session.cpp ../../src/board.cpp ../../src/action.cpp hime.cpp && $BIN_PATH test.py
fi

