#!/usr/bin/env python
import hime

master = hime.Master()
master.loadSkill("a0,dummy,dummy skill,0")
print master.skill("a0").desc

