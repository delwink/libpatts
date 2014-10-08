Example Table Structure
=======================

The below structure might resemble the database with one user in the midst of
logging a day's work. I have modeled the state of the database to resemble a
simple idea of logging my activities while creating PATTS.

### User

id  | state | firstName | lastName  | mysqlUser
--- | ----- | --------- | --------- | ---------
0   | 1     | David     | McMackins | david

### Client

id  | state | firstName | lastName
--- | ----- | --------- | ---------
0   | 1     | Mister    | Moneybags

### TaskType

id  | state | parentID | displayName
--- | ----- | -------- | -----------------
0   | 1     | NULL     | Work
1   | 1     | 0        | PATTS Development
2   | 1     | 1        | Program Design
3   | 1     | 1        | Writing Code

### TaskItem

There will be dates in the Time columns, but for the sake of demonstration, I
have left them out of the example table.

This may change, but I have decided for now to use an id of -1 for any 
database transaction that should inherit the value of that field from its 
parent.

id  | state | typeID | parentTaskItemID | onClock | userID | clientID | startTime | endTime
--- | ----- | ------ | ---------------- | ------- | ------ | -------- | --------- | -------
0   | 1     | 0      | NULL             | 1       | 0      | NULL     | 9:00      | NULL
1   | 1     | 1      | 0                | 1       | 0      | 0        | 9:00      | NULL
2   | 1     | 2      | 1                | 0       | 0      | -1       | 9:00      | 10:00
3   | 1     | 3      | 1                | 1       | 0      | -1       | 10:30     | NULL
