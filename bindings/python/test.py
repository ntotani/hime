#!/usr/bin/env python
import hime

master = hime.Master()
master.load_skill("a0,dummy,dummy skill,0")
master.load_piece("0,dummy,mar,phys,a0,p0,40,40,40")
print master.skill("a0").desc
print master.piece("0").param.power

