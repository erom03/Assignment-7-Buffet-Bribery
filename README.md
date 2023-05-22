# Assignment-7-Buffet-Bribery
 
## Objective
 
Give practice with binary search in C.
Give practice with heaps in C.

## Story

The ship is ready to go, but the bill for paying the repair crew is due. You have worked out a
deal for paying some of the bill by using your buffet on your main cruise ship. You have food
shipments arriving that could be used exclusively for the repair crew. The shipments are slated
to arrive and expire at known times. The mass of each shipment is also known in advance.

The repair crew will send over some of their individuals to eat at some given time, and they will
leave at some other given time. You can assume that the individuals will eat at a constant rate.
You want to ensure that at any point in time there is always some food for the crew to eat.

## Problem

Given the arrival, expiration, and size of each food shipment, and the start and end time of the
repair crew determine the highest constant rate they can eat at such that they will not starve
(they always have some food to eat).

## Input

The first line of input will be a single integer, N, (1 ≤ N ≤ 200,000), representing the number of
food shipments. The following N lines will each contain a single shipment description. A
shipment description consists of 3 integers, a, e, and m (0 ≤ a < e ≤ 31,536,000; 1 ≤ m ≤
1,000,000), representing the arrival, expiration, and mass of each shipmen

Following the shipment descriptions will be a line with 2 integers, S and E (0 ≤ S < E ≤
31,536,000), representing the start eating time and end eating time of the crew.

## Output

Output should contain a single line containing a value representing the largest constant rate that
the crew could consume food at while always having something to eat. The output rate should
be in mass units per time unit. It’s guaranteed that there will always be a positive rate that the
crew could eat at. Your answer will be considered correct if it has an absolute or relative error of
at most $10^{-5}$.
