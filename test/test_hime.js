var assert = require("assert");
var hime = require('../build/hime.js');
var enc = require('./encoding.min.js');

var s1 = new hime.Skill("1", enc.convert("全体回復", "UTF8"), enc.convert("味方全員を@回復する", "UTF8"), 30);
var s2 = new hime.Skill("1", enc.convert("癒やし", "UTF8"), enc.convert("周りの駒が毎ターン@ずつ回復する", "UTF8"), 30);
assert.equal("全体回復", enc.convert(s1.name));
assert.equal("周りの駒が毎ターン@ずつ回復する", enc.convert(s2.desc));
var prm = new hime.Parameter(60, 50, 80);
assert.equal(60, prm.power);
var mp = new hime.MasterPiece("1", enc.convert("姫", "UTF8"), hime.Planet.kSun, hime.PieceAction.kHeal, s1, s2, prm);
assert.equal("姫", enc.convert(mp.name));
assert.equal(hime.Planet.kSun, mp.planet);
assert.equal(30, mp.active_skill.rate);
assert.equal(50, mp.param.defense);
var op = new hime.OwnedPiece(mp, "a")
assert.equal("1", op.master.id);

//var sc = hime.SessionContext.implement({random: function() { return 777; }});
var vec = new hime.OwnedPieceVector();
vec.push_back(op);
var vecvec = new hime.OwnedPieceVectorVector();
vecvec.push_back(vec);
var s = new hime.session_factory(0, 2, 0, 0, vecvec);
assert.equal(2, s.player_num);
assert.equal(1, s.owned_pieces.size());

