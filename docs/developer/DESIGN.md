+-------------------+
|   CyrusDialog     |   <-- UI layer
|  (owns widgets,   |
|   wires signals)  |
+---------+---------+
          |
          v
+-------------------+            +------------------+
| InitiativeController | <-----> | CharacterModel   |
| (logic: add chars,   |         | (data: characters)|
|  advance/backtrack,  |         +------------------+
|  send to combat log) |
+----+-----------+----+
     |           |
     | signals   | slots
     v           v
+-----------+   +------------------+
| Initiative |   | CombatLogModel  |
| View       |   | (data: log text,|
| (UI widget |   |  icons, etc.)   |
|  + delegate)|  +-----------------+
+------+------+
       |
       v
+-------------------+
| InitiativeDelegate|
| (painting, rects, |
|  editing support) |
+-------------------+
