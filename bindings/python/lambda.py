#!/usr/bin/env python
from __future__ import print_function
import json
import ctypes
ctypes.cdll.LoadLibrary('libboost_python.so.1.53.0')
ctypes.cdll.LoadLibrary('hime.so')
from hime import *

print('Loading function')

def lambda_handler(event, context):
    #print("Received event: " + json.dumps(event, indent=2))
    #print("value1 = " + event['key1'])
    #print("value2 = " + event['key2'])
    #print("value3 = " + event['key3'])
    master = Master()
    master.load_skill("a0,dummy,dummy skill,0")
    master.load_piece("0,dummy,mar,phys,a0,p0,40,40,40")
    #print(master.skill("a0").desc)
    #print(master.piece("0").param.power)

    p1 = create_owned_piece(master.piece("0"), "a")
    p2 = create_owned_piece(master.piece("0"), "b")
    #print p1.id
    #print p2.master.name

    b = SessionBuilder(0, 2, 1, 1)
    b.push_piece(p1, 0)
    b.push_piece(p2, 1)
    session = b.build()
    #print session.player_num

    form = Formation()
    form.add("a", Point(8, 3))
    form.add("b", Point(0, 3))
    session.commit_formation(form)

    cmds = Commands()
    cmds.add(0, 0)
    return session.process_turn(cmds)
    #return event['key1']  # Echo back the first key value
    #raise Exception('Something went wrong')

