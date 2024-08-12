MAIN1  err     r3,LIST,r4
e3     prn     #48
        macr    m_macr
        cmp     r3,#-6
        bne     END
        endmacr
        lea     STR,r6
m_drive:  inc     r6
        mov     *r6,K
        sub     r1,r4
        m_macr
        dec     K
        jmp     LOOP
END:    stop
STR:    .string     "abcd"
LIST:   .data   6,-9
        .data   -100
K:      .data   31
