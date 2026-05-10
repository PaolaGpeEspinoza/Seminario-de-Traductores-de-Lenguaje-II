org 100h
.data
    a DW 0
    b DW 0
.code
start:
    MOV AX, 4C00h
    INT 21h
end start
