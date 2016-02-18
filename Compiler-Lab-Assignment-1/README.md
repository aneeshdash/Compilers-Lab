A basic compiler designed in C which takes a program as input and outputs its corresponding 3 addrress and 8086 code.

It supports the following language:
 
Statement -> relation | if relation then statement | while relation do statement | begin opt_stmts end<br \>
relation  -> expression [><=] relation<br \>
expression-> term expression | [+-] term expression | epsilon<br \>
term	  -> factor term | [*/] factor term | epsilon<br \>
factor	  -> num_or_id | (expression)<br \>

Compile and run code_gen.c with input file and it outputs 3addr.txt file as output (create the file before hand)
then run gen_8086.c which takes input the file (3addr.txt) and outputs 8806 code to 8086.txt