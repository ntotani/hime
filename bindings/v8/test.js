var seed = 1;
var playerNum = 2;
var boardId = 1;
/*
var pieces = [
    [
        {id:"a", master:1, power:22, defense:22, regist:20}
    ],[
        {id:"b", master:1, power:22, defense:22, regist:20}
    ]
];
var session = new hime.Session(seed, playerNum, boardId, pieces);
session.commitFormation({a:{i:8, j:3}, b:{i:0, j:3}});
var commands = [
    {piece:0, card:0}
];
var acts = session.processTurn(commands);

// validate new command
// append command
// notice if need

*/

var hime = require('bindings')('hime');

var obj = new hime.Session(seed, playerNum, boardId);
console.log(obj.playerNum());

