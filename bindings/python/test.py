#!/usr/bin/env python
import hime

master = hime.Master()
master.load_skill("a0,dummy,dummy skill,0")
master.load_piece("0,dummy,mar,phys,a0,p0,40,40,40")
print master.skill("a0").desc
print master.piece("0").param.power

p1 = hime.create_owned_piece(master.piece("0"), "a")
p2 = hime.create_owned_piece(master.piece("0"), "b")
print p1.id
print p2.master.name

b = hime.SessionBuilder(0, 2, 1, 1)
b.push_piece(p1, 0)
b.push_piece(p2, 1)
session = b.build()
print session.player_num

