.import bad_media

        dc.b    'XBRA'
        dc.b    'Inst'
old_mediach:
        ds.l    1
        .export new_mediach
new_mediach:
        move.w  bad_media(pc), d0
        cmp     4(sp), d0
        bne.b   m_end
        moveq   #2, d0
        move    #-1, bad_media 
        rts
m_end:  move.l  old_mediach(pc), a0
        jmp     (a0)
.export  old_mediach

        dc.b    'XBRA'
        dc.b    'Inst'
old_rw: ds.l    1
        .export new_rw
new_rw:
        move.w  bad_media(pc), d0
        cmp     14(sp), d0
        bne.b   r_end
        moveq   #-14, d0
        move.w  #-1, bad_media
        rts
r_end:  move.l  old_rw(pc), a0
        jmp     (a0)
.export  old_rw
