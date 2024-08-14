MAIN1:   lea     r3  ,   LIST  ,   r4
e3     prn      
        macr    m_macr
m_macr  cmp     r3,#-6
        bne     END
        endmacr
        lea     STR,r6
m_dog:  inc     r6`
        macr    m_drive
        mov     *r6,K
        sub     r1,r4
        endmacr
        m_drive
        m_macr
        dec     K
        jmp     LOOP
END:    stop
STR:    .string     "abcd"
LIST:   .data   6,-9
        .data   -100
K:      .data   31
