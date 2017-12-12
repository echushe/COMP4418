% COMP4418 Assignment 2
% Question 1
#const k = 3.

% encoding
k{c(X) : v(X)}k.
:- not e(X, Y), c(X), c(Y), X != Y.

% Vertices
v(1..6).

% Edges
e(1,2). e(2,1).
e(1,4). e(4,1).
e(1,3). e(3,1).
e(2,4). e(4,2).
e(2,6). e(6,2).
e(2,5). e(5,2).
e(3,4). e(4,3).
e(3,5). e(5,3).
e(3,6). e(6,3).
e(4,5). e(5,4).
e(5,6). e(6,5).

% Display
#show c/1.
