        macr    m_macr
        cmp     r3,#-6
        bne     END
        endmacr
        leaf     STR,r6
        inc     r6
        mov     *r6,K
        sub     r1,r4
        m_macr
        macr    m_drive
        cmp     r2,#-6
        endmacr
        dec     K
        jmp     LOOP
END:    stop
STR:    .string     "abcd"
LIST:   .data   6,-9
        .data   -100
K:      .data   31
