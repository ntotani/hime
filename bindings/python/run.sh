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

g++ -DPIC -fPIC -std=c++1y -lboost_python -lpython2.7 -I../../include -I$USR_PATH/include/python2.7 -shared -o hime.so ../../src/master.cpp hime.cpp
$BIN_PATH test.py

